/****************************************************************************\
**  mnmAppUtil.hpp
**
**      mnmAppUtil provides an interface to the Application.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_APPUTIL_HPP
#error mnmAppUtil.hpp multiply included
#endif
#define MNM_APPUTIL_HPP


//============================================================================
//============================================================================
namespace mnmAppUtil
{
	//------------------------------------------------------------------------
	//	UpdateTitleBar - update the titlebar with the appropriate text
	//------------------------------------------------------------------------
	void UpdateTitleBar( bool i_bDisplayDirtyFlag );
};

