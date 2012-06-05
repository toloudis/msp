/*****************************************************************************
**  smdlSubdivNetworkMgr.cpp
**
**	A smdlSubdivNetworkMgr keeps track of shared subdivision networks
**	to try to save overall memory usage.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlSubdivNetworkMgr.hpp"

#include "Core/Env/envSharedAssetMgr.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"


//============================================================================
//============================================================================
namespace
{
	//============================================================================
	//============================================================================
	struct SubdivAssetInfo
	{
		const fsLocator& m_Locator;
		const std::vector<shared_ptr<mdlSubdivInfo>>& m_SubdivInfos;
		int m_InitialSubdivLevel;
		int m_MaxSubdivLevel;

		bool operator==(const SubdivAssetInfo& i_Info) const
		{
			return (this->m_Locator == i_Info.m_Locator);
		}
		bool operator<(const SubdivAssetInfo& i_Info) const
		{
			return (this->m_Locator < i_Info.m_Locator);
		}
	};

	//============================================================================
	//============================================================================
	class SubdivNetworkSharedMgr : public envSharedAssetMgr<smdlSubdivNetworkSet, SubdivAssetInfo>
	{
	protected:
		//--------------------------------------------------------------------
		// This function needs to be implemented by base class, 
		//	load asset from file and return pointer to new asset class,
		//	allocated on heap.
		//--------------------------------------------------------------------
		virtual smdlSubdivNetworkSet* LoadAsset(const SubdivAssetInfo& i_SubdivInfo)
		{
			std::auto_ptr<smdlSubdivNetworkSet> pNetworkSet(new smdlSubdivNetworkSet());

			// Create shared networks for subdivisions
			const int num_subdivs = i_SubdivInfo.m_SubdivInfos.size();
			for (int i=0; i<num_subdivs; i++)
			{
				// create subdiv network
				const bool c_bMergeVertices = true; // close up seams
				smdlSubdivNetwork *pNetwork = new smdlSubdivNetwork(*i_SubdivInfo.m_SubdivInfos[i], 
																	i_SubdivInfo.m_InitialSubdivLevel,
																	i_SubdivInfo.m_MaxSubdivLevel,
																	c_bMergeVertices);
				pNetworkSet->m_SubdivNetworks.push_back( shared_ptr<smdlSubdivNetwork>(pNetwork) );
			}

			return pNetworkSet.release();
		}
	};

	SubdivNetworkSharedMgr l_SharedSubdivNetworkMgr;
}

//--------------------------------------------------------------------
// If this asset has already been loaded, return pointer to shared
//	instance and increment reference count. Otherwise, load the 
//	resource from the file and create a new instance of the 
//	asset class.
//--------------------------------------------------------------------
smdlSubdivNetworkSet* smdlSubdivNetworkMgr::CreateNetworks(const fsLocator& i_Locator,
									 const std::vector< shared_ptr<mdlSubdivInfo> >& i_SubdivInfos,
									 int i_InitialSubdivLevel,
									 int i_MaxSubdivLevel)
{
	SubdivAssetInfo subdiv_info = {i_Locator, i_SubdivInfos, i_InitialSubdivLevel, i_MaxSubdivLevel };
	return l_SharedSubdivNetworkMgr.Load( subdiv_info );
}

//--------------------------------------------------------------------
// Release asset. Decrements reference count - when reference 
//	count reaches zero, asset class instance is deleted.
//	Returns reference counts remaining or -1 if not found.
//--------------------------------------------------------------------
int smdlSubdivNetworkMgr::ReleaseNetworks(smdlSubdivNetworkSet* i_pNetworkSet)
{
	return l_SharedSubdivNetworkMgr.Release( i_pNetworkSet );
}

//--------------------------------------------------------------------
// Remove the given filename from shared management such that the
// next call to LoadModelTemplate() will reload the original
// geometry file.
//--------------------------------------------------------------------
void smdlSubdivNetworkMgr::StopSharing(const fsLocator& i_Locator)
{
	// comparison is based on filename, but have to construct a
	// subdiv info in order to refer to it through the shared manager
	std::vector<shared_ptr<mdlSubdivInfo>> empty_infos;
	SubdivAssetInfo subdiv_info = {i_Locator, empty_infos, 0, 0 };
	l_SharedSubdivNetworkMgr.StopSharing( subdiv_info );
}
