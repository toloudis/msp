/*****************************************************************************
**  rpnCameraChoice.hpp
**
**     Choice box displaying current and available cameras and
**	director's cuts
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/qtGUI/rpnCameraChoice.hpp"

#include "Features/RenderPanels/rpnOperations.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"

#ifdef USE_QT

//--------------------------------------------------------------------
//	Static pointer to the instance of the control, will be cleared
//	when the panel grid is deleted.
//--------------------------------------------------------------------
rpnCameraChoice* rpnCameraChoice::Instance = NULL;

//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
rpnCameraChoice::rpnCameraChoice(QWidget* parent)
:	QComboBox(parent),
	m_CameraLastIndex(0),
	m_bDisableNotify(false)
{
	this->setMinimumWidth(160);
	this->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	QObject::connect(this, QOverload<int>::of(&QComboBox::activated), [this](int) { SelectedIndexChanged(); });
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rpnCameraChoice::~rpnCameraChoice()
{
	if (rpnCameraChoice::Instance == this)
		rpnCameraChoice::Instance = NULL;
}

//------------------------------------------------------------------------
// Set list of camera names
//------------------------------------------------------------------------
void rpnCameraChoice::Update()
{			
	// listbox_cameras
	this->m_bDisableNotify = true;
	this->clear();
	this->addItem("Editor Camera");
	this->m_bDisableNotify = false;

	nameString name_str;
	std::string desc;
	// Add names of cameras
	const int num_cameras = camsCameraMgr::GetNumCameras();
	for (int i=0; i<num_cameras; i++)
	{
		camsCameraMgr::GetCameraName(i, name_str);
		camsCameraMgr::GetCameraDescription(i, desc);
		QString str = QString("%1 - %2").arg(tqtQString(name_str.GetString()), tqtQString(desc));
		this->addItem( str );
	}
	// Add names of directors cuts
	const int num_dcuts = camsDirectorsCutMgr::GetNumDirectorsCuts();
	for (int i=0; i<num_dcuts; i++)
	{
		camsDirectorsCutMgr::GetDirectorsCutName(i, name_str);
		camsDirectorsCutMgr::GetDirectorsCutDescription(i, desc);
		QString str = QString("%1 - %2").arg(tqtQString(name_str.GetString()), tqtQString(desc));
		this->addItem( str );
	}

	// Select EditorCamera by default, the render panels will set the 
	// camera index when things change.
	SetSelectedIndex(0);
}

//------------------------------------------------------------------------
// This function is called from outside when a render panel
// changes, all we need to do is set the index, not do any notifying
//------------------------------------------------------------------------
void rpnCameraChoice::SetSelectedIndex(int i_Index)
{
	this->m_bDisableNotify = true;
	this->setCurrentIndex(i_Index);
	if ( i_Index > 0 )
		m_CameraLastIndex = i_Index;
	this->m_bDisableNotify = false;
}

//------------------------------------------------------------------------
// Swap editor and scripted camera
//------------------------------------------------------------------------
void rpnCameraChoice::DoSwapCam()
{			
	// store the current selected index BEFORE it changes.
	int sel_index = this->currentIndex();

	//	only save the selected index if it ISN'T the editor cam.
	//
	if ( sel_index != m_CameraLastIndex )
	{
		if (m_CameraLastIndex < this->count())
		{
			this->setCurrentIndex( m_CameraLastIndex );
			update_selected_index();
		}
	}
	else
	{
		select_editor_cam();
	}

}


//------------------------------------------------------------------------
// Set camera in render panels based on selection
//------------------------------------------------------------------------
int rpnCameraChoice::update_selected_index()
{
	int sel_index = this->currentIndex();

	if (!this->m_bDisableNotify)
	{
		if (sel_index == 0)
		{
			rpnOperations::ChangeCameraToEditor();
		}
		else if (sel_index > 0)
		{
			int cam_index = sel_index - 1;
			const int num_cameras = camsCameraMgr::GetNumCameras();
			if (cam_index < num_cameras)
			{
				nameString name_str;
				camsCameraMgr::GetCameraName(cam_index, name_str);

				rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(cam_index),
											name_str.GetString());

				// This line causes the actual camera object in the
				// system to be selected. Which is important when keying
				// the camera or setting its properties.
				// Because of the disable notify check, this is only happening
				// when the user has altered this camera selection in
				// this dialog.
				//rpnOperations::SelectCamera(cam_index);
			}
			else
			{
				// We have selected a directors cut
				int dcut_index = cam_index - num_cameras;

				nameString name_str;
				camsDirectorsCutMgr::GetDirectorsCutName(dcut_index, name_str);

				rpnOperations::ChangeDirectorsCut(camsDirectorsCutMgr::GetDirectorsCut(dcut_index),
												  name_str.GetString());

				// This line causes the actual directors cut object in the
				// system to be selected.
				// Because of the disable notify check, this is only happening
				// when the user has altered this camera selection in
				// this dialog.
				rpnOperations::SelectDirectorsCut(camsDirectorsCutMgr::GetDirectorsCut(dcut_index));
			}
		}
	}

	return sel_index;
}

//------------------------------------------------------------------------
// set the editor camera active
//------------------------------------------------------------------------
void rpnCameraChoice::select_editor_cam()
{
	// editor is first in list
	this->setCurrentIndex( 0 );
	rpnOperations::ChangeCamera(cam3dMgr::GetEditorCameraProxy(),
								"Editor");
}

//------------------------------------------------------------------------
// event callbacks
//------------------------------------------------------------------------
void rpnCameraChoice::SelectedIndexChanged()
{
	int sel_index;
	sel_index = update_selected_index();

	if (sel_index >= 0)
	{
		if ( sel_index > 0 )
		{
			m_CameraLastIndex = sel_index;
			//nameString name_str;
			//camsCameraMgr::GetCameraName(sel_index-1, name_str);						
			//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, name_str.GetString().c_str() );
		}
		else
		{
			//SelectEditorCam();
			//guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, "EditCam" );
		}
	}
}

#endif // USE_QT