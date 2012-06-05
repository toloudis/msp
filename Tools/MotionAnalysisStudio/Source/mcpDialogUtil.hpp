/*****************************************************************************
**	mcpDialogUtil.hpp
**
**	API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef MCP_DIALOGUTIL_HPP
#error mcpDialogUtil.hpp multiply included
#endif
#define MCP_DIALOGUTIL_HPP


namespace mcpDialogUtil
{

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowLightsDialog(bool i_bShow);

}	// end of namespace
