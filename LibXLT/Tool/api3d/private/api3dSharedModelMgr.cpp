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
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif


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

		//--------------------------------------------------------------------
		// A part of a file: its templates are shared apart from the whole
		// file's
		//--------------------------------------------------------------------
		struct PartLocator
		{
			fsLocator m_Locator;
			std::string m_Part;

			bool operator < (const PartLocator& i_Other) const
			{
				if (m_Locator < i_Other.m_Locator)
					return true;
				if (i_Other.m_Locator < m_Locator)
					return false;
				return m_Part < i_Other.m_Part;
			}
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		class SharedPartMgr : public envSharedAssetMgr<entModelTemplate, PartLocator>
		{
		public:
			//----------------------------------------------------------------
			// Load one part of a file
			//----------------------------------------------------------------
			virtual entModelTemplate* LoadAsset(const PartLocator& i_Locator)
			{
				DBG_TRACE("Loading file: " << i_Locator.m_Locator << " (" << i_Locator.m_Part.c_str() << ")");
				return entImport::LoadGeometry(i_Locator.m_Locator, i_Locator.m_Part);
			}
		};
		SharedPartMgr l_SharedPartMgr;
	}

	//========================================================================
	// See if the file has already been loaded. If so, return that
	// model template. Otherwise, load the geometry file.
	//========================================================================
	entModelTemplate* LoadModelTemplate(const fsLocator& i_Locator, const std::string& i_Part)
	{
		try
		{
			if (!i_Part.empty())
			{
				const PartLocator part = { i_Locator, i_Part };
				if (c_UseSharedAssetMgr)
					return l_SharedPartMgr.Load(part);
				else
					return l_SharedPartMgr.LoadAsset(part);
			}
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
		{
			const int count = l_SharedModelMgr.Release(i_ModelTemplate);
			if (count >= 0)
				return count;
			return l_SharedPartMgr.Release(i_ModelTemplate);
		}
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
	void StopSharing(const fsLocator& i_Locator, const std::string& i_Part)
	{
		if (c_UseSharedAssetMgr)
		{
			if (i_Part.empty())
				l_SharedModelMgr.StopSharing(i_Locator);
			else
				l_SharedPartMgr.StopSharing(PartLocator{ i_Locator, i_Part });
		}

		// Also have to stop sharing the subdivision networks
		// because the subdiv info may change when reloading the new geometry.
		smdlSubdivNetworkMgr::StopSharing(i_Locator);
	}
}
