/*****************************************************************************
**	ImportDialog.hpp
**
**	Import dialog in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef IMPORTDIALOG_HPP
#error ImportDialog.hpp multiply included
#endif
#define IMPORTDIALOG_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef IMPORT_DATA_HPP
#include "Features/Import/ImportData.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <map>


//============================================================================
//============================================================================
class docDocument;


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Features/Import/wxGUI/ImportDialogBase.h"


//----------------------------------------------------------------------------
// Class ImportDialog
//----------------------------------------------------------------------------
class ImportDialog : public ImportDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ImportDialog( wxWindow* parent, ImportData& i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~ImportDialog();

	private:
		//------------------------------------------------------------------------
		// private functions
		//------------------------------------------------------------------------
		void load_document(const fsLocator& i_ImportLoc);
		void fillintree();
		void update_ImportDataList();
		void clear_data();

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void filePicker_FileChanged( wxFileDirPickerEvent& i_Event );
		virtual void treeCtrl_LeftMouseDown( wxMouseEvent& i_Event );
		virtual void buttonImport_Click( wxCommandEvent& i_Event );
		virtual void AllowDupes_OnCheckBox( wxCommandEvent& i_Event );

		ImportData& m_Data;
		shared_ptr<docDocument> m_pDoc;
};

#endif // USE_WXWIDGETS
