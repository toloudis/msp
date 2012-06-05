#include "plgPlugIn.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
plgPlugIn::plgPlugIn():
	m_pfnLibraryFuncInit(0),
	m_pfnLibraryFuncCleanUp(0)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plgPlugIn::~plgPlugIn()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plgPlugIn::SetDLLHandle( HINSTANCE i_hDLL )
{
	m_hDLLHandle = i_hDLL;

	m_pfnLibraryFuncInit	= (t_pfnLIBRARYFUNC)GetProcAddress(i_hDLL, "LibraryInit");
	m_pfnLibraryFuncCleanUp	= (t_pfnLIBRARYFUNC)GetProcAddress(i_hDLL, "LibraryCleanUp");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plgPlugIn::SetFileName( const std::string& i_FileName )
{
	m_PlugInFilename = i_FileName;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
std::string& plgPlugIn::GetFileName()
{
	return m_PlugInFilename;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
int plgPlugIn::LibraryInit()
{
	if ( m_pfnLibraryFuncInit == 0 )
	{
		return -1;
	}
	else
	{
		return m_pfnLibraryFuncInit();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int plgPlugIn::LibraryCleanUp()
{
	if ( m_pfnLibraryFuncCleanUp == 0 )
	{
		return -1;
	}
	else
	{
		return m_pfnLibraryFuncCleanUp();
	}
}

