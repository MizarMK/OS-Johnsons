#ifndef BUFFET_H
#define BUFFET_H

#include <vector>
#include <stdexcept>
#include <semaphore.h>
#include <mutex>              // std::mutex, std::unique_lock
#include <functional>
#include <condition_variable> // std::condition_variable
using namespace std;

/**
 * Type of pizza slice
 */
    enum SliceType {
        Veggie,
        Cheese,
        Meat,
        Works,
    };


/**
 * Class to manage pizza buffet
 * \version 1.1
 */
    class Buffet
    {
    private:
        /** max number of slices in buffet */
        int buffetMax;
	
        /** count number of veg slices on buffet */
        int vegSlices;

        /** count waiting veg threads */
        int vegetariansWaiting;

        /** list of pizza slice types*/
        vector<SliceType> buffet;

        /** list tracking requests of unsatisfied veg threads */
        vector<int> vegRequests;

        /** list of corresponding locks for unsatisfied veg threads */
        vector<sem_t*> blockedVegs;

        /** list tracking requests of unsatisfied omni threads */
        vector<int> omniRequests;

        /** list of corresponding locks for unsatisfied omni threads */
        vector<sem_t*> blockedOmnis;

        /** boolean dictating if buffet has closed */
        bool buffetClosed;

        /** mutex lock for adding slices to buffet */
        sem_t addState;

        /** mutex lock for removing slices from buffet */
        sem_t buffetState;

        /** lock to prevent multiple vegetarians from seeking slices simultaneously */
        sem_t vegMutex;

        /** lock to prevent multiple omnivores from seeking slices simultaneously */
        sem_t omniMutex;

        /** lock for full buffet */
        sem_t fullBlock;

        /** lock for empty buffet */
        sem_t emptyBlock;

        /** lock for blocking vegs on incomplete takes */
        sem_t vegBlock;

        /** lock for blocking omnivores on incomplete takes */
        sem_t omniBlock;

        /** mutex lock for adding slices to buffet */
        mutex _addState;

        /** mutex lock for removing slices from buffet */
        mutex _buffetState;

        /** lock to prevent multiple vegetarians from seeking slices simultaneously */
        mutex _vegMutex;

        /** lock to prevent multiple omnivores from seeking slices simultaneously */
        mutex _omniMutex;

        /** condition variable for blocking producers if buffet is full */
        condition_variable buffetFull;

        /** condition variable for blocking consumers if buffet is empty */
        condition_variable buffetEmpty;


    public:
        /**
         * Construct buffet for maximum given pizza slices
         * \param maxSlices max slices on buffet (non-negative)
         * \throws invalid_argument if maxSlices < 0
         */
    Buffet(const int maxSlices = 20);

    /**
     * Free any buffet resources
     */
    ~Buffet();

    bool isBuffetFull();

    bool isBuffetEmpty();

    /** integer conveying how many vegetarians are currently blocked */
    int vegetariansBlocked;

    /** integer conveying how many omnis are currently blocked */
    int omnisBlocked;

    /** integer conveying how many threads are currently blocked on empty buffet */
    int blockOnEmpty;

    void removeRequest(SliceType type, int desired);

    bool satisfyVeg(int desired);

    bool satisfyOmni(int desired);

    void producerRelease();

    /**
     * Called by non-vegetarian patrons to request a desired number of
     * slides and return a list of slices, oldest slice first. This
     * method will block callers until they can have all the
     * desired slices; it won't take any slices from the buffet until
     * they can get everything they want. If a vegetarian is waiting
     * for slices, this will only take non-vegetarian slices (slices
     * other than cheese and veggie). Otherwise, this method will
     * give them any type of slice. If the buffet is closed, this
     * method will throw an exception.
     * \param desired number of desired slices
     * \return list of slices
     * \throws invalid_argument if desired < 0 or desired > maxSlices
     * \throws runtime_error if buffet closed
     */
    vector<SliceType> TakeAny(const int desired);

    /**
     * Called by non-vegetarian patrons in the instance that a vegetarian is
     * waiting.to request a desired number of
     * slides and return a list of slices, oldest slice first. This
     * method will block (Non-Veg) callers until they can have all the
     * desired slices; it won't take any slices from the buffet until
     * they can get everything they want. If the buffet is closed, this
     * method will throw an exception.
     * \param desired number of desired slices
     * \return list of slices
     * \throws invalid_argument if desired < 0 or desired > maxSlices
     * \throws runtime_error if buffet closed
     */
    vector<SliceType> TakeNonVeg(const int desired);

    /**
     * This method is like takeAny(), but it returns the oldest
     * vegetarian slices in the slice list. As with takeAny(), 
     * it will block the caller until they can have all
     * the slices they want.  Once the buffet is closed, this method
     * will throw an exception.
     * \param desired number of desired slices
     * \return list of slices
     * \throws invalid_argument if desired < 0 or desired > maxSlices
     * \throws runtime_error if buffet is closed
     */
    vector<SliceType> TakeVeg(const int desired);

    /**
     * gets slicetypes from specific indexes, clears buffet, and unlocks mutex
     * @param plate vector to add slices to
     * @param sliceIndexes indexes of slices to add
     * @param sem mutex to clear
     * @return list of slices to consumer thread
     */
    vector<SliceType> getSliceTypes(vector<SliceType> plate, vector<int> sliceIndexes, sem_t *sem);

    /**
     * gets slicetypes from specific indexes, clears buffet, and unlocks mutex
     * @param plate vector to add slices to
     * @param sliceIndexes indexes of slices to add
     * @param sem mutex to clear
     * @return list of slices to consumer thread
     */
    vector<SliceType> getSliceType(vector<SliceType> plate, vector<int> sliceIndexes, int type);


        /**
         * This method adds count slices, all of the given slice type,
         * stype, to the buffet. The buffet only has a finite capacity of
         * slices (given to constructor). If there isn't enough room for
         * all the slices, it will add as many as it can to the buffet and
         * then block the calling thread until there's room for more.
         * (Note that this is different from the behavior of takeAny() an
         * takeVeg(), where you don't take any slices until you can have
         * all of them.) This method immediately returns false when the
         * buffet is closed.
         * \param count number of slices to add to buffet
         * \param stype type of slices to add to buffet
         * \return true if slices were added or false if buffet closed
         * \throws invalid_argument if count < 0
         */
    bool AddPizza(const int count, const SliceType stype);

    /**
     * This function marks the buffet as closed, causing threads
     * blocked in takeAny(), takeVeg() and addPizza() to immediately
     * return false (and threads subsequently calling these function
     * to immediately return).
     */
    void close();
};
 
#endif
