/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleHashtable_h
#define MuscleHashtable_h

#include "support/MuscleSupport.h"

#ifdef _MSC_VER
# pragma warning(disable: 4786)
#endif

#ifndef MUSCLE_HASHTABLE_DEFAULT_CAPACITY
# define MUSCLE_HASHTABLE_DEFAULT_CAPACITY 7
#endif

BEGIN_NAMESPACE(muscle);

// implementation detail; please ignore this!
static const uint32 MUSCLE_HASHTABLE_INVALID_HASH_CODE = (uint32)-1;

/** This hashing functor type handles the "easy" cases, where the KeyType is
 *  something that can be static_cast into a uint32 by the compiler.
 *  For more complicated key types, you'll need to define your own 
 *  specialization of this functor template.  (See util/String.h for an example of this)
 */
template <class T> class HashFunctor
{
public:
   /** Default implementation just casts the key to a uint32 to get the hash value. */
   uint32 operator () (const T x) const {return (uint32) x;}
};

template <class KeyType, class ValueType, class HashFunctorType> class Hashtable;  // forward declaration

/**
 * This class is an iterator object, used for iterating over the set
 * of keys or values in a Hashtable.  Note that the Hashtable
 * maintains the ordering of keys and values, unlike many hash table
 * implementations.
 *
 * Given a Hashtable object, you can obtain one or more of these
 * iterator objects by calling the Hashtable's GetIterator() method.
 *
 * This iterator actually contains separate state for two iterations:
 * one for iterating over the values in the Hashtable, and one for 
 * iterating over the keys.  These two iterations can be done independently
 * of each other.
 *
 * It is safe to modify or delete a hashtable during a traversal; the active iterators
 * will be automatically notified so that they do the right thing.
 */
template <class KeyType, class ValueType, class HashFunctorType = HashFunctor<KeyType> > class HashtableIterator
{
public:
   /**
    * Default constructor.  It's here only so that you can include HashtableIterators
    * as member variables, in arrays, etc.  HashtableIterators created with this
    * constructor are "empty", so they won't be useful until you set them equal to a 
    * HashtableIterator that was returned by Hashtable::GetIterator().
    */
   HashtableIterator();

   /** Copy Constructor. */
   HashtableIterator(const HashtableIterator<KeyType, ValueType, HashFunctorType> & rhs);

   /** Convenience Constructor -- makes an iterator equivalent to the value returned by table.GetIterator().  */
   HashtableIterator(const Hashtable<KeyType, ValueType, HashFunctorType> & table, bool backwards = false);

   /** Convenience Constructor -- makes an iterator equivalent to the value returned by table.GetIteratorAt().  */
   HashtableIterator(const Hashtable<KeyType, ValueType, HashFunctorType> & table, const KeyType & startAt, bool backwards);

   /** Destructor */
   ~HashtableIterator();

   /** Assignment operator. */
   HashtableIterator<KeyType, ValueType, HashFunctorType> & operator=(const HashtableIterator<KeyType, ValueType, HashFunctorType> & rhs);

   /** Returns true iff there are more keys left in the key traversal.  */
   bool HasMoreKeys() const {return (_nextKeyCookie != NULL);}

   /**
    * Gets the next key in the key traversal.
    * @param setNextKey On success, the next key is copied into this object.
    * @return B_NO_ERROR on success, B_ERROR if there are no more keys left.
    */
   status_t GetNextKey(KeyType & setNextKey);

   /**
    * Gets a pointer to the next key in the key traversal.
    * Note that the returned pointer is only guaranteed valid as long as the Hashtable remains unchanged.
    * @return A pointer to the next key in the key traversal, or NULL if there are no more keys left.
    */
   const KeyType * GetNextKey();

   /**
    * Peek at the next key without modifying the state of the traversal.
    * @param setKey On success, the next key is copied into this object.
    * @return B_NO_ERROR on success, or B_ERROR if there are no more keys left.
    */
   status_t PeekNextKey(KeyType & setKey) const;

   /**
    * Peek at the next key without modifying the state of the traversal.
    * Note that the returned pointer is only guaranteed valid as long as the Hashtable remains unchanged.
    * @return a pointer to the next key in the key traversal, or NULL if there are no keys left.
    */
   const KeyType * PeekNextKey() const;

   /** Returns true iff there are more values left in the value traversal.  */
   bool HasMoreValues() const {return (_nextValueCookie != NULL);}

   /**
    * Get the next value in the values traversal.
    * @param setNextValue On success, the next value in the traversal is copied into this object.
    * @return B_NO_ERROR on success, B_ERROR if there are no more values left in the value traversal.
    */
   status_t GetNextValue(ValueType & setNextValue);

   /**
    * Get the next value in the values traversal.
    * Note that the returned pointer is only guaranteed valid as long as the Hashtable remains unchanged.
    * @return a pointer to the next value in the value traversal, or NULL if there are no values left.
    */
   ValueType * GetNextValue();

   /**
    * Peek at the next value without modifying the state of the traversal.
    * @param setValue On success, the next value is copied into this object.
    * @return B_NO_ERROR on success, or B_ERROR if there are no more values left.
    */
   status_t PeekNextValue(ValueType & setValue) const;

   /**
    * Peek at the next value without modifying the state of the traversal.
    * Note that the returned pointer is only guaranteed valid as long as the Hashtable remains unchanged.
    * @return a pointer to the next value in the value traversal, or NULL if there are no values left.
    */
   ValueType * PeekNextValue() const;

   /** Convenience method -- equivalent to calling both GetNextKey() and GetNextValue(). 
     * @param setKey on success, this parameter will contain the next key in the iteration.
     * @param setValue on success, this parameter will contain the next value in the iteration.
     * @returns B_NO_ERROR if values were written into (setKey) and (setValue), or B_ERROR if the iteration is complete.
     */
   status_t GetNextKeyAndValue(KeyType & setKey, ValueType & setValue);

   /** Convenience method -- equivalent to calling both GetNextKey() and GetNextValue(). 
     * @param setKey on success, this parameter will contain the next key in the iteration.
     * @param setValuePtr on success, this pointer will point to the next value in the iteration.
     * @returns B_NO_ERROR if values were written into (setKey) and (setValue), or B_ERROR if the iteration is complete.
     */
   status_t GetNextKeyAndValue(KeyType & setKey, ValueType * & setValuePtr);

   /** Convenience method -- equivalent to calling both GetNextKey() and GetNextValue(). 
     * @param setKey on success, this parameter will contain the next key in the iteration.
     * @param setValuePtr on success, this pointer will point to the next value in the iteration.
     * @returns B_NO_ERROR if values were written into (setKey) and (setValue), or B_ERROR if the iteration is complete.
     */
   status_t GetNextKeyAndValue(KeyType & setKey, const ValueType * & setValuePtr);

   /** Convenience method -- equivalent to calling both GetNextKey() and GetNextValue(). 
     * @param setKeyPtr on success, this pointer will point to the next key in the iteration.
     * @param setValue on success, this parameter will contain the next value in the iteration.
     * @returns B_NO_ERROR if values were written into (setKey) and (setValue), or B_ERROR if the iteration is complete.
     */
   status_t GetNextKeyAndValue(const KeyType * & setKeyPtr, ValueType & setValue);

   /** Convenience method -- equivalent to calling both GetNextKey() and GetNextValue(). 
     * @param setKeyPtr on success, this pointer will point to the next key in the iteration.
     * @param setValuePtr on success, this pointer will point to the next value in the iteration.
     * @returns B_NO_ERROR if values were written into (setKey) and (setValue), or B_ERROR if the iteration is complete.
     */
   status_t GetNextKeyAndValue(const KeyType * & setKeyPtr, ValueType * & setValuePtr);

   /** Convenience method -- equivalent to calling both GetNextKey() and GetNextValue(). 
     * @param setKeyPtr on success, this pointer will point to the next key in the iteration.
     * @param setValuePtr on success, this pointer will point to the next value in the iteration.
     * @returns B_NO_ERROR if values were written into (setKey) and (setValue), or B_ERROR if the iteration is complete.
     */
   status_t GetNextKeyAndValue(const KeyType * & setKeyPtr, const ValueType * & setValuePtr);

private:
   void SetOwner(const Hashtable<KeyType,ValueType,HashFunctorType> * optTable);

   /** The Hashtable class needs access to our internals to do the iteration operation */
   friend class Hashtable<KeyType, ValueType, HashFunctorType>;

   void * _scratchSpace[2];   // ignore this; it's temp scratch space used by EnsureSize().

   void * _nextKeyCookie;
   void * _nextValueCookie;
   bool _backwards;

   HashtableIterator<KeyType, ValueType, HashFunctorType> * _prevIter;  // for the doubly linked list so that the table can notify us if it is modified
   HashtableIterator<KeyType, ValueType, HashFunctorType> * _nextIter;  // for the doubly linked list so that the table can notify us if it is modified
   const Hashtable<KeyType, ValueType, HashFunctorType>   * _owner;     // table that we are associated with
};

/**
 *  This is a handy templated Hashtable class, rather similar to Java's java.util.Hashtable,
 *  but with a number of enhancements.  Enhancements include:  Iterator objects that are
 *  safe to use even if the table is modified or deleted during a traversal (not thread-safe though),
 *  traversal ordering is maintained at all times and may optionally be sorted or auto-sorted,
 *  and the traversal order may be manually changed as well.
 * 
 *  The Hashtable class uses a HashFunctor template argument to specify how the hash code for
 *  a given KeyType object can be computed.  For built-in numeric types (e.g. int, long, etc)
 *  the default HashFunctor can be used.  For more complex types (e.g. class objects), you will
 *  need to either create your own custom HashFunctor, or add a () operator to your class that
 *  returns a uint32 HashCode.  The best way to do a custom HashFunctor is to specialize the
 *  default HashFunctor type for your class;  check the example in util/String.h for how to do this.
 */
template <class KeyType, class ValueType, class HashFunctorType = HashFunctor<KeyType> > class Hashtable
{
public:
   /** This is the signature of the type of callback function that you can pass into the
     * SetKeyCompareFunction() and SortByKey() methods.  The function should work like strcmp(),
     * return a negative value if (key1) is less than (key2), 0 if the two keys are equal,
     * or a positive value if (key1) is greater than (key2).
     * @param key1 The first key.
     * @param key2 The second key.
     * @param cookie A user-defined value that was passed in to the Sort() or SetCompareCookie() method.
     * @return An integer indicating which key is "larger", as defined above.
     */
   typedef int (*KeyCompareFunc)(const KeyType& key1, const KeyType& key2, void * cookie);

   /** This is the signature of the type of callback function that you can pass into the
     * SetValueCompareFunction() and SortByValue() methods.  The function should work like strcmp(),
     * return a negative value if (value1) is less than (value2), 0 if the two values are equal,
     * or a positive value if (value1) is greater than (value2).
     * @param value1 The first value.
     * @param value2 The second value.
     * @param cookie A user-defined value that was passed in to the Sort() or SetCompareCookie() method.
     * @return An integer indicating which value is "larger", as defined above.
     */
   typedef int (*ValueCompareFunc)(const ValueType& key1, const ValueType& key2, void * cookie);

   /** Tokens for the various auto-sort modes we know how to do. */
   enum {
      AUTOSORT_DISABLED = 0,
      AUTOSORT_BY_KEY,
      AUTOSORT_BY_VALUE
   };

   /** This constructor creates a standard, non-sorting Hashtable.
    *  @param initialCapacity Specifies the number of table slots to initially pre-allocate.   Defaults to 7.
    */
   Hashtable(uint32 initialCapacity = MUSCLE_HASHTABLE_DEFAULT_CAPACITY);

   /** Copy Constructor.  */
   Hashtable(const Hashtable<KeyType,ValueType,HashFunctorType> & rhs);

   /** Assignment operator. */
   Hashtable<KeyType,ValueType,HashFunctorType> & operator= (const Hashtable<KeyType,ValueType,HashFunctorType> & rhs);

   /** Equality operator.  Returns true iff both hash tables contains the same set of keys and values.
     * Note that the ordering of the keys is NOT taken into account!
     */
   bool operator== (const Hashtable<KeyType,ValueType,HashFunctorType> & rhs) const;

   /** Returns the opposite of the equality operator. */
   bool operator!= (const Hashtable<KeyType,ValueType,HashFunctorType> & rhs) const {return !(*this == rhs);}

   /** Destructor. */
   ~Hashtable();

   /** Returns the number of items stored in the table. */
   uint32 GetNumItems() const {return _count;}

   /** Convenience method;  Returns true iff the table is empty (i.e. if GetNumItems() is zero). */
   bool IsEmpty() const {return _count == 0;}

   /** Returns true iff the table contains a mapping with the given key.  (O(1) search time) */
   bool ContainsKey(const KeyType& key) const {return ((_count>0)&&(GetEntry(ComputeHash(key), key) != NULL));}

   /** Returns true iff the table contains a mapping with the given value.  (O(n) search time) */
   bool ContainsValue(const ValueType& value) const;

   /** Returns the given key's position in the hashtable's linked list, or -1 if the key wasn't found.  O(n) count time (if the key exists, O(1) if it doesn't) */
   int32 IndexOfKey(const KeyType& key) const;

   /** Attempts to retrieve the associated value from the table for a given key.  (O(1) lookup time)
    *  @param key The key to use to look up a value.
    *  @param setValue On success, the associated value is copied into this object.
    *  @return B_NO_ERROR on success, B_ERROR if their was no value found for the given key.
    */
   status_t GetValue(const KeyType& key, ValueType& setValue) const; 

   /** Retrieve a pointer to the associated value object for the given key.  (O(1) lookup time)
    *  @param key The key to use to look up a value.
    *  @return A pointer to the internally held value object for the given key,
    *          or NULL of no object was found.  Note that this object is only
    *          guaranteed to remain valid as long as the Hashtable remains unchanged.
    */
   ValueType * GetValue(const KeyType & key) const;

   /** Given a lookup key, returns the a copy of the actual key as held by the table.
    *  This method is only useful in rare situations where the hashing or comparison
    *  functions are such that lookupKeys and held keys are not guaranteed equivalent.
    *  @param lookupKey The key used to look up the held key object.
    *  @param setKey On success, the actual key held in the table is placed here.
    *  @return B_NO_ERROR on success, or B_ERROR on failure.
    */
   status_t GetKey(const KeyType & lookupKey, KeyType & setKey) const;

   /** Given a key, returns a pointer to the actual key object in this table that matches
    *  that key, or NULL if there is no matching key.  This method is only useful in rare
    *  situations where the hashing or comparison functions are such that lookup keys and
    *  held keys are not guaranteed equivalent.
    *  @param lookupKey The key used to look up the key object
    *  @return A pointer to an internally held key object on success, or NULL on failure.
    */
   const KeyType * GetKey(const KeyType & lookupKey) const;

   /** Get an iterator for use with this table.
     * @param backwards Set this to true if you want to iterate through the item list backwards.  Defaults to false.
     * @return an iterator object that can be used to examine the items in the hash table. 
     */
   HashtableIterator<KeyType,ValueType,HashFunctorType> GetIterator(bool backwards = false) const 
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> ret; 
      InitializeIterator(ret, backwards);
      return ret;
   }

   /** Get an iterator for use with this table, starting at the given entry.
     * @param startAt The key in this table to start the iteration at.
     * @param backwards Set this to true if you want to iterate through the item list backwards.  Defaults to false.
     * @return an iterator object that can be used to examine the items in the hash table, starting at
     *         the specified key.  If the specified key is not in this table, an empty iterator will be returned.
     */
   HashtableIterator<KeyType,ValueType,HashFunctorType> GetIteratorAt(const KeyType & startAt, bool backwards = false) const 
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> ret;
      InitializeIteratorAt(ret, startAt, backwards);
      return ret;
   }

   /** Returns a pointer to the (index)'th key in this Hashtable.
    *  (This Hashtable class keeps its entries in a well-defined order)
    *  Note that this method is an O(N) operation, so for iteration, always use GetIterator() instead.
    *  @param index Index of the key to return a pointer to.  Should be in the range [0, GetNumItems()-1].
    *  @returns Pointer to the key at position (index) on success, or NULL on failure (bad index?)
    */
   const KeyType * GetKeyAt(uint32 index) const;

   /** Returns the (index)'th key in this Hashtable.
    *  (This Hashtable class keeps its entries in a well-defined order)
    *  Note that this method is an O(N) operation, so for iteration, always use GetIterator() instead.
    *  @param index Index of the key to return a pointer to.  Should be in the range [0, GetNumItems()-1].
    *  @param retKey On success, the contents of the (index)'th key will be written into this object.
    *  @return B_NO_ERROR on success, or B_ERROR on failure.
    */
   status_t GetKeyAt(uint32 index, KeyType & retKey) const;

   /** Places the given (key, value) mapping into the table.  Any previous entry with a key of (key) will be replaced.  
    *  (average O(1) insertion time, unless auto-sorting is enabled, in which case it becomes O(N) insertion time for
    *  keys that are not already in the table)
    *  @param key The key that the new value is to be associated with.
    *  @param value The value to associate with the new key.
    *  @param setPreviousValue If there was a previously existing value associated with (key), it will be copied into this object.
    *  @param optSetReplaced If set non-NULL, this boolean will be set to true if (setPreviousValue) was written into, false otherwise.
    *  @return B_NO_ERROR If the operation succeeded, B_ERROR if it failed (out of memory?)
    */
   status_t Put(const KeyType& key, const ValueType& value, ValueType & setPreviousValue, bool * optSetReplaced = NULL) {return (PutAux(ComputeHash(key), key, value, &setPreviousValue, optSetReplaced) != NULL) ? B_NO_ERROR : B_ERROR;}

   /** Places the given (key, value) mapping into the table.  Any previous entry with a key of (key) will be replaced. 
    *  (average O(1) insertion time, unless auto-sorting is enabled, in which case it becomes O(N) insertion time for
    *  keys that are not already in the table)
    *  @param key The key that the new value is to be associated with.
    *  @param value The value to associate with the new key.
    *  @return B_NO_ERROR If the operation succeeded, B_ERROR if it failed (out of memory?)
    */
   status_t Put(const KeyType& key, const ValueType& value) {return (PutAux(ComputeHash(key), key, value, NULL, NULL) != NULL) ? B_NO_ERROR : B_ERROR;}

   /** Removes a mapping from the table.  (O(1) removal time)
    *  @param key The key of the key-value mapping to remove.
    *  @return B_NO_ERROR if a key was found and the mapping removed, or B_ERROR if the key wasn't found.
    */
   status_t Remove(const KeyType& key) {return RemoveAux(key, NULL);}

   /** Removes the mapping with the given (key) and places its value into (setRemovedValue).  (O(1) removal time)
    *  @param key The key of the key-value mapping to remove.
    *  @param setRemovedValue On success, the removed value is copied into this object.
    *  @return B_NO_ERROR if a key was found and the mapping removed, or B_ERROR if the key wasn't found.
    */
   status_t Remove(const KeyType& key, ValueType & setRemovedValue) {return RemoveAux(key, &setRemovedValue);}

   /** Removes all mappings from the hash table.  (O(N) clear time)
    *  @param releaseCachedData If set true, we will immediately free any buffers we may contain.  
    *                           Otherwise, we'll keep them around in case they can be re-used later on.
   */
   void Clear(bool releaseCachedData = false);

   /** This method can be used to activate or deactivate auto-sorting on this Hashtable.
     * If active, auto-sorting ensures that whenever Put() is called, the new/updated item is
     * moved to the correct place in the iterator traversal list.  Note that using auto-sort
     * means that Put() becomes an O(N) operation instead of O(1).
     * Default mode is AUTOSORT_DISABLED.
     * @param mode Either AUTOSORT_DISABLED, AUTOSORT_BY_KEY, or AUTOSORT_BY_VALUE.
     * @param sortNow If true, Sort() will be called to ensure that the table is in a sorted state.
     *                You can avoid an immediate sort by specifying this parameter as false, but be aware 
     *                that auto-sorting won't sort properly if the contents of the table aren't already in
     *                in a correctly sorted state--so you may then need to call Sort() before auto-sorting 
     *                will sort properly.
     */
   void SetAutoSortMode(int mode, bool sortNow = true) {_autoSortMode = mode; if (sortNow) (void) Sort();}

   /** Returns this hash table's current auto-sort mode. */
   int GetAutoSortMode() const {return _autoSortMode;}

   /** This method can be used to set the "cookie value" that will be passed in to the
     * comparison functions specified by SetKeyCompareFunction() and SetValueCompareFunction().
     * This value will be passed along during whenever a user-defined compare function is called implicitely.
     * 
     */
   void SetCompareCookie(void * cookie) {_compareCookie = cookie;}

   /** Returns the current comparison cookie, as previously set by SetCompareCookie().  Default value is NULL. */
   void * GetCompareCookie() const {return _compareCookie;}

   /** You can call this to set a custom function to use to compare keys.  By default, the Key class's '==' operator 
    *  is used to compare keys, and no key sorting is done.  But for some key types (e.g. const char *'s) '==' is not 
    *  terribly useful, and in any case it doesn't handle sorting properly, so if you like you you can supply your 
    *  own key-comparison-function here.
    *  @see KeyCompareFunc for a description of the callback function's required semantics.
    *  @param func The new comparison function to use for this table, or NULL to revert to no key-sorting-function.
    */
   void SetKeyCompareFunction(KeyCompareFunc func) {_userKeyCompareFunc = func;}

   /** Returns the current comparison function used for comparing keys.  Default value is NULL. */
   KeyCompareFunc GetKeyCompareFunction() const {return _userKeyCompareFunc;}

   /** You can call this to set a custom function to use to compare values.  By default, no value sorting is done.
    *  If you wish to enable auto-sort-by-value, you will need to a value-comparison-function here.  
    *  @see ValueCompareFunc for a description of the callback function's required semantics.
    *  @param func The new comparison function to use for this table, or NULL to revert to no value-sorting-function.
    */
   void SetValueCompareFunction(ValueCompareFunc func) {_userValueCompareFunc = func;}

   /** Returns the current comparison function used for comparing values.  Default value is NULL. */
   ValueCompareFunc GetValueCompareFunction() const {return _userValueCompareFunc;}

   /** This method doesn anhefficient zero-copy swap of this hash table's contents with those of (swapMe).  
    *  That is to say, when this method returns, (swapMe) will be identical to the old state of this 
    *  Hashtable, and this Hashtable will be identical to the old state of (swapMe). 
    *  Any active iterators present for either table will swap owners also, becoming associated with the other table.
    *  @param swapMe The table whose contents and iterators are to be swapped with this table's.
    */
   void SwapContents(Hashtable<KeyType,ValueType,HashFunctorType> & swapMe);

   /** Moves the given entry to the head of the HashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved to the front of the sequence.
     * @return B_NO_ERROR on success, or B_ERROR if (moveMe) was not found in the table.
     */
   status_t MoveToFront(const KeyType & moveMe);

   /** Moves the given entry to the tail of the HashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved to the end of the sequence.
     * @return B_NO_ERROR on success, or B_ERROR if (moveMe) was not found in the table.
     */
   status_t MoveToBack(const KeyType & moveMe);

   /** Moves the given entry to the spot just in front of the other specified entry in the 
     * HashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved.
     * @param toBeforeMe Key of the item that (moveMe) should be placed in front of.
     * @return B_NO_ERROR on success, or B_ERROR if (moveMe) was not found in the table, 
     *         or was the same item as (toBeforeMe).
     */
   status_t MoveToBefore(const KeyType & moveMe, const KeyType & toBeforeMe);

   /** Moves the given entry to the spot just behind the other specified entry in the 
     * HashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved.
     * @param toBehindMe Key of the item that (moveMe) should be placed behind.
     * @return B_NO_ERROR on success, or B_ERROR if (moveMe) was not found in the table, 
     *         or was the same item as (toBehindMe).
     */
   status_t MoveToBehind(const KeyType & moveMe, const KeyType & toBehindMe);

   /** Forcefully sorts the iteration traversal list of this table using the preferred sorting mode.
     * The preferred sorting mode will be determined as follows:
     * <ol>
     *  <li>If the auto-sort mode is set, the auto-sort mode will be used.</li>
     *  <li>Else if the sort-by-value callback function is set, sort-by-value will be used.</li>
     *  <li>Else if the sort-by-key callback function is set, sort-by-key will be used.</li>
     *  <li>Else we return B_ERROR.</li>
     * </ol>
     * This method uses a very efficient O(log(N)) MergeSort algorithm.
     * @return B_NO_ERROR on success, or B_ERROR if no sort method could be determined (as described above)
     */
   status_t Sort() {return SortByAux((_autoSortMode != AUTOSORT_BY_VALUE) ? _userKeyCompareFunc : NULL, (_autoSortMode != AUTOSORT_BY_KEY) ? _userValueCompareFunc : NULL, _compareCookie);}

   /** Forcefully sorts the iteration traversal list of this table using the given key comparison function.
     * This method uses a very efficient O(log(N)) MergeSort algorithm.
     * @param func The key-comparison function to use.  (If NULL, this call becomes a no-op)
     * @param optCompareCookie Optional cookie to pass to func().
     */
   void SortByKey(KeyCompareFunc func, void * optCompareCookie = NULL) {(void) SortByAux(func, NULL, optCompareCookie);}

   /** Forcefully sorts the iteration traversal list of this table using the given value comparison function.
     * This method uses a very efficient O(log(N)) MergeSort algorithm.
     * @param func The value-comparison function to use.  (If NULL, this call becomes a no-op)
     * @param optCompareCookie Optional cookie to func().
     */
   void SortByValue(ValueCompareFunc func, void * optCompareCookie = NULL) {(void) SortByAux(NULL, func, optCompareCookie);}

   /** Synonym for GetValue(), retained for compatibility with old code.
    *  @deprecated Use GetValue() instead
    */
   status_t Get(const KeyType& key, ValueType& setValue) const {return GetValue(key, setValue);}

   /** Synonym for GetValue(), retained for compatibility with old code.
    *  @deprecated Use GetValue() instead
    */
   ValueType * Get(const KeyType & key) const {return GetValue(key);}

   /** Convenience method -- returns a pointer to the value specified by (key),
    *  or if no such value exists, it will Put() a (key,value) pair in the Hashtable,
    *  and then return a pointer to the newly-placed value.  Returns NULL on
    *  error only (out of memory?)
    *  @param key The key to look for a value with
    *  @param defValue The value to auto-place in the Hashtable if (key) isn't found.
    *  @returns a Pointer to the retrieved or placed value.
    */
   ValueType * GetOrPut(const KeyType & key, const ValueType & defValue = ValueType())
   {
      ValueType * ret = Get(key);
      if ((ret == NULL)&&(Put(key, defValue) == B_NO_ERROR)) ret = Get(key);
      return ret;
   }

   /** This method resizes the Hashtable larger if necessary, so that it has at least (newTableSize)
    *  entries in it.  It is not necessary to call this method, but if you know in advance how many
    *  items you will be adding to the table, you can make the population of the table more efficient
    *  by calling this method before adding the items.  This method will only grow the table, it will
    *  never shrink it.
    *  @param newTableSize Number of slots that you with to have in the table (note that this is different
    *                      from the number of actual items in the table)
    *  @return B_NO_ERROR on success, or B_ERROR on failure (out of memory?)
    */
   status_t EnsureSize(uint32 newTableSize);

   /** Returns the number of table-slots that we currently have allocated.  Since we often
    *  pre-allocate slots to avoid unnecessary reallocations, this number will usually be
    *  greater than the value returned by GetNumItems().  It will never be less than that value.
    */
   uint32 GetNumAllocatedSlots() const {return _tableSize;}

private:
   /** Our hashtable iterator class needs access to our internals to register itself with us. */
   friend class HashtableIterator<KeyType, ValueType, HashFunctorType>;

   void InitializeIterator(HashtableIterator<KeyType,ValueType,HashFunctorType> & iter, bool backwards) const
   {
      iter.SetOwner(this);
      iter._backwards     = backwards;
      iter._nextKeyCookie = iter._nextValueCookie = (backwards ? _iterTail : _iterHead);
   }

   void InitializeIteratorAt(HashtableIterator<KeyType,ValueType,HashFunctorType> & iter, const KeyType & startAt, bool backwards) const
   {
      iter.SetOwner(this);
      iter._backwards     = backwards;
      iter._nextKeyCookie = iter._nextValueCookie = ((_count>0)?GetEntry(ComputeHash(startAt), startAt):NULL);
   }

   class HashtableEntry
   {
   public:
      // Note:  _bucketPrev and _bucketNext are intentionally not set here
      HashtableEntry() : _hash(MUSCLE_HASHTABLE_INVALID_HASH_CODE), _iterPrev(NULL), _iterNext(NULL)
#ifndef _MSC_VER
                         , _mapTo(this), _mappedFrom(this)  // this is slightly more efficient...
#endif
      {
#ifdef _MSC_VER
         _mapTo      = this;  // ... but VC++ complains if (this) is in the init list, so for VC++ we do it here
         _mappedFrom = this;
#endif
      }

      ~HashtableEntry() {/* empty */}

      // Frees up our memory and returns us to the free-list.
      void ReturnToFreeList(const KeyType & defaultKey, const ValueType & defaultValue, HashtableEntry ** freeHead)
      {
         _hash  = MUSCLE_HASHTABLE_INVALID_HASH_CODE;
         _key   = defaultKey;
         _value = defaultValue;

         _iterPrev = _iterNext = _bucketPrev = NULL;
         _bucketNext = *freeHead; 
         if (_bucketNext) _bucketNext->_bucketPrev = this;
         *freeHead = this;
      }

      /** Removes this entry from the free list, so that we are ready for use.
        * @param freeHead current head of the free list
        * @returns the new head of the free list
        */
      HashtableEntry * RemoveFromFreeList(HashtableEntry * freeHead) 
      {
         if (_bucketNext) _bucketNext->_bucketPrev = _bucketPrev;
         if (_bucketPrev) _bucketPrev->_bucketNext = _bucketNext;
         HashtableEntry * ret = (freeHead == this) ? _bucketNext : freeHead;
         _bucketPrev = _bucketNext = NULL;
         return ret;
      }

      void SwapMaps(HashtableEntry * e)
      {
         muscleSwap(e->_mapTo, _mapTo);
         _mapTo->_mappedFrom = this;
         e->_mapTo->_mappedFrom = e;
      }

      inline HashtableEntry * GetMapTo() const {return _mapTo;}
      inline HashtableEntry * GetMappedFrom() const {return _mappedFrom;}

      static HashtableEntry * CreateEntriesArray(uint32 size)
      {
         HashtableEntry * ret = newnothrow_array(HashtableEntry,size);
         if (ret)
         {
            HashtableEntry * prev = NULL;
            for (uint32 i=0; i<size; i++) 
            {
               HashtableEntry * cur = &ret[i];
               cur->_bucketPrev = prev;
               if (prev) prev->_bucketNext = cur;
               prev = cur;
            }
            ret[size-1]._bucketNext = NULL;
         }
         else WARN_OUT_OF_MEMORY;
         return ret;
      }
  
      uint32 _hash;                // precalculated for efficiency
      KeyType _key;                // used for '==' checking
      ValueType _value;            // payload
      HashtableEntry* _bucketPrev; // for making linked lists in our bucket
      HashtableEntry* _bucketNext; // for making linked lists in our bucket
      HashtableEntry* _iterPrev;   // for user's table iteration
      HashtableEntry* _iterNext;   // for user's table iteration

   private:
      HashtableEntry* _mapTo;
      HashtableEntry* _mappedFrom;
   };

private:
   // Auxilliary methods
   HashtableEntry * PutAux(uint32 hash, const KeyType& key, const ValueType& value, ValueType * optSetPreviousValue, bool * optReplacedFlag);
   status_t RemoveAux(const KeyType& key, ValueType * setRemovedValue);
   status_t SortByAux(KeyCompareFunc optKeyFunc, ValueCompareFunc optValueFunc, void * cookie);

   inline uint32 ComputeHash(const KeyType & key) const
   {
      uint32 ret = _functor(key);
      if (ret == MUSCLE_HASHTABLE_INVALID_HASH_CODE) ret++;  // avoid using the guard value as a hash code (unlikely but possible)
      return ret;
   }

   // Linked list maintainence
   void InsertSortedIterationEntry(HashtableEntry * e, KeyCompareFunc optKeyFunc, ValueCompareFunc optValueCompareFunc, void * cookie);
   void InsertIterationEntry(HashtableEntry * e, HashtableEntry * optBehindThisOne);
   void RemoveIterationEntry(HashtableEntry * e);
   HashtableEntry * GetEntry(uint32 hash, const KeyType& key) const;
   HashtableEntry * GetEntryAt(uint32 idx) const;
   bool IsBucketHead(const HashtableEntry * e) const {return ((e->_hash != MUSCLE_HASHTABLE_INVALID_HASH_CODE)&&(_table[e->_hash%_tableSize].GetMapTo() == e));}

   int CompareEntries(const HashtableEntry & left, const HashtableEntry & right, KeyCompareFunc optKeyFunc, ValueCompareFunc optValueFunc, void * cookie) const;

   // HashtableIterator's private API
   void RegisterIterator(HashtableIterator<KeyType,ValueType,HashFunctorType> * iter) const
   {
      // add him to the head of our linked list of iterators
      iter->_prevIter = NULL;
      iter->_nextIter = _iterList;  
      if (_iterList) _iterList->_prevIter = iter;
      _iterList = iter;
   }

   void UnregisterIterator(HashtableIterator<KeyType,ValueType,HashFunctorType> * iter) const
   {
      if (iter->_prevIter) iter->_prevIter->_nextIter = iter->_nextIter;
      if (iter->_nextIter) iter->_nextIter->_prevIter = iter->_prevIter;
      if (iter == _iterList) _iterList = iter->_nextIter;
      iter->_prevIter = iter->_nextIter = NULL;
   }

   KeyType * GetKeyFromCookie(void * c) const {return c ? &(((HashtableEntry *)c)->_key) : NULL;}
   ValueType * GetValueFromCookie(void * c) const  {return c ? &(((HashtableEntry *)c)->_value) : NULL;}

   void IterateCookie(void ** c, bool backwards) const 
   {
      HashtableEntry * entry = *((HashtableEntry **)c);
      *c = entry ? (backwards ? entry->_iterPrev : entry->_iterNext) : NULL;
   }

   uint32 NextPrime(uint32 start) const;

   const uint32 _initialCapacity;
   uint32 _count;       // the number of valid elements in the hashtable
   uint32 _tableSize;   // the number of entries in _table (or the number to allocate if _table is NULL)

   HashtableEntry * _table;       // our array of table entries
   HashtableEntry * _iterHead;    // start of linked list to iterate through
   HashtableEntry * _iterTail;    // end of linked list to iterate through

   HashtableEntry * _freeHead;    // head of the list of unused HashtableEntries in our _table array

   KeyCompareFunc _userKeyCompareFunc;       // (optional) used to compare two Keys
   ValueCompareFunc _userValueCompareFunc;   // (optional) used to compare two Values
   int _autoSortMode;                        // one of the AUTOSORT_BY_* tokens
   void * _compareCookie;                    // (optional) passed to the CompareFuncs

   HashFunctorType _functor;  // used to compute hash codes for key objects

   mutable HashtableIterator<KeyType, ValueType, HashFunctorType> * _iterList;  // list of existing iterators for this table
};

//===============================================================
// Implementation of Hashtable
// Necessary location for appropriate template instantiation.
//===============================================================
template <class KeyType, class ValueType, class HashFunctorType>
Hashtable<KeyType,ValueType,HashFunctorType>::Hashtable(uint32 initialCapacity)
   : _initialCapacity(initialCapacity), _count(0), _tableSize(initialCapacity), _table(NULL), _iterHead(NULL), _iterTail(NULL), _freeHead(NULL), _userKeyCompareFunc(NULL), _userValueCompareFunc(NULL), _autoSortMode(AUTOSORT_DISABLED), _compareCookie(NULL), _iterList(NULL)
{
   // empty
}

template <class KeyType, class ValueType, class HashFunctorType>
Hashtable<KeyType,ValueType,HashFunctorType>::
Hashtable(const Hashtable<KeyType,ValueType,HashFunctorType> & rhs)
   : _initialCapacity(rhs._initialCapacity), _count(0), _tableSize(rhs._tableSize), _table(NULL), _iterHead(NULL), _iterTail(NULL), _freeHead(NULL), _userKeyCompareFunc(rhs._userKeyCompareFunc), _userValueCompareFunc(rhs._userValueCompareFunc), _autoSortMode(rhs._autoSortMode), _compareCookie(rhs._compareCookie), _iterList(NULL)
{
   *this = rhs;
}

template <class KeyType, class ValueType, class HashFunctorType>
Hashtable<KeyType, ValueType, HashFunctorType> &
Hashtable<KeyType, ValueType, HashFunctorType> ::
operator=(const Hashtable<KeyType, ValueType, HashFunctorType> & rhs)
{
   if (this != &rhs)
   {
      Clear();
      if (EnsureSize(rhs.GetNumItems()) == B_NO_ERROR)
      {
         HashtableIterator<KeyType, ValueType, HashFunctorType> iter(rhs);
         const KeyType * nextKey;
         while((nextKey = iter.GetNextKey()) != NULL) (void) Put(*nextKey, *iter.GetNextValue());  // no good way to handle out-of-mem here?  
      }
   }
   return *this;
}

template <class KeyType, class ValueType, class HashFunctorType>
bool
Hashtable<KeyType, ValueType, HashFunctorType> ::
operator==(const Hashtable<KeyType, ValueType, HashFunctorType> & rhs) const
{
   if (this == &rhs) return true;
   if (GetNumItems() != rhs.GetNumItems()) return false;

   HashtableIterator<KeyType, ValueType, HashFunctorType> iter(*this);
   const KeyType * myNextKey;
   const ValueType * myNextValue;
   while(iter.GetNextKeyAndValue(myNextKey, myNextValue) == B_NO_ERROR)
   {
      const ValueType * hisNextValue = rhs.Get(*myNextKey);
      if ((hisNextValue == NULL)||(*hisNextValue != *myNextValue)) return false;
   }
   return true;
}

template <class KeyType, class ValueType, class HashFunctorType>
Hashtable<KeyType,ValueType,HashFunctorType>::~Hashtable()
{
   Clear();
   delete [] _table;
}

template <class KeyType, class ValueType, class HashFunctorType>
bool
Hashtable<KeyType,ValueType,HashFunctorType>::ContainsValue(const ValueType& value) const
{
   HashtableIterator<KeyType, ValueType, HashFunctorType> iter(*this);
   ValueType * v;
   while((v = iter.GetNextValue()) != NULL) if (*v == value) return true;
   return false;
}

template <class KeyType, class ValueType, class HashFunctorType>
int32
Hashtable<KeyType,ValueType,HashFunctorType>::IndexOfKey(const KeyType& key) const
{
   const HashtableEntry * entry = (_count>0)?GetEntry(ComputeHash(key), key):NULL;
   int32 count = -1;
   if (entry)
   {
      if (entry == _iterTail) count = GetNumItems()-1;
      else
      {
         while(entry)
         {
            entry = entry->_iterPrev; 
            count++;
         }
      }
   }
   return count;
}

template <class KeyType, class ValueType, class HashFunctorType>
const KeyType * 
Hashtable<KeyType,ValueType,HashFunctorType>::GetKeyAt(uint32 index) const
{
   HashtableEntry * e = GetEntryAt(index);
   return e ? &e->_key : NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
Hashtable<KeyType,ValueType,HashFunctorType>::GetKeyAt(uint32 index, KeyType & retKey) const
{
   HashtableEntry * e = GetEntryAt(index);
   if (e)
   {
      retKey = e->_key;
      return B_NO_ERROR;
   }
   return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t
Hashtable<KeyType,ValueType,HashFunctorType>::GetValue(const KeyType& key, ValueType & setValue) const
{
   ValueType * ptr = GetValue(key);
   if (ptr)
   {
      setValue = *ptr;
      return B_NO_ERROR;
   }
   else return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
ValueType *
Hashtable<KeyType,ValueType,HashFunctorType>::GetValue(const KeyType& key) const
{
   HashtableEntry * e = (_count>0)?GetEntry(ComputeHash(key), key):NULL;
   return e ? &e->_value : NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
const KeyType * 
Hashtable<KeyType,ValueType,HashFunctorType>::GetKey(const KeyType& lookupKey) const
{
   HashtableEntry * e = (_count>0)?GetEntry(ComputeHash(lookupKey), lookupKey):NULL;
   return e ? &e->_key : NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t
Hashtable<KeyType,ValueType,HashFunctorType>::GetKey(const KeyType& lookupKey, KeyType & setKey) const
{
   const KeyType * ptr = GetKey(lookupKey);
   if (ptr)
   {
      setKey = *ptr;
      return B_NO_ERROR;
   }
   else return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
typename Hashtable<KeyType,ValueType,HashFunctorType>::HashtableEntry *
Hashtable<KeyType,ValueType,HashFunctorType>::GetEntry(uint32 hash, const KeyType& key) const
{
   HashtableEntry * e = _table[hash%_tableSize].GetMapTo();
   if (IsBucketHead(e))  // if the e isn't the start of a bucket, then we know our entry doesn't exist
   {
      // While loops separated out for efficiency (one less if statement) --jaf
      if (_userKeyCompareFunc)
      {
         while(e)
         {
            if ((e->_hash == hash)&&(_userKeyCompareFunc(e->_key, key, _compareCookie) == 0)) return e;
            e = e->_bucketNext; 
         }
      }
      else
      {
         while(e)
         {
            if ((e->_hash == hash)&&(e->_key == key)) return e;
            e = e->_bucketNext; 
         }
      }
   }
   return NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
typename Hashtable<KeyType,ValueType,HashFunctorType>::HashtableEntry *
Hashtable<KeyType,ValueType,HashFunctorType>::GetEntryAt(uint32 idx) const
{
   HashtableEntry * e = NULL;
   if (idx < _count)
   {
      if (idx < _count/2)
      {
         e = _iterHead;
         while((e)&&(idx--)) e = e->_iterNext;
      }
      else
      {
         idx = _count-(idx+1);
         e = _iterTail;
         while((e)&&(idx--)) e = e->_iterPrev;
      }
   }
   return e;
}

template <class KeyType, class ValueType, class HashFunctorType>
int
Hashtable<KeyType,ValueType,HashFunctorType>::CompareEntries(const HashtableEntry & left, const HashtableEntry & right, KeyCompareFunc optKeyFunc, ValueCompareFunc optValueFunc, void * cookie) const
{
   return optValueFunc ? optValueFunc(left._value, right._value, cookie) : (optKeyFunc ? optKeyFunc(left._key, right._key, cookie) : 0);
}

// Linked-list MergeSort adapted from Simon Tatham's C code at http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.c
template <class KeyType, class ValueType, class HashFunctorType>
status_t 
Hashtable<KeyType,ValueType,HashFunctorType>::SortByAux(KeyCompareFunc optKeyFunc, ValueCompareFunc optValueFunc, void * cookie)
{
   if ((optKeyFunc)||(optValueFunc))
   {
      if (_iterHead)
      {
         for (uint32 mergeSize = 1; /* empty */; mergeSize *= 2)
         {
            HashtableEntry * p = _iterHead;
            _iterHead = _iterTail = NULL;

            uint32 numMerges = 0;  /* count number of merges we do in this pass */
            while(p) 
            {
               numMerges++;  /* there exists a merge to be done */

               /* step `mergeSize' places along from p */
               HashtableEntry * q = p;
               uint32 psize = 0;
               for (uint32 i=0; i<mergeSize; i++) 
               {
                   psize++;
                   q = q->_iterNext;
                   if (!q) break;
               }

               /* now we have two lists; merge them */
               for (uint32 qsize=mergeSize; ((psize > 0)||((qsize > 0)&&(q))); /* empty */) 
               {
                  HashtableEntry * e;

                  /* decide whether next element of the merge comes from p or q */
                       if (psize == 0)                                                {e = q; q = q->_iterNext; qsize--;}
                  else if ((qsize == 0)||(!q))                                        {e = p; p = p->_iterNext; psize--;}
                  else if (CompareEntries(*p,*q,optKeyFunc,optValueFunc,cookie) <= 0) {e = p; p = p->_iterNext; psize--;}
                  else                                                                {e = q; q = q->_iterNext; qsize--;}

                  /* append to our new more-sorted list */
                  if (_iterTail) _iterTail->_iterNext = e;
                            else _iterHead = e;
                  e->_iterPrev = _iterTail;
                  _iterTail = e;
               }

               p = q; /* now p has stepped `mergeSize' places along, and q has too */
            }
            _iterTail->_iterNext = NULL;
            if (numMerges <= 1) return B_NO_ERROR;
         }
      }
      return B_NO_ERROR;  // it's easy to sort an empty list :^)
   }
   return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t
Hashtable<KeyType,ValueType,HashFunctorType>::EnsureSize(uint32 requestedSize)
{
   if (_tableSize >= requestedSize) return B_NO_ERROR;  // no need to do anything if we're already big enough!
   // 1. Initialize the scratch space for our active iterators.
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> * nextIter = _iterList;
      while(nextIter)
      {
         nextIter->_scratchSpace[0] = nextIter->_scratchSpace[1] = NULL;  // these will hold our switch-to-on-success values
         nextIter = nextIter->_nextIter;
      }
   }
    
   // 2. Create a new, bigger table, and fill it with a copy of all of our data.
   Hashtable<KeyType,ValueType,HashFunctorType> biggerTable(NextPrime(muscleMax(_count, requestedSize)));
   biggerTable.SetKeyCompareFunction(_userKeyCompareFunc);
   biggerTable.SetValueCompareFunction(_userValueCompareFunc);
   biggerTable.SetCompareCookie(_compareCookie);
   biggerTable.SetAutoSortMode(_autoSortMode);
   {
      HashtableEntry * next = _iterHead;
      while(next)
      {
         HashtableEntry * hisClone = biggerTable.PutAux(next->_hash, next->_key, next->_value, NULL, NULL);
         if (hisClone)
         {
            // Mark any iterators that will need to be redirected to point to the new nodes.
            HashtableIterator<KeyType,ValueType,HashFunctorType> * nextIter = _iterList;
            while(nextIter)
            {
               if (nextIter->_nextKeyCookie   == next) nextIter->_scratchSpace[0] = hisClone;
               if (nextIter->_nextValueCookie == next) nextIter->_scratchSpace[1] = hisClone;
               nextIter = nextIter->_nextIter;
            }
         }
         else return B_ERROR;  // oops, out of mem, too bad.  

         next = next->_iterNext;
      }
   }

   // 3. Swap contents with the bigger table, but don't swap iterator lists (we want to keep ours!)
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> * temp = _iterList;
      _iterList = NULL;
      SwapContents(biggerTable);
      _iterList = temp;
   }

   // 4. Lastly, fix up our iterators to point to their new entries.
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> * nextIter = _iterList;
      while(nextIter)
      {
         nextIter->_nextKeyCookie   = nextIter->_scratchSpace[0];
         nextIter->_nextValueCookie = nextIter->_scratchSpace[1];
         nextIter = nextIter->_nextIter;
      }
   }

   return B_NO_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
void
Hashtable<KeyType,ValueType,HashFunctorType>::SwapContents(Hashtable<KeyType,ValueType,HashFunctorType> & swapMe)
{
   muscleSwap(_count,                swapMe._count);
   muscleSwap(_tableSize,            swapMe._tableSize);
   muscleSwap(_table,                swapMe._table);
   muscleSwap(_iterHead,             swapMe._iterHead);
   muscleSwap(_iterTail,             swapMe._iterTail);
   muscleSwap(_freeHead,             swapMe._freeHead);
   muscleSwap(_userKeyCompareFunc,   swapMe._userKeyCompareFunc);
   muscleSwap(_userValueCompareFunc, swapMe._userValueCompareFunc);
   muscleSwap(_autoSortMode,         swapMe._autoSortMode);
   muscleSwap(_compareCookie,        swapMe._compareCookie);
   muscleSwap(_iterList,             swapMe._iterList);

   // Lastly, swap the owners of all iterators, so that they will unregister from the correct table when they die
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> * next = _iterList;
      while(next)
      {
         next->_owner = &swapMe;
         next = next->_nextIter;
      }
   }
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> * next = swapMe._iterList;
      while(next)
      {
         next->_owner = this;
         next = next->_nextIter;
      }
   }
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
Hashtable<KeyType,ValueType,HashFunctorType>::MoveToFront(const KeyType & moveMe)
{
   HashtableEntry * e = (_count>0)?GetEntry(ComputeHash(moveMe), moveMe):NULL;
   if (e == NULL) return B_ERROR;
   if (e->_iterPrev)
   {
      RemoveIterationEntry(e);
      InsertIterationEntry(e, NULL);
   }
   return B_NO_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
Hashtable<KeyType,ValueType,HashFunctorType>::MoveToBack(const KeyType & moveMe)
{
   HashtableEntry * e = (_count>0)?GetEntry(ComputeHash(moveMe), moveMe):NULL;
   if (e == NULL) return B_ERROR;
   if (e->_iterNext)
   {
      RemoveIterationEntry(e);
      InsertIterationEntry(e, _iterTail);
   }
   return B_NO_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
Hashtable<KeyType,ValueType,HashFunctorType>::MoveToBefore(const KeyType & moveMe, const KeyType & toBeforeMe)
{
   if (_count > 0)
   {
      HashtableEntry * e = GetEntry(ComputeHash(moveMe),     moveMe);
      HashtableEntry * f = GetEntry(ComputeHash(toBeforeMe), toBeforeMe);
      if ((e == NULL)||(f == NULL)||(e == f)) return B_ERROR;
      if (e->_iterNext != f)
      {
         RemoveIterationEntry(e);
         InsertIterationEntry(e, f->_iterPrev);
      }
      return B_NO_ERROR;
   }
   else return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
Hashtable<KeyType,ValueType,HashFunctorType>::MoveToBehind(const KeyType & moveMe, const KeyType & toBehindMe)
{
   if (_count > 0)
   {
      HashtableEntry * d = GetEntry(ComputeHash(toBehindMe), toBehindMe);
      HashtableEntry * e = GetEntry(ComputeHash(moveMe),     moveMe);
      if ((d == NULL)||(e == NULL)||(d == e)) return B_ERROR;
      if (e->_iterPrev != d)
      {
         RemoveIterationEntry(e);
         InsertIterationEntry(e, d);
      }
      return B_NO_ERROR;
   }
   else return B_ERROR;
}

// Adds (e) to the end of our iteration linked list, or to an appropriate location to maintain sorting, if a sorting function is specified.
template <class KeyType, class ValueType, class HashFunctorType>
void
Hashtable<KeyType,ValueType,HashFunctorType>::InsertSortedIterationEntry(HashtableEntry * e, KeyCompareFunc optKeyFunc, ValueCompareFunc optValueFunc, void * cookie)
{
   HashtableEntry * insertAfter = _iterTail;  // default to appending to the end of the list
   if ((_iterHead)&&((optKeyFunc)||(optValueFunc)))
   {
      // We're in sorted mode, so we'll try to place this guy in the correct position.
           if (CompareEntries(*e, *_iterHead, optKeyFunc, optValueFunc, cookie) < 0) insertAfter = NULL;  // easy; append to the head of the list
      else if (CompareEntries(*e, *_iterTail, optKeyFunc, optValueFunc, cookie) < 0)  // only iterate through if we're before the tail, otherwise the tail is fine
      {
         HashtableEntry * prev = _iterHead;
         HashtableEntry * next = _iterHead->_iterNext;  // more difficult;  find where to insert into the middle
         while(next)
         {
            if (CompareEntries(*e, *next, optKeyFunc, optValueFunc, cookie) < 0)
            {
               insertAfter = prev;
               break;
            }
            else 
            {
               prev = next;
               next = next->_iterNext;
            }
         }   
      }
   }
   InsertIterationEntry(e, insertAfter);
}

// Adds (e) to the our iteration linked list, behind (optBehindThis), or at the head if (optBehindThis) is NULL.
template <class KeyType, class ValueType, class HashFunctorType>
void
Hashtable<KeyType,ValueType,HashFunctorType>::InsertIterationEntry(HashtableEntry * e, HashtableEntry * optBehindThis)
{
   e->_iterPrev = optBehindThis;
   e->_iterNext = optBehindThis ? optBehindThis->_iterNext : _iterHead;
   if (e->_iterPrev) e->_iterPrev->_iterNext = e;
                else _iterHead = e;
   if (e->_iterNext) e->_iterNext->_iterPrev = e;
                else _iterTail = e;
}

// Remove (e) from our iteration linked list
template <class KeyType, class ValueType, class HashFunctorType>
void 
Hashtable<KeyType,ValueType,HashFunctorType>::RemoveIterationEntry(HashtableEntry * e)
{
   // Update any iterators that were pointing at (e), so that they now point to the entry after e.
   HashtableIterator<KeyType, ValueType, HashFunctorType> * next = _iterList;
   while(next)
   {
      if (next->_nextKeyCookie   == e) (void) next->GetNextKey();
      if (next->_nextValueCookie == e) (void) next->GetNextValue();
      next = next->_nextIter;
   }

   if (_iterHead == e) _iterHead = e->_iterNext;
   if (_iterTail == e) _iterTail = e->_iterPrev;
   if (e->_iterPrev) e->_iterPrev->_iterNext = e->_iterNext;
   if (e->_iterNext) e->_iterNext->_iterPrev = e->_iterPrev;
   e->_iterPrev = e->_iterNext = NULL; 
}

template <class KeyType, class ValueType, class HashFunctorType>
typename Hashtable<KeyType,ValueType,HashFunctorType>::HashtableEntry *
Hashtable<KeyType,ValueType,HashFunctorType>::PutAux(uint32 hash, const KeyType& key, const ValueType& value, ValueType * optSetPreviousValue, bool * optReplacedFlag)
{
   if (optReplacedFlag) *optReplacedFlag = false;

   if (_table == NULL)  // demand-allocate this
   {
      _freeHead = _table = HashtableEntry::CreateEntriesArray(_tableSize);
      if (_table == NULL) return NULL;
   }

   // If we already have an entry for this key in the table, we can just replace its contents
   HashtableEntry * e = GetEntry(hash, key);
   if (e)
   {
      if (optSetPreviousValue) *optSetPreviousValue = e->_value;
      if (optReplacedFlag)     *optReplacedFlag     = true;
      e->_value = value;
      if ((_autoSortMode == AUTOSORT_BY_VALUE)&&(_userValueCompareFunc))
      {
         // If our new value has changed our position in the sort-order, then adjust the traversal list
         if ((e->_iterPrev)&&(_userValueCompareFunc(e->_value, e->_iterPrev->_value, _compareCookie) < 0))
         {
            const HashtableEntry * moveToBefore = e->_iterPrev;
            while((moveToBefore->_iterPrev)&&(_userValueCompareFunc(e->_value, moveToBefore->_iterPrev->_value, _compareCookie) < 0)) moveToBefore = moveToBefore->_iterPrev;
            (void) MoveToBefore(e->_key, moveToBefore->_key);
         }
         else if ((e->_iterNext)&&(_userValueCompareFunc(e->_value, e->_iterNext->_value, _compareCookie) > 0))
         {
            const HashtableEntry * moveToBehind = e->_iterNext;
            while((moveToBehind->_iterNext)&&(_userValueCompareFunc(e->_value, moveToBehind->_iterNext->_value, _compareCookie) > 0)) moveToBehind = moveToBehind->_iterNext;
            (void) MoveToBehind(e->_key, moveToBehind->_key);
         }
      }
      return e;
   }

   // Rehash the table if the threshold is exceeded
   if (_count == _tableSize) return (EnsureSize(_tableSize*2) == B_NO_ERROR) ? PutAux(hash, key, value, optSetPreviousValue, optReplacedFlag) : NULL;

   HashtableEntry * slot = _table[hash%_tableSize].GetMapTo();
   if (IsBucketHead(slot))
   {
      // This slot's chain is already present -- so just create a new entry in the chain's linked list to hold our item
      e = _freeHead;
      _freeHead = e->RemoveFromFreeList(_freeHead);
      e->_hash       = hash;
      e->_key        = key;
      e->_value      = value;

      // insert e into the list immediately after (slot)
      e->_bucketPrev = slot;
      e->_bucketNext = slot->_bucketNext;
      if (e->_bucketNext) e->_bucketNext->_bucketPrev = e;
      slot->_bucketNext = e;

      InsertSortedIterationEntry(e, (_autoSortMode == AUTOSORT_BY_KEY) ? _userKeyCompareFunc : NULL, (_autoSortMode == AUTOSORT_BY_VALUE) ? _userValueCompareFunc : NULL, _compareCookie);
   }
   else 
   {
      if (slot->_hash != MUSCLE_HASHTABLE_INVALID_HASH_CODE)
      {
         // Hey, some other bucket is using my starter-slot!
         // To get around this, we'll swap my starter-slot for an empty one and use that instead.
         slot->GetMappedFrom()->SwapMaps(_freeHead->GetMappedFrom());
         slot = _freeHead;
      }
      _freeHead = slot->RemoveFromFreeList(_freeHead);

      // First entry in slot; just copy data over
      slot->_hash  = hash;
      slot->_key   = key;
      slot->_value = value;  
 
      slot->_bucketPrev = slot->_bucketNext = NULL;
      e = slot;
      InsertSortedIterationEntry(slot, (_autoSortMode == AUTOSORT_BY_KEY) ? _userKeyCompareFunc : NULL, (_autoSortMode == AUTOSORT_BY_VALUE) ? _userValueCompareFunc : NULL, _compareCookie);
   }

   _count++;

   return e; 
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t
Hashtable<KeyType,ValueType,HashFunctorType>::RemoveAux(const KeyType& key, ValueType * optSetValue)
{
   HashtableEntry * e = (_count>0)?GetEntry(ComputeHash(key), key):NULL;
   if (e)
   {
      RemoveIterationEntry(e);
      if (optSetValue) *optSetValue = e->_value;

      HashtableEntry * prev = e->_bucketPrev;
      HashtableEntry * next = e->_bucketNext;
      if (prev)
      {
         prev->_bucketNext = next;
         if (next) next->_bucketPrev = prev;
      }
      else if (next) 
      {
         next->_bucketPrev = NULL;
         e->GetMappedFrom()->SwapMaps(next->GetMappedFrom());
      }

      e->ReturnToFreeList(KeyType(), ValueType(), &_freeHead);

      _count--;
      return B_NO_ERROR;
   }
   return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
void
Hashtable<KeyType,ValueType,HashFunctorType>::Clear(bool releaseCachedBuffers)
{
   // First go through our list of active iterators, and let them all know they are now invalid
   while(_iterList)
   {
      HashtableIterator<KeyType,ValueType,HashFunctorType> * next = _iterList->_nextIter;
      _iterList->_owner = NULL;
      _iterList->_nextKeyCookie = _iterList-> _nextValueCookie = NULL;
      _iterList->_prevIter      = _iterList->_nextIter         = NULL;
      _iterList = next;
   }

   if (_count > 0)
   {
      if (releaseCachedBuffers == false)
      {
         // Go through our list of valid entries and reset them all to their default state
         // This is important since sometimes they may be holding on to large allocated memory buffers, etc
         KeyType   blankKey   = KeyType();
         ValueType blankValue = ValueType();
         while(_iterHead)
         {
            HashtableEntry * next = _iterHead->_iterNext;  // save for later
            _iterHead->ReturnToFreeList(blankKey, blankValue, &_freeHead);
            _iterHead = next;
         }
      }
      _iterHead = _iterTail = NULL;
      _count = 0;
   }
   if (releaseCachedBuffers)
   {
      delete [] _table;
      _table     = NULL;
      _freeHead  = NULL;
      _tableSize = _initialCapacity;
   }
}

template <class KeyType, class ValueType, class HashFunctorType>
uint32
Hashtable<KeyType,ValueType,HashFunctorType>::NextPrime(uint32 start) const
{
   if (start % 2 == 0) start++;
   uint32 i;
   for (; ; start += 2)
   {
      for (i = 3; i * i <= start; i += 2) if (start % i == 0) break;
      if (i * i > start) return start;
   }
}

//===============================================================
// Implementation of HashtableIterator
//===============================================================

template <class KeyType, class ValueType, class HashFunctorType>
HashtableIterator<KeyType, ValueType, HashFunctorType>::HashtableIterator() : _nextKeyCookie(NULL), _nextValueCookie(NULL), _owner(NULL)
{
   // empty
}

template <class KeyType, class ValueType, class HashFunctorType>
HashtableIterator<KeyType, ValueType, HashFunctorType>::HashtableIterator(const HashtableIterator<KeyType, ValueType, HashFunctorType> & rhs) : _owner(NULL)
{
   *this = rhs;
}

template <class KeyType, class ValueType, class HashFunctorType>
HashtableIterator<KeyType, ValueType, HashFunctorType>::HashtableIterator(const Hashtable<KeyType, ValueType, HashFunctorType> & table, bool backwards) : _owner(NULL)
{
   table.InitializeIterator(*this, backwards);
}

template <class KeyType, class ValueType, class HashFunctorType>
HashtableIterator<KeyType, ValueType, HashFunctorType>::HashtableIterator(const Hashtable<KeyType, ValueType, HashFunctorType> & table, const KeyType & startAt, bool backwards) : _owner(NULL)
{
   table.InitializeIteratorAt(*this, startAt, backwards);
}

template <class KeyType, class ValueType, class HashFunctorType>
HashtableIterator<KeyType, ValueType, HashFunctorType>::~HashtableIterator()
{
   if (_owner) _owner->UnregisterIterator(this);
}

template <class KeyType, class ValueType, class HashFunctorType>
void
HashtableIterator<KeyType,ValueType,HashFunctorType>::SetOwner(const Hashtable<KeyType,ValueType,HashFunctorType> * table)
{
   if (_owner != table)
   {
      if (_owner) _owner->UnregisterIterator(this);
      _owner = table;
      if (_owner) _owner->RegisterIterator(this);
   }
}

template <class KeyType, class ValueType, class HashFunctorType>
HashtableIterator<KeyType,ValueType,HashFunctorType> &
HashtableIterator<KeyType,ValueType,HashFunctorType>:: operator=(const HashtableIterator<KeyType,ValueType,HashFunctorType> & rhs)
{
   if (this != &rhs)
   {
      SetOwner(rhs._owner);
      _backwards       = rhs._backwards;
      _nextKeyCookie   = rhs._nextKeyCookie;
      _nextValueCookie = rhs._nextValueCookie;
   }
   return *this;
}


template <class KeyType, class ValueType, class HashFunctorType>
status_t
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKey(KeyType& key) 
{
   const KeyType * ret = GetNextKey();
   if (ret) 
   {
      key = *ret;
      return B_NO_ERROR;
   }
   else return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextValue(ValueType& val)
{
   ValueType * ret = GetNextValue();
   if (ret)
   {
      val = *ret;
      return B_NO_ERROR;
   } 
   else return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t
HashtableIterator<KeyType,ValueType,HashFunctorType>::PeekNextKey(KeyType& key) const
{
   const KeyType * ret = PeekNextKey();
   if (ret)
   {
      key = *ret;
      return B_NO_ERROR;
   }
   else return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t
HashtableIterator<KeyType,ValueType,HashFunctorType>::PeekNextValue(ValueType& val) const
{
   ValueType * ret = PeekNextValue();
   if (ret)
   {
      val = *ret;
      return B_NO_ERROR;
   }
   else return B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
ValueType *
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextValue()
{
   ValueType * val = PeekNextValue();
   if (val)
   {
      _owner->IterateCookie(&_nextValueCookie, _backwards);
      return val;
   }
   return NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
const KeyType *
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKey() 
{
   const KeyType * ret = PeekNextKey();
   if (ret)
   {
      _owner->IterateCookie(&_nextKeyCookie, _backwards);
      return ret;
   }
   return NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
ValueType *
HashtableIterator<KeyType,ValueType,HashFunctorType>::PeekNextValue() const
{
   return (_owner) ? _owner->GetValueFromCookie(_nextValueCookie) : NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
const KeyType *
HashtableIterator<KeyType,ValueType,HashFunctorType>::PeekNextKey() const
{
   return (_owner) ? _owner->GetKeyFromCookie(_nextKeyCookie) : NULL;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKeyAndValue(KeyType & setKey, ValueType & setValue)
{
   return (GetNextKey(setKey) == B_NO_ERROR) ? GetNextValue(setValue) : B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKeyAndValue(KeyType & setKey, ValueType * & setValuePtr)
{
   return ((setValuePtr = GetNextValue()) != NULL) ? GetNextKey(setKey) : B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKeyAndValue(KeyType & setKey, const ValueType * & setValuePtr)
{
   return ((setValuePtr = GetNextValue()) != NULL) ? GetNextKey(setKey) : B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKeyAndValue(const KeyType * & setKeyPtr, ValueType & setValue)
{
   return ((setKeyPtr = GetNextKey()) != NULL) ? GetNextValue(setValue) : B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKeyAndValue(const KeyType * & setKeyPtr, ValueType * & setValuePtr)
{
   return (((setKeyPtr = GetNextKey()) != NULL)&&((setValuePtr = GetNextValue()) != NULL)) ? B_NO_ERROR : B_ERROR;
}

template <class KeyType, class ValueType, class HashFunctorType>
status_t 
HashtableIterator<KeyType,ValueType,HashFunctorType>::GetNextKeyAndValue(const KeyType * & setKeyPtr, const ValueType * & setValuePtr)
{
   return (((setKeyPtr = GetNextKey()) != NULL)&&((setValuePtr = GetNextValue()) != NULL)) ? B_NO_ERROR : B_ERROR;
}

END_NAMESPACE(muscle);

#endif
