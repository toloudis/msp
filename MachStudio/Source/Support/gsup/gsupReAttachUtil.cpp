/*****************************************************************************
**	gsupReAttachUtil.cpp
**
**	GUI independent way to choose a new named object for an attachment
**	that can no longer be found.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/gsup/gsupReAttachUtil.hpp"

#include "Core/prty/prtyListBoxUIInfo.hpp"
#include "Core/prty/prtyName.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"

//============================================================================
//============================================================================
namespace gsupReAttachUtil
{
	namespace
	{
	}	// end of namespace

	//--------------------------------------------------------------------
	//	Prompt user for for replacement for missing object.
	//	Returns true is a replacement was given and returns the name
	//	within o_NewAttachment.
	//--------------------------------------------------------------------
	bool  PromptReattach(const std::vector<nameString>& i_Names,
						 const std::string& i_Title,
						 const std::string& i_Message,
						 nameString& o_NewAttachment)
	{				
		// Configuration
		prtyName replacementName("Replacement");

		// Setup dialog
		prtyPropertyUIInfoContainer replaceInfo;
		prtyListBoxUIInfo *pLBUII = new prtyListBoxUIInfo(&replacementName);
		for (int i=0; i<i_Names.size(); i++)
			pLBUII->AddItem(i_Names[i].GetString());
		replaceInfo.Add(pLBUII);

		if (guiPropertyDialog::ShowModal(i_Title.c_str(), 
										 replaceInfo, 
										 i_Message.c_str()) == guiPropertyDialog::e_OK)
		{
			o_NewAttachment = replacementName.GetValue();
			return true;
		}
		return false;
	}

}	// end of namespace
