/*****************************************************************************
**	prefsQuickMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/prefsQuickMgr.hpp"

#include "Features/Prefs/wxGUI/prefsQuickDialog.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythProperty.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/prty/prtyListBoxUIInfo.hpp"
//#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include <string>
#include <vector>

// Includes from old managed file
#ifndef CMA_COMMANDMGR_HPP
#include "Tool/cma/cmaCommandMgr.hpp"
#endif


//============================================================================
//============================================================================
namespace
{
	const char* lc_Key_Start		= "QuickPrefs";
	const char* lc_Key_Command		= "Command";

	const char * lc_PrefsFileName			= "QuickPrefs.cfg";
	const char * lc_PrefsFileName_Default	= "QuickPrefs-Default.cfg";

	std::vector<prefsQuickDataInterest*>	l_QuickPrefsInterestList;

	bool	l_bDataReadIn = false;

	//====================================================================
	//====================================================================
	class prefsQuickObject : public prtyObject
	{
		public:
			//------------------------------------------------------------
			//------------------------------------------------------------
			prefsQuickObject()
			:	m_pLBUIInfo(0)
			{
				RegisterProperties();		
			}
			~prefsQuickObject()
			{		
			}

			//------------------------------------------------------------
			//------------------------------------------------------------
			void ReadPrefs();
			void WritePrefs();

		private:
			//------------------------------------------------------------
			//------------------------------------------------------------
			void ReadPrefsFromConfigFile(fsLocator& i_ConfigFile);
			void WritePrefsToConfigFile();

			//------------------------------------------------------------------------
			//------------------------------------------------------------------------
			void BuildList();

			//------------------------------------------------------------
			//------------------------------------------------------------
			virtual void RegisterProperties();

			//--------------------------------------------------------------------
			//	CommandsPropertyChanged - notification that commands have changed.
			//--------------------------------------------------------------------
			void CommandsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

		public:
			prefsQuickData m_Data;

		private:
			prtyListBoxUIInfo* m_pLBUIInfo;		// not the owner
	};
	static prefsQuickObject* l_pPrefsQuickObject = 0;

	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void prefsQuickObject::ReadPrefs()
	{
		//	fill the variable with all the commands (unchecked)
		//
		m_Data.m_Commands.ClearList();
		const std::vector<cmaCommand*>& cmds = cmaCommandMgr::GetCommandList();
		int csize = cmds.size();
		m_Data.m_Commands.SetNumberOfItems(csize);
		for (int j = 0; j < csize; ++j)
		{
			m_Data.m_Commands.SetValueText(j, cmds[j]->GetTag());
			m_Data.m_Commands.SetValueFlag(j, false);
		}

		//	try to read the config files
		//
		std::string cfgpath;
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
		cfgdir.Push(lc_PrefsFileName);

		if (fsFileUtil::FileExists(cfgdir))
		{
			ReadPrefsFromConfigFile(cfgdir);
		}
		else
		{
			// no hot key config, so check for default
			cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
			cfgdir.Push( lc_PrefsFileName_Default );
			if (fsFileUtil::FileExists(cfgdir))
			{
				ReadPrefsFromConfigFile(cfgdir);
			}
		}

		BuildList();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void prefsQuickObject::WritePrefs()
	{
		WritePrefsToConfigFile();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void prefsQuickObject::ReadPrefsFromConfigFile(fsLocator& i_ConfigFile)
	{
		//
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		guiXMLTextReader::Open(cfgpath.c_str());

		//	read in the preferences
		//
		//DBG_LOG("--------------quick ReadPrefs");
		guiXMLTextReader::gui_Node_Type node_type;
		std::string keyname, strvalue;
		while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
		{
			if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
			{
				//	each command gets added
				if (keyname == lc_Key_Command)
				{
					std::string value; 
					guiXMLTextReader::Convert(strvalue, value);

					for (int j = 0; j < m_Data.m_Commands.GetNumberOfItems(); ++j)
					{
						if (m_Data.m_Commands.GetValueText(j) == value)
						{
							m_Data.m_Commands.SetValueFlag(j, true);
							//DBG_LOG4("%02d (%5s) %s equal to (%s)", j, m_Data.m_Commands.GetValueFlag(j) ? "true":"false", m_Data.m_Commands.GetValueText(j).c_str(), value.c_str() );
						}
					}
				};
			}
		}

		guiXMLTextReader::Close();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void prefsQuickObject::WritePrefsToConfigFile()
	{
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
		cfgdir.Push(lc_PrefsFileName);
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);

		//	quick and dirty XML Writer
		guiXMLTextWriter::Open(cfgpath.c_str());
		guiXMLTextWriter::WriteStartElement(lc_Key_Start);

		//	write the actual values
		for (int j = 0; j < m_Data.m_Commands.GetNumberOfItems(); ++j)
		{
			if (m_Data.m_Commands.GetValueFlag(j) == true)
			{
				guiXMLTextWriter::WriteElement(lc_Key_Command, m_Data.m_Commands.GetValueText(j));
			}
		}

		//	finish it up
		guiXMLTextWriter::WriteEndElement();
		guiXMLTextWriter::Close();
	}		
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void prefsQuickObject::BuildList()
	{
		//DBG_LOG("--------------------quick BuildList");
		m_pLBUIInfo->Clear();
		int csize = m_Data.m_Commands.GetNumberOfItems();
		for (int i=0; i < csize; ++i)
		{
			//DBG_LOG3("%02d (%5s) %s", i, m_Data.m_Commands.GetValueFlag(i) ? "true":"false", m_Data.m_Commands.GetValueText(i).c_str() );
			m_pLBUIInfo->AddItem( m_Data.m_Commands.GetValueText(i), m_Data.m_Commands.GetValueFlag(i) );
		}
		m_pLBUIInfo->UpdateControl();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//virtual 
	void prefsQuickObject::RegisterProperties()
	{
		prtyListBoxUIInfo* pLBUII;
		pLBUII = new prtyListBoxUIInfo(&(m_Data.m_Commands), "Prefs", "A list of the quick commands");
		pLBUII->m_bChecked = true;
		pLBUII->m_bOnlyOneSelected = false;
		AddProperty( pLBUII );
		m_pLBUIInfo = pLBUII;
	
		m_Data.m_Commands.AddCallback(new prtyCallbackWrapper<prefsQuickObject>(this, &prefsQuickObject::CommandsPropertyChanged));
	}

	//--------------------------------------------------------------------
	//	CommandsPropertyChanged - notification that commands have changed.
	//--------------------------------------------------------------------
	void prefsQuickObject::CommandsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		// what to do?
	}

	//====================================================================
	//====================================================================
	class QuickPrefsNameResolver : public pythProperty::NameResolver
	{
		//------------------------------------------------------------
		// Resolve the string "Preferences" into our property object
		// in order to be accessible from python.
		//------------------------------------------------------------
		virtual prefsQuickObject* ResolveName(std::string &i_PropertyObjectName)
		{
			if (i_PropertyObjectName == std::string("QuickPrefs"))
			{
				return l_pPrefsQuickObject;
			}
			return NULL;
		}
	};

	shared_ptr<QuickPrefsNameResolver> l_NameResolver(new QuickPrefsNameResolver);
}


//============================================================================
//============================================================================
namespace prefsQuickMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init()
	{
		//	main prefs
		//
		if ( l_pPrefsQuickObject == 0 )
		{
			l_pPrefsQuickObject = new prefsQuickObject();

			// Add name resolver now that we have a property object
			pythProperty::AddNameResolver( l_NameResolver ); 
		}

		ReadPrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp()
	{
		if (l_pPrefsQuickObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_pPrefsQuickObject;
			l_pPrefsQuickObject = NULL;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadPrefs()
	{
		l_pPrefsQuickObject->ReadPrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WritePrefs()
	{
		//
		//	main prefs
		//
		l_pPrefsQuickObject->WritePrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Show(const int i_ScreenX, const int i_ScreenY)
	{

#ifdef USE_WXWIDGETS

		//	Note: I don't like the hard-code, but I'm having troubles getting wxWidgets to change size after the fact.
		int x,y;
		x = i_ScreenX - 200;
		y = i_ScreenY - 150;
		prefsQuickDialog* pQuickDlg = new prefsQuickDialog(twxSystem::g_pMainForm, wxID_ANY, wxEmptyString, wxPoint(x,y));
		pQuickDlg->BuildButtons( l_pPrefsQuickObject->m_Data, i_ScreenX, i_ScreenY );
		pQuickDlg->ShowModal();
		delete pQuickDlg;
#endif
	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add Prefs actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
/*
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Import
		pCmd = new cmaCommandSimple("Preferences", 
									"Tools", 
									"Application Preferences",
									&PrefsDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Tools", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Tools", "Preferences", pCmd );
*/
	}

#ifdef USE_WXWIDGETS
#endif

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prefsQuickData& Data()
	{
		if ( !l_bDataReadIn )
		{
			ReadPrefs();
			l_bDataReadIn = true;
		}

		return l_pPrefsQuickObject->m_Data;
	}

	//------------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//------------------------------------------------------------------------
	void RegisterInterest( prefsQuickDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		l_QuickPrefsInterestList.push_back( i_pInterest );
	}

	//------------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//------------------------------------------------------------------------
	void UnRegisterInterest( prefsQuickDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		envSTLHelpers::RemoveOneValue( l_QuickPrefsInterestList, i_pInterest );
	}

	//------------------------------------------------------------------------
	//	Clear() - clear the interest list
	//------------------------------------------------------------------------
	void ClearInterests()
	{
		l_QuickPrefsInterestList.clear();
	}

	//------------------------------------------------------------------------
	//	This gets called to let the interests know that the data has changed
	//------------------------------------------------------------------------
	void DataUpdated()
	{
		for (int i = 0; i < l_QuickPrefsInterestList.size(); ++i)
		{
			l_QuickPrefsInterestList[i]->prefsQuickDataUpdated(l_pPrefsQuickObject->m_Data);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_pPrefsQuickObject;
	}
}
