#include "plgLibraryMgr.hpp"
#include "plgPlugIn.hpp"

#include <vector>


namespace plgLibraryMgrData
{
	std::vector<plgPlugIn*>	m_Libraries;
}

namespace plgLibraryMgr
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int LoadPlugIn( plgPlugIn* i_pPlugIn )
{
	if ( i_pPlugIn == 0 ) return -1;	// throw an error

	HINSTANCE hDLL = 0;
	hDLL = LoadLibrary( i_pPlugIn->GetFileName().c_str() );

	if ( hDLL == 0 ) return -1;			// throw an error

	i_pPlugIn->SetDLLHandle( hDLL );

	plgLibraryMgrData::m_Libraries.push_back( i_pPlugIn );

	return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int FreePlugIn(const std::string& i_LibraryFileName)
{
	//plgPlugIn* pPlugIn;

	//plgLibraryMgrData::m_Libraries.push_back( pPlugIn );

	return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int FreeAllPlugIns()
{
	//plgPlugIn* pPlugIn;

	//plgLibraryMgrData::m_Libraries.push_back( pPlugIn );

	return 0;
}

}
