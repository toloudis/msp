/*****************************************************************************\
**	keyfKeyCreator.hpp
**
**		Utility for creating key drivers for channels.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef KEYF_KEYCREATOR_HPP
#error keyfKeyCreator.hpp multiply included
#endif
#define KEYF_KEYCREATOR_HPP


class tmlnChannel;
class tmlnScriptObject;

//============================================================================
//============================================================================
namespace keyfKeyCreator
{
	//--------------------------------------------------------------------
	//	Creates a key driver for the given channel at the current time.
	//	Returns true if a driver could be successfully created.
	//--------------------------------------------------------------------
	bool CreateKey(tmlnScriptObject *i_pObject, 
				   tmlnChannel *i_pChannel,
				   bool i_bDoUndo);

};
