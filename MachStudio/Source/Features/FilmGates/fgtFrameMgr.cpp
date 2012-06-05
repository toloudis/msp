//****************************************************************************
//	fgtFrameMgr.hpp
//
///		Manage the group of frames
//
//	StudioGPU
//	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
#include "Features/FilmGates/fgtFrameMgr.hpp"

#include "Features/FilmGates/fgtCommandShowFrame.hpp"
#include "Features/Prefs/PrefsMgr.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsCategoryConfigFileUtil.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/ma/maPoint2d.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#include <vector>
#include <sstream>


//============================================================================
//============================================================================
namespace fgtFrameMgr
{
	//========================================================================
	//========================================================================
	namespace
	{
		fgtFrame* l_pActionSafeFrame = NULL;
		fgtFrame* l_pTitleSafeFrame = NULL;

		struct safe_frames
		{
			float m_Ratio;
			float m_ActionSafeH;
			float m_ActionSafeV;
			float m_TitleSafeH;
			float m_TitleSafeV;
		};
		std::vector<safe_frames> l_SafeFrameList;

		const char* lc_SafeFrames_FileName	= "SafeFrames.cfg";

		const char * lc_Key_Ratio		= "Ratio";
		const char * lc_Key_VideoRatio	= "VideoRatio";
		const char * lc_Key_ActionSafeH	= "ActionSafeH";
		const char * lc_Key_ActionSafeV	= "ActionSafeV";
		const char * lc_Key_TitleSafeH	= "TitleSafeH";
		const char * lc_Key_TitleSafeV	= "TitleSafeV";
	}	// end of namespace


	//--------------------------------------------------------------------
	/// Init
	//--------------------------------------------------------------------
	void Init()
	{
		l_pActionSafeFrame	= new fgtFrame();
		const maFloatRGBA ASframecolor( 0.67f, 0.67f, 0.67f, 0.67f);
		l_pActionSafeFrame->SetColor( ASframecolor );
		l_pActionSafeFrame->Show(false);	// turn off initially

		l_pTitleSafeFrame	= new fgtFrame();
		const maFloatRGBA TSframecolor( 0.467f, 0.467f, 0.467f, 0.467f );
		l_pTitleSafeFrame->SetColor( TSframecolor );
		l_pTitleSafeFrame->Show(false);		// turn off initially
	}

	//--------------------------------------------------------------------
	///	CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
		delete l_pActionSafeFrame;
		delete l_pTitleSafeFrame;
	}

	//--------------------------------------------------------------------
	/// Read in the valid safe frames from an XML file
	//--------------------------------------------------------------------
	void Read()
	{
		l_SafeFrameList.resize(10);

		// stop any running threads
		gpxRenderControl::ConfirmSingleThread();

		//	read in the configuration file for filmgate
		//
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
		cfgdir.Push( lc_SafeFrames_FileName );
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);

		guiXMLTextReader::Open(cfgpath.c_str());

		//	read in the preferences
		//
		int index = -1;
		guiXMLTextReader::gui_Node_Type node_type;
		std::string keyname, strvalue;
		while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
		{
			if ((node_type == guiXMLTextReader::e_Element) && (keyname.length() > 0))
			{
				if (keyname == lc_Key_Ratio)
				{
					++index;
					if (l_SafeFrameList.size() < index)
					{
						l_SafeFrameList.resize(index+1);
					}
				}
			}
			else if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
			{
				if (keyname == lc_Key_VideoRatio)
				{
					std::string temp;
					size_t pos;
					float h,v;
					if (strvalue.find(":", 0) != std::string::npos)
					{
						pos = strvalue.find(":", 0);		//store the position of the delimiter
						temp = strvalue.substr(0, pos);		//get the token
						h = atof(temp.c_str());
					}
					if (strvalue.find(":", pos) != std::string::npos)
					{
						temp = strvalue.substr(pos+1, std::string::npos);		//get the token
						v = atof(temp.c_str());
					}
					l_SafeFrameList[index].m_Ratio = (h / v);
				}
				else if (keyname == lc_Key_ActionSafeH)
					{float value; guiXMLTextReader::Convert(strvalue, value); l_SafeFrameList[index].m_ActionSafeH = value;}
				else if (keyname == lc_Key_ActionSafeV)
					{float value; guiXMLTextReader::Convert(strvalue, value); l_SafeFrameList[index].m_ActionSafeV = value;}
				else if (keyname == lc_Key_TitleSafeH)
					{float value; guiXMLTextReader::Convert(strvalue, value); l_SafeFrameList[index].m_TitleSafeH = value;}
				else if (keyname == lc_Key_TitleSafeV)
					{float value; guiXMLTextReader::Convert(strvalue, value); l_SafeFrameList[index].m_TitleSafeV = value;}

				//	DEBUG ONLY: title safe v is the last entry in each group
				//if (keyname == lc_Key_TitleSafeV)
				//	DBG_LOG("Read SafeFrame: " << index << " (ratio=" << l_SafeFrameList[index].m_Ratio << "  action="  << l_SafeFrameList[index].m_ActionSafeH << "," << l_SafeFrameList[index].m_ActionSafeV << "  title="  << l_SafeFrameList[index].m_TitleSafeH << "," << l_SafeFrameList[index].m_TitleSafeV << ")");
			}
		}

		guiXMLTextReader::Close();
	}

	//--------------------------------------------------------------------
	///	show or hide all safe frames.  This will also keep track of
	///	the current state of each frame before hiding them.
	///
	/// @param i_bShow show or hide the frame
	//--------------------------------------------------------------------
	void Show( bool i_bActionSafeShow, bool i_bTitleSafeShow )
	{
		if (l_pActionSafeFrame != NULL)
			l_pActionSafeFrame->Show(i_bActionSafeShow);
		if (l_pTitleSafeFrame != NULL)
			l_pTitleSafeFrame->Show(i_bTitleSafeShow);
	}

	//--------------------------------------------------------------------
	///	show or hide all frame gates.  This will also keep track of
	///	the current state of each frame gate before hiding them.
	///
	/// @param i_bShow show or hide the frame
	//--------------------------------------------------------------------
	void Show( bool i_bShow )
	{
		if (l_pActionSafeFrame != NULL)
			l_pActionSafeFrame->Show(i_bShow);
		if (l_pTitleSafeFrame != NULL)
			l_pTitleSafeFrame->Show(i_bShow);
	}

	//--------------------------------------------------------------------
	///	recreate/resize the frames.  This is needed when the render
	///	window safe frame changes.
	//--------------------------------------------------------------------
	void ResizeFrames()
	{
		if (l_SafeFrameList.size() == 0)
			return;

		//	calculate the ratio and find the nearest match if no exact match occurs
		int index = 0;
		maPoint2d winsize = tma3dScreenUtil::GetWindowSize();
		float winratio = winsize.GetX() / winsize.GetY();
		float diff = fabs( l_SafeFrameList[0].m_Ratio - winratio );
		
		for (int i=0; i < l_SafeFrameList.size(); ++i)
		{
			float newdiff = fabs( l_SafeFrameList[i].m_Ratio - winratio );
			if (newdiff < diff)
			{
				diff	= newdiff;
				index	= i;
			}
		}

		//	set the appropriate percentages for the frames
		if (l_pActionSafeFrame != NULL)
		{
			//DBG_LOG("Resizing ACTION SAFE FRAME #" << index);
			l_pActionSafeFrame->SetPercentages( l_SafeFrameList[index].m_ActionSafeH, l_SafeFrameList[index].m_ActionSafeV );
			l_pActionSafeFrame->Resize();
		}
		if (l_pTitleSafeFrame != NULL)
		{
			//DBG_LOG("Resizing TITLE SAFE FRAME #" << index);
			l_pTitleSafeFrame->SetPercentages( l_SafeFrameList[index].m_TitleSafeH, l_SafeFrameList[index].m_TitleSafeV );
			l_pTitleSafeFrame->Resize();
		}
	}

	//--------------------------------------------------------------------
	/// Display Action Safe frame
	//--------------------------------------------------------------------
	void SetViewActionFrame(bool i_bVal)
	{
		if (l_pActionSafeFrame != NULL)
			l_pActionSafeFrame->Show(i_bVal);

		PrefsMgr::GetDataSimple().m_bActionSafeFrameVisible = i_bVal;
	}
	bool GetViewActionFrame()
	{
		return PrefsMgr::GetDataSimple().m_bActionSafeFrameVisible.GetValue();
	}

	//--------------------------------------------------------------------
	/// Display Title Safe frame
	//--------------------------------------------------------------------
	void SetViewTitleFrame(bool i_bVal)
	{
		if (l_pTitleSafeFrame != NULL)
			l_pTitleSafeFrame->Show(i_bVal);

		PrefsMgr::GetDataSimple().m_bTitleSafeFrameVisible = i_bVal;
		//l_bShowPropertyKeys = i_bVal;
	}
	bool GetViewTitleFrame()
	{
		return PrefsMgr::GetDataSimple().m_bTitleSafeFrameVisible.GetValue();
		//return l_bShowPropertyKeys;
	}
}
