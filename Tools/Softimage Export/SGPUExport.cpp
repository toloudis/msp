//*****************************************************************************
/*!	\file SGPUExport.cpp
 	\brief Defines the entry point required for loading the DemoImportExport
 	plug-in.
 */
//*****************************************************************************

#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_decl.h>

#ifdef _WIN32
#include <windows.h>

//Support plug-in presence in memory for installer
class AutoMutex
{
public:
	AutoMutex()
	{
		m_mutex = CreateMutex( 0, 0, TEXT("{05C99F22-F6F8-4a6b-8AD3-59C6F8A88A5A}") );
	}

	~AutoMutex()
	{
		CloseHandle( m_mutex );
	}

private:
	HANDLE m_mutex;
};


AutoMutex g_mutex;

#endif

using namespace XSI; 

//*****************************************************************************
/*!	Register the commands, menus, and properties that implement this plug-in.
	\param in_reg The PluginRegistrar created by Softimage for this plug-in.
 */
//*****************************************************************************

XSIPLUGINCALLBACK CStatus XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"StudioGPU");
	in_reg.PutName(L"MSP Export Plug-in");
	in_reg.PutVersion(1,0);
	
	// Register commands for importing and exporting a polygon mesh
	in_reg.RegisterCommand(L"MSPExportMesh");

	// Install a menu for the export tool
	in_reg.RegisterMenu(siMenuMainFileExportID, L"MSP Export Tool", false,false);

	// Register a custom property to use as the import/export UI	
	in_reg.RegisterProperty(L"MSPExportProp");

	// Register a custom property to use as the import/export UI	
	in_reg.RegisterProperty(L"MSPMeshflags");

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	return CStatus::OK;
}


int main()
{
}