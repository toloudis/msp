/*****************************************************************************
**  mslGraphView.hpp
**
**    MetaSL Node Graph view in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_GRAPHVIEW_HPP
#error mslGraphView.hpp multiply included
#endif
#define MSL_GRAPHVIEW_HPP

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
class mslGraphView : public wxPanel
{
public:
    // ctor(s)
    mslGraphView(wxWindow* parent);
	~mslGraphView();

    //! Set the graph view.
    /*! \param  graph_view  Graph view to set in the window. */
    void SetView(IGraph_view *graph_view);

    //! Get the graph view.
    /*! \returns		Graph view associated with the window. */
    IGraph_view *GetView();

    //! Resize the graph window.
    /*! \param  size    Target size. */
    void Resize(const wxSize &size);

    //! Called on resize.
    /* \param event    Size event. */
    void OnSize(wxSizeEvent &event);

private:
    IGraph_view  *m_graph_view;		//!< mental mill graph view interface

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
