/*****************************************************************************
**	chnlCommandUtil.hpp
**
**		API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_COMMANDUTIL_HPP
#error chnlCommandUtil.hpp multiply included
#endif
#define CHNL_COMMANDUTIL_HPP

#ifndef TMLN_CREATOR_HPP
#include "Support/tmln/tmlnCreator.hpp"
#endif

//============================================================================
//============================================================================
class tmlnScriptObject;

//============================================================================
//============================================================================
namespace chnlCommandUtil
{
	//--------------------------------------------------------------------
	// CreateDriverButton
	//--------------------------------------------------------------------
	void  CreateDriverButton( const char* i_pDriverName, 
							  const char* i_pTabPageName);

	//--------------------------------------------------------------------
	// CreateDriverButtons
	//--------------------------------------------------------------------
	void  CreateDriverButtons(tmlnDriverCreator* i_pDriverCreator,
							  const char* i_pTabPageName);
}
