/****************************************************************************\
**	pwxCategoryLabel.hpp
**
**		Label that expands and compresses the properties beneath it.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_CATEGORYLABEL_HPP
#error pwxCategoryLabel.hpp multiply included
#endif
#define PWX_CATEGORYLABEL_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxCategoryLabel : public wxControl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pwxCategoryLabel(wxWindow* i_pParent, 
						 const std::string& i_DialogName,
						 const wxString& i_LabelText,
						 wxSizer* i_pSizer,
						 int i_ItemIndex);
		//----------------------------------------------------------------------------
		// Return true if this category label should be initially collapsed
		// based on what the user has done previously.
		//----------------------------------------------------------------------------
		bool GetInitialCollapsedState() const;

	protected:
		//--------------------------------------------------------------------
		// implement/override some base class virtuals
		//--------------------------------------------------------------------
		virtual wxSize DoGetBestSize() const;

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent &i_Event);
		void OnClick(wxMouseEvent& i_Event);

		std::string m_DialogName;
		wxString m_LabelText;
		wxSizer *m_pSizer;
		int m_ItemIndex;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
