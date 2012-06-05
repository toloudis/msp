/*****************************************************************************
**  anKeyAnimation.hpp
**
**      anKeyAnimation is an animation type which stores keyframes and
**	linearly interpolates between them.  A keyframe is a combination of a
**	time and a value, which represents the state of the animation at that
**	time.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AN_KEYANIMATION_HPP
#error anKeyAnimation.hpp multiply included
#endif
#define AN_KEYANIMATION_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifndef AN_TYPEDANIMATION_HPP
#include "Graphics/an/anTypedAnimation.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
//	anKeyAnimation
//============================================================================
template <class T>
class anKeyAnimation : public anTypedAnimation<T>
{
	public:
		//--------------------------------------------------------------------
		//	anKeyAnimation constructor.  The animation _must_ contain a value
		//	at zero time; this is the constructor parameter.
		//--------------------------------------------------------------------
		anKeyAnimation(const T& i_ZeroValue);

		//--------------------------------------------------------------------
		//	destructor - virtual
		//--------------------------------------------------------------------
		virtual ~anKeyAnimation();

		//--------------------------------------------------------------------
		//	GetNumKeys returns the number of time/keyvalue pairs
		//--------------------------------------------------------------------
		int GetNumKeys() const;

		//--------------------------------------------------------------------
		//	GetKeyData returns the information for the given key number
		//--------------------------------------------------------------------
		void GetKeyData(int i_KeyNumber, float& o_Time, T& o_Value) const;

		//--------------------------------------------------------------------
		//	GetValue returns the value of the animation parameter at the
		//	given time (referenced to the beginning of the animation at
		//	0 seconds).
		//--------------------------------------------------------------------
		virtual T GetValue(float i_Time) const;

		//--------------------------------------------------------------------
		//	AddKey adds a keyframe to the list
		//--------------------------------------------------------------------
		void AddKey(float i_Time, const T& i_Value);

		//--------------------------------------------------------------------
		//	Clone returns a copy of "this" allocated on the heap.
		//--------------------------------------------------------------------
		virtual anAnimation* Clone() const;

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

	private:

		//--------------------------------------------------------------------
		//	GetLowerKeyIndex returns the index for the keyframe which is
		//	immediately below or equal to the given time value (i_Time).
		//--------------------------------------------------------------------
		int GetLowerKeyIndex(float i_Time) const;

		//--------------------------------------------------------------------
		//	GetBracketingKeyData returns the key data for the keyframes
		//	immediately above and immmediately below the given time value
		//	(i_Time).
		//--------------------------------------------------------------------
		void GetBracketingKeyData(	float i_Time,
									int& o_Key0,
									int& o_Key1) const;

		//--------------------------------------------------------------------
		//	GetCubicKeyData returns the key data for the 4 keyframes
		//	above and below the given time value
		//	(i_Time).
		//--------------------------------------------------------------------
		void GetCubicKeyData(	float i_Time,
								int& o_Key0,
								int& o_Key1,
								int& o_Key2,
								int& o_Key3) const;

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
//	anKeyAnimation implementation
//============================================================================

//----------------------------------------------------------------------------
//	anKeyAnimation constructor.  The animation _must_ contain a value
//	at zero time; this is the constructor parameter.
//----------------------------------------------------------------------------
template <class T>
anKeyAnimation<T>::anKeyAnimation(const T& i_ZeroValue)
{
	this->AddKey(0.0f, i_ZeroValue);
}

//----------------------------------------------------------------------------
//	destructor - virtual
//----------------------------------------------------------------------------
template <class T>
anKeyAnimation<T>::~anKeyAnimation()
{
}

//----------------------------------------------------------------------------
//	GetNumKeys returns the number of time/keyvalue pairs
//----------------------------------------------------------------------------
template <class T>
inline int anKeyAnimation<T>::GetNumKeys() const
{
	return m_Keys.size();
}

//----------------------------------------------------------------------------
//	GetKeyData returns the information for the given key number
//----------------------------------------------------------------------------
template <class T>
inline void anKeyAnimation<T>::GetKeyData(int i_KeyNumber, float& o_Time, T& o_Value) const
{
	DBG_ASSERT(m_Keys.size() > 0, "Animation should contain at least one key");
	if (m_Keys.size() ==  0)
		return;	// no valid data to get
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
int anKeyAnimation<T>::GetLowerKeyIndex(float i_Time) const
{
	DBG_ASSERT(m_Keys.size() > 0, "Animation should contain at least one key");
	if (m_Keys.size() == 0)
		return 0;

	if( i_Time < 0 ) return 0;
	if( i_Time > this->GetLength() ) return m_Keys.size(); // -1?

	std::map<float, int>::const_iterator it;
	it = m_KeyMap.lower_bound(i_Time); // should this be upper_bound?

	if( it == m_KeyMap.end() )
		return m_Keys.size();

	return it->second;
}

//----------------------------------------------------------------------------
//	GetBracketingKeyData returns the key data for the keyframes
//	immediately above and immmediately below the given time value
//	(i_Time).
//----------------------------------------------------------------------------
template <class T>
void anKeyAnimation<T>::GetBracketingKeyData(	float i_Time,
												int& o_Key0,
												int& o_Key1) const
{
	DBG_ASSERT(m_Keys.size() > 0, "Animation should contain at least one key");
	if (m_Keys.size() == 0)
		return;
	DBG_ASSERT( i_Time >= 0.0f, "Negative time not handled in anKeyAnimation");
	if (i_Time < 0.0f)
		i_Time = 0.0f;
	DBG_ASSERT( i_Time <= this->GetLength(), "Out of range time in anKeyAnimation");
	if (i_Time > this->GetLength())
		i_Time = this->GetLength();

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

//--------------------------------------------------------------------
//	GetCubicKeyData returns the key data for the 4 keyframes
//	above and below the given time value
//	(i_Time).
//--------------------------------------------------------------------
template <class T>
void anKeyAnimation<T>::GetCubicKeyData(float i_Time,
										int& o_Key0,
										int& o_Key1,
										int& o_Key2,
										int& o_Key3) const
{
	DBG_ASSERT( i_Time >= 0.0f, "Negative time not handled in anKeyAnimation");
	if (i_Time < 0.0f)
		i_Time = 0.0f;
	DBG_ASSERT( i_Time <= this->GetLength(), "Out of range time in anKeyAnimation");
	if (i_Time > this->GetLength())
		i_Time = this->GetLength();

	if( m_Keys.size() == 0 )
	{
		o_Key0 = o_Key1 = o_Key2 = o_Key3 = 0;
		return;
	}

	//	After this we know we have at least 2 keys
	//
	std::map<float, int>::const_iterator begin = m_KeyMap.begin();
	std::map<float, int>::const_iterator end = m_KeyMap.end();
	std::map<float, int>::const_iterator last = end;
	--last;

	//	upper_bound returns the first element with key greater than i_Time
	//
	std::map<float, int>::const_iterator it2 = m_KeyMap.upper_bound(i_Time);

	std::map<float, int>::const_iterator it0;
	std::map<float, int>::const_iterator it1;
	std::map<float, int>::const_iterator it3;

	//	we'll try to simplify this by handling 4 cases separately
	if( !this->GetLooping() )
	{
		if( !this->GetReversing() )
		{
			// once thru
			if( (it2 == end) || (it2 == last) )
			{
				it2 = last;
				it3 = last;
			}
			else
			{
				it3 = it2;
				++it3;
			}

			if( it2 == begin )
			{
				it1 = begin;
				it0 = begin;
			}
			else
			{
				it1 = it2;
				--it1;

				if( it1 == begin )
				{
					it0 = begin;
				}
				else
				{
					it0 = it1;
					--it0;
				}
			}
		}
		else
		{
	 		//	reverse once
			if( it2 == end )
			{
				it3 = it2 = last;
				--it3;
			}
			else
			{
				if( it2 == last )
				{
					it3 = it2;
					--it3;
				}
				else
				{
					it3 = it2;
					++it3;
				}
			}

			if( it2 == begin )
			{
				it1 = begin;
				it0 = begin;
			}
			else
			{
				it1 = it2;
				--it1;

				if( it1 == begin )
				{
					it0 = begin;
				}
				else
				{
					it0 = it1;
					--it0;
				}
			}
		}
	}
	else
	{
		if( !this->GetReversing() )
		{
			//	looping but not reversing
			if( it2 == end )
			{
				it2 = last;
				it3 = begin;
			}
			else
			{
				if( it2 == last )
				{
					it3 = begin;
				}
				else
				{
					it3 = it2;
					++it3;
				}
			}

			if( it2 == begin )
			{
				it0 = it1 = last;
				--it0;
			}
			else
			{
				it1 = it2;
				--it1;

				if( it1 == begin )
				{
					it0 = begin;
				}
				else
				{
					it0 = it1;
					--it0;
				}
			}
		}
		else
		{
			//	looping and reversing
			if( it2 == end )
			{
				it3 = it2 = last;
				--it3;
			}
			else
			{
				it3 = it2;
				if( it2 == last )
					--it3;
				else
					++it3;
			}

			if( it2 == begin )
			{
				it1 = it2;
				++it1;

				it0 = it1;
				if( it1 == last )
					--it0;
				else
					++it0;
			}
			else
			{
				it1 = it2;
				--it1;

				it0 = it1;
				if( it1 == begin )
					++it0;
				else
					--it0;
			}
		}
	}

	o_Key0 = it0->second;
	o_Key1 = it1->second;
	o_Key2 = it2->second;
	o_Key3 = it3->second;
}

//----------------------------------------------------------------------------
//	GetValue returns the value of the animation parameter at the
//	given time.
//----------------------------------------------------------------------------
template <class T>
T anKeyAnimation<T>::GetValue(float i_Time) const
{
	// convert to our time coordinate
	float anim_time = this->GetCorrectedTime(i_Time);

	int key1, key2;
	this->GetBracketingKeyData(	anim_time,
								key1,
								key2);

	float t1 = m_Keys[key1].m_Time;
	float t2 = m_Keys[key2].m_Time;
	const T& v1 = m_Keys[key1].m_Value;
	const T& v2 = m_Keys[key2].m_Value;

	if( t1 == t2 )
		return v1;

	// do a linear interpolation
	return T(v1 + ((anim_time - t1) / (t2 - t1)) * (v2 - v1));
}

//----------------------------------------------------------------------------
//	AddKey adds a keyframe to the list
//----------------------------------------------------------------------------
template <class T>
void anKeyAnimation<T>::AddKey(float i_Time, const T& i_Value)
{
	int new_num = m_Keys.size();
	m_Keys.push_back(KeyInfo(i_Time, i_Value));
	m_KeyMap[i_Time] = new_num;
	if( i_Time > this->GetLength() )
		this->SetLength(i_Time);
}

//--------------------------------------------------------------------
//	Clone returns a copy of "this" allocated on the heap.
//--------------------------------------------------------------------
template <class T>
anAnimation* anKeyAnimation<T>::Clone() const
{
	return new anKeyAnimation<T>(*this);
}

//--------------------------------------------------------------------
//	Rescale changes the time scale of the animation by the given
//	factor.  For example, if the factor is 2.0, the animation will be
//	twice as long and appear to go half as fast.
//--------------------------------------------------------------------
template <class T>
void anKeyAnimation<T>::Rescale(float i_Scale)
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

	this->SetLength(max_time);
}

//--------------------------------------------------------------------
//	GetKeyTime returns the time value for the nth key (in sorted
//	order).
//--------------------------------------------------------------------
template <class T>
float anKeyAnimation<T>::GetKeyTime(int i_Num) const
{
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

