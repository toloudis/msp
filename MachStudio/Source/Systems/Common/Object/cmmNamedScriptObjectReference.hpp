/****************************************************************************\
**	cmmNamedScriptObjectReference.hpp
**
**		Reference class for script objects that have named property objects.
**	The name reference won't work for them directly, they have to look up the
**	name as an index and then ask for the script object instead of the
**	property object.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_NAMEDSCRIPTOBJECTREFERENCE_HPP
#error cmmNamedScriptObjectReference.hpp multiply included
#endif
#define CMM_NAMEDSCRIPTOBJECTREFERENCE_HPP

#ifndef REL_OBJECTREFERENCE_HPP
#include "Core/rel/relObjectReference.hpp"
#endif 
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif 
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif 


//============================================================================
//============================================================================
template<class xxxObjectMgr>
class cmmNamedScriptObjectReference : public relObjectReference
{
	public:
		cmmNamedScriptObjectReference(const nameString& i_Name)
			: m_Name(i_Name) {}

		virtual relObject* GetObject() const
		{
			int index = xxxObjectMgr::GetIndexForObject(m_Name);
			if (index >= 0)
			{
				relObject *pRelObj = xxxObjectMgr::GetObject(index);
				if (pRelObj)
				{
					return pRelObj;
				}
			}
			DBG_WARNING("Could not resolve named script object " << m_Name.GetString());
			return NULL;
		}
	private:
		nameString m_Name;
};
