/*****************************************************************************
**	tmlnDriverUserScriptOrientation.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Drivers/UserScript/tmlnDriverUserScriptOrientation.hpp"

#include "Support/pyth/pythUserScriptUtil.hpp"



//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverUserScriptOrientation::tmlnDriverUserScriptOrientation(tmlnChannelOrientation &i_Channel, chDefs::Name i_ChunkName)
:	tmlnDriverUserScriptTemplate<tmlnChannelOrientation>(i_Channel, i_ChunkName, "value = (0,0,0)")
{
}

//--------------------------------------------------------------------
//  Update the value of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverUserScriptOrientation::Operate(const maTime& i_Time)
{
	std::string user_script = m_PythonScript.GetValue();
	if (!user_script.empty())
	{
		std::string error_msg;
		float eulerX = 0, eulerY = 0, eulerZ = 0;
		if (pythUserScriptUtil::ExecuteUserScript(user_script, eulerX, eulerY, eulerZ, error_msg))
		{
			// Check and handle blend into driver
			if (IsBefore(i_Time))
			{
				maRotation goal;
				goal.SetEuler(eulerX, eulerY, eulerZ);
				maRotation cur = m_Channel.GetQuaternion();

				// If smooth blend, add in influence of the gradients
				if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
				{
					maRotation ori = tmlnBlendDriverOrientation::DoSquadBlend(&m_Channel,
						i_Time, this->GetBeginTime(), this->GetEndTime(), goal, cur);
					m_Channel.SetQuaternion(ori);	
				}
				else
				{
					// Linear blend
					float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
					//maRotation ori = goal*percent + cur*(1.0f - percent);	// linear blend
					maRotation ori;
					ori.Slerp(cur, goal, percent);
					m_Channel.SetQuaternion(ori);	
				}
			}
			else 
			{
				// within driver range
				m_Channel.SetEuler(eulerX, eulerY, eulerZ);
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
void tmlnDriverUserScriptOrientation::GetBeginValue(tmlnChannel* i_pChannel, maRotation& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetQuaternion();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverUserScriptOrientation::GetEndValue(tmlnChannel* i_pChannel, maRotation& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Channel.GetQuaternion();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverUserScriptOrientation::GetEndGradient(tmlnChannel* i_pChannel, maRotation& o_Gradient)
{
	tmlnBlendDriverOrientation::ComputeOrientationGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Channel.GetQuaternion(), 
		o_Gradient);
}
