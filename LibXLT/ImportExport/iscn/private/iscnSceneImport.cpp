/****************************************************************************\
**  iscnSceneImport.cpp
**
**      see iscnSceneImport.hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ISCN_SCENEIMPORT_HPP
#include "ImportExport/iscn/iscnSceneImport.hpp"
#endif
#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <algorithm>
#include <vector>

namespace iscnSceneImport
{
	namespace
	{
		std::vector<iscnSceneImportImpl*> l_pImplementations;

		iscnSceneImportImpl* find_implementation(const fsLocator& i_File)
		{
			for (iscnSceneImportImpl* pImpl : l_pImplementations)
			{
				if (pImpl->IsRecognized(i_File))
					return pImpl;
			}
			return NULL;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddImplementation(iscnSceneImportImpl* i_pImplementation)
	{
		l_pImplementations.push_back(i_pImplementation);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RemoveImplementation(iscnSceneImportImpl* i_pImplementation)
	{
		std::vector<iscnSceneImportImpl*>::iterator it =
			std::find(l_pImplementations.begin(), l_pImplementations.end(), i_pImplementation);
		if (it != l_pImplementations.end())
		{
			delete *it;
			l_pImplementations.erase(it);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool IsRecognized(const fsLocator& i_File)
	{
		return find_implementation(i_File) != NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::string GetFilePatterns()
	{
		std::string patterns;
		for (iscnSceneImportImpl* pImpl : l_pImplementations)
		{
			if (!patterns.empty())
				patterns += ";";
			patterns += pImpl->GetFilePatterns();
		}
		return patterns;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool Read(const fsLocator& i_File, const iscnSceneSettings& i_Settings, iscnSceneDesc& o_Scene)
	{
		o_Scene = iscnSceneDesc();
		iscnSceneImportImpl* pImpl = find_implementation(i_File);
		if (!pImpl)
		{
			DBG_ERROR("Unrecognized scene file format " << i_File);
			return false;
		}
		return pImpl->Read(i_File, i_Settings, o_Scene);
	}
}
