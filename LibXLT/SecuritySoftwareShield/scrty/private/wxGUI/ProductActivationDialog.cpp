/*****************************************************************************
**	ProductActivationDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "SecuritySoftwareShield/scrty/private/wxGUI/ProductActivationDialog.hpp"


#ifdef USE_WXWIDGETS
//============================================================================
//============================================================================
namespace
{

}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
ProductActivationDialog::ProductActivationDialog( wxWindow* parent )
:	ProductActivationDialogBase( parent )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ProductActivationDialog::~ProductActivationDialog()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
BSTR ProductActivationDialog::GetSerial()
{
	BSTR serialstr = SysAllocString(m_textCtrl_Serial->GetLineText(0).wc_str(wxConvLocal));
	return serialstr;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ProductActivationDialog::button_ActivateSerial_OnButtonClick( wxCommandEvent& i_Event )
{
	// Close the dialog
	this->EndModal( wxOK );
}
	
#endif // USE_WXWIDGETS
