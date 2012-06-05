/*****************************************************************************
**	scObjectMgr.cpp
**
**		scObjectMgr helps in managing scObjects by providing a list-based
**	organization of objects.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sc/scObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Graphics/Sc/scThreadGroup.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
scObjectMgr::scObjectMgr()
{
}

//------------------------------------------------------------------------
//	Add adds a scObject to the list managed by the scObjectMgr.
//------------------------------------------------------------------------
void scObjectMgr::Add(scObject* i_pObject)
{
	DBG_ASSERT( std::find(m_Objects.begin(), m_Objects.end(), i_pObject) == m_Objects.end(), "Object already in list");
	if (std::find(m_Objects.begin(), m_Objects.end(), i_pObject) != m_Objects.end())
		return;
	DBG_ASSERT(i_pObject, "NULL object");
	if (!i_pObject)
		return;
	m_Objects.push_back(i_pObject);
}

//------------------------------------------------------------------------
//	Remove() causes the given scObject to be removed from the list
//	but NOT deleted.
//------------------------------------------------------------------------
void scObjectMgr::Remove(scObject* i_pObject)
{
	ObjectList::iterator object_it = std::find(m_Objects.begin(), m_Objects.end(), i_pObject);
	DBG_ASSERT(object_it != m_Objects.end(), "Object not found in list");
	if (object_it == m_Objects.end())
		return;

	m_Objects.erase(object_it);
}

//------------------------------------------------------------------------
//	Destroy causes the given scObject to be removed from the list
//	and deleted.
//------------------------------------------------------------------------
void scObjectMgr::Destroy(scObject* i_pObject)
{
	ObjectList::iterator object_it = std::find(m_Objects.begin(), m_Objects.end(), i_pObject);
	DBG_ASSERT(object_it != m_Objects.end(), "Object not found in list");
	if (object_it == m_Objects.end())
		return;
	m_Objects.erase(object_it);

	delete i_pObject;
}

//------------------------------------------------------------------------
//	Animate calls animate on all scObjects in the list.
//------------------------------------------------------------------------
void scObjectMgr::Think( float i_fSimulationTime )
{
	ObjectList::iterator it = m_Objects.begin();
	ObjectList::iterator end = m_Objects.end();

	ObjectList to_destroy;

	while( it != end )
	{
		scObject* obj = (*it);
		DBG_ASSERT(obj != NULL, "Object pointer is NULL");
		
		if (obj != NULL)
		{
			if (obj->GetRenderable())
				obj->Animate(i_fSimulationTime);

			if( obj->IsExpired() )
			{
				to_destroy.push_back( obj );
			}
		}

		++it;
	}

	// The Animate calls above may have spawned threads to
	// do the animation in parallel. Wait for all of the threads
	// to finish before continuing.
	scThreadGroup::WaitForAll();

	//envSTLHelpers::ForAll(to_destroy, Destroy);
	end = to_destroy.end();
	for (it = to_destroy.begin(); it != end; ++it)
	{
		Destroy(*it);
	}
}

