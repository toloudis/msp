#pragma once

#ifndef CHNL_DRIVERCLIP_HPP
#include "Features/Channels/mGUI/chnlDriverClip.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMLN_TIMELINE_HPP
#include "Support/tmln/tmlnTimeLine.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace TimelineControls;


namespace StudioFramework
{
	/// <summary> 
	/// Summary for chnlMoveDriversForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class chnlMoveDriversForm : public System::Windows::Forms::Form
	{
	public: 
		chnlMoveDriversForm(System::Collections::ArrayList ^ i_ChannelList, bool i_bSelectedOnly)
		{
			InitializeComponent();

			m_ChannelList = i_ChannelList;
			m_LastTimeValue = 0;
			numericUpDown_time->Value = 0;
			m_bSelectedOnly = i_bSelectedOnly;

			//cmmObjectForm::FormInstance = this;
			m_pMemory = gcnew tmaDialogMemory( this );

			SetupData();
		}

	public: 
		~chnlMoveDriversForm()
		{
			delete m_pMemory;

			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Label ^  label_moveamount;
	private: System::Windows::Forms::NumericUpDown ^  numericUpDown_time;

	private: tmaDialogMemory^	m_pMemory;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->numericUpDown_time = (gcnew System::Windows::Forms::NumericUpDown());
			this->label_moveamount = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_time))->BeginInit();
			this->SuspendLayout();
			// 
			// numericUpDown_time
			// 
			this->numericUpDown_time->DecimalPlaces = 2;
			this->numericUpDown_time->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 131072});
			this->numericUpDown_time->Location = System::Drawing::Point(169, 8);
			this->numericUpDown_time->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 65536});
			this->numericUpDown_time->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {20, 0, 0, System::Int32::MinValue});
			this->numericUpDown_time->Name = L"numericUpDown_time";
			this->numericUpDown_time->Size = System::Drawing::Size(48, 20);
			this->numericUpDown_time->TabIndex = 0;
			this->numericUpDown_time->ValueChanged += gcnew System::EventHandler(this, &chnlMoveDriversForm::numericUpDown_time_ValueChanged);
			// 
			// label_moveamount
			// 
			this->label_moveamount->Location = System::Drawing::Point(4, 8);
			this->label_moveamount->Name = L"label_moveamount";
			this->label_moveamount->Size = System::Drawing::Size(155, 21);
			this->label_moveamount->TabIndex = 1;
			this->label_moveamount->Text = L"Time amount to move drivers";
			this->label_moveamount->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// chnlMoveDriversForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(224, 37);
			this->Controls->Add(this->label_moveamount);
			this->Controls->Add(this->numericUpDown_time);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"chnlMoveDriversForm";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"Move Drivers";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown_time))->EndInit();
			this->ResumeLayout(false);

		}		
	private: System::Void numericUpDown_time_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				if (numericUpDown_time->Value != m_LastTimeValue)
				{
					float diff = (float)System::Decimal::ToDouble(numericUpDown_time->Value - m_LastTimeValue);

					//	go through and move the selected clips only
					//
					std::set<tmlnDriver*> drivers;
					double time = tmlnTimeLine::GetValue(); // + 0.01f;
					for (int i=0; i<m_MovingClips->Count; i++)
					{
						chnlDriverClip^ clip = dynamic_cast<chnlDriverClip^>(m_MovingClips[i]);
						if (clip)
						{
							tmlnDriver* driver = &(clip->Driver());
							drivers.insert( dynamic_cast<tmlnDriver*>( driver ) );
						}
					}
					std::set<tmlnDriver*>::iterator it, end = drivers.end();
					for (it = drivers.begin(); it != end; ++it)
					{
						(*it)->SetBeginTime( (*it)->GetBeginTime() + diff );
						//double duration = m_MovingClips[i]->Duration;
						//m_MovingClips[i]->SetTime( m_MovingClips[i]->BeginTime + diff, duration );
						//dynamic_cast<ChannelControl^>(m_MovingChannels[i])->Invalidate();
					}
					for (int i=0; i<m_MovingClips->Count; i++)
					{
						dynamic_cast<ChannelControl^>(m_MovingChannels[i])->Invalidate();
					}
					// update the last move
					 m_LastTimeValue = numericUpDown_time->Value;
				}
			 }
	private: void SetupData()
			 {
				m_MovingClips = gcnew ChannelClipList;
				m_MovingChannels = gcnew System::Collections::ArrayList;

				if (m_bSelectedOnly)
				{
					//	go through and move the selected clips only
					//
					for (int i=0; i<m_ChannelList->Count; i++)
					{
						ChannelControl^ channel = dynamic_cast<ChannelControl^>(m_ChannelList[i]);
						ChannelClipList ^sel_clips = channel->SelectedClips;
						const int num_MovingClips = sel_clips->Count;

						//	don't allow this interaction if the channel is locked
						//
						if ((num_MovingClips > 0) && !channel->Locked)
						{
							for (int j=0; j<num_MovingClips; ++j)
							{
								this->m_MovingClips->Add( sel_clips[j] );
								this->m_MovingChannels->Add( channel );
							}
						}
					}
				}
				else
				{
					//	go through and move all the clips that start after the current time
					//
					double time = tmlnTimeLine::GetValue();
					for (int i=0; i<m_ChannelList->Count; i++)
					{
						ChannelControl^ channel = dynamic_cast<ChannelControl^>(m_ChannelList[i]);
						ChannelClipList ^clips = channel->Clips;
						for (int c=0; c < clips->Count; c++)
						{
							ChannelClip ^clip = clips[c];
							chnlDriverClip^ dclip = dynamic_cast<chnlDriverClip^>(clip);

							if (clip->BeginTime >= time)
							{
								this->m_MovingClips->Add( clip );
								this->m_MovingChannels->Add( channel );
							}
						}
					}
				}
			 }

	private: System::Decimal m_LastTimeValue;
	private: System::Collections::ArrayList ^ m_ChannelList;
	private: ChannelClipList ^ m_MovingClips;
	private: System::Collections::ArrayList ^ m_MovingChannels;
	private: bool m_bSelectedOnly;
	};
}
#endif // _MANAGED
