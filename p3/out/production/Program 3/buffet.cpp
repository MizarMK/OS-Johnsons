/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 3
 * Class:       CSI 4337
 ************************************************/
#include "buffet.h"

Buffet::Buffet(const int maxSlices) {
    if (maxSlices < 0)
        throw new invalid_argument("maxSlices Input MUST be > 0");

    this->buffetMax = maxSlices;
    this->buffetClosed = false;
    sem_init(&this->buffetState, 0 ,1); // buffet mutex that prevents removing slices simultaneously
    sem_init(&this->vegMutex, 0 ,1); // vegetarian mutex starts off allowing 1 vegetarian at a time
    sem_init(&this->omniMutex, 0 ,1); // // vegetarian mutex starts off allowing 1 omnivore at a time
}

Buffet::~Buffet() {
    close();
    this->buffet.clear();
    sem_destroy(&this->addState);
    sem_destroy(&this->buffetState);
    sem_destroy(&this->vegMutex);
    sem_destroy(&this->omniMutex);
}

bool Buffet::AddPizza(const int count, const SliceType stype) {
    if (count <= 0)
        throw new invalid_argument("count Input MUST be > 0");
    if (!this->buffetClosed) {
        sem_wait(&this->addState);
        for (int i = 0; i < count; i++) {
            while (this->buffet.size() == buffetMax && !this->buffetClosed); // SpinLock for maximum Capacity reached
            if (this->buffetClosed) {
                sem_post(&this->addState);
                return false;
            }
            this->buffet.push_back(stype);
        }
        sem_post(&this->addState);
        return true;
    }
    return false;
}

void Buffet::close() {
    int waitStatus;
    buffetClosed = true; // have all idling threads in other method return false via dependency on buffetStatus

    /// free all waiting threads
    while(sem_getvalue(&this->buffetState, &waitStatus) == 0 and waitStatus < 0) { // free all waiting threads
        sem_post(&this->buffetState);
    }
    while(sem_getvalue(&this->vegMutex, &waitStatus) == 0 and waitStatus < 0) { // free all waiting threads
        sem_post(&this->vegMutex);
    }
    while(sem_getvalue(&this->omniMutex, &waitStatus) == 0 and waitStatus < 0) { // free all waiting threads
        sem_post(&this->omniMutex);
    }
    while(sem_getvalue(&this->addState, &waitStatus) == 0 and waitStatus < 0) { // free all waiting threads
        sem_post(&this->addState);
    }


}

vector<SliceType> Buffet::TakeAny(const int desired) {
    if (desired <= 0 or desired > this->buffetMax)
        throw new invalid_argument("count Input MUST be > 0");
    if (this->buffetClosed)
        throw new runtime_error("ERROR: buffet closed");

    int vegStatus;
    if (sem_getvalue(&this->vegMutex, &vegStatus) == 0 and vegStatus < 0){
        return TakeNonVeg(desired); // call non-Veg method; still induces wait for slice takes
    }
        /// No vegetarians waiting yet -- proceed with logic to search/take slices atomically
    else {
        vector<SliceType> plate;       // slices being atomically taken
        vector<int> sliceIndexes;      // index of slices to be taken

        while(sliceIndexes.size() < desired and !this->buffetClosed) {
            if (sem_getvalue(&this->vegMutex, &vegStatus) == 0 and vegStatus < 0) {
                return TakeNonVeg(desired); // call non-Veg method; still induces wait for slice takes
            }

            sem_wait(&this->omniMutex); // lock omnivores and buffet take state
            sem_wait(&this->buffetState);

            plate.clear();       // clear plate and the indexes the thread is watching
            sliceIndexes.clear();

            for (int i = 0; i < buffet.size() and plate.size() < desired; i++) {
                sliceIndexes.push_back(i); // add the index and the pizza to the plate; removed if non-atomic
                plate.push_back(this->buffet.at(i));
            }
            if (sliceIndexes.size() < desired) { // let other threads go if this one can't
                sem_post(&this->buffetState);
                sem_post(&this->omniMutex); // lock omnivores and buffet take state
            }
        }
        if (this->buffetClosed)
            throw new runtime_error("ERROR: buffet closed");
        return getSliceTypes(plate, sliceIndexes, &this->omniMutex);
    }
}

vector<SliceType> Buffet::TakeNonVeg(const int desired) {
    vector<SliceType> plate;       // slices being atomically taken
    vector<int> sliceIndexes;      // index of slices to be taken

    while(sliceIndexes.size() < desired and !this->buffetClosed) {
        sem_wait(&this->omniMutex); // lock omnivores and buffet take state
        sem_wait(&this->buffetState);

        plate.clear();       // clear plate and the indexes the thread is watching
        sliceIndexes.clear();

        for (int i = 0; i < buffet.size() && plate.size() < desired; i++) {
            if (this->buffet.at(i) == Meat or this->buffet.at(i) == Works) {
                sliceIndexes.push_back(i);
                plate.push_back(this->buffet.at(i));
            }
        }
        if (sliceIndexes.size() < desired) { // let other threads go if this one can't
            sem_post(&this->buffetState);
            sem_post(&this->omniMutex); // lock omnivores and buffet take state
        }
    }
    if (this->buffetClosed)
        throw new runtime_error("ERROR: buffet closed");
    return getSliceTypes(plate, sliceIndexes, &this->omniMutex);
}

vector<SliceType> Buffet::TakeVeg(const int desired) {
    if (desired <= 0 or desired > this->buffetMax)
        throw new invalid_argument("count Input MUST be > 0");
    if (this->buffetClosed)
        throw new runtime_error("ERROR: buffet closed");

    vector<SliceType> plate;       // slices being atomically taken
    vector<int> sliceIndexes;      // index of slices to be taken

    while(sliceIndexes.size() < desired and !this->buffetClosed) {
        sem_wait(&this->vegMutex); // lock omnivores and buffet take state
        sem_wait(&this->buffetState);

        plate.clear();       // clear plate and the indexes the thread is watching
        sliceIndexes.clear();

        for (int i = 0; i < buffet.size() && plate.size() < desired; i++) {
            if (this->buffet.at(i) == Cheese or this->buffet.at(i) == Veggie) {
                sliceIndexes.push_back(i);
                plate.push_back(this->buffet.at(i));
            }
        }
        if (sliceIndexes.size() < desired) { // let other threads go if this one can't
            sem_post(&this->buffetState);
            sem_post(&this->vegMutex); // lock omnivores and buffet take state
        }
    }
    if (this->buffetClosed)
        throw new runtime_error("ERROR: buffet closed");
    return getSliceTypes(plate, sliceIndexes, &this->vegMutex);
}

vector<SliceType> Buffet::getSliceTypes(vector<SliceType> plate, vector<int> sliceIndexes, sem_t *sem) {
    for (int i = sliceIndexes.size() - 1; i >= 0; i--) {
        this->buffet.erase(this->buffet.begin() + sliceIndexes.at(i));
    }
    sem_post(&this->buffetState);// unlock any consumers currently waiting to browse the buffet
    sem_post(sem);   // allow the next veg consumer into buffet
    return plate;
}


