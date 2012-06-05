#include <stdafx.h>
#include "ExportStudioGPU.h"
#include "ExportMesh.h"

namespace
{

void
ShowMessage( const wchar_t* message, bool isInteractive, const wchar_t* pluginName )
{
	if (isInteractive)
		RhinoMessageBox( message, pluginName, MB_OK );
	else
		RhinoApp().Print( L"%s: %s\n", message, pluginName );
}

};

bool
ExportStudioGPU(CRhinoDoc& doc, ON_wString filename, bool onlySelected, bool isInteractive, const wchar_t* pluginName, bool& isCancelled)
{
	isCancelled = false;

	Log log;

	wchar_t drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	_wsplitpath( filename, drive, dir, fname, ext );


	ON_wString logname = drive;
	logname += dir;
	logname += fname;
	logname += L".log";
	log.StartLog( logname, L"Export into StudioGPU format" );

	
	ON_SimpleArray<const CRhinoObject*> objects;

	CRhinoObjectIterator oIter(doc, CRhinoObjectIterator::undeleted_objects);
	if (onlySelected)
		oIter.EnableSelectedFilter();
	oIter.EnableVisibleFilter();

	CRhinoObject* obj;
	for ( obj = oIter.First(); obj; obj = oIter.Next() )
	{
		objects.Append( obj );
	}


	const CRhinoAppRenderMeshSettings& rms = RhinoApp().AppSettings().RenderMeshSettings();
    //ON_MeshParameters mp = rms.QualityMeshParameters();
    ON_MeshParameters mp = rms.FastMeshParameters();

    // Set the user interface style.
	int ui_style = isInteractive?0:2; // simple ui

    ON_ClassArray<CRhinoObjectMesh> meshes;
	ExportMesh eMesh;

    // Mesh the selected objects.
    CRhinoCommand::result rc = RhinoMeshObjects( objects, mp, ui_style, meshes );
    if( rc != CRhinoCommand::success )
	{
		if ( rc == CRhinoCommand::cancel )
		{
			log.WriteLog( L"Operation Cancelled\n" );
			isCancelled = true;
		}
		else if ( rc == CRhinoCommand::nothing )
		{
			log.WriteLog( L"No valid objects to export\n" );
			ShowMessage( L"No valid objects to export", isInteractive, pluginName );
			isCancelled = false;
			return true;
		}
		else
		{
			log.WriteLog( L"Triangulation failed\n" );
			ShowMessage( L"Triangulation failed", isInteractive, pluginName );
			isCancelled = false;
		}

		return false;
	}
	
	//Start export meshes
	if (!eMesh.StartExport(filename))
	{
		log.WriteLog( L"Internal plug-in error (GXB file export)\n" );
		ShowMessage( L"Internal plug-in error (GXB file export)", isInteractive, pluginName );
		return false;
	}

	//Add meshes to export object
	int i;
	for( i = 0; i < meshes.Count(); i++ )
	{
		if (!eMesh.AddMesh( doc, meshes[i], log ))
		{
			if (!meshes[i].m_mesh_attributes.m_name.Compare(""))
				log.WriteLog( L"Export mesh failed (no name)\n" );
			else
				log.WriteLog( L"Export mesh failed (name: %s)\n", meshes[i].m_mesh_attributes.m_name.operator const wchar_t*() );
			return false;
		}
	}

	//Finalize exporting
	if (!eMesh.FinishExport())
	{
		log.WriteLog( L"Scene export failed (GXB file export error)!\n" );
		ShowMessage( L"Scene export failed (GXB file export error)!", isInteractive, pluginName );
		return false;
	}

	//RhinoApp().Print( L"Export completed successfully\n" );

	return true;
}
