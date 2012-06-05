/****************************************************************************\
**	smdlSubdivNetworkMgr.hpp
**
**	A smdlSubdivNetworkMgr keeps track of shared subdivision networks
**	to try to save overall memory usage.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SUBDIVNETWORKMGR_HPP
#error smdlSubdivNetworkMgr.hpp multiply included
#endif
#define SMDL_SUBDIVNETWORKMGR_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <vector>


//============================================================================
// forward declarations
//============================================================================
class fsLocator;
class mdlSubdivInfo;
class smdlSubdivNetwork;


//============================================================================
// Asset class representing the set of subdivision networks
//	generated from a CHX file.
//============================================================================
struct smdlSubdivNetworkSet
{
	std::vector<shared_ptr<smdlSubdivNetwork>> m_SubdivNetworks;
};


//============================================================================
//============================================================================
namespace smdlSubdivNetworkMgr
{
	//--------------------------------------------------------------------
	// If this asset has already been loaded, return pointer to shared
	//	instance and increment reference count. Otherwise, load the 
	//	resource from the file and create a new instance of the 
	//	asset class.
	//--------------------------------------------------------------------
	smdlSubdivNetworkSet* CreateNetworks(const fsLocator& i_Locator,
										 const std::vector< shared_ptr<mdlSubdivInfo> >& i_SubdivInfos,
										 int i_InitialSubdivLevel,
										 int i_MaxSubdivLevel);

	//--------------------------------------------------------------------
	// Release asset. Decrements reference count - when reference 
	//	count reaches zero, asset class instance is deleted.
	//	Returns reference counts remaining or -1 if not found.
	//--------------------------------------------------------------------
	int ReleaseNetworks(smdlSubdivNetworkSet* i_pNetworkSet);

	//--------------------------------------------------------------------
	// Remove the given filename from shared management such that the
	// next call to LoadModelTemplate() will reload the original
	// geometry file.
	//--------------------------------------------------------------------
	void StopSharing(const fsLocator& i_Locator);
}
