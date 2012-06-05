/*****************************************************************************
**	ltstInfluenceUtil.cpp
**
**	see hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/ltstInfluenceUtil.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/ltst/ltstLightSetsData.hpp"

#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace ltstInfluenceUtil
{

	namespace
	{
	}	// end of namespace


	//--------------------------------------------------------------------
	// Look for all pieces of geometry in the selection list
	// and then figure out which light sets influence these pieces of
	// geometry. Return the light sets in o_InfluenceLightSets.
	//--------------------------------------------------------------------
	void  GetLightSetsForSelectedObjects(std::set<nameString>& o_InfluenceLightSets)
	{
		const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			if ( nameObject *pNamedObject = sel3dCastUtil::CastPickObject<nameObject>(*sit) )
			{
				nameString item_name = pNamedObject->GetName();
				if (ltstLightSetMgr::IsObject(item_name))
				{
					std::vector<ltstLightSetObjectData> light_sets;
					ltstLightSetMgr::GetLightSetsForObject( item_name, light_sets );

					// See if this is an individual fragment selection
					int fragment_index = -1;
					if (fgmtPropertyObject *pSurfaceObject = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(*sit) )
					{
						if (fgmtScriptObject *pScriptObject = sel3dCastUtil::CastPickObject<fgmtScriptObject>(*sit))
						{
							fragment_index = pScriptObject->GetIndexForName(pSurfaceObject->GetName());
						}
					}

					for (int i=0; i<light_sets.size(); ++i)
					{
						// If we have chosen the whole object, accept all light sets that
						// influence any part of the object. Otherwise, only accept the
						// light set if the fragment index is one of the lit fragments.
						if ((fragment_index == -1) ||
							(envSTLHelpers::Contains(light_sets[i].m_LitFragmentIndices,fragment_index)))
						{
							o_InfluenceLightSets.insert( light_sets[i].m_Name );
						}
					}
				}
			}
		}
	}


}	// end of namespace
