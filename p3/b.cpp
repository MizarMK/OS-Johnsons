/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 3
 * Class:       CSI 4337
 ************************************************/
#include "buffet.h"
#include <iostream>

using namespace std;


/**
 * create buffetSemaphore obj with a requested max slices size
 * @param maxSlices max number of Slices in buffet
 */
Buffet::Buffet(int maxSlices) {
    if (maxSlices < 0)
        throw invalid_argument("maxSlices Input MUST be > 0");

    this->buffetMax = maxSlices;
    this->buffetClosed = false;
    sem_init(&this->buffetState, 0 ,1); // buffet mutex that prevents removing slices simultaneously
    sem_init(&this->vegMutex, 0 ,1); // vegetarian mutex starts off allowing 1 vegetarian at a time
    sem_init(&this->omniMutex, 0 ,1); // // vegetarian mutex starts off allowing 1 omnivore at a time
    sem_init(&this->addState, 0, 1);

    sem_init(&this->fullBlock,0,0);
    sem_init(&this->emptyBlock,0,0);
    sem_init(&this->omniBlock,0, 0);
    sem_init(&this->vegBlock,0, 0);

    vegetariansWaiting = 0;
}

bool Buffet::satisfyVeg(int desired) {
    if (desired == -1) { // producer checking if it should free omni
        for (int i : vegRequests) { // vegs waiting, only count meat slices
            if (i <= vegSlices)
                return true;
        }
    }
    else { // consumer checking if it should free itself
        return desired <= vegSlices;
    }
    return false;
}

bool Buffet::satisfyOmni(int desired) {
    if (desired == -1) { // producer checking if it should free omni
        if (vegetariansWaiting <= 0) { // no vegs waiting, count all slices
            for (int i : omniRequests) {
                if (i <= buffet.size()) {
                    return true;
                }
            }
        } else {
            for (int i : omniRequests) { // vegs waiting, only count meat slices
                if (i <= buffet.size() - vegSlices) {
                    return true;
                }
            }
        }
    }
    else { // consumer checking if it should free itself
        if (vegetariansWaiting <= 0) { // no vegs waiting, count all slices
            return desired <= buffet.size();
        } else {
            return desired < buffet.size() - vegSlices;
        }
    }
    return false;
}
void Buffet::producerRelease() {
    if ((satisfyVeg(-1) && vegetariansBlocked > 0) || buffetClosed) { // let blocked vegs out if possible
        sem_post(&this->vegBlock);
    }
    else if ((satisfyOmni(-1) && omnisBlocked > 0) || buffetClosed) { // otherwise unblock omnis
        sem_post(&this->omniBlock);
    }
    while (!buffet.empty() && blockOnEmpty > 0) // release all threads waiting on empty
        sem_post(&this->emptyBlock);
    sem_post(&this->vegMutex); // FIXME TRACK vegsBlocked, omnisBlocked, and emptyBlocked
    sem_post(&this->omniMutex);
}

bool Buffet::AddPizza(int count, SliceType stype) {
    if (count <= 0)
        throw invalid_argument("count Input MUST be > 0");
    try {
        if (!this->buffetClosed) {
            sem_wait(&this->addState);
            sem_wait(&this->vegMutex);
            sem_wait(&this->omniMutex);
            for (int i = 0; i < count; i++) {
                if (this->buffet.size() == buffetMax && !this->buffetClosed) { // SpinLock for maximum Capacity reached
                    producerRelease();
                    sem_wait(&this->fullBlock); // wait here until signaled by consumer
                    sem_wait(&this->vegMutex);
                    sem_wait(&this->omniMutex);
                }
                if (this->buffetClosed) {
                    producerRelease();
                    sem_post(&this->addState);
                    return false;
                }
                this->buffet.push_back(stype);
                if (stype == Veggie or stype == Cheese)
                    vegSlices++;
            }
            producerRelease();
            sem_post(&this->addState);
            return true;
        }
    } catch (exception e) {
        fprintf(stderr, "Error adding pizza to buffet");
    }
    return false;
}

void Buffet::close() {
    buffetClosed = true; // have all idling threads in other method return false via dependency on buffetStatus
    producerRelease();

    sem_post(&this->buffetState);
    while(vegetariansWaiting > 0) {
        sem_post(&this->vegMutex);
    }
    sem_post(&this->omniMutex);
    sem_post(&this->addState);
}

void Buffet::removeRequest(SliceType type, int desired) {
    if (type == Meat) {
        for (int i = 0; i < this->omniRequests.size();i++){
            if (omniRequests.at(i) == desired) {
                this->omniRequests.erase(this->omniRequests.begin() + i);
                i = this->omniRequests.size();
            }
        }
    }
    else if (type == Veggie) {
        for (int i = 0; i < this->vegRequests.size();i++){
            if (vegRequests.at(i) == desired) {
                this->vegRequests.erase(this->vegRequests.begin() + i);
                i = this->vegRequests.size();
            }
        }
    }
}


vector<SliceType> Buffet::TakeAny(int desired) {
    try {
        if (desired <= 0 or desired > this->buffetMax)
            throw invalid_argument("count Input MUST be > 0");
        if (this->buffetClosed)
            throw runtime_error("ERROR: buffet closed");

        vector<SliceType> plate;       // slices being atomically taken
        vector<int> sliceIndexes;      // index of slices to be taken

        while (sliceIndexes.size() < desired && !this->buffetClosed) {
            if (buffet.empty()) {
                blockOnEmpty++;
                sem_wait(&this->emptyBlock);
                blockOnEmpty--;
            }
            sem_wait(&this->omniMutex); // lock omnivores and buffet take state
            sem_wait(&this->buffetState);
            // System.out.println(buffet);

            plate.clear();       // clear plate and the indexes the thread is watching
            sliceIndexes.clear();

            for (int i = 0; i < buffet.size() && plate.size() < desired; i++) {
                if (vegetariansWaiting <= 0) {
                    sliceIndexes.push_back(i); // add the index and the pizza to the plate; removed if non-atomic
                    plate.push_back(this->buffet.at(i));
                }
                else {
                    if (!(this->buffet.at(i) == Cheese or this->buffet.at(i) == Veggie)) {
                        sliceIndexes.push_back(i);
                        plate.push_back(this->buffet.at(i));
                    }
                }
            }
            if (sliceIndexes.size() < desired) { // let other threads go if this one can't
                bool entry = true; // only allows threads woken by producer to wake other consumer threads
                sem_post(&this->buffetState);
                sem_wait(&this->omniMutex);
                omniRequests.push_back(desired); // couldn't be satisfied, leave unblocking to producers
                do { // while can't be satisfied: release special mutex, then wait on it
                    if (omnisBlocked > 0 && !entry)
                        sem_post(&this->omniBlock);
                    sem_wait(&this->omniBlock);
                    entry = false;
                } while (!satisfyOmni(desired) && !buffetClosed);
                removeRequest(Meat, desired);
            }
        }
        if (this->buffetClosed) {
            throw runtime_error("ERROR: buffet closed");

        }
        return getSliceTypes(plate, sliceIndexes, &omniMutex);

    } catch (exception &e) {
        fprintf(stderr, "Error taking (any) pizza slices");
    }
    return {};
}

vector<SliceType> Buffet::TakeVeg(int desired) {
    try {
        if (desired <= 0 or desired > this->buffetMax)
            throw invalid_argument("count Input MUST be > 0");
        if (this->buffetClosed)
            throw runtime_error("ERROR: buffet closed");

        sem_wait(&this->buffetState);
        vegetariansWaiting++; // mutex increase veg idle count
        sem_post(&this->buffetState);

        vector<SliceType> plate;       // slices being atomically taken
        vector<int> sliceIndexes;      // index of slices to be taken

        while(sliceIndexes.size() < desired && !this->buffetClosed) {
            if (buffet.empty()) {
                blockOnEmpty++;
                sem_wait(&this->emptyBlock);
                blockOnEmpty--;
            }
            sem_wait(&this->vegMutex);
            sem_wait(&this->buffetState);
            // System.out.println(".");

            plate.clear();
            sliceIndexes.clear();

            for (int i = 0; i < buffet.size() && plate.size() < desired; i++) {
                if (this->buffet.at(i) == Cheese or this->buffet.at(i) == Veggie) {
                    sliceIndexes.push_back(i);
                    plate.push_back(this->buffet.at(i));
                }
            }
            if (sliceIndexes.size() < desired) {
                bool entry = true;
                sem_post(&this->buffetState);
                sem_post(&this->vegMutex);
                vegRequests.push_back(desired); // couldn't be satisfied, leave unblocking to producers
                do { // while can't be satisfied: release special mutex, then wait on it
                    if (vegetariansBlocked > 0 && !entry)
                        sem_post(&this->vegBlock);
                    sem_wait(&this->vegBlock);
                    entry = false;
                } while (!satisfyVeg(desired) && !buffetClosed);
                removeRequest(Veggie, desired);
            }
        }
        vegetariansWaiting--;
        if (this->buffetClosed)
            throw runtime_error("ERROR: buffet closed");
        return getSliceTypes(plate, sliceIndexes, &vegMutex);

    } catch (exception &e) {
        fprintf(stderr, "Error taking (veg) pizza slices");
    }
    return {};
}

/**
 * gets slicetypes from specific indexes, clears buffet, and unlocks mutex
 * @param plate vector to add slices to
 * @param sliceIndexes indexes of slices to add
 * @param sem mutex to clear
 * @return list of slices to consumer thread
 */
vector<SliceType> Buffet::getSliceTypes(vector<SliceType> plate, vector<int> sliceIndexes, sem_t *sem) {
    for (int i = sliceIndexes.size() - 1; i >= 0; i--) {
        if (this->buffet.at(sliceIndexes.at(i)) == Cheese or this->buffet.at(sliceIndexes.at(i)) == Veggie)
            vegSlices--;
        this->buffet.erase(this->buffet.begin() + sliceIndexes.at(i));
        // System.out.println(buffet);
    }
        sem_post(&this->buffetState);
    if (!buffet.empty())
        sem_post(&this->emptyBlock);
    sem_post(&this->fullBlock);
    sem_post(sem);
    return plate;
}