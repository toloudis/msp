#pragma once

#ifndef ENVT_TEXTURELIST_HPP
#include "envtTextureList.hpp"
#endif
#ifndef GSUP_TREEVIEWUTIL_HPP
#include "gsupTreeViewUtil.hpp"
#endif

#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif
#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "tmaManagedControlUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "tmaManagedStringUtils.hpp"
#endif
#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace SystemEnvironments
{
	/// <summary> 
	/// Summary for envtTextureChooser
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class envtTextureChooser : public System::Windows::Forms::Form
	{
	public: 
		envtTextureChooser(void)
		{
			InitializeComponent();

			BuildTreeView();
		}

		void BuildTreeView()
		{
			fsysFileList file_list;
			envtTextureList::BuildFileList(file_list);

			treeView_textures->BeginUpdate();

			//this->treeView_textures->Controls->Clear();
			tmaManagedControlUtil::DisposeOfControls(this->treeView_textures->Controls);

			gsupTreeViewUtil::PopulateTreeView( treeView_textures, file_list, true );

			treeView_textures->EndUpdate();
		}

		System::String ^ GetResultString()
		{
			return resultString;
		}
        
	public: 
		~envtTextureChooser()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button ^  buttonOK;
	private: System::Windows::Forms::Button ^  buttonCancel;
	private: System::String ^ resultString;
	private: System::Windows::Forms::TreeView ^  treeView_textures;

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
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonCancel = (gcnew System::Windows::Forms::Button());
			this->treeView_textures = (gcnew System::Windows::Forms::TreeView());
			this->SuspendLayout();
			// 
			// buttonOK
			// 
			this->buttonOK->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->buttonOK->Location = System::Drawing::Point(20, 194);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(87, 28);
			this->buttonOK->TabIndex = 1;
			this->buttonOK->Text = L"OK";
			this->buttonOK->Click += gcnew System::EventHandler(this, &envtTextureChooser::buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->buttonCancel->Location = System::Drawing::Point(127, 194);
			this->buttonCancel->Name = L"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(93, 28);
			this->buttonCancel->TabIndex = 2;
			this->buttonCancel->Text = L"Cancel";
			this->buttonCancel->Click += gcnew System::EventHandler(this, &envtTextureChooser::buttonCancel_Click);
			// 
			// treeView_textures
			// 
			this->treeView_textures->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView_textures->Location = System::Drawing::Point(8, 8);
			this->treeView_textures->Name = L"treeView_textures";
			this->treeView_textures->Size = System::Drawing::Size(224, 176);
			this->treeView_textures->TabIndex = 3;
			this->treeView_textures->DoubleClick += gcnew System::EventHandler(this, &envtTextureChooser::treeView_textures_DoubleClick);
			// 
			// envtTextureChooser
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(243, 231);
			this->Controls->Add(this->treeView_textures);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Name = L"envtTextureChooser";
			this->Text = L"Choose Texture";
			this->ResumeLayout(false);

		}		

		//

	private: System::Void SelectedItemOrOK()
			{
				// make sure the user selected a childless node.
				//
				if ( treeView_textures->SelectedNode != nullptr )
				{
					if ( treeView_textures->SelectedNode->GetNodeCount(true) == 0 )
					{
						resultString = this->treeView_textures->SelectedNode->Text;

						this->DialogResult = ::DialogResult::OK;
						this->Close();

						// change focus to the main app
						//muiMainWindow::Focus();
					}
				}
			}
	private: System::Void buttonOK_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 SelectedItemOrOK();
			 }

	private: System::Void buttonCancel_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 this->DialogResult = ::DialogResult::Cancel;
				 this->Close();
			 }

	private: System::Void treeView_textures_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 SelectedItemOrOK();
			 }
};
}
#endif // _MANAGED

