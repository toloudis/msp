/*****************************************************************************
**	eonExportUtil.hpp
**
**		Utilities for configuring export to EON and baking of textures.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef EON_EXPORTUTIL_HPP
#error eonExportUtil.hpp multiply included
#endif
#define EON_EXPORTUTIL_HPP


#ifndef MNM_CONSTANTS_HPP
#include "Support/mnm/mnmConstants.hpp"
#endif 

//============================================================================
//============================================================================
#ifdef EON_REALITY
namespace eonExportUtil
{
	//------------------------------------------------------------------------
	// DoExportDialog
	//------------------------------------------------------------------------
	void DoExportDialog();
}
#endif // EON_REALITY
