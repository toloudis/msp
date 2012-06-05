/*****************************************************************************
**  chnlDriverClip.hpp
**
**      Derivation from ChannelClip defined in TimelineControls library
**	to handle callbacks from when the clip's time is altered.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_DRIVERCLIP_HPP
#error chnlDriverClip.hpp multiply included
#endif
#define CHNL_DRIVERCLIP_HPP

#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif
#ifndef CHNL_OPERATIONS_HPP
#include "Features/Channels/chnlOperations.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef PRTY_PROPERTY_HPP
#include "Core/prty/prtyProperty.hpp"
#endif
#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif

#ifdef _MANAGED


//============================================================================
//============================================================================
public ref class chnlDriverClip : public TimelineControls::ChannelClip
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlDriverClip(	tmlnDriver& i_Driver, 
						System::String^ i_Name, 
						double i_BeginTime, 
						double i_EndTime)
		:	TimelineControls::ChannelClip(	i_Name, 
											i_BeginTime, 
											i_EndTime,
											(TimelineControls::ChannelClip::BlendType)i_Driver.GetBlendType(), 
											i_Driver.GetBlendTime(),
											i_Driver.IsRestoreOriginalValue()),
			m_Driver(i_Driver)
		{

		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlDriverClip(chnlDriverClip^ i_Clip)
		:	TimelineControls::ChannelClip( i_Clip ),
			m_Driver((i_Clip->Driver()))
		{
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chnlDriverClip()
		{
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual chnlDriverClip^ Clone(tmlnDriver& i_Driver)
		{
			//	implement this for each child class
			chnlDriverClip^ pDC = gcnew chnlDriverClip(i_Driver, this->Name, this->BeginTime, this->EndTime);
			return (pDC);
		}

		//--------------------------------------------------------------------
		// Access to driver
		//--------------------------------------------------------------------
		tmlnDriver&	Driver()
		{
			return m_Driver;
		}

		//--------------------------------------------------------------------
		// Update - get values from driver and set into clip. Called when
		//	a property of the driver has changed.
		//--------------------------------------------------------------------
		void Update()
		{
			this->BeginTime = m_Driver.GetBeginTime();
			this->EndTime = m_Driver.GetEndTime();
			this->Blend = (TimelineControls::ChannelClip::BlendType)m_Driver.GetBlendType();
			this->BlendTime = m_Driver.GetBlendTime();
			this->Restore = m_Driver.IsRestoreOriginalValue();

			maFloatRGBA color;
			color = m_Driver.GetClipFillColor();
			this->SetFillColor( color.GetRed(), color.GetGreen(), color.GetBlue() );

			this->Name = gcnew System::String(m_Driver.GetName().c_str());
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void SetTime(double i_BeginTime, double i_Duration) override
		{
			if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

			//DBG_LOG2("SetTime!: %f %f", i_BeginTime, i_Duration);
			m_Driver.SetBeginTime((float)i_BeginTime, prtyProperty::eNewUndo);
			m_Driver.SetEndTime((float)(i_BeginTime + i_Duration), prtyProperty::eContinueUndo);
			TimelineControls::ChannelClip::SetTime(i_BeginTime, i_Duration);

			chnlDialogUtil::UpdateTimeTicks();
			chnlOperations::NotifyScriptObject();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void SetBlend(	TimelineControls::ChannelClip::BlendType i_Type, 
								double i_BlendTime) override
		{
			if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

			//DBG_LOG2("SetBlend!: %d %f", i_Type, i_BlendTime);
			m_Driver.SetBlendType((tmlnDriver::BlendType)i_Type, prtyProperty::eNewUndo);
			m_Driver.SetBlendTime((float)i_BlendTime, prtyProperty::eContinueUndo);
			TimelineControls::ChannelClip::SetBlend(i_Type, i_BlendTime);
			chnlOperations::NotifyScriptObject();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void SetClipFillColor( float i_Red, float i_Green, float i_Blue )
		{
			this->SetFillColor( i_Red, i_Green, i_Blue );
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void SetRestore(bool i_bRestore)  override
		{
			if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

			//DBG_LOG1("SetRestore!: %d", i_bRestore);
			m_Driver.SetRestoreOriginalValue(i_bRestore);
			TimelineControls::ChannelClip::SetRestore(i_bRestore);
			chnlOperations::NotifyScriptObject();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void ShowProperties()  override
		{
			if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

			//if (m_Driver.IsAutoPopUpEditProperties())
				m_Driver.DoEditProperties();
			chnlOperations::NotifyScriptObject();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Select() override
		{
			if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

			m_Driver.DoSelect();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void SelectIcon() override
		{
			if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

			m_Driver.DoSelectIcon();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual System::String^ GetHoverDescription() override
		{
			if (&m_Driver == 0) return nullptr;	// if the user selected something else while this was open, don't let them crash

			std::string str = m_Driver.GetHoverDescription();
			return gcnew System::String(str.c_str());
		}

		//--------------------------------------------------------------------
		// Return string for description of driver (include its current state)
		// to display in the gui when the mouse interacts with the clip.
		//--------------------------------------------------------------------
		virtual System::String^ GetInteractionDescription(double i_InteractStart, double i_InteractDuration) override
		{
			if (&m_Driver == 0) return nullptr;	// if the user selected something else while this was open, don't let them crash

			std::string str = m_Driver.GetInteractionDescription(i_InteractStart, i_InteractDuration);
			return gcnew System::String(str.c_str());
		}

	private:
		tmlnDriver& m_Driver;
};

#endif // _MANAGED
