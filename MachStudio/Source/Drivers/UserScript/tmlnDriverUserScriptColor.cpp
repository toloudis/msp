/*****************************************************************************
**	tmlnDriverUserScriptColor.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/UserScript/tmlnDriverUserScriptColor.hpp"

#include "Support/pyth/pythUserScriptUtil.hpp"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverUserScriptColor::tmlnDriverUserScriptColor(tmlnChannelColor &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverUserScriptTemplate<tmlnChannelColor>(i_Channel, i_ChunkName, "value = (1,1,1,1)")
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverUserScriptColor::Operate(const maTime& i_Time)
{
	maFloatRGBA goal(1,1,1,1);
	std::string user_script = m_PythonScript.GetValue();
	if (!user_script.empty())
	{
		std::string error_msg;
		if (pythUserScriptUtil::ExecuteUserScript(user_script, goal, error_msg))
		{
			// Check and handle blend into driver
			//
			if (IsBefore(i_Time))
			{
				maFloatRGBA cur = m_Channel.GetColor();

				float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
				maFloatRGBA color = goal*percent + cur*(1.0f - percent);	// linear blend
				m_Channel.SetColor(color);
			}
			else
			{
				// Within driver range
				//
				m_Channel.SetColor(goal);
			}
		}

		// Display any errors to user in read-only property:
		this->m_ErrorMessage.SetValue( error_msg );

		// Unless the script is empty, the expression's return value might change 
		// at any time without telling us, so we need to always operate
		this->MarkDirty();
	}
}
