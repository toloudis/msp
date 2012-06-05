/*****************************************************************************
**	api3dSharedModelMgr.cpp
**
**		api3dSharedModelMgr maintains the templates for the models loaded
**	so that a duplicate of a model can be made without loading the file again
**	and so that video memory is shared when possible.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dSharedModelMgr.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Env/envSharedAssetMgr.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Fs/fsResourceFinderDir.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/Gf/gfFileX.hpp"
#include "Graphics/Ent/entImport.hpp"
#include "Graphics/Ent/entModelTemplate.hpp"
#include "Graphics/smdl/private/smdlSubdivNetworkMgr.hpp"


//============================================================================
//============================================================================
namespace api3dSharedModelMgr
{
	namespace
	{
		//bga - 7/09 enabling file based instancing again...
		const bool c_UseSharedAssetMgr = true;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		class SharedModelMgr : public envSharedAssetMgr<entModelTemplate, fsLocator>
		{
		public:
			//----------------------------------------------------------------
			// Load Animation from filename
			//----------------------------------------------------------------
			virtual entModelTemplate* LoadAsset(const fsLocator& i_Locator)
			{
				DBG_TRACE("Loading file: " << i_Locator);

				// Should we do anything here to check lower case?
				return entImport::LoadGeometry(i_Locator);
			}
		};
		SharedModelMgr l_SharedModelMgr;
	}

	//========================================================================
	// See if the file has already been loaded. If so, return that
	// model template. Otherwise, load the geometry file.
	//========================================================================
	entModelTemplate* LoadModelTemplate(const fsLocator& i_Locator)
	{
		try
		{
			if (c_UseSharedAssetMgr)
				return l_SharedModelMgr.Load(i_Locator);
			else
				return l_SharedModelMgr.LoadAsset(i_Locator);
		}
		catch ( const envExceptionX& i_Ex)
		{
			DBG_ERROR("Problem occurred loading geometry from: " << i_Ex.GetErrorMessage());
		}
		catch (const std::bad_alloc&)
		{
			DBG_ERROR("Out of memory while loading " << i_Locator);
		}
		catch (...)
		{
			DBG_ERROR("General exception while loading "  << i_Locator);
		}
		return NULL;
	}

	//========================================================================
	// Release a usage of the template. If the usage count goes to
	// zero, then the template is deleted. 
	// Returns reference counts remaining or -1 if not found.
	//========================================================================
	int ReleaseModelTemplate(entModelTemplate* i_ModelTemplate)
	{
		if (c_UseSharedAssetMgr)
			return l_SharedModelMgr.Release(i_ModelTemplate);
		else
		{
			// No sharing so just delete the template
			delete i_ModelTemplate;
			return 0;
		}	
	}

	//========================================================================
	// Remove the given filename from shared management such that the
	// next call to LoadModelTemplate() will reload the original
	// geometry file.
	//========================================================================
	void StopSharing(const fsLocator& i_Locator)
	{
		if (c_UseSharedAssetMgr)
			l_SharedModelMgr.StopSharing(i_Locator);

		// Also have to stop sharing the subdivision networks
		// because the subdiv info may change when reloading the new geometry.
		smdlSubdivNetworkMgr::StopSharing(i_Locator);
	}
}
