/*****************************************************************************
**	tmlnDriverUserScriptFloat.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/UserScript/tmlnDriverUserScriptFloat.hpp"

#include "Support/pyth/pythUserScriptUtil.hpp"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverUserScriptFloat::tmlnDriverUserScriptFloat(tmlnChannelFloat &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverUserScriptTemplate<tmlnChannelFloat>(i_Channel, i_ChunkName, "value = 0")
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverUserScriptFloat::Operate(const maTime& i_Time)
{
	float goal = 0;
	std::string user_script = m_PythonScript.GetValue();
	if (!user_script.empty())
	{
		std::string error_msg;
		if (pythUserScriptUtil::ExecuteUserScript(user_script, goal, error_msg))
		{
			// Check and handle blend into driver
			if (IsBefore(i_Time))
			{
				float cur = m_Channel.GetValue();
				float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
				float value = goal*percent + cur*(1.0f - percent);	// linear blend

				// If smooth blend, add in influence of the gradients
				if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
				{
					AddGradientInfluence(&m_Channel, i_Time, 
						this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
						goal, value);
				}

				m_Channel.SetValue(value);
			}
			else
			{
				// within driver range
				m_Channel.SetValue(goal);
			}
		}

		// Display any errors to user in read-only property:
		this->m_ErrorMessage.SetValue( error_msg );

		// Unless the script is empty, the expression's return value might change 
		// at any time without telling us, so we need to always operate
		this->MarkDirty();
	}
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverUserScriptFloat::GetBeginValue(tmlnChannel* i_pChannel, float& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverUserScriptFloat::GetEndValue(tmlnChannel* i_pChannel, float& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverUserScriptFloat::GetEndGradient(tmlnChannel* i_pChannel, float& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Channel.GetValue(), 
		o_Gradient);
}