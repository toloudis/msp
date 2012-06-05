#pragma once

#ifndef CPTR_RENDEROPTIONSDIALOGUTIL_HPP
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#endif
#ifndef CPTR_RENDERUTIL_HPP
#include "Features/Capture/cptrRenderUtil.hpp"
#endif
#ifndef CPTR_RENDEROUTPUTDATAUTIL_HPP
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#endif

#ifndef RNDR_PREFSMGR_HPP
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#endif
#ifndef PRTY_FORMCONTROLBUILDER_HPP
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif

#include <assert.h>

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//
//============================================================================
namespace StudioFramework
{
	/// <summary> 
	/// Summary for RenderOptions
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class RenderOptions : public System::Windows::Forms::Form
	{
	public: 
		RenderOptions() 
		{
			InitializeComponent();

			SetComponentInitialValues();

			m_pMemory = gcnew tmaDialogMemory( this );
		}

	protected: 
		~RenderOptions()
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
	private: System::Windows::Forms::TabControl ^  tabControl_capture;
	private: System::Windows::Forms::TabPage^  tabPage_render;
	private: System::Windows::Forms::TabPage^  tabPage_options;
	private: System::Windows::Forms::Button ^  button_capture;
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_capture = (gcnew System::Windows::Forms::TabControl());
			this->button_capture = (gcnew System::Windows::Forms::Button());
			this->tabPage_render = (gcnew System::Windows::Forms::TabPage());
			this->tabPage_options = (gcnew System::Windows::Forms::TabPage());
			this->tabControl_capture->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_capture
			// 
			this->button_capture->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_capture->Location = System::Drawing::Point(192, 361);
			this->button_capture->Name = L"button_capture";
			this->button_capture->Size = System::Drawing::Size(75, 23);
			this->button_capture->TabIndex = 21;
			this->button_capture->Text = L"Render!";
			this->button_capture->Click += gcnew System::EventHandler(this, &RenderOptions::button_capture_Click);
			// 
			// tabPage_render
			// 
			this->tabPage_render->AutoScroll = true;
			this->tabPage_render->BackColor = System::Drawing::Color::Transparent;
			this->tabPage_render->Location = System::Drawing::Point(4, 22);
			this->tabPage_render->Name = L"tabPage_render";
			this->tabPage_render->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_render->Size = System::Drawing::Size(456, 327);
			this->tabPage_render->TabIndex = 5;
			this->tabPage_render->Text = L"Render";
			this->tabPage_render->UseVisualStyleBackColor = true;
			// 
			// tabPage_options
			// 
			this->tabPage_options->AutoScroll = true;
			this->tabPage_options->Location = System::Drawing::Point(4, 22);
			this->tabPage_options->Name = L"tabPage_options";
			this->tabPage_options->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_options->Size = System::Drawing::Size(456, 327);
			this->tabPage_options->TabIndex = 7;
			this->tabPage_options->Text = L"Options";
			this->tabPage_options->UseVisualStyleBackColor = true;
			// 
			// tabControl_capture
			// 
			this->tabControl_capture->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_capture->Controls->Add(this->tabPage_options);
			this->tabControl_capture->Controls->Add(this->tabPage_render);
			this->tabControl_capture->Location = System::Drawing::Point(0, 0);
			this->tabControl_capture->Name = L"tabControl_capture";
			this->tabControl_capture->SelectedIndex = 0;
			this->tabControl_capture->Size = System::Drawing::Size(464, 353);
			this->tabControl_capture->TabIndex = 8;
			// 
			// RenderOptions
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(466, 392);
			this->Controls->Add(this->tabControl_capture);
			this->Controls->Add(this->button_capture);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(432, 400);
			this->Name = L"RenderOptions";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Capture Options";
			this->TopMost = true;
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &RenderOptions::RenderOptions_Closing);
			this->tabControl_capture->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

	private: tmaDialogMemory^ m_pMemory;

private: System::Void button_capture_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			SetDataValues();

			cptrRenderOptionsDialogUtil::Hide();

			this->Close();
		 }
private: System::Void RenderOptions_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
		 {
			 //	if the dialog is closing and the user didn't hit capture then exit out of capture mode
			 //
			 if ( !cptrRenderUtil::GetCapture() )
			 {
				cptrRenderOptionsDialogUtil::SetExitting(true);
			 }
		 }


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetComponentInitialValues()
	{
		//	build the dynamic tabs
		//
		bool lc_SHOW_CATEGORY = true;

		// add the RENDER tab
		prtyObject* pRDO = rndrPrefsMgr::GetDataObject(rndrPrefsMgr::e_RenderFullPrefs);
		prtyFormControlBuilder::BuildForm( tabPage_render, (pRDO->GetList()), lc_SHOW_CATEGORY );

		// add the OPTIONS tab
		prtyFormControlBuilder::SetLabelWidth(240);

		prtyObject* pOO = cptrRenderOutputDataUtil::GetDataObject();

		// debug only
		//cptrRenderOutputDataUtil::Debug();

		// Hand ordered the properies -- pOO->SortListByPropertyName();
		pOO->SortListByCategory();
		prtyFormControlBuilder::BuildForm( tabPage_options, (pOO->GetList()), lc_SHOW_CATEGORY );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Void SetDataValues()
	{
	}
};
}

#endif // _MANAGED
