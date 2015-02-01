#ifndef __Keymap_h__
#define __Keymap_h__

#include <iostream>
#include <map>

using namespace std;

template <typename T1, typename T2>
class KeyMap
{
public:

    bool AddItem( T1 const & key, T2 const & value);
    bool RemoveItemFor( T1 const & key);

    T2 ValueFor(T1 key,bool* found = NULL);
    T2 ValueAt(int32 position);

    T1 KeyAt(int32 position);
    void RemoveItemAt(int32 position);

    uint32 CountItems();

    void PrintToStream();
    bool MakeEmpty();
    bool IsEmpty();
    
private:

    bool getValue( T1 const & key, T2 *value);        //returns value
    bool getIndex( T2 const & value, T1 *key );       //returns key

    bool getFirst( T1 *key, T2 *value );
    bool getNext( T1 *key, T2 *value );

    typename std::map<T1,T2>::iterator begin();
    typename std::map<T1,T2>::iterator end();

    std::map<T1, T2> m_Map;
    typename std::map<T1,T2>::iterator m_pPos;
};

template <typename T1, typename T2>
bool KeyMap<T1,T2>::AddItem( T1 const & key, T2 const & value)
{

  std::pair<typename std::map<T1,T2>::iterator,bool> result = m_Map.insert( make_pair(key, value) );
    if (!result.second) {
        return false;
    }

    return true;
}

template <typename T1, typename T2>
bool KeyMap<T1,T2>::RemoveItemFor( T1 const & key )
{
  m_Map.erase( key );
  return true;
}

template <typename T1, typename T2>
bool KeyMap<T1,T2>::IsEmpty()
{
    return m_Map.empty();
}


template <typename T1, typename T2>
bool KeyMap<T1,T2>::MakeEmpty()
{
    m_Map.clear();
    return true;
}



template <typename T1, typename T2>
bool KeyMap<T1,T2>::getIndex( T2 const & value, T1 *key )
{
   //do something with all elements having a certain value
   typename std::map<T1,T2>::iterator pos;
   for (pos = m_Map.begin(); pos != m_Map.end(); ++pos) {
      if (pos->second == value) {
          *key = (pos->first);
          return true;
      }
   }
   return false;
}

template <typename T1, typename T2>
bool KeyMap<T1,T2>::getFirst( T1 *key, T2 *value )
{
    if (m_Map.empty() ) {
        return false;
    }
    //do something with all elements having a certain value
    typename std::map<T1,T2>::iterator pos;
    m_pPos= m_Map.begin();
    *key = m_pPos->first;
    *value=  m_pPos->second;
    return true;
}

template <typename T1, typename T2>
bool KeyMap<T1,T2>::getNext( T1 *key, T2 *value )
{
    if ( m_Map.empty() ) {
        return false;
    }
    if ( ++m_pPos == m_Map.end() )  {
        return false;
    }
    *key = m_pPos->first;
    *value=  m_pPos->second;
    return true;
}

template <typename T1, typename T2>
bool KeyMap<T1,T2>::getValue(T1 const & key, T2 *value)
{
    typename std::map<T1,T2>::iterator pos;
    pos = m_Map.find(key);       
    if (m_Map.end() != pos) {
        *value = pos->second;
        return true;
    }
    return false;
}

template <typename T1, typename T2>
T2 KeyMap<T1,T2>::ValueFor(T1 key, bool* found)
{
	T2 pointer;
	if (found)
	{
		*found = getValue(key, &pointer);
		return ((*found) ? pointer : NULL);
	}
	bool f = getValue(key, &pointer);
	return (f ? pointer : NULL);
}

template <typename T1, typename T2>
T2 KeyMap<T1,T2>::ValueAt(int32 position)
{
	typename std::map<T1,T2>::iterator i = begin();
	std::advance(i, position);
	if(i == end())
		return NULL;
	else
		return i->second;
}

template <typename T1, typename T2>
T1 KeyMap<T1,T2>::KeyAt(int32 position)
{
	typename std::map<T1,T2>::iterator i = begin();
	std::advance(i, position);
	if(i == end())
		return NULL;
	else
		return i->first;
}

template <typename T1, typename T2>
void KeyMap<T1,T2>::RemoveItemAt(int32 position)
{
	typename std::map<T1,T2>::iterator i = begin();
	std::advance(i, position);
	RemoveItemFor(i->first);
}


template <typename T1, typename T2>
uint32 KeyMap<T1,T2>::CountItems()
{
    return m_Map.size();
}

template <typename T1, typename T2>
void KeyMap<T1,T2>::PrintToStream()
{
    typename std::map<T1,T2>::iterator pos;
    cout << std::endl << " -- KeyMap::PrintToStream --" << std::endl;
    for (pos = m_Map.begin(); pos != m_Map.end(); ++pos) {
        cout << pos->first <<" : " << pos->second << std::endl;
   }
}

template <typename T1, typename T2>
typename std::map<T1,T2>::iterator KeyMap<T1,T2>::begin()
{
  m_pPos = m_Map.begin();
  return m_pPos;
}

template <typename T1, typename T2>
typename std::map<T1,T2>::iterator KeyMap<T1,T2>::end()
{
  m_pPos = m_Map.end();
  return m_pPos;
}

#endif // __Keymap_h__
