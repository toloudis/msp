/*****************************************************************************
**  rpnRenderPane.hpp
**
**     Managed class for a single pane when using multiple views
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_RENDERPANE_HPP
#error rpnRenderPane.hpp multiply included
#endif
#define RPN_RENDERPANE_HPP

#ifndef RPN_PANELVIEWER_HPP
#include "Features/RenderPanels/rpnPanelViewer.hpp"
#endif
#ifndef RPN_OPERATIONS_HPP
#include "Features/RenderPanels/rpnOperations.hpp"
#endif

#ifndef CAMS_CAMERAMGR_HPP
#include "Support/cams/camsCameraMgr.hpp"
#endif
#ifndef CAMS_DIRECTORSCUTMGR_HPP
#include "Support/cams/camsDirectorsCutMgr.hpp"
#endif
#ifndef CAM3D_MGR_HPP
#include "Tool/cam3d/cam3dMgr.hpp"
#endif
#ifndef FGT_FRAMEMGR_HPP
#include "Features/FilmGates/fgtFrameMgr.hpp"
#endif
#ifndef G3D_VIEWER_HPP
#include "Graphics/g3d/g3dViewer.hpp"
#endif
#ifndef IN_DEVICEMGR_HPP
#include "InputDI/in/inDeviceMgr.hpp"
#endif
#ifndef MNM_CONSTANTS_HPP
#include "Support/mnm/mnmConstants.hpp"
#endif
#ifndef GUI_STATUSBARMGR_HPP
#include "Tool/gui/guiStatusBarMgr.hpp"
#endif
#ifndef MUI_TIMECODEMGR_HPP
#include "Support/mnm/mnmTimeCodeMgr.hpp"
#endif
#ifndef PREFS_QUICKMGR_HPP
#include "Features/Prefs/prefsQuickMgr.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif
#ifndef TMA3D_RENDERVIEW_HPP
#include "Tool/tma3d/tma3dRenderView.hpp"
#endif
#ifndef TMA3D_SCREENUTIL_HPP
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#endif
#ifndef TMA3D_CURSORMGR_HPP
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#endif

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

//
namespace StudioFramework
{

//============================================================================
//============================================================================
//public ref class rpnRenderPane : public System::Windows::Forms::Panel
public ref class rpnRenderPane : public System::Windows::Forms::Control
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rpnRenderPane()
		:	m_pRenderView(NULL), 
			m_pPanelViewer(NULL)
		{
			// Create menu
			CreateContextMenu();
			
			// Set up the control itself
			this->BackColor = System::Drawing::Color::Black;
			this->Name = "renderPane";
			this->EnabledChanged += gcnew System::EventHandler(this, &rpnRenderPane::renderPane_Enabled);
			this->Resize += gcnew System::EventHandler(this, &rpnRenderPane::renderPane_Resize);
			this->Click += gcnew System::EventHandler(this, &rpnRenderPane::renderPane_Click);
			this->Enter += gcnew System::EventHandler(this, &rpnRenderPane::renderPane_Enter);
			this->Leave += gcnew System::EventHandler(this, &rpnRenderPane::renderPane_Leave);
			this->MouseEnter += gcnew System::EventHandler(this, &rpnRenderPane::renderPane_MouseEnter);
			this->MouseLeave += gcnew System::EventHandler(this, &rpnRenderPane::renderPane_MouseLeave);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &rpnRenderPane::OnMouseMove );
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &rpnRenderPane::renderPane_MouseDown );
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &rpnRenderPane::renderPane_MouseUp );
		}

		//----------------------------------------------------------------------------
		// Assign a render view for this control to manage.
		//----------------------------------------------------------------------------
		void SetRenderView(tma3dRenderView *i_pRenderView)
		{
			m_pRenderView = i_pRenderView;
			if (m_pRenderView)
			{
				m_pPanelViewer = dynamic_cast<rpnPanelViewer*>(m_pRenderView->GetViewer());
				this->DoResize();
			}
			else
				m_pPanelViewer = NULL;
		}

		//----------------------------------------------------------------------------
		// Turn visibility of text label on or off
		//----------------------------------------------------------------------------
		void SetTextVisible(bool i_bShow)
		{
			if (m_pPanelViewer)
			{
				m_pPanelViewer->SetTextVisible( i_bShow );
			}
		}

		//----------------------------------------------------------------------------
		// Set camera and label. If this view is active, the camera manipulator
		//	will also be set.
		//----------------------------------------------------------------------------
		void SetCamera(camCamera *i_pCamera, const std::string& i_Label)
		{
			if (m_pRenderView)
			{
				this->m_pRenderView->SetCamera( i_pCamera );

				if (this->m_pPanelViewer)
				{
					this->m_pPanelViewer->SetTextString( i_Label );
					this->m_pPanelViewer->SetDirectorsCut( NULL );
				}

				//if (this->Focused)
				if (IsActiveView())
				{
					// Set our camera into cam3dMgr in order to activate the 
					//	camera manipulator
					this->m_pRenderView->ActivateCameraManipulator();

					// Highlight selection in GUI
					rpnOperations::HighlightCamera( i_pCamera );

					// Use our camera name in the status bar panel
					guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, i_Label.c_str() );
				}
			}
		}

		//----------------------------------------------------------------------------
		// Set directors cut and label. If this view is active, the camera
		//	manipulator will de disabled.
		//----------------------------------------------------------------------------
		void SetDirectorsCut(camsDirectorsCut *i_pDirectorsCut, const std::string& i_Label)
		{
			if (m_pRenderView)
			{
				if (this->m_pPanelViewer)
				{
					this->m_pPanelViewer->SetTextString( i_Label );
					this->m_pPanelViewer->SetDirectorsCut( i_pDirectorsCut );
				}

				//if (this->Focused)
				if (IsActiveView())
				{
					// Deactivate the camera manipulator while viewing the
					// directors cut - can't edit camera because the camera
					// is not selected in order to key the changes.
					cam3dMgr::EnableManip(false);

					// Highlight selection in GUI
					rpnOperations::HighlightDirectorsCut( i_pDirectorsCut );

					// Use our directors cut name in the status bar panel
					guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, i_Label.c_str() );
				}
			}
		}


		//----------------------------------------------------------------------------
		// ConfirmCameraManip - make sure that the camera manip for this viewer is 
		//	tracking changes in the scripted camera. Should be called every
		//	Think() cycle.
		//----------------------------------------------------------------------------
		void ConfirmCameraManip()
		{
			if (m_pRenderView && IsActiveView() && cam3dMgr::IsManipEnabled())
			{
				this->m_pRenderView->ActivateCameraManipulator();
			}
		}

		//----------------------------------------------------------------------------
		// ConfirmCamera - make sure that the camera for this viewer is one
		//	of the editor or scripted cameras. This should be called when 
		//	cameras are deleted.
		//----------------------------------------------------------------------------
		void ConfirmCamera()
		{
			if (m_pRenderView)
			{
				camCamera* pCamera = m_pRenderView->GetViewer()->GetCamera();

				// Check editor cameras
				if ((pCamera != &cam3dMgr::GetEditorCamera()) &&
					(pCamera != &cam3dMgr::GetTopCamera()) &&
					(pCamera != &cam3dMgr::GetSideCamera()) &&
					(pCamera != &cam3dMgr::GetFrontCamera()))
				{
					// Check scripted cameras
					int index = camsCameraMgr::GetIndexForCamera( pCamera );
					if (index < 0)
					{
						// Camera not found, so it was probably deleted,
						// switch to editor camera.
						this->SetCamera( &cam3dMgr::GetEditorCamera(), "Editor" );
					}
				}
			
				// Confirm director's cut still exists also
				if (this->m_pPanelViewer &&
					this->m_pPanelViewer->GetDirectorsCut())
				{
					int index = camsDirectorsCutMgr::GetIndexForDirectorsCut( this->m_pPanelViewer->GetDirectorsCut() );
					if (index < 0)
					{
						// Director's cut not found, so it was probably deleted,
						// switch to editor camera.
						this->SetCamera( &cam3dMgr::GetEditorCamera(), "Editor" );
					}
				}
			}
		}
		
		//--------------------------------------------------------------------
		// Focus_Camera centers camera with respect to the point
		//--------------------------------------------------------------------
		void FocusCamera(const maPoint3d& i_Focus, float i_Radius)
		{
			if (m_pRenderView)
			{
				// I'd rather be doing this through the camera manip interface,
				// but only the active panel has a camera manip.
				//
				camCamera* pCamera = m_pRenderView->GetViewer()->GetCamera();

				maVector3d view = pCamera->GetPosition() - pCamera->GetTarget();
				pCamera->LookAt(i_Focus + view, i_Focus, pCamera->GetUp());

				if (pCamera->IsOrthographic())
				{
					const float width_modifier = 1.6f;
					pCamera->SetOrthoWidth( width_modifier * i_Radius );
				}
			}
		}

		//--------------------------------------------------------------------
		//	Return true if this render pane is the active view
		//--------------------------------------------------------------------
		bool IsActiveView()
		{
			return (tma3dRenderView::GetActiveRenderView() == this->m_pRenderView);
		}

		//--------------------------------------------------------------------
		//	Return true if this render pane is using a directors cut
		//--------------------------------------------------------------------
		bool HasDirectorsCut()
		{
			return (this->m_pPanelViewer && 
					this->m_pPanelViewer->HasDirectorsCut() );
		}

		//----------------------------------------------------------------------------
		// Select the camera object for the camera used in the view.
		// Switching out of directors cut mode if necessary.
		//----------------------------------------------------------------------------
		void SelectCamera()
		{
			if (m_pRenderView)
			{
				int cam_index = camsCameraMgr::GetIndexForCamera( m_pRenderView->GetViewer()->GetCamera() );
				if (cam_index < camsCameraMgr::GetNumCameras())
				{
					if ( this->HasDirectorsCut() )
					{
						// Switch from directors cut to the camera that 
						// was active in the cut at this time
						nameString name_str;
						camsCameraMgr::GetCameraName(cam_index, name_str);

						this->SetCamera(camsCameraMgr::GetCamera(cam_index),
													name_str.GetString());
					}
				
					rpnOperations::SelectCamera( cam_index );
				}
			}
		}


	private:
		tma3dRenderView *m_pRenderView;
		rpnPanelViewer *m_pPanelViewer; // just a cast of the viewer owned by m_pRenderView

	private: System::Windows::Forms::ContextMenu ^  MainMenu;
	private: System::Windows::Forms::MenuItem ^  menuItem_select;
	private: System::Windows::Forms::MenuItem ^  menuItem_selectClear;
	private: System::Windows::Forms::MenuItem ^  menuItem_selectCamera;
	private: System::Windows::Forms::MenuItem ^  menuItem_camera;
	private: System::Windows::Forms::MenuItem ^  menuItem_cameraEditor;
	private: System::Windows::Forms::MenuItem ^  menuItem_cameraEditPersp;
	private: System::Windows::Forms::MenuItem ^  menuItem_cameraEditTop;
	private: System::Windows::Forms::MenuItem ^  menuItem_cameraEditFront;
	private: System::Windows::Forms::MenuItem ^  menuItem_cameraEditSide;
	private: System::Windows::Forms::MenuItem ^  menuItem_cameraScripted;
	private: System::Windows::Forms::MenuItem ^  menuItem_directorsCut;

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_Enabled(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (this->m_pRenderView != NULL)
			{
				this->m_pRenderView->EnableViewer( this->Enabled );
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_Resize(System::Object ^  sender, System::EventArgs ^  e)
		{
			DoResize();
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void DoResize()
		{
			if (this->m_pRenderView != NULL)
			{
				System::Drawing::Rectangle rect = this->ClientRectangle;
				//DBG_LOG2("Render window size: %d %d", rect.Width, rect.Height);
					
				if (rect.Width > 0 && rect.Height > 0)
				{
					// Resize the underlying window
					this->m_pRenderView->ResizeWindow(rect.Width, rect.Height);

					// Update the ScreenUtil function if we are the active render view
					if (tma3dRenderView::GetActiveRenderView() == this->m_pRenderView)
					{
						tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.Left, (float)rect.Top ),
							maPoint2d( (float)rect.Width, (float)rect.Height ) );

						// See comment below...
						//	update the frame gate
						fgtFrameMgr::ResizeFrames();

						// Update the time code manager to adjust the text to
						// the new window size and aspect ratio
						mnmTimeCodeMgr::Update();
					}
				}
			}

			// Because all of the viewers are the same size in the current layout
			// schemes, we can resize the film gates just once when resizing.
			// Eventually, we are going to have to resize the film gates in the
			// Render() function of the rpnPanelViewer every frame. When this
			// happens, the ResizeFrames() function of the FilmGates classes
			// should be changed to not destroy and recreate fragments, but to
			// scale them.
			//if (m_bFilmGateResponsible)
			//{
			//	//	update the frame gate
			//	fgtFrameMgr::ResizeFrames();
			//}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			// Try to take the focus
			if (this->Focus())
			{
                tma3dCursorMgr::CursorOverViewCallback(true);
				//tma3dCursorMgr::CursorPosChangedCallback( e->X, e->Y );
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_Enter(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (this->m_pRenderView)
			{
				tma3dRenderView::SetActiveRenderView(this->m_pRenderView);

				System::Drawing::Rectangle rect = this->ClientRectangle;
				if (rect.Width > 0 && rect.Height > 0)
				{
					tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.Left, (float)rect.Top ),
						maPoint2d( (float)rect.Width, (float)rect.Height ) );

					//	update the frame gate
					fgtFrameMgr::ResizeFrames();
				}

				if (this->HasDirectorsCut())
				{
					// Deactivate the camera manipulator while viewing the
					// directors cut - can't edit camera because the camera
					// is not selected in order to key the changes.
					cam3dMgr::EnableManip(false);

					// Highlight selection in GUI
					rpnOperations::HighlightDirectorsCut( this->m_pPanelViewer->GetDirectorsCut() );
				}
				else
				{
					// Set our camera into cam3dMgr in order to activate the 
					//	camera manipulator
					this->m_pRenderView->ActivateCameraManipulator();

					// Highlight selection in GUI
					rpnOperations::HighlightCamera( this->m_pRenderView->GetViewer()->GetCamera() );
				}

				if (this->m_pPanelViewer)
				{
					// Use our camera name in the status bar panel
					guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, 
						this->m_pPanelViewer->GetTextString().c_str() );
				}
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_Leave(System::Object ^  sender, System::EventArgs ^  e)
		{
			// don't do anything on leave, just wait for another render view to take
			// away the focus from us in its Enter function. Otherwise, we just
			// keep the active view
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_MouseEnter(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (this->Focused)
			{
                tma3dCursorMgr::CursorOverViewCallback(true);
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_MouseLeave(System::Object ^  sender, System::EventArgs ^  e)
		{
			if (this->Focused)
			{
				tma3dCursorMgr::CursorOverViewCallback(false);
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void OnMouseMove(Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
		{
			if (this->Focused)
			{
				tma3dCursorMgr::CursorPosChangedCallback( e->X, e->Y );
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_MouseDown(Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
		{
			// Enable the context menu based on whether ALT or CTRL is pressed
			if (e->Button == ::MouseButtons::Right)
			{
				inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
				if (   pKeyboard->IsDown(inKeys::e_LALT) 
					|| pKeyboard->IsDown(inKeys::e_RALT)
					|| pKeyboard->IsDown(inKeys::e_LCTRL) 
					|| pKeyboard->IsDown(inKeys::e_RCTRL))
				{
					this->ContextMenu = nullptr;
				}
				else
				{
					this->ContextMenu = this->MainMenu;
				}
			}
			else if (e->Button == ::MouseButtons::Middle)
			{
				//	sum up the local locations to get the screen coordinates
				//
				System::Drawing::Point pnt = this->PointToScreen(e->Location);
				prefsQuickMgr::Show(pnt.X, pnt.Y);
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void renderPane_MouseUp(Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
		{
			if (e->Button == ::MouseButtons::Middle)
			{
				// make selection
			}
		}

		//----------------------------------------------------------------------------
		// Set one of the editor cameras: perspective, ortho top, ortho front, ortho side
		//----------------------------------------------------------------------------
		System::Void cameraEditPersp_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->SetCamera( &cam3dMgr::GetEditorCamera(), "Editor" );
		}
		System::Void cameraEditTop_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->SetCamera( &cam3dMgr::GetTopCamera(), "Top" );
		}
		System::Void cameraEditSide_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->SetCamera( &cam3dMgr::GetSideCamera(), "Side" );
		}
		System::Void cameraEditFront_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->SetCamera( &cam3dMgr::GetFrontCamera(), "Front" );
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void menuCamera_Popup(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->menuItem_cameraScripted->MenuItems->Clear();

			std::vector<nameString> camera_names;
			camsCameraMgr::GetCameraNames(camera_names);
			for (int i=0; i<camera_names.size(); ++i)
			{
				MenuItem ^menu_item = gcnew System::Windows::Forms::MenuItem();
				menu_item->Index = i;
				menu_item->Text = gcnew System::String(camera_names[i].GetString().c_str());
				menu_item->Click += gcnew System::EventHandler(this, &rpnRenderPane::cameraScripted_Click);
				this->menuItem_cameraScripted->MenuItems->Add(menu_item);
			}

			this->menuItem_directorsCut->MenuItems->Clear();
			std::vector<nameString> dircut_names;
			camsDirectorsCutMgr::GetDirectorsCutNames(dircut_names);
			for (int i=0; i<dircut_names.size(); ++i)
			{
				MenuItem ^menu_item = gcnew System::Windows::Forms::MenuItem();
				menu_item->Index = i;
				menu_item->Text = gcnew System::String(dircut_names[i].GetString().c_str());
				menu_item->Click += gcnew System::EventHandler(this, &rpnRenderPane::directorsCut_Click);
				this->menuItem_directorsCut->MenuItems->Add(menu_item);
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void cameraScripted_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			MenuItem ^menu_item = safe_cast<System::Windows::Forms::MenuItem^>(sender);
			if (menu_item)
			{
				camCamera *pCamera = camsCameraMgr::GetCamera(menu_item->Index);
				nameString name;
				camsCameraMgr::GetCameraName(menu_item->Index, name);
				this->SetCamera( pCamera, name.GetString() );
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void directorsCut_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			MenuItem ^menu_item = safe_cast<System::Windows::Forms::MenuItem^>(sender);
			if (menu_item)
			{
				camsDirectorsCut *pDirectorsCut = camsDirectorsCutMgr::GetDirectorsCut(menu_item->Index);
				nameString name;
				camsDirectorsCutMgr::GetDirectorsCutName(menu_item->Index, name);
				this->SetDirectorsCut( pDirectorsCut, name.GetString() );
			}
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void selectClear_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			sel3dMgr::CreateUndoOperation();
			sel3dMgr::ClearSelection();
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		System::Void selectCamera_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			this->SelectCamera();
		}

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void CreateContextMenu()
		{
			this->MainMenu = gcnew System::Windows::Forms::ContextMenu();
			this->menuItem_camera = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_camera->Index = 0;
			this->menuItem_camera->Text = "Camera";
			this->menuItem_camera->Popup += gcnew System::EventHandler(this, &rpnRenderPane::menuCamera_Popup);
			this->MainMenu->MenuItems->Add(this->menuItem_camera);
			this->menuItem_cameraEditor = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_cameraEditor->Index = 0;
			this->menuItem_cameraEditor->Text = "Editor";
			this->menuItem_camera->MenuItems->Add(this->menuItem_cameraEditor);
			this->menuItem_cameraEditPersp = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_cameraEditPersp->Index = 0;
			this->menuItem_cameraEditPersp->Text = "Perspective";
			this->menuItem_cameraEditPersp->Click += gcnew System::EventHandler(this, &rpnRenderPane::cameraEditPersp_Click);
			this->menuItem_cameraEditor->MenuItems->Add(this->menuItem_cameraEditPersp);
			this->menuItem_cameraEditTop = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_cameraEditTop->Index = 1;
			this->menuItem_cameraEditTop->Text = "Top";
			this->menuItem_cameraEditTop->Click += gcnew System::EventHandler(this, &rpnRenderPane::cameraEditTop_Click);
			this->menuItem_cameraEditor->MenuItems->Add(this->menuItem_cameraEditTop);
			this->menuItem_cameraEditFront = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_cameraEditFront->Index = 2;
			this->menuItem_cameraEditFront->Text = "Front";
			this->menuItem_cameraEditFront->Click += gcnew System::EventHandler(this, &rpnRenderPane::cameraEditFront_Click);
			this->menuItem_cameraEditor->MenuItems->Add(this->menuItem_cameraEditFront);
			this->menuItem_cameraEditSide = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_cameraEditSide->Index = 3;
			this->menuItem_cameraEditSide->Text = "Side";
			this->menuItem_cameraEditSide->Click += gcnew System::EventHandler(this, &rpnRenderPane::cameraEditSide_Click);
			this->menuItem_cameraEditor->MenuItems->Add(this->menuItem_cameraEditSide);
			this->menuItem_cameraScripted = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_cameraScripted->Index = 1;
			this->menuItem_cameraScripted->Text = "Scripted";
			this->menuItem_camera->MenuItems->Add(this->menuItem_cameraScripted);
			this->menuItem_directorsCut = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_directorsCut->Index = 1;
			this->menuItem_directorsCut->Text = "Director's Cut";
			this->menuItem_camera->MenuItems->Add(this->menuItem_directorsCut);

			this->menuItem_select = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_select->Index = 1;
			this->menuItem_select->Text = "Select";
			this->MainMenu->MenuItems->Add(this->menuItem_select);
			this->menuItem_selectClear = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_selectClear->Index = 0;
			this->menuItem_selectClear->Text = "Clear Selection";
			this->menuItem_selectClear->Click += gcnew System::EventHandler(this, &rpnRenderPane::selectClear_Click);
			this->menuItem_select->MenuItems->Add(this->menuItem_selectClear);
			this->menuItem_selectCamera = gcnew System::Windows::Forms::MenuItem();
			this->menuItem_selectCamera->Index = 0;
			this->menuItem_selectCamera->Text = "Select Camera";
			this->menuItem_selectCamera->Click += gcnew System::EventHandler(this, &rpnRenderPane::selectCamera_Click);
			this->menuItem_select->MenuItems->Add(this->menuItem_selectCamera);

			this->ContextMenu = this->MainMenu;
		}
};


}	// end of namespace

#endif // _MANAGED
