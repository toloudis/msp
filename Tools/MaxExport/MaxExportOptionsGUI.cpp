
/*****************************************************************************
**  MaxExportOptionsGUI.cpp
**
**	class which holds the GUI and data-binding members of
**  the max export options
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MaxExportOptionsGUI.hpp"
#include "MaxExportUtils.hpp"
#include "SgpuMaxExportVersion.hpp"
#include "SgpuExportLib/include/SgpuExportLibVersion.hpp"

#include "Core/env/envString.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsXMLWriter.hpp"

#include <string>

using namespace std;
using namespace MaxExp;





// Interface * maxInterface,  DWORD genericOpts 
SgpuExportOptionsGUI::SgpuExportOptionsGUI()
{
	//Init ought to have the value returned
	//by GetCOREInterface as its first argument.
	//But since SgpuExportOptionsGUI will be statically initialized,
	//because of the m_TheSgpuExportOptionsGUI member,
	//at the time of this static initialization,
	//(which happens when the plugin is loaded)
	//the value returned by GetCOREInterface() is
	//not fully working. 
	//So just pass in a NULL
	Init( NULL, 0 );		
}


SgpuExportOptionsGUI::~SgpuExportOptionsGUI()
{
	Cleanup();
}

void SgpuExportOptionsGUI::Init( Interface *i_pMaxInterface, DWORD i_GenericOpts )
{

	bool bExportSelected =  (i_GenericOpts & SCENE_EXPORT_SELECTED) == SCENE_EXPORT_SELECTED;
	m_Options.Init( bExportSelected );
	if( NULL != i_pMaxInterface )
	{
		int dTicks = GetTicksPerFrame();
		Interval iv = i_pMaxInterface->GetAnimRange();
		TimeValue startTime = iv.Start();
		TimeValue endTime = iv.End();
		float fRendImageAspect = i_pMaxInterface->GetRendImageAspect();
		float fRendApertureWidth = i_pMaxInterface->GetRendApertureWidth();
		m_Options.InitAnimRangeParams( dTicks, startTime, endTime );
		m_Options.InitRenderCameraParams( fRendImageAspect, fRendApertureWidth );
		const   MSTR &tstrInputFileName = GetCOREInterface()->GetCurFilePath();
		int nRet = MbcsToUnicode( tstrInputFileName.data(), m_Options.m_wsCurMaxFilepath );
		if( nRet < 0 )
		{
			DBG_WARNING( " Cannot get unicode version of current filename: " << tstrInputFileName.data() );
		}
	}
}


bool SgpuExportOptionsGUI::ShowDialog()
{
	BOOL doExport = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_SGPU_EXPORT_OPTIONS), GetCOREInterface()->GetMAXHWnd(), (DLGPROC)SgpuExportOptionsGUI::ExportOptionsDlgProcS, (LPARAM)this) != FALSE;
	if (!doExport) return false;
	//SaveOptions();

	return true;
}

void SgpuExportOptionsGUI::EnableDlgButton(HWND hDlg, int nIDDlgItem, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, nIDDlgItem), bEnable);
}

INT_PTR SgpuExportOptionsGUI::ExportOptionsDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM)
{
	float fStartFrame;
	float fEndFrame;
	BOOL bTranslated;
	char pzTempBuf[256];
	std::string sVersion("Exporter Dll Version: " );
	sVersion += SGPU_MAX_EXPORTER_VERSION;
	switch (message)
	{
	case WM_INITDIALOG: {
		CenterWindow(hWnd, GetParent(hWnd));  
		SetDlgItemText( hWnd, IDC_SGPU_EXPORT_VERSION, sVersion.c_str() );
		//set up radio button		
		CheckDlgButton(hWnd, IDC_MODEL_EXPORT, false);
		CheckDlgButton(hWnd, IDC_VERT_ANIM_EXPORT, false);
		CheckDlgButton(hWnd, IDC_CAMERA_ANIM_EXPORT, false);
		CheckDlgButton(hWnd, IDC_MERGE_BASED_ON_MTLS, false );
		switch( m_Options.m_eExportIntent)
		{
		
		case eVertAnim:			
			CheckDlgButton(hWnd, IDC_VERT_ANIM_EXPORT, true);
			break;
		case eCameraAnim:				
			CheckDlgButton(hWnd, IDC_CAMERA_ANIM_EXPORT, true);
			break;
		default:
		case eModel:
			CheckDlgButton(hWnd, IDC_MODEL_EXPORT, true);
			break;
		}
		
		// setup checkboxes
		CheckDlgButton(hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS, m_Options.m_bExportChildIfParentExports );						
		CheckDlgButton(hWnd, IDC_MERGE_BASED_ON_MTLS, m_Options.m_bMergeBasedOnMtls );				
		CheckDlgButton(hWnd, IDC_SGPU_EXPORT_GEOM, m_Options.m_bExportGeom );
		SetDlgItemInt(hWnd, IDC_EDIT_START_FRAME, static_cast< unsigned int>( m_Options.m_fStartFrame ), false);
		SetDlgItemInt(hWnd, IDC_EDIT_END_FRAME, static_cast< unsigned int>( m_Options.m_fEndFrame ), false);
		SetDlgItemInt(hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH,  static_cast< unsigned int > ( m_Options.m_nMaxNumTrianglesInMergedMesh ), false );
		EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, false );		
		SetDlgItemText( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, _gcvt( m_Options.m_fToleranceVertexAnim, 12, pzTempBuf ) );
		CheckDlgButton(hWnd, IDC_VERTEX_ANIM_COMPRESSION, m_Options.m_bCompressVertexAnim );
		if (  IsDlgButtonChecked( hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS ) == BST_CHECKED )
		{					
			EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, true );						
		}	
		if( m_Options.m_eExportIntent != eVertAnim )
		{
			EnableDlgButton( hWnd, IDC_SGPU_EXPORT_GEOM, false );
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMPRESSION, false );
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, false );
		} 
		if( m_Options.m_eExportIntent != eVertAnim || m_Options.m_eExportIntent != eCameraAnim )
		{
			EnableDlgButton( hWnd, IDC_EDIT_START_FRAME, false);
			EnableDlgButton( hWnd, IDC_EDIT_END_FRAME, false );
		} 
		if( m_Options.m_eExportIntent != eModel )
		{
			EnableDlgButton( hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS, false );
			EnableDlgButton( hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS, false );
			EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, false );
		}

		return TRUE;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
	case IDC_MODEL_EXPORT:
		if( IsDlgButtonChecked( hWnd, IDC_MODEL_EXPORT) == BST_CHECKED )
		{
			CheckDlgButton(hWnd, IDC_VERT_ANIM_EXPORT, false);
			CheckDlgButton(hWnd, IDC_CAMERA_ANIM_EXPORT, false);		
			CheckDlgButton(hWnd, IDC_SGPU_EXPORT_GEOM, m_Options.m_bExportGeom);
			EnableDlgButton( hWnd, IDC_SGPU_EXPORT_GEOM, false );				
			SetDlgItemInt(hWnd, IDC_EDIT_START_FRAME, static_cast< unsigned int>( m_Options.m_fStartFrame ), false);
			SetDlgItemInt(hWnd, IDC_EDIT_END_FRAME, static_cast< unsigned int>( m_Options.m_fEndFrame ), false);
			EnableDlgButton( hWnd, IDC_EDIT_START_FRAME, false);
			EnableDlgButton( hWnd, IDC_EDIT_END_FRAME, false );
			CheckDlgButton(hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS, m_Options.m_bExportChildIfParentExports );	
			EnableDlgButton( hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS, true );
			CheckDlgButton(hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS, m_Options.m_bMergeBasedOnMtls );	
			EnableDlgButton( hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS, true );
			SetDlgItemInt(hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH,  static_cast< unsigned int > ( m_Options.m_nMaxNumTrianglesInMergedMesh ), false );			
			EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, false );
			if( m_Options.m_bMergeBasedOnMtls )
			{				
				EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, true );						
			}
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMPRESSION, false );
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, false );
			
		}

		break;
	case IDC_VERT_ANIM_EXPORT:
			if( IsDlgButtonChecked( hWnd, IDC_VERT_ANIM_EXPORT) == BST_CHECKED )
		 {
			CheckDlgButton(hWnd, IDC_MODEL_EXPORT, false);			
			CheckDlgButton(hWnd, IDC_CAMERA_ANIM_EXPORT, false);
			CheckDlgButton(hWnd, IDC_SGPU_EXPORT_GEOM, m_Options.m_bExportGeom);
			EnableDlgButton( hWnd, IDC_SGPU_EXPORT_GEOM, true );			
			SetDlgItemInt(hWnd, IDC_EDIT_START_FRAME, static_cast< unsigned int>( m_Options.m_fStartFrame ), false);
			SetDlgItemInt(hWnd, IDC_EDIT_END_FRAME, static_cast< unsigned int>( m_Options.m_fEndFrame ), false);
			EnableDlgButton( hWnd, IDC_EDIT_START_FRAME, true);
			EnableDlgButton( hWnd, IDC_EDIT_END_FRAME, true);
			CheckDlgButton(hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS,  m_Options.m_bExportChildIfParentExports );	
			EnableDlgButton( hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS, false );
			CheckDlgButton(hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS, m_Options.m_bMergeBasedOnMtls );	
			EnableDlgButton( hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS, false );
			SetDlgItemInt(hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH,  static_cast< unsigned int > ( m_Options.m_nMaxNumTrianglesInMergedMesh ), false );			
			EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, false );		
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMPRESSION, true );
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, false );	
			if (  IsDlgButtonChecked( hWnd, IDC_VERTEX_ANIM_COMPRESSION ) == BST_CHECKED )
			{					
				EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, true );						
			}	
		 }
		break;
	case IDC_CAMERA_ANIM_EXPORT:
			if( IsDlgButtonChecked( hWnd, IDC_CAMERA_ANIM_EXPORT) == BST_CHECKED )
		 {
			 CheckDlgButton(hWnd, IDC_MODEL_EXPORT, false);			
			 CheckDlgButton(hWnd, IDC_VERT_ANIM_EXPORT, false);
			 CheckDlgButton(hWnd, IDC_SGPU_EXPORT_GEOM, m_Options.m_bExportGeom);
			 EnableDlgButton( hWnd, IDC_SGPU_EXPORT_GEOM, false );	
			 SetDlgItemInt(hWnd, IDC_EDIT_START_FRAME, static_cast< unsigned int>( m_Options.m_fStartFrame ), false);
			 SetDlgItemInt(hWnd, IDC_EDIT_END_FRAME, static_cast< unsigned int>( m_Options.m_fEndFrame ), false);
			 EnableDlgButton( hWnd, IDC_EDIT_START_FRAME, true);
			 EnableDlgButton( hWnd, IDC_EDIT_END_FRAME, true);
			 CheckDlgButton(hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS,  m_Options.m_bExportChildIfParentExports );	
			 EnableDlgButton( hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS, false );	
			 CheckDlgButton(hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS, m_Options.m_bMergeBasedOnMtls );	
			 EnableDlgButton( hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS, false );
			 SetDlgItemInt(hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH,  static_cast< unsigned int > ( m_Options.m_nMaxNumTrianglesInMergedMesh ), false );			
			 EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, false );			
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMPRESSION, false );
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, false );

		 }
		break;
	case IDC_SGPU_MERGE_BASED_ON_MTLS:
			EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, false );							
			if (  IsDlgButtonChecked( hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS ) == BST_CHECKED )
			{					
				EnableDlgButton( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, true );						
			}	
		break;
	
	case IDC_VERTEX_ANIM_COMPRESSION:
			EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, false );							
			if (  IsDlgButtonChecked( hWnd, IDC_VERTEX_ANIM_COMPRESSION ) == BST_CHECKED )
			{					
				EnableDlgButton( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, true );						
			}	
		break;
	case IDC_SGPU_EXPORT_GEOM:
		break;
	case IDOK:
		m_Options.m_bExportChildIfParentExports = IsDlgButtonChecked( hWnd, IDC_SGPU_EXPORT_CHILD_IF_PARENT_EXPORTS ) == BST_CHECKED;
		m_Options.m_bExportGeom = IsDlgButtonChecked( hWnd, IDC_SGPU_EXPORT_GEOM ) == BST_CHECKED;
		bTranslated=false;
		fStartFrame = static_cast< float > ( GetDlgItemInt( hWnd, IDC_EDIT_START_FRAME, &bTranslated, false ) );
		m_Options.m_fStartFrame = ( bTranslated ) ? fStartFrame : m_Options.m_fStartFrame;
		m_Options.m_fStartFrame = SgpuConvert::Clamp( m_Options.m_fStartFrame, 0.0f, m_Options.m_fMaxEndFrame);
		fEndFrame = static_cast< float > ( GetDlgItemInt( hWnd, IDC_EDIT_END_FRAME, &bTranslated, false ) );	
		m_Options.m_fEndFrame = ( bTranslated ) ? fEndFrame : m_Options.m_fEndFrame;
		m_Options.m_fEndFrame = SgpuConvert::Clamp( m_Options.m_fEndFrame, 0.0f, m_Options.m_fMaxEndFrame);
		m_Options.m_StartTime = m_Options.m_fStartFrame * GetTicksPerFrame();
		m_Options.m_EndTime = m_Options.m_fEndFrame * GetTicksPerFrame();
		m_Options.m_bMergeBasedOnMtls = IsDlgButtonChecked( hWnd, IDC_SGPU_MERGE_BASED_ON_MTLS ) == BST_CHECKED;
		m_Options.m_nMaxNumTrianglesInMergedMesh = GetDlgItemInt( hWnd, IDC_SGPU_MAX_NUM_TRI_IN_MERGED_MESH, &bTranslated, false );
		m_Options.m_bCompressVertexAnim = IsDlgButtonChecked( hWnd, IDC_VERTEX_ANIM_COMPRESSION ) == BST_CHECKED;
		GetDlgItemText( hWnd, IDC_VERTEX_ANIM_COMP_TOLERANCE, pzTempBuf, 256 );
		m_Options.m_fToleranceVertexAnim = static_cast< float > ( atof( pzTempBuf ) );
		if( IsDlgButtonChecked( hWnd, IDC_VERT_ANIM_EXPORT) == BST_CHECKED )
		{
			m_Options.m_eExportIntent = eVertAnim;
		}
		else if( IsDlgButtonChecked( hWnd, IDC_CAMERA_ANIM_EXPORT) == BST_CHECKED )
		{
			m_Options.m_eExportIntent = eCameraAnim;
		} else
		{
			m_Options.m_eExportIntent = eModel;
		}
		EndDialog(hWnd, 1);
		break;

	case IDCANCEL:
		EndDialog(hWnd, 0);
		break;
	default:
		break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


INT_PTR CALLBACK SgpuExportOptionsGUI::ExportOptionsDlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	SgpuExportOptionsGUI* exp;
	if (message == WM_INITDIALOG)
	{
		exp = (SgpuExportOptionsGUI*) lParam;
		SetWindowLongPtr(hWnd, GWLP_USERDATA,lParam); 
	}
	else
	{
		exp = (SgpuExportOptionsGUI*)(size_t) GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}
	return exp ? exp->ExportOptionsDlgProc(hWnd, message, wParam, lParam) : FALSE;
}
