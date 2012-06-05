/********************************************************************************************\
**  mcpCallbacks.hpp
**
**      Callabacks out to GUI layer.
**			1) when user clicks on material with mouse
**			2) when new model is loaded or cleared
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	MCP_CALLBACKS_HPP
#error	mcpCallbacks.hpp included recursively.
#endif
#define	MCP_CALLBACKS_HPP


//============================================================================================
// for receiving callbacks when new model is loaded or cleared
//============================================================================================
class mcpModelChangeCallback
{
public:
	virtual void ModelChange() = 0;
};

