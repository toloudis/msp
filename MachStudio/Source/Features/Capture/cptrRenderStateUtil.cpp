/*****************************************************************************
**	cptrRenderStateUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderStateUtil.hpp"

#include "Features/Capture/cptrRenderBatchData.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/cptrRenderStateDataParser.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//============================================================================
//============================================================================
namespace cptrRenderStateUtil
{

	char* c_RENDERSTATE_EXTENSION = ".rst";	// Render STate save

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void generate_renderstate_filename( fsLocator& i_Locator )
	{
		captRenderOutputData& data = captRenderOutputDataUtil::Data();
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
		//DBG_LOG("RenderState filename = (" << outdir.c_str() << ")" );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//float LoadRenderPosition()
	//{
	//	//if ( placeholder::l_pDialog == nullptr )
	//	//{
	//	//	return -1.0f;
	//	//}

	//	//DBG_LOG("Load Render Position");

	//	fsLocator load_file;
	//	generate_renderstate_filename( load_file );
	//	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	//	try
	//	{
	//		if (fsFileUtil::FileExists(load_file))
	//		{
	//			cptrRenderStateDataParser::ReadData( load_file, data );
	//			data.m_RenderPosSaveFile = load_file;
	//			return data.m_RenderPosTime.GetValue();
	//		}
	//	}
	//	catch ( const envExceptionX& i_Ex )
	//	{
	//		std::string msg = "Error reading saved render position, " + i_Ex.GetErrorMessage();
	//		DBG_ERROR(msg);
	//		guiMessageBox::Show(msg.c_str(), "Render Error", guiMessageBox::e_OKOnly);
	//	}

	//	return -1.0f;
	//}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void SaveRenderPosition()
	{
		//DBG_LOG("Save Render Position");

		//	Set the current time
		//
		maTime currtime = tmlnTimeLine::GetValue();

		captRenderOutputData& data = captRenderOutputDataUtil::Data();
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
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Error saving render position, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Render Error", guiMessageBox::e_OKOnly);
		}

		//	store this to allow deleting later
		data.m_RenderPosSaveFile = save_file;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void DeleteRenderPosition()
	{
		captRenderOutputData& data = captRenderOutputDataUtil::Data();

		//std::string fname;
		//fsFileUtil::LocatorToANSIFilename(data.m_RenderPosSaveFile.GetValue(), fname);
		//DBG_LOG("deleting (" << fname.c_str() << ")" );

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
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Error deleing saved render position, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Render Error", guiMessageBox::e_OKOnly);
		}

		//	clear out the data fields
		fsLocator empty;
		data.m_RenderPosSaveFile.SetValue(empty);
		data.m_RenderPosTime.SetValue( captRenderOutputData::c_InitRenderPosTime );
	}

}	// end of namespace

