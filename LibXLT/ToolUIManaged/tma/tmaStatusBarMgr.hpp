#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaStatusBarMgr.hpp
//**
//**      A managed interface to the status bar.
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_STATUSBARMGR_HPP
//#error tmaStatusBarMgr.hpp multiply included
//#endif
//#define TMA_STATUSBARMGR_HPP
//
//#ifndef TMA_SYSTEM_HPP
//#include "ToolUIManaged/tma/tmaSystem.hpp"
//#endif
//
//#ifndef	TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//
//#include <string>
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////============================================================================
//public ref class tmaStatusBarMgr
//{
//public:
//	static tmaStatusBarMgr^ g_pMgr = nullptr;
//	static System::Windows::Forms::StatusStrip^ g_pStatusStrip = nullptr;
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaStatusBarMgr()
//	{
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	~tmaStatusBarMgr()
//	{
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void Initialize()
//	{
//		int count = tmaSystem::g_pMainForm->Controls->Count;
//
//		int i;
//		for ( i=0; i < count ; i++ )
//		{
//			g_pStatusStrip = dynamic_cast<System::Windows::Forms::StatusStrip^>(tmaSystem::g_pMainForm->Controls[i]);
//			if ( g_pStatusStrip != nullptr )
//			{
//				break;
//			}
//		}
//
//		//DBG_LOG( (g_pStatusStrip != nullptr), "No Status Bar" );
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void DeInitialize()
//	{
//		g_pStatusStrip = nullptr;
//	};
//
//	//---------------------------------------------------------------------------
//	//	AddPanel()
//	//		i_Width:
//	//			-1	= variable
//	//			0	= size of content
//	//			#	= a set width
//	//---------------------------------------------------------------------------
//	int AddPanel(int i_Width)
//	{
//		if ( g_pStatusStrip != nullptr )
//		{
//			g_pStatusStrip->Visible = true;
//
//			ToolStripStatusLabel^ pTSSL = gcnew ToolStripStatusLabel();
//			pTSSL->BorderSides = ((System::Windows::Forms::ToolStripStatusLabelBorderSides)((((System::Windows::Forms::ToolStripStatusLabelBorderSides::Left | System::Windows::Forms::ToolStripStatusLabelBorderSides::Top)
//									| System::Windows::Forms::ToolStripStatusLabelBorderSides::Right)
//									| System::Windows::Forms::ToolStripStatusLabelBorderSides::Bottom)));
//			pTSSL->BorderStyle = Border3DStyle::SunkenInner;
//			pTSSL->Alignment = ToolStripItemAlignment::Left;
//			if (i_Width == -1)
//			{
//				pTSSL->AutoSize = true;
//				pTSSL->Spring = true;
//			}
//			else if (i_Width == 0)
//			{
//				pTSSL->AutoSize = true;
//				pTSSL->Spring = false;
//			}
//			else
//			{
//				pTSSL->AutoSize = false;
//				pTSSL->Spring = false;
//				pTSSL->Width = i_Width;
//			}
//			//pTSSL->ToolTipText = "";
//			//pTSSL->Text = System.DateTime.Today.ToLongDateString();
//
//			return g_pStatusStrip->Items->Add(pTSSL);
//		}
//		return -1;
//	}
//	int AddPanel()
//	{
//		return AddPanel(-1);
//	}
//
//	//---------------------------------------------------------------------------
//	//	RemovePanel()
//	//---------------------------------------------------------------------------
//	//void RemovePanel(int i_PanelIndex)
//	//{
//	//	if ( g_pStatusStrip != nullptr )
//	//	{
//	//		if ((g_pStatusStrip->Items->Count >= (i_PanelIndex+1))
//	//		{
//	//			g_pStatusStrip->Items->RemoveAt(i_PanelIndex);
//	//		}
//	//	}
//	//}
//
//	//---------------------------------------------------------------------------
//	//	SetText()
//	//---------------------------------------------------------------------------
//	void SetText( int i_PanelIndex, const char * i_Text )
//	{
//		if ( g_pStatusStrip != nullptr )
//		{
//			if ((g_pStatusStrip->Items->Count == 0) && (i_PanelIndex == 0))
//			{
//				g_pStatusStrip->Text = gcnew String(i_Text);
//			}
//			else
//			{
//				if (this->g_pStatusStrip->Items->Count >= (i_PanelIndex+1) )
//				{
//					this->g_pStatusStrip->Items[i_PanelIndex]->Text = gcnew String(i_Text);
//				}
//			}
//		}
//	};
//
//	//---------------------------------------------------------------------------
//	//	GetText()
//	//---------------------------------------------------------------------------
//	const char * GetText(int i_PanelIndex)
//	{
//		if ( g_pStatusStrip != nullptr )
//		{
//
//			if ((g_pStatusStrip->Items->Count == 0) && (i_PanelIndex == 0))
//			{
//				std::string o_string;
//				tmaManagedStringUtils::ManagedStringToStdString( g_pStatusStrip->Text, o_string );
//				return o_string.c_str();
//			}
//			else
//			{
//				if (this->g_pStatusStrip->Items->Count >= (i_PanelIndex+1) )
//				{
//					std::string o_string;
//					tmaManagedStringUtils::ManagedStringToStdString( g_pStatusStrip->Items[i_PanelIndex]->Text, o_string );
//					return o_string.c_str();
//				}
//			}
//
//		}
//
//		return 0;
//	};
//};
//
//#endif // _MANAGED
