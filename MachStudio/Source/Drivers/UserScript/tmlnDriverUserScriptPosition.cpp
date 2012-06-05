/*****************************************************************************
**	tmlnDriverUserScriptPosition.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/UserScript/tmlnDriverUserScriptPosition.hpp"

#include "Support/pyth/pythUserScriptUtil.hpp"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverUserScriptPosition::tmlnDriverUserScriptPosition(tmlnChannelPosition &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverUserScriptTemplate<tmlnChannelPosition>(i_Channel, i_ChunkName, "value = (0,0,0)")
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverUserScriptPosition::Operate(const maTime& i_Time)
{
	maPoint3d goal;
	std::string user_script = m_PythonScript.GetValue();
	if (!user_script.empty())
	{
		std::string error_msg;
		if (pythUserScriptUtil::ExecuteUserScript(user_script, goal, error_msg))
		{
			// Check and handle blend into driver
			if (IsBefore(i_Time))
			{
				maPoint3d cur = m_Channel.GetPosition();
				float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
				maPoint3d pos = goal*percent + cur*(1.0f - percent);

				// If smooth blend, add in influence of the gradients
				if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
				{
					AddGradientInfluence(&m_Channel, i_Time, 
						this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
						goal, pos);
				}

				m_Channel.SetPosition(pos);
			}
			else
			{
				m_Channel.SetPosition(goal);
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
void tmlnDriverUserScriptPosition::GetBeginValue(tmlnChannel* i_pChannel, maPoint3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetPosition();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverUserScriptPosition::GetEndValue(tmlnChannel* i_pChannel, maPoint3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetPosition();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverUserScriptPosition::GetEndGradient(tmlnChannel* i_pChannel, maPoint3d& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Channel.GetPosition(), 
		o_Gradient);
}
