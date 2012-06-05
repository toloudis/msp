/*****************************************************************************
**  mslToolView.hpp
**
**    MetaSL Tool view in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_TOOLVIEW_HPP
#error mslToolView.hpp multiply included
#endif
#define MSL_TOOLVIEW_HPP

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
class mslToolView : public wxPanel
{
public:
    // ctor(s)
    mslToolView(wxWindow* parent);
	~mslToolView();

    //! Set the tool view.
    /*! \param  tool_view    Tool view to set in the window. */
    void SetView(ITool_view *tool_view);

    //! Get the tool view.
    /*! \returns    Tool view associated with the window. */
    ITool_view *GetView();

    //! Resize the graph window.
    /*! \param  size    Target size. */
    void Resize(const wxSize &size);

    //! Called on resize.
    /* \param event    Size event. */
    void OnSize(wxSizeEvent &event);

private:
    ITool_view  *m_tool_view;		//!< mental mill tool view interfaces

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
