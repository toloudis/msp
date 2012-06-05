/*****************************************************************************
**	scObjectMgr.hpp
**
**		scObjectMgr helps in managing scObjects by providing a list-based
**	organization of objects.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SC_OBJECTMGR_HPP
#error scObjectMgr.hpp multiply included
#endif
#define SC_OBJECTMGR_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#include <list>


//============================================================================
//============================================================================
class scObject;


//============================================================================
//============================================================================
class scObjectMgr
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	scObjectMgr();

	//------------------------------------------------------------------------
	//	Add() adds a scObject to the list managed by the scObjectMgr.
	//------------------------------------------------------------------------
	void Add( scObject* i_pObject );

	//------------------------------------------------------------------------
	//	Remove() causes the given scObject to be removed from the list
	//	but NOT deleted
	//------------------------------------------------------------------------
	void Remove( scObject* i_pObject );

	//------------------------------------------------------------------------
	//	Destroy() causes the given scObject to be removed from the list
	//	and deleted.
	//------------------------------------------------------------------------
	void Destroy( scObject* i_pObject );

	//------------------------------------------------------------------------
	//	Think calls animate on all scObjects in the list and any event
	//	handlers associated with the object
	//------------------------------------------------------------------------
	void Think( float i_fSimulationTime );

private:
	typedef std::list<scObject*> ObjectList;
	ObjectList m_Objects;
};

