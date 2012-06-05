/*****************************************************************************
**  anKeyData.hpp
**
**      anKeyData is template which stores keyframes and
**	linearly interpolates between them.  A keyframe is a combination of a
**	time and a value, which represents the state of the animation at that
**	time.  This class is used as the basis for shared keyframe data.
**	It does not derive from anAnimation, it is used by derivations
**	of anAnimation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AN_KEYDATA_HPP
#error anKeyData.hpp multiply included
#endif
#define AN_KEYDATA_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
//	anKeyDataBase - organization of data by time frames
//============================================================================
template <class T>
class anKeyDataBase
{
	public:

		//--------------------------------------------------------------------
		//	anKeyData constructor.  Can pass in a value for the zero time
		//	if desired, but it is no longer required.
		//--------------------------------------------------------------------
		anKeyDataBase();
		anKeyDataBase(const T& i_ZeroValue);

		//--------------------------------------------------------------------
		//	destructor - virtual
		//--------------------------------------------------------------------
		virtual ~anKeyDataBase();

		//--------------------------------------------------------------------
		// Clear key values, resetting to this zero time value, or to empty
		//	array
		//--------------------------------------------------------------------
		void Clear(const T& i_ZeroValue);
		void Clear();

		//--------------------------------------------------------------------
		//	GetNumKeys returns the number of time/keyvalue pairs
		//--------------------------------------------------------------------
		int GetNumKeys() const;

		//--------------------------------------------------------------------
		//	GetKeyData returns the information for the given key number
		//--------------------------------------------------------------------
		void GetKeyData(int i_KeyNumber, float& o_Time, T& o_Value) const;

		//--------------------------------------------------------------------
		//	AddKey adds a keyframe to the list
		//--------------------------------------------------------------------
		void AddKey(float i_Time, const T& i_Value);

		//--------------------------------------------------------------------
		//	AlterKey alters value of an existing key
		//--------------------------------------------------------------------
		void AlterKey(int i_KeyIndex, const T& i_Value);

		//--------------------------------------------------------------------
		//	Rescale changes the time scale of the animation by the given
		//	factor.  For example, if the factor is 2.0, the animation will be
		//	twice as long and appear to go half as fast.
		//--------------------------------------------------------------------
		virtual void Rescale(float i_Scale);

		//--------------------------------------------------------------------
		//	GetKeyTime returns the time value for the nth key (in sorted
		//	order).
		//--------------------------------------------------------------------
		float GetKeyTime(int i_Num) const;

		//--------------------------------------------------------------------
		//	GetLength returns the time value for the last key
		//--------------------------------------------------------------------
		float GetLength() const;

		//--------------------------------------------------------------------
		//	GetBracketingKeyData returns the key data for the keyframes
		//	immediately above and immmediately below the given time value
		//	(i_Time).
		//--------------------------------------------------------------------
		void GetBracketingKeyData(	float i_Time,
									int& o_Key0,
									int& o_Key1) const;

		//--------------------------------------------------------------------
		//	GetLowerKeyIndex returns the index for the keyframe which is
		//	immediately below or equal to the given time value (i_Time).
		//--------------------------------------------------------------------
		int GetLowerKeyIndex(float i_Time) const;

	protected:

		struct KeyInfo
		{
			KeyInfo(float i_Time, const T& i_Value) :	m_Time(i_Time), m_Value(i_Value) {}

			float m_Time;
			T m_Value;
		};

		std::vector<KeyInfo> m_Keys;
		std::map<float, int> m_KeyMap;
};


//============================================================================
//	anKeyData - Adds a GetValue function for types that can do
//		a little arithmetic to interpolate a value between frames.
//============================================================================
template <class T>
class anKeyData : public anKeyDataBase<T>
{
	public:
		//--------------------------------------------------------------------
		//	anKeyData constructor.  Can pass in a value for the zero time
		//	if desired, but it is no longer required.
		//--------------------------------------------------------------------
		anKeyData();
		anKeyData(const T& i_ZeroValue);

		//--------------------------------------------------------------------
		//	GetValue returns the value of the animation parameter at the
		//	given time (referenced to the beginning of the animation at
		//	0 seconds).
		//--------------------------------------------------------------------
		virtual T GetValue(float i_Time) const;

};


//============================================================================
//	anKeyDataBase implementation
//============================================================================

//----------------------------------------------------------------------------
//	anKeyData constructor.  
//----------------------------------------------------------------------------
template <class T>
anKeyDataBase<T>::anKeyDataBase()
{
}
template <class T>
anKeyDataBase<T>::anKeyDataBase(const T& i_ZeroValue)
{
	this->AddKey(0.0f, i_ZeroValue);
}

//----------------------------------------------------------------------------
//	destructor - virtual
//----------------------------------------------------------------------------
template <class T>
anKeyDataBase<T>::~anKeyDataBase()
{
}


//--------------------------------------------------------------------
// Clear key values, resetting to this zero time value
//--------------------------------------------------------------------
template <class T>
inline void anKeyDataBase<T>::Clear()
{
	m_Keys.clear();
	m_KeyMap.clear();
}
template <class T>
inline void anKeyDataBase<T>::Clear(const T& i_ZeroValue)
{
	this->Clear();
	this->AddKey(0.0f, i_ZeroValue);
}

//----------------------------------------------------------------------------
//	GetNumKeys returns the number of time/keyvalue pairs
//----------------------------------------------------------------------------
template <class T>
inline int anKeyDataBase<T>::GetNumKeys() const
{
	return m_Keys.size();
}

//----------------------------------------------------------------------------
//	GetKeyData returns the information for the given key number
//----------------------------------------------------------------------------
template <class T>
inline void anKeyDataBase<T>::GetKeyData(int i_KeyNumber, float& o_Time, T& o_Value) const
{
	DBG_ASSERT(m_Keys.size() > 0, "Animation should contain at least one key");
	if (m_Keys.size() == 0)
		return;		// no valid data to return
	DBG_ASSERT(i_KeyNumber < m_Keys.size(), "Key number out of range");
	if (i_KeyNumber >= m_Keys.size())
		i_KeyNumber = m_Keys.size() - 1;
	DBG_ASSERT(i_KeyNumber >= 0, "Key number out of range");
	if (i_KeyNumber < 0)
		i_KeyNumber = 0;

	const KeyInfo& key_info = m_Keys[i_KeyNumber];
	o_Time = key_info.m_Time;
	o_Value = key_info.m_Value;
}

//----------------------------------------------------------------------------
//	GetLowerKeyIndex returns the index for the keyframe which is
//	immediately below or equal to the given time value (i_Time).
//----------------------------------------------------------------------------
template <class T>
int anKeyDataBase<T>::GetLowerKeyIndex(float i_Time) const
{
	DBG_ASSERT(m_Keys.size() > 0, "Animation should contain at least one key");
	if (m_Keys.size() == 0)
		return 0;

	std::map<float, int>::const_iterator it;

	if( i_Time < 0 ) return 0;
	if( i_Time > this->GetLength() )
	{
		it = m_KeyMap.end();
		--it;
		return it->second;
	}

	it = m_KeyMap.lower_bound(i_Time);

	if( it == m_KeyMap.end() )
		return m_Keys.size();

	// a non-exact match returns iterator *greater* than key, so back up one
	if (it->first > i_Time)
		--it;

	DBG_ASSERT(it->first <= i_Time, "Got later key! " << it->first << " asked " << i_Time);
	return it->second;
}

//----------------------------------------------------------------------------
//	GetBracketingKeyData returns the key data for the keyframes
//	immediately above and immmediately below the given time value
//	(i_Time).
//----------------------------------------------------------------------------
template <class T>
void anKeyDataBase<T>::GetBracketingKeyData(	float i_Time,
												int& o_Key0,
												int& o_Key1) const
{
	DBG_ASSERT(m_Keys.size() > 0, "Animation should contain at least one key");
	if (m_Keys.size() == 0)
		return;

	// allow less than 0 and greater than last key to return
	// begin and end keys
//	DBG_ASSERT( i_Time >= 0.0f, "Negative time not handled in anKeyData");
//	DBG_ASSERT( i_Time <= this->GetLength(), "Out of range time in anKeyData");

	// upper_bound() should give us the key value directly above the given time
	//
	std::map<float, int>::const_iterator it;
	it = m_KeyMap.upper_bound(i_Time);

	if( it == m_KeyMap.end() )
	{
		--it;
		o_Key0 = o_Key1 = it->second;
		return;
	}

	if( it == m_KeyMap.begin() )
	{
		// return the first key only
		//
		o_Key0 = o_Key1 = 0;
		return;
	}

	// go back to lower bound key
	--it;

	o_Key0 = it->second;

	// to get the next key we simply increment the iterator.  If the
	// lower_bound function is correct and we don't have duplicate keys
	// this will give us a time value after the i_Time;
	//
	it++;
	o_Key1 = it->second;
}

//----------------------------------------------------------------------------
//	AddKey adds a keyframe to the list
//----------------------------------------------------------------------------
template <class T>
void anKeyDataBase<T>::AddKey(float i_Time, const T& i_Value)
{
	int new_num = m_Keys.size();
	m_Keys.push_back(KeyInfo(i_Time, i_Value));
	m_KeyMap[i_Time] = new_num;
}

//--------------------------------------------------------------------
//	AlterKey alters value of an existing key
//--------------------------------------------------------------------
template <class T>
void anKeyDataBase<T>::AlterKey(int i_KeyIndex, const T& i_Value)
{
	m_Keys[i_KeyIndex].m_Value = i_Value;
}

//--------------------------------------------------------------------
//	Rescale changes the time scale of the animation by the given
//	factor.  For example, if the factor is 2.0, the animation will be
//	twice as long and appear to go half as fast.
//--------------------------------------------------------------------
template <class T>
void anKeyDataBase<T>::Rescale(float i_Scale)
{
	float max_time = -1.0f;

	int i;
	int num = m_Keys.size();
	m_KeyMap.clear();

	for( i = 0 ; i < num ; i++ )
	{
		float& time = m_Keys[i].m_Time;
		time *= i_Scale;
		if( time > max_time )
			max_time = time;

		m_KeyMap[time] = i;
	}
}

//--------------------------------------------------------------------
//	GetKeyTime returns the time value for the nth key (in sorted
//	order).
//--------------------------------------------------------------------
template <class T>
float anKeyDataBase<T>::GetKeyTime(int i_Num) const
{
	DBG_ASSERT(m_KeyMap.size() > 0, "Animation should contain at least one key");
	if (m_KeyMap.size() == 0)
		return 0.0f;

	std::map<float, int>::const_iterator it = m_KeyMap.begin();
	std::map<float, int>::const_iterator end = m_KeyMap.end();

	int cur_num = 0;

	for( cur_num = 0 ; cur_num < i_Num ; cur_num++ )
	{
		if( it == end )
		{
			--it;
			break;
		}

		++it;
	}

	return it->first;
}

//--------------------------------------------------------------------
//	GetLength returns the time value for the last key
//--------------------------------------------------------------------
template <class T>
float anKeyDataBase<T>::GetLength() const
{
	if (m_KeyMap.empty())
		return 0.0f;

	DBG_ASSERT(m_KeyMap.size() > 0, "Animation should contain at least one key");
	if (m_KeyMap.size() == 0)
		return 0.0f;

	std::map<float, int>::const_iterator it = m_KeyMap.end();
	--it;
	return it->first;
}


//============================================================================
//	anKeyData implementation
//============================================================================

//----------------------------------------------------------------------------
//	anKeyData constructor.  The animation _must_ contain a value
//	at zero time; this is the constructor parameter.
//----------------------------------------------------------------------------
template <class T>
anKeyData<T>::anKeyData()
:	anKeyDataBase<T>()
{
}
template <class T>
anKeyData<T>::anKeyData(const T& i_ZeroValue)
: anKeyDataBase<T>(i_ZeroValue)
{
}
//----------------------------------------------------------------------------
//	GetValue returns the value of the animation parameter at the
//	given time.
//----------------------------------------------------------------------------
template <class T>
T anKeyData<T>::GetValue(float i_Time) const
{
	int key1, key2;
	this->GetBracketingKeyData(	i_Time,
								key1,
								key2);

	float t1 = m_Keys[key1].m_Time;
	float t2 = m_Keys[key2].m_Time;
	const T& v1 = m_Keys[key1].m_Value;
	const T& v2 = m_Keys[key2].m_Value;

	if( t1 == t2 )
		return v1;

	// do a linear interpolation
	float alpha = (i_Time - t1) / (t2 - t1);
	//return T(v1 + (v2 - v1) * alpha);
	return T(v1 * (1 - alpha) + v2 * alpha);
}
