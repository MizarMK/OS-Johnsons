/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 5
 * Class:       CSI 4337
 ************************************************/
#include "buffet.h"
#include <iostream>

const int OMNITYPE = 0;
const int VEGTYPE = 1;

using namespace std;
using namespace placeholders;

Buffet::Buffet(const int maxSlices) {
    if (maxSlices < 0)
        throw (invalid_argument("maxSlices Input MUST be > 0"));

    this->buffetMax = maxSlices;
    this->vegetariansWaiting = 0;
    this->buffetClosed = false;
}

Buffet::~Buffet() {
    close();
    this->buffet.clear();
}

bool Buffet::isBuffetFull() {
    return this->buffet.size() < (unsigned)this->buffetMax or buffetClosed;
}

bool Buffet::isBuffetEmpty() {
    return !this->buffet.empty() or buffetClosed;
}

bool Buffet::AddPizza(const int count, const SliceType stype) {
    unique_lock<mutex> aState{this->_addState, defer_lock};
    if (count <= 0)
        throw(invalid_argument("count Input MUST be > 0"));
    if (!this->buffetClosed) {
        aState.lock(); // acquire lock first before potential wait
        for (int i = 0; i < count; i++) {
            // while (this->buffet.size() == buffetMax && !this->buffetClosed) {} // SpinLock for maximum Capacity
	    this->buffetFull.wait(aState, bind(&Buffet::isBuffetFull, this)); // SpinLock for full Capacity
            if (this->buffetClosed) {
		aState.unlock();
		this->buffetFull.notify_one(); // release blocked prod thread
                return false;
            }
            _buffetState.lock();
            this->buffet.push_back(stype);
            _buffetState.unlock();
        }
        // sem_post(&this->_addState);
        aState.unlock();
        this->buffetEmpty.notify_one(); // release blocked consumer thread to let them know they can eat (if any)
	this->buffetFull.notify_one(); // release blocked prudcer thread to let them know they can add (if any)
	return true;
    }
    aState.lock();
    this->buffetEmpty.notify_all();
    aState.unlock();
    return false;
}

void Buffet::close() {
    this->buffetClosed = true; // have all idling threads in other method return false via dependency on _buffetStatus
    buffetEmpty.notify_all();
    buffetFull.notify_all();
    /// FIXME free all waiting threads
}

vector<SliceType> Buffet::TakeAny(const int desired) {
    if (desired <= 0 or desired > this->buffetMax)
        throw(invalid_argument("count Input MUST be > 0"));
    if (this->buffetClosed)
        throw (runtime_error("ERROR: buffet closed"));

    unique_lock<mutex> oState{this->_omniMutex, defer_lock};
    unique_lock<mutex> bState{this->_buffetState, defer_lock};

    vector<SliceType> plate;       // slices being atomically taken
    vector<int> sliceIndexes;      // index of slices to be taken
        /// No vegetarians waiting yet -- proceed with logic to search/take slices atomically
	oState.lock(); // initially acquire mutex for potential wait
    while(sliceIndexes.size() < (unsigned)desired and !this->buffetClosed) {
    // sem_wait(&this->_omniMutex); // lock omnivores and buffet take state
    // sem_wait(&this->_buffetState);
    this->buffetEmpty.wait(oState, bind(&Buffet::isBuffetEmpty, this)); // SpinLock for empty Capacity
    if (this->buffetClosed) {
            oState.unlock();
            this->buffetEmpty.notify_one(); // release blocked prod thread to enter close cycle
        throw (runtime_error("ERROR: buffet closed"));
    }
        bState.lock();

        plate.clear();       // clear plate and the indexes the thread is watching
        sliceIndexes.clear();

        //cout << plate.size() << " -- "<< buffet.size() << endl;
        for (unsigned int i = 0; i < buffet.size() and plate.size() < (unsigned)desired; i++) {
            if (vegetariansWaiting > 0) {
                if (this->buffet.at(i) == Meat or this->buffet.at(i) == Works) {
                    sliceIndexes.push_back(i);
                    plate.push_back(this->buffet.at(i));
                }
            }
            else {
                sliceIndexes.push_back(i); // add the index and the pizza to the plate; removed if non-atomic
                plate.push_back(this->buffet.at(i));
            }
        }
        //cout << desired << " -- " << plate.size() << endl;
        if (sliceIndexes.size() < (unsigned)desired) { // let other threads go if this one can't
            // sem_post(&this->_buffetState);
            // sem_post(&this->_omniMutex); // lock omnivores and buffet take state
            bState.unlock();
            this->buffetEmpty.wait(oState);
        }
    }
    if (this->buffetClosed)
        throw (runtime_error("ERROR: buffet closed"));
    return getSliceType(plate, sliceIndexes, OMNITYPE);
}

vector<SliceType> Buffet::TakeVeg(const int desired) {
    if (desired < 0 or desired > this->buffetMax)
        throw(invalid_argument("count Input MUST be > 0"));
    if (this->buffetClosed)
        throw (runtime_error("ERROR: buffet closed"));

    unique_lock<mutex> vState{this->_vegMutex, defer_lock};
    unique_lock<mutex> bState{this->_buffetState, defer_lock};

    vector<SliceType> plate;       // slices being atomically taken
    vector<int> sliceIndexes;      // index of slices to be taken

    vegetariansWaiting++;
    vState.lock();
    int k = 0;
    while(sliceIndexes.size() < (unsigned)desired and !this->buffetClosed) {
        // sem_wait(&this->_vegMutex); // lock omnivores and buffet take state
        // sem_wait(&this->_buffetState);
        if (k == 0)
            this->buffetEmpty.wait(vState, bind(&Buffet::isBuffetEmpty, this)); // SpinLock for empty Capacity
	    if (this->buffetClosed) {
            vState.unlock();
            this->buffetEmpty.notify_one(); // release blocked prudcer thread to let them know they can add (if any)
        }
        bState.lock();

        plate.clear();       // clear plate and the indexes the thread is watching
        sliceIndexes.clear();

        for (unsigned int i = 0; i < buffet.size() and plate.size() < (unsigned)desired; i++) {
            if (this->buffet.at(i) == Cheese or this->buffet.at(i) == Veggie) {
                sliceIndexes.push_back(i);
                plate.push_back(this->buffet.at(i));
            }
        }
        if (sliceIndexes.size() < (unsigned)desired) { // let other threads go if this one can't
            // sem_post(&this->_buffetState);
            // sem_post(&this->_vegMutex); // lock omnivores and buffet take state
            bState.unlock();
	        this->buffetEmpty.wait(vState);
        }
    }
    if (this->buffetClosed) {
        throw (runtime_error("ERROR: buffet closed"));
    }
    vegetariansWaiting--;
    return getSliceType(plate, sliceIndexes, VEGTYPE);
}

vector<SliceType> Buffet::getSliceType(vector<SliceType> plate, vector<int> sliceIndexes, int type) {
    //cout << "found enough slices!" << endl;
    //cout << "plate: " << plate.size() << endl;
    //cout << "buffet: " << this->buffet.size() << endl;
    for (int i = sliceIndexes.size() - 1; i >= 0; i--) {
        this->buffet.erase(this->buffet.begin() + sliceIndexes.at(i));
    }
    // cout << "done eating (" << plate.at(0) << ")!" << endl;
    // sem_post(&this->_buffetState);// unlock any consumers currently waiting to browse the buffet
    // sem_post(sem);   // allow the next veg consumer into buffet
    this->_buffetState.unlock();
    if (type == OMNITYPE)
        this->_omniMutex.unlock();
    else if (type == VEGTYPE)
        this->_vegMutex.unlock();
    this->buffetEmpty.notify_one();
    this->buffetFull.notify_one();
    return plate;
}
