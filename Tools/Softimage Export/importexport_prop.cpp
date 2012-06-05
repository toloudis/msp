//*****************************************************************************
/*!	\file importexport_prop.cpp
 	\brief Defines the callbacks that define the layout and behavior of the 
 	Import Export Demo property page.
 */
//*****************************************************************************

#include <xsi_application.h>
#include <xsi_model.h>
#include <xsi_customproperty.h>
#include <xsi_parameter.h>
#include <xsi_string.h>
#include <xsi_griddata.h>
#include <xsi_value.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_actionsource.h>
#include <xsi_animationsourceitem.h>
#include <xsi_griddata.h>
#include <xsi_gridwidget.h>
#include <xsi_mixer.h>
#include <xsi_timecontrol.h>
#include "exportmesh_command.h"
#include "gxbexporter.h"
#include "helper.h"


#define LABEL_MIN 140
#define LABEL_RATIO 40

// define columns of animations list
#define ANIMATION_LIST_EXPORT_COL 0
#define ANIMATION_LIST_NAME_COL 1
#define ANIMATION_LIST_START_COL 2
#define ANIMATION_LIST_END_COL 3
#define ANIMATION_LIST_IKFREQ_COL 4


//*****************************************************************************
/*!	Helper function for accessing the Export custom property.
 */
//*****************************************************************************
XSI::CustomProperty GetImportExportProp()
{	
	XSI::Application app;
	XSI::Model root = app.GetActiveSceneRoot();
	XSI::CustomProperty prop = root.GetProperties().GetItem(L"MSPExportProp");
	if (!prop.IsValid())
	{
		prop = root.AddProperty( L"MSPExportProp" );
	}
	return prop;
}

//*****************************************************************************
/*!	Helper function for getting a parameter from the custom property.
	\param in_strName Name of the option to get.
 */
//*****************************************************************************
XSI::Parameter GetImportExportOption( const XSI::CString& in_strName )
{
	XSI::CustomProperty pset = GetImportExportProp();
	if (!pset.IsValid()) return XSI::CRef();
	
	XSI::Parameter param = pset.GetParameters().GetItem(in_strName);
	
	if (!param.IsValid())
	{	
		pset.AddParameter(
			in_strName,
			XSI::CValue::siInt4,
			XSI::siReadOnly,
			L"",
			L"",
			(LONG)0,
			(LONG)0,
			(LONG)100,
			(LONG)0,
			(LONG)100,
			param);	
	}
	return param;
}

struct AnimationEntry
{
	std::wstring animationName;
	long startFrame; 
	long endFrame; 
	long stepFrame;
};


void findAnimations( XSI::Model& model, std::vector<AnimationEntry> & animList)
{

	if ( model.HasMixer())
	{
		// Scan the mixer for all clips
		// At this point we're only interested in the top-level and do not
		// cascade into all clip containers, since we're interested in the
		// top-level timeline splits
		XSI::Mixer mixer = model.GetMixer();
		XSI::CRefArray clips = mixer.GetClips();
		for (int c = 0; c < clips.GetCount(); ++c)
		{
			XSI::Clip clip(clips[c]);
			XSI::CString clipType = clip.GetType();
			if (clipType == XSI::siClipAnimationType ||
				clipType == XSI::siClipShapeType  ||
				clipType == XSI::siClipAnimCompoundType || // nested fcurves
				clipType == XSI::siClipShapeCompoundType) // nested shape
			{
				XSI::TimeControl timeControl = clip.GetTimeControl();
				AnimationEntry anim;
				anim.animationName = _tstr(clip.GetName());
				anim.startFrame = static_cast<long>( timeControl.GetStartOffset() );
				long length = static_cast< long > ( (1.0f / timeControl.GetScale()) * 
					(timeControl.GetClipOut() - timeControl.GetClipIn() + 1) );
				anim.endFrame = anim.startFrame + length - 1;
				animList.push_back(anim);
			}
		}
		
	}
}

void getAnimations( XSI::X3DObject& root, std::vector<AnimationEntry> & animList)
{
	animList.clear();
	XSI::Model rmodel = root.GetModel();
	XSI::CString sname = root.GetName();
	const TCHAR *pzName = _tstr( sname );
	findAnimations(rmodel, animList);

	// Find all children (recursively)
	
	XSI::CRefArray children2 = root.FindChildren(L"", XSI::siModelType, XSI::CStringArray());
	if( children2.GetCount() > 0)
	{
		int i=0;
	}
	XSI::CRefArray children = root.FindChildren(L"", L"", XSI::CStringArray());
	for (int c = 0; c < children.GetCount(); ++c)
	{
		XSI::X3DObject child(children[c]);
		
		XSI::Model model = child.GetModel();
		getAnimations(child, animList);
	}
}


void populateAnimationsList(XSI::GridData gd)
{
	// 5 columns
	gd.PutColumnCount(4);

	// Export column is a check box
	gd.PutColumnType(ANIMATION_LIST_EXPORT_COL, XSI::siColumnBool);

	// Labels
	gd.PutColumnLabel(ANIMATION_LIST_EXPORT_COL, L"");
	gd.PutColumnLabel(ANIMATION_LIST_NAME_COL, L"Name");
	gd.PutColumnLabel(ANIMATION_LIST_START_COL, L"Start");
	gd.PutColumnLabel(ANIMATION_LIST_END_COL, L"End");


	XSI::Application app;
	XSI::Model appRoot(app.GetActiveSceneRoot());
	std::vector< AnimationEntry > animList; 
	getAnimations(appRoot, animList);
	gd.PutRowCount(  static_cast<LONG>(animList.size()) );
	long row = 0;
	std::vector< AnimationEntry >::const_iterator a;
	for ( a = animList.begin(); 
			a != animList.end(); ++a, ++row)
	{
		gd.PutCell(ANIMATION_LIST_NAME_COL, row, XSI::CString(a->animationName.c_str()));
		// default to export
		gd.PutCell(ANIMATION_LIST_EXPORT_COL, row, true);
		gd.PutCell(ANIMATION_LIST_START_COL, row, XSI::CValue((LONG)a->startFrame));
		gd.PutCell(ANIMATION_LIST_END_COL, row, XSI::CValue((LONG)a->endFrame));
	}
}
//*****************************************************************************
/*!	Callback that defines the parameters (options) of the custom property.
	\param in_ctxt The context that encapsulates the custom property to initialize.
 */
//*****************************************************************************

XSIPLUGINCALLBACK
XSI::CStatus MSPExportProp_Define( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );
	XSI::CustomProperty pset( ctxt.GetSource() );

	XSI::Parameter param;
	pset.AddParameter(L"ExportIntent",XSI::CValue::siInt4,XSI::siPersistable,L"",L"",(LONG)0,(LONG)0,(LONG)3,(LONG)0,(LONG)3,param);
	pset.AddParameter(L"Subd_type",XSI::CValue::siInt4,XSI::siPersistable,L"",L"",(LONG)3,(LONG)0,(LONG)3,(LONG)0,(LONG)3,param);	
	pset.AddParameter(L"Subd_level",XSI::CValue::siInt4,XSI::siPersistable,L"",L"",(LONG)0,(LONG)0,(LONG)6,(LONG)0,(LONG)6,param);
	pset.AddParameter( 
		GXBExportDoc::Texture_ExportFilepath::m_scriptName, 
		XSI::CValue::siInt4, 
		XSI::siPersistable,
		L"", L"", 
		GXBExportDoc::Texture_ExportFilepath::m_default, 
		(LONG)GXBExportDoc::Texture_ExportFilepath::m_min,
		(LONG)GXBExportDoc::Texture_ExportFilepath::m_max, 
		(LONG)GXBExportDoc::Texture_ExportFilepath::m_min, 
		(LONG)GXBExportDoc::Texture_ExportFilepath::m_max, 
		param
		);
	pset.AddParameter( 
		GXBExportDoc::MergeBasedOnMtls::m_scriptName, 
		XSI::CValue::siBool, 
		XSI::siPersistable,
		L"", L"", 
		GXBExportDoc::MergeBasedOnMtls::m_default,
		param
		);
	pset.AddParameter( 
		GXBExportDoc::MaxNumTrianglesInMergedMesh::m_scriptName, 
		XSI::CValue::siInt4, 
		XSI::siPersistable,
		L"", L"", 
		GXBExportDoc::MaxNumTrianglesInMergedMesh::m_default, 
		(LONG)GXBExportDoc::MaxNumTrianglesInMergedMesh::m_min,
		(LONG)GXBExportDoc::MaxNumTrianglesInMergedMesh::m_max, 
		(LONG)GXBExportDoc::MaxNumTrianglesInMergedMesh::m_min, 
		(LONG)GXBExportDoc::MaxNumTrianglesInMergedMesh::m_max, 
		param
		);
	pset.AddParameter( 
		GXBExportDoc::ExportGeomForVertexAnim::m_scriptName, 
		XSI::CValue::siBool, 
		XSI::siPersistable,
		L"", L"", 
		GXBExportDoc::ExportGeomForVertexAnim::m_default,
		param
		);
	pset.AddParameter( 
		GXBExportDoc::AnimExportStartFrame::m_scriptName, 
		XSI::CValue::siInt4, 
		XSI::siPersistable,
		L"", L"", 
		GXBExportDoc::AnimExportStartFrame::m_default, 
		param
		);	
	pset.AddParameter( 
		GXBExportDoc::AnimExportEndFrame::m_scriptName, 
		XSI::CValue::siInt4, 
		XSI::siPersistable,
		L"", L"", 
		GXBExportDoc::AnimExportEndFrame::m_default, 
		param
		);
	XSI::CValue dft;	// Used for arguments we don't want to set
	pset.AddParameter(	L"ExportFilename", XSI::CValue::siString, XSI::siPersistable, L"", L"", dft, param ) ;	

	XSI::Application app;
	XSI::CString strDefaultFile( app.GetInstallationPath( XSI::siUserPath ) );
#ifdef unix
	strDefaultFile += L"/mesh_data.gxb";
#else
	strDefaultFile += L"\\mesh_data.gxb";
#endif
	param.PutValue( strDefaultFile );
	
  return XSI::CStatus::OK;
}

//*****************************************************************************
/*!	Callback that defines the layout of controls on the property page.
	\param in_ctxt The context that encapsulates the custom property layout to 
	initialize.
 */
//*****************************************************************************
XSIPLUGINCALLBACK
XSI::CStatus MSPExportProp_DefineLayout( XSI::CRef& in_ctxt )
{	
	XSI::Context ctxt(in_ctxt);
	XSI::PPGLayout ppg = ctxt.GetSource() ;

	ppg.Clear();

	// define the export section
	ppg.AddGroup(L"MSP file export");


	XSI::PPGItem versionItem = ppg.AddStaticText( XSI::CString(L"Version: ") + PLUGIN_VERSION );

	LONG labelMinPixels = LABEL_MIN;
	LONG labelPercentage = LABEL_RATIO;

	

	XSI::PPGItem item = ppg.AddItem( L"ExportFilename", L"Export Filename", XSI::siControlFilePath ) ;
	// Export is the default option, so this control will give an "overwrite?" warning if
	// the file already exists.
	// Other attributes exist that are not demonstrated,
	// See the siPPGItemAttribute enum documentation for more info

	// If the parameter value is empty that the browser will open in the Models
	// subdirectory of the current project
	item.PutAttribute( XSI::siUIInitialDir, L"project" ) ;
	item.PutAttribute( XSI::siUISubFolder, L"Models" ) ;

	// This decides what file extentions to display
	item.PutAttribute( XSI::siUIFileFilter, L"MSP Export files (*.gxb)|*.gxb|(*.gab)|*.gab|(*.cam)|*.cam||" ) ;  

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );
	
	ppg.AddSpacer();


	XSI::CValueArray uiExportIntentItems(6) ;
	uiExportIntentItems[0] = L"ModelExport" ; 
	uiExportIntentItems[1] = (LONG)eModelExport;
	uiExportIntentItems[2] = L"VertexAnimation Export" ; 
	uiExportIntentItems[3] = (LONG)eVertexAnimExport;
	uiExportIntentItems[4] = L"CameraExport" ; 
	uiExportIntentItems[5] = (LONG)eCameraAnimExport;

	item = ppg.AddEnumControl(
		L"ExportIntent",
		uiExportIntentItems,
		L"Choice",
		XSI::siControlCombo ) ;
	item.PutLabel(L"Export Intent");

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );


#if defined(NEVER)

	XSI::CValueArray uiSubdTypeItems(6) ;
	uiSubdTypeItems[0] = L"Catmull-Clark" ; 
	uiSubdTypeItems[1] = (LONG)XSI::siCatmullClark;
	uiSubdTypeItems[2] = L"XSI-Doo-Sabin" ; 
	uiSubdTypeItems[3] = (LONG)XSI::siXSIDooSabin;
	uiSubdTypeItems[4] = L"Linear" ; 
	uiSubdTypeItems[5] = (LONG)XSI::siLinearSubdivision;

	item = ppg.AddEnumControl(
		L"Subd_type",
		uiSubdTypeItems,
		L"Choice",
		XSI::siControlCombo ) ;
	item.PutLabel(L"Subdivision Rule Type");

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );

	

	item = ppg.AddItem( L"Subd_level" ) ;
	item.PutLabel(L"Subdivision level");

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );
#endif
	ppg.AddGroup( XSI::CString(L"ModelExportIntent Options" ) );
	// NOTE: Previous versions had a limitation where 
	// the values had to match the index (0,1,2...) This is now fixed.
	XSI::CValueArray radioItems( 6 ) ;
	radioItems[0] = GXBExportDoc::Texture_ExportFilepath::m_valueNames[0];	radioItems[1] = (LONG)GXBExportDoc::Texture_ExportFilepath::eRelative  ;
	radioItems[2] = GXBExportDoc::Texture_ExportFilepath::m_valueNames[1];	radioItems[3] = (LONG)GXBExportDoc::Texture_ExportFilepath::eAbsolute ;
	radioItems[4] = GXBExportDoc::Texture_ExportFilepath::m_valueNames[2]; radioItems[5] = (LONG)GXBExportDoc::Texture_ExportFilepath::eCopyTextureToExportDir;
	item = ppg.AddEnumControl( GXBExportDoc::Texture_ExportFilepath::m_scriptName, radioItems, L"", XSI::siControlRadio ) ;
	
	item.PutLabel( GXBExportDoc::Texture_ExportFilepath::m_friendlyName );

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );

	item = ppg.AddEnumControl( GXBExportDoc::MergeBasedOnMtls::m_scriptName, NULL,GXBExportDoc::MergeBasedOnMtls::m_friendlyName , XSI::siControlBoolean ) ;
	
	item.PutLabel( GXBExportDoc::MergeBasedOnMtls::m_friendlyName );

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );
	
	item = ppg.AddItem( GXBExportDoc::MaxNumTrianglesInMergedMesh::m_scriptName,  GXBExportDoc::MaxNumTrianglesInMergedMesh::m_friendlyName, XSI::siControlNumber );
	item.PutAttribute( XSI::siUIDecimals, 0);
	item.PutAttribute( XSI::siUINoSlider, true);
	item.PutLabel( GXBExportDoc::MaxNumTrianglesInMergedMesh::m_friendlyName );
	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );

	item = ppg.AddEnumControl( GXBExportDoc::ExportGeomForVertexAnim::m_scriptName, NULL, L"", XSI::siControlBoolean ) ;
	
	item.PutLabel( GXBExportDoc::ExportGeomForVertexAnim::m_friendlyName );

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );

	ppg.EndGroup();
	
	ppg.AddGroup( XSI::CString(L"VertexAnim/CameraAnim Intent Options" ) );
	item = ppg.AddItem( GXBExportDoc::AnimExportStartFrame::m_scriptName,  GXBExportDoc::AnimExportStartFrame::m_friendlyName, XSI::siControlNumber );
	item.PutAttribute( XSI::siUIDecimals, 0);
	item.PutAttribute( XSI::siUINoSlider, true);
	item.PutLabel( GXBExportDoc::AnimExportStartFrame::m_friendlyName );
	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );


	item = ppg.AddItem( GXBExportDoc::AnimExportEndFrame::m_scriptName,  GXBExportDoc::AnimExportEndFrame::m_friendlyName, XSI::siControlNumber );
	item.PutAttribute( XSI::siUIDecimals, 0);
	item.PutAttribute( XSI::siUINoSlider, true);
	item.PutLabel( GXBExportDoc::AnimExportEndFrame::m_friendlyName );
	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );
	ppg.EndGroup();


	ppg.EndGroup();

  return XSI::CStatus::OK;
}

//*****************************************************************************
/*!	Callback that handles UI events (such as button clicks) on property page.
	page.
	\param in_ctxt The context that encapsulates the custom property property
	page event.
 */
//*****************************************************************************

XSIPLUGINCALLBACK
XSI::CStatus MSPExportProp_PPGEvent( const XSI::CRef& in_ctxt )
{
	XSI::Application app ;
	static bool hasSkel = false;

	XSI::PPGEventContext ctx( in_ctxt ) ;

	XSI::PPGEventContext::PPGEvent eventID = ctx.GetEventID() ;
	if ( eventID == XSI::PPGEventContext::siOnInit )
	{
		// This event meant that the UI was just created.
		// It gives us a chance to set some parameter values.
		// We could even change the layout completely at this point.
		AnimParams aparams;
		GXBExporter::GetAnimParameters( aparams );
		XSI::CustomProperty prop = ctx.GetSource() ;
		//Demonstrate how to use the PPGLayout to populate the items inside the ComboBox
		XSI::PPGLayout ppg = prop.GetPPGLayout();
		
		prop.GetParameter( GXBExportDoc::AnimExportStartFrame::m_scriptName ).PutValue( static_cast<LONG>(aparams.m_StartFrame) );
		prop.GetParameter( GXBExportDoc::AnimExportEndFrame::m_scriptName ).PutValue( static_cast<LONG>(aparams.m_EndFrame) );
		//Redraw the PPG to show the new combo items
		ctx.PutAttribute(L"Refresh",true);
	}
	return XSI::CStatus::OK ;
}
