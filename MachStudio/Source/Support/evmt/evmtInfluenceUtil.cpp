/*****************************************************************************
**	evmtInfluenceUtil.cpp
**
**	see hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/evmt/evmtInfluenceUtil.hpp"
#include "Support/evmt/evmtEnvironmentMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace evmtInfluenceUtil
{

	namespace
	{
	}	// end of namespace


	//--------------------------------------------------------------------
	// Look for all pieces of geometry in the selection list
	// and then figure out which environments influence these pieces of
	// geometry. Return the environments in o_InfluenceEnvironments.
	//--------------------------------------------------------------------
	void  GetEnvironmentsForSelection(std::set<nameString>& o_InfluenceEnvironments)
	{
		const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			if ( nameObject *pNamedObject = sel3dCastUtil::CastPickObject<nameObject>(*sit) )
			{
				nameString item_name = pNamedObject->GetName();
				if (evmtEnvironmentMgr::IsObject(item_name))
				{
					nameString environment_name;
					if (evmtEnvironmentMgr::GetEnvironmentNameFromObject( item_name, environment_name ))
					{
						o_InfluenceEnvironments.insert( environment_name );
					}
				}
			}
		}
	}


}	// end of namespace
