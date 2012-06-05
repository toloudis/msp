/*****************************************************************************
**	ProductActivationDialog.hpp
**
**		Simple production activation dialog
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef PRODUCTACTIVATIONDIALOG_HPP
#error ProductActivationDialog.hpp multiply included
#endif
#define PRODUCTACTIVATIONDIALOG_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "SecuritySoftwareShield/scrty/private/wxGUI/ProductActivationDialogBase.h"


//----------------------------------------------------------------------------
// Class ProductActivationDialog
//----------------------------------------------------------------------------
class ProductActivationDialog : public ProductActivationDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ProductActivationDialog( wxWindow* parent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~ProductActivationDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		BSTR GetSerial();

	private:
		//------------------------------------------------------------------------
		// private functions
		//------------------------------------------------------------------------
		//void load_document(const fsLocator& i_ImportLoc);
		//void fillintree();
		//void update_ImportDataList();
		//void clear_data();

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		//virtual void filePicker_FileChanged( wxFileDirPickerEvent& i_Event );
		//virtual void treeCtrl_LeftMouseDown( wxMouseEvent& i_Event );
		virtual void button_ActivateSerial_OnButtonClick( wxCommandEvent& i_Event );
};

#endif // USE_WXWIDGETS
