#ifndef P3_BUFFETMUTEX_H
#define P3_BUFFETMUTEX_H

#include <vector>
#include <stdexcept>
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

    /** list of pizza slice types*/
    vector<SliceType> buffet;

    /** boolean dictating if buffet has closed */
    bool buffetClosed;

    /** mutex lock for adding slices to buffet */
    std::mutex addState;

    /** mutex lock for removing slices from buffet */
    std::mutex buffetState;

    /** lock to prevent multiple vegetarians from seeking slices simultaneously */
    std::mutex vegMutex;

    /** lock to prevent multiple omnivores from seeking slices simultaneously */
    std::mutex omniMutex;

    /** condition variable for blocking producers if buffet is full */
    std::condition_variable buffetFull;

    /** condition variable for blocking consumers if buffet is empty */
    std::condition_variable buffetEmpty;


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

    /** integer conveying how many vegetarians are currently waiting to take slices */
    int vegetariansWaiting;

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
    vector<SliceType> getSliceTypes(vector<SliceType> plate, vector<int> sliceIndexes, int type);


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


#endif //P3_BUFFETMUTEX_H
