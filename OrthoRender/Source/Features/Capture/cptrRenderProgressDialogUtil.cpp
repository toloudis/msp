/*****************************************************************************
**	cptrRenderProgressDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"

#include "Features/Capture/cptrRenderBatchData.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrRenderStateDataParser.hpp"
#include "Features/Capture/mGUI/cptrRenderProgressForm.h"

#include "Core/Ch/chReader.hpp"
#include "Core/Ch/chWriter.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Fs/fsFileStream.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"


//============================================================================
//============================================================================
namespace cptrRenderProgressDialogUtil
{
#ifdef _MANAGED
	//public ref class dynControlsForm : public System::Windows::Forms::Form
	public ref class placeholder
	{
	public:
		static StudioFramework::cptrRenderProgressForm^ l_pDialog = nullptr;
		static int l_RenderBatchID = 0;
	};
#endif

	char* c_RENDERSTATE_EXTENSION = ".rst";	// Render STate save

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog == nullptr )
		{
			placeholder::l_pDialog = gcnew StudioFramework::cptrRenderProgressForm();
			placeholder::l_pDialog->SetRenderBatchID( placeholder::l_RenderBatchID );
		}

		placeholder::l_pDialog->Reset();
		placeholder::l_pDialog->Show();
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
#ifdef _MANAGED
		if (placeholder::l_pDialog != nullptr )
			placeholder::l_pDialog->Hide();
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetRenderBatchID( int i_ID )
	{
#ifdef _MANAGED
		placeholder::l_RenderBatchID = i_ID;
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for scenes
	//--------------------------------------------------------------------
	void SetScenesRenderPercentage( float i_percentage, std::string& i_label )
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			//DBG_LOG1("Setting Scenes Render Percentage %6.3f", i_percentage);
			placeholder::l_pDialog->SetScenesRenderPercentage(i_percentage, i_label);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for cameras
	//--------------------------------------------------------------------
	void SetCamerasRenderPercentage( float i_percentage, std::string& i_label )
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			//DBG_LOG1("Setting Camera Render Percentage %6.3f", i_percentage);
			placeholder::l_pDialog->SetCamerasRenderPercentage(i_percentage, i_label);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for camera
	//--------------------------------------------------------------------
	void SetCameraRenderPercentage( float i_percentage, std::string& i_label )
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			//DBG_LOG1("Setting Camera Render Percentage %6.3f", i_percentage);
			placeholder::l_pDialog->SetCameraRenderPercentage(i_percentage, i_label);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the time that has elapsed
	//--------------------------------------------------------------------
	void SetTimeElapsed( float i_fTimeElapsed )
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			//DBG_LOG1("Setting Time Elapsed %6.3f", i_fTimeElapsed);
			placeholder::l_pDialog->SetTimeElapsed(i_fTimeElapsed);
		}
#endif
	}

	//--------------------------------------------------------------------
	//	set the percentage for scene
	//--------------------------------------------------------------------
	void SetSceneLabel( std::string& i_label )
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			placeholder::l_pDialog->SetSceneLabel( i_label );
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void EnablePauseButton()
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			placeholder::l_pDialog->EnablePauseButton();
		}
#endif
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void DisablePauseButton()
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			placeholder::l_pDialog->DisablePauseButton();
		}
#endif
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void generate_renderstate_filename( fsLocator& i_Locator )
	{
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		if ( data.m_bBatchMode.GetValue() )
		{
			cptrRenderBatchData& bdata = cptrRenderBatchDataUtil::Data();
			//i_Locator = bdata.m_BatchFilename;
			i_Locator = gfPaths::GetPath( mnmPaths::e_SaveFootage );
			i_Locator.Push( bdata.m_BatchFilename.GetValue().GetLastName() );
		}
		else
		{
			//i_Locator = docSingleDocumentMgr::GetFilename();
			i_Locator = gfPaths::GetPath( mnmPaths::e_SaveFootage );
			i_Locator.Push( docSingleDocumentMgr::GetFilename().GetLastName() );
		}

		itString fname = i_Locator.GetLastName();
		fname.StripExtension();
		fname += itString(c_RENDERSTATE_EXTENSION);
		i_Locator.Pop();
		i_Locator.Push(fname);

		//	DEBUG only
		//std::string outdir;
		//fsFileUtil::LocatorToANSIFilename(i_Locator, outdir);
		//DBG_LOG1("RenderState filename = (%s)", outdir.c_str() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	float LoadRenderPosition()
	{
		//if ( placeholder::l_pDialog == nullptr )
		//{
		//	return -1.0f;
		//}

		//DBG_LOG0("Load Render Position");

		fsLocator load_file;
		generate_renderstate_filename( load_file );
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

		try
		{
			cptrRenderStateDataParser::ReadData( load_file, data );
			data.m_RenderPosSaveFile = load_file;
			return data.m_RenderPosTime.GetValue();
		}
		catch( const fsReadOnlyX& /*i_Ex*/ )
		{
		}
		catch( const fsInvalidLocatorX& /*i_Ex*/ )
		{
			// if the (render position) file doesn't exist, pop out.
		}
		catch( const fsFileDoesntExistX& /*i_Ex*/ )
		{		
			//std::string filename;
			//fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			//DBG_LOG1("fsFileDoesntExistX: %s", filename.c_str());
			//std::string msg = "File does not exist: " + filename;
			//MessageBox::Show(msg.c_str(), "Error");
		}
		catch( const fsDirectoryDoesntExistX& i_Ex )
		{		
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			DBG_LOG1("fsDirectoryDoesntExistX: %s", filename.c_str());
			std::string msg = "Directory does not exist: " + filename;
			guiMessageBox::Show(msg.c_str(), "Error");
		}
		catch( ... )
		{		
			guiMessageBox::Show("General exception error", "Error");
			throw;
		}

		return -1.0f;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void SaveRenderPosition()
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog != nullptr )
		{
			//DBG_LOG0("Save Render Position");

			//	Set the current time
			//
			float currtime = tmlnTimeLine::GetValue();

			cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
			data.m_RenderPosTime = currtime;		// for saving render position

			fsLocator save_file;
			generate_renderstate_filename( save_file );

			// Create document file
			//
			if (fsFileUtil::FileExists(save_file))
			{
				fsFileUtil::DeleteFile(save_file);
			}

			fsFileUtil::CreateFile(save_file);

			//	Save the data
			//
			//	Current SceneFilename, Camera, and Scene Time
			//
			try
			{
				cptrRenderStateDataParser::WriteData( save_file, data );
			}
			catch( const fsReadOnlyX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				std::string msg = "File is read only: " + filename;
				guiMessageBox::Show(msg.c_str(), "Error");
			}
			catch( ... )
			{
				guiMessageBox::Show("General exception error", "Error");
				throw;
			}

			//	store this to allow deleting later
			data.m_RenderPosSaveFile = save_file;
		}
#endif
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void DeleteRenderPosition()
	{
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

		//std::string fname;
		//fsFileUtil::LocatorToANSIFilename(data.m_RenderPosSaveFile.GetValue(), fname);
		//DBG_LOG1("deleting (%s)", fname.c_str());

		//	if the file doesn't exist, don't try to delete it.
		//
		if (!fsFileUtil::FileExists(data.m_RenderPosSaveFile.GetValue()))
		{
			return;
		}

		try
		{
			fsFileUtil::DeleteFile( data.m_RenderPosSaveFile.GetValue() );
		}
		catch( const fsFileDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::string msg = "File doesn't exist, cannot delete: " + filename;
			guiMessageBox::Show(msg.c_str(), "Error");
			DBG_ERROR1("Invalid Locator (%s)", msg.c_str());
		}
		catch (const fsInvalidLocatorX& i_Ex)
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			std::string msg = "Invalid Locator: " + filename;
			guiMessageBox::Show(msg.c_str(), "Error");
			DBG_ERROR1("Invalid Locator (%s)", msg.c_str());
		}
		catch( ... )
		{
			guiMessageBox::Show("General exception error", "Error");
			DBG_ERROR0("General Exception Error");
			throw;
		}

		//	clear out the data fields
		fsLocator empty;
		data.m_RenderPosSaveFile.SetValue(empty);
		data.m_RenderPosTime.SetValue( -1 );
	}

}	// end of namespace

