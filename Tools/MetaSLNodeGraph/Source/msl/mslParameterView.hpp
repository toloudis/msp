/*****************************************************************************
**  mslParameterView.hpp
**
**    MetaSL Parameter view in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_PARAMETERVIEW_HPP
#error mslParameterView.hpp multiply included
#endif
#define MSL_PARAMETERVIEW_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class mslParameterView : public wxPanel
{
public:
    // ctor(s)
    mslParameterView(wxWindow* parent);
	~mslParameterView();

    //! Set the Parameter view.
    /*! \param  Parameter_view    Parameter view to set in the window. */
    void SetView(IParameter_view *Parameter_view);

    //! Get the Parameter view.
    /*! \returns    Parameter view associated with the window. */
    IParameter_view *GetView();

    //! Resize the graph window.
    /*! \param  size    Target size. */
    void Resize(const wxSize &size);

    //! Called on resize.
    /* \param event    Size event. */
    void OnSize(wxSizeEvent &event);

private:
    IParameter_view  *m_Parameter_view;		//!< mental mill Parameter view interfaces

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
