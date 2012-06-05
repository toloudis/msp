#pragma once

#ifndef CPTR_RENDERSTATSDATA_HPP
#include "Features/Capture/cptrRenderStatsData.hpp"
#endif
#ifndef CPTR_RENDERSTATSDIALOGUTIL_HPP
#include "Features/Capture/cptrRenderStatsDialogUtil.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace StudioFramework
{
	/// <summary> 
	/// Summary for cptrRenderStatsMain
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cptrRenderStatsMain : public System::Windows::Forms::Form
	{
	public: 
		cptrRenderStatsMain()
		{
			InitializeComponent();
		}

	protected: 
		~cptrRenderStatsMain()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;
	private: System::Windows::Forms::TextBox ^  textBox_stats;

		tmaDialogMemory^	m_pMemory;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBox_stats = gcnew System::Windows::Forms::TextBox();
			this->SuspendLayout();
			// 
			// textBox_stats
			// 
			this->textBox_stats->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_stats->Location = System::Drawing::Point(8, 8);
			this->textBox_stats->Multiline = true;
			this->textBox_stats->Name = "textBox_stats";
			this->textBox_stats->ReadOnly = true;
			this->textBox_stats->Size = System::Drawing::Size(432, 488);
			this->textBox_stats->TabIndex = 0;
			this->textBox_stats->Text = "[stats will show here]";
			// 
			// cptrRenderStatsMain
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(448, 502);
			this->Controls->Add(this->textBox_stats);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = "cptrRenderStatsMain";
			this->Text = "Render Log";
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &cptrRenderStatsMain::cptrRenderStatsMain_Closing);
			this->Load += gcnew System::EventHandler(this, &cptrRenderStatsMain::cptrRenderStatsMain_Load);
			this->ResumeLayout(false);

		}		


	
	private: System::Void cptrRenderStatsMain_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
			{
				cptrRenderStatsDialogUtil::Hide();
			}

	private: System::Void cptrRenderStatsMain_Load(System::Object ^  sender, System::EventArgs ^  e)
			 {
				m_pMemory = gcnew tmaDialogMemory( this );
			 }
	public: System::Void UpdateStatString( System::String^ i_pString )
			{
				this->textBox_stats->Text = i_pString;
				this->textBox_stats->Refresh();
			}
	};
}

#endif // _MANAGED
