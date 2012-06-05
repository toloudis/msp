/*****************************************************************************
**	gsupReAttachUtil.hpp
**
**	GUI independent way to choose a new named object for an attachment
**	that can no longer be found.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GSUP_REATTACHUTIL_HPP
#error gsupReAttachUtil.hpp multiply included
#endif
#define GSUP_REATTACHUTIL_HPP

#include <string>
#include <vector>

//============================================================================
//============================================================================
class nameString;

//============================================================================
//============================================================================
namespace gsupReAttachUtil
{
	//--------------------------------------------------------------------
	//	Prompt user for for replacement for missing object.
	//	Returns true is a replacement was given and returns the name
	//	within o_NewAttachment.
	//--------------------------------------------------------------------
	bool  PromptReattach(const std::vector<nameString>& i_Names,
						 const std::string& i_Title,
						 const std::string& i_Message,
						 nameString& o_NewAttachment);

}	// end of namespace
