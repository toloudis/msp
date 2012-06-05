/*****************************************************************************
**	tmlnDriverUserScriptBoolean.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/UserScript/tmlnDriverUserScriptBoolean.hpp"

#include "Support/pyth/pythUserScriptUtil.hpp"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverUserScriptBoolean::tmlnDriverUserScriptBoolean(tmlnChannelBoolean &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverUserScriptTemplate<tmlnChannelBoolean>(i_Channel, i_ChunkName, "value = False")
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverUserScriptBoolean::Operate(const maTime& i_Time)
{
	bool goal = false;
	std::string user_script = m_PythonScript.GetValue();
	if (!user_script.empty())
	{
		std::string error_msg;
		if (pythUserScriptUtil::ExecuteUserScript(user_script, goal, error_msg))
		{
			m_Channel.SetState(goal);
		}

		// Display any errors to user in read-only property:
		this->m_ErrorMessage.SetValue( error_msg );

		// Unless the script is empty, the expression's return value might change 
		// at any time without telling us, so we need to always operate
		this->MarkDirty();
	}
}
