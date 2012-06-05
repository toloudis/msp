/****************************************************************************\
**	dmState.hpp
**
**	State class for use with dmFSM.  Look at Game Programing Gems 3 CH 3.3
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DM_STATE_HPP
#error dmState.hpp multiply included
#endif
#define DM_STATE_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif


//============================================================================
//============================================================================
class dmStateBase
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dmStateBase() {};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void ExecuteBeginState() = 0;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void ExecuteState() = 0;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void ExecuteEndState() = 0;

};


//============================================================================
//============================================================================
template <class T>
class dmState : public dmStateBase
{
public:
//	typedef typename void (T::*PFNSTATE)(void);
	typedef void (T::*PFNSTATE)(void);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dmState();

	//--------------------------------------------------------------------
	//	Setup functions
	//--------------------------------------------------------------------
	void SetState(T* i_pInstance,PFNSTATE i_pfnBeginState,PFNSTATE i_pfnState,PFNSTATE i_pfnEndState);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline void ExecuteBeginState();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline void ExecuteState();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline void ExecuteEndState();

protected:

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline PFNSTATE GetBeginState();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline PFNSTATE GetState();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline PFNSTATE GetEndState();

private:
	T *m_pInstance;									// Instance Pointer
	PFNSTATE m_pfnBeginState;						// State Function Pointer
	PFNSTATE m_pfnState;							// State Function Pointer
	PFNSTATE m_pfnEndState;							// State Function Pointer
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
dmState<T>::dmState()
:	m_pInstance(0),
	m_pfnBeginState(0),
	m_pfnState(0),
	m_pfnEndState(0)	
{
}

//--------------------------------------------------------------------
//	Setup functions
//--------------------------------------------------------------------
template <class T>
void dmState<T>::SetState(T* i_pInstance,PFNSTATE i_pfnBeginState,PFNSTATE i_pfnState,PFNSTATE i_pfnEndState)
{
	DBG_ASSERT(i_pInstance != 0, "Invalid instance ptr.");
	DBG_ASSERT(i_pfnBeginState != 0, "Invalid begin state ptr.");
	DBG_ASSERT(i_pfnState != 0, "Invalid state ptr.");
	DBG_ASSERT(i_pfnEndState != 0, "Invalid end state ptr.");

	m_pInstance = i_pInstance;
	m_pfnBeginState = i_pfnBeginState;
	m_pfnState = i_pfnState;
	m_pfnEndState = i_pfnEndState;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
inline void dmState<T>::ExecuteBeginState()
{
	DBG_ASSERT( m_pInstance != 0 && m_pfnBeginState != 0, "State was not properly initialized");
	(m_pInstance->*m_pfnBeginState)();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
inline void dmState<T>::ExecuteState()
{
	DBG_ASSERT( m_pInstance != 0 && m_pfnState != 0, "State was not properly initialized");
	(m_pInstance->*m_pfnState)();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
inline void dmState<T>::ExecuteEndState()
{
	DBG_ASSERT( m_pInstance != 0 && m_pfnEndState != 0, "State was not properly initialized");
	(m_pInstance->*m_pfnEndState)();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
inline typename dmState<T>::PFNSTATE dmState<T>::GetBeginState()
{
	return m_pfnBeginState;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
inline typename dmState<T>::PFNSTATE dmState<T>::GetState()
{
	return m_pfnState;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
inline typename dmState<T>::PFNSTATE dmState<T>::GetEndState()
{
	return m_pfnEndState;
}

