/*****************************************************************************
**	MayaExportUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/MayaExport/MayaExportUtil.hpp"

#include "Features/MayaExport/ExportMain.h"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Support/mexp/mexpExportData.hpp"
#include "Support/mexp/mexpMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <string>


//============================================================================
//============================================================================
//using namespace StudioFramework;


//============================================================================
//============================================================================
namespace MayaExportUtil
{
	namespace
	{
		bool l_bAdded = false;

		class CommandMayaExport : public cmaCommand
		{
			public:
				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				static const std::string GetConstTagName()
				{
					return std::string("Maya Ascii Export");
				}

				//--------------------------------------------------------------------
				// constructor
				//--------------------------------------------------------------------
				CommandMayaExport()
				: cmaCommand( std::string(GetConstTagName()),
							CommandMayaExport::CommandExecuteHandler,
							NULL )
				{
					this->SetDescription(std::string("Export data to Maya"));
					this->SetCategory( std::string("Import - Export") );
				}

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				static void CommandExecuteHandler( cmaCommand* pCmd )
				{
#ifdef _MANAGED
					mexpExportData data = mexpMgr::GetPotentialExportData();
					
					Features::ExportMain ^dialog = gcnew Features::ExportMain(data);
					if (dialog->ShowDialog() == ::DialogResult::OK)
					{
						const char *c_Filter = "Maya ascii (*.ma)|*.ma|All files (*.*)|*.*";
						fsLocator file_loc;
						if (guiFileDialogUtils::GetSaveFileName(c_Filter, file_loc))
						{
							try
							{
								mexpMgr::DoExport(file_loc, data);
							}
							catch( const fsReadOnlyX& i_Ex )
							{
								std::string filename;
								fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
								DBG_WARNING1("fsReadOnlyX: %s", filename.c_str());
								std::string msg = "File is read only: " + filename;
								guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
							}
							catch( const fsDiskFullX& i_Ex )
							{
								std::string filename;
								fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
								DBG_WARNING1("fsDiskFullX: %s", filename.c_str());

								std::string msg = "Out of disk space or the disk is corrupt.  Could not write " + filename;
								DBG_ERROR1("%s", msg.c_str() );
								guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
							}
						}
					}

					delete dialog;
#endif
				}
		};

	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add Import actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		DBG_ASSERT0( !l_bAdded, "Tried to add menu item multiple times" );

		l_bAdded = true;

		int index;
		index = guiMenuMgr::AddMenuItem( "Tools", "Maya Ascii Export" );

		cmaCommand* pCmd = new CommandMayaExport();
		guiCommandMgr::Add( pCmd, CommandMayaExport::GetConstTagName(), index );
	}

}
