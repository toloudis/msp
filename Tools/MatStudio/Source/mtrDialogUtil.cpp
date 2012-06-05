/*****************************************************************************
**	mtrDialogUtil.cpp
**
**	API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "StdAfx.h"
#include "mtrDialogUtil.hpp"
#include "mtrOperations.hpp"

#include "MainForm.h"
#include "MaterialDialog.h"
#include "LightsDialog.h"

#include "mtrErrorHandler.hpp"

#include "dbgLog.hpp"

using namespace MatStudio;

namespace mtrDialogUtil
{

	namespace
	{
		mtrMaterialTemplate l_MaterialData;
		int l_SelIndex = -1;

		void setup_material(int i_Index)
		{
			mtrOperations::SetSelectedMaterialIndex(i_Index);
			l_MaterialData = mtrLevel::GetMaterialData(i_Index);
			if (MaterialDialog::FormInstance)
				MaterialDialog::FormInstance->UpdateData();
		}

		class myCallback : public mtrMaterialSelectCallback, 
							public mtrModelChangeCallback
		{
		public:
			virtual void SelectMaterial(int i_Index)
			{
				l_SelIndex = i_Index;
				MainForm::FormInstance->SelectMaterial(i_Index);
				setup_material(i_Index);
			}
		

			virtual void ModelChange()
			{
				if (MainForm::FormInstance != 0)
				{
					MainForm::FormInstance->ClearMaterialNames();

					int num_mats = mtrLevel::GetNumMaterials();
					for (int i=0; i<num_mats; i++)
						MainForm::FormInstance->AddMaterialName(mtrLevel::GetMaterialName(i));

					if (num_mats > 0)
					{
						MainForm::FormInstance->EnableUI(true);
						MainForm::FormInstance->SelectMaterial(0);
						mtrLevel::SelectMaterial(0);
						setup_material(0);
					}
					else
					{
						MainForm::FormInstance->EnableUI(false);
						ShowMaterialDialog(false);
					}
				}
			}
		};

		class myErrorHandler : public mtrErrorHandler
		{
		public:
			//============================================================================
			//	FileDoesntExist()
			//============================================================================
			virtual void	FileDoesntExist(const char* i_Filename)
			{
				System::String* msg = S"File doesn't exist: ";
				System::Windows::Forms::MessageBox::Show( msg->Concat( new System::String(i_Filename) ),
					S"File not found");
			}

			//============================================================================
			//	FileReadOnly()
			//============================================================================
			virtual void	FileReadOnly(const char* i_Filename)
			{
				System::String* msg = S"File is read only: ";
				System::Windows::Forms::MessageBox::Show( msg->Concat( new System::String(i_Filename) ),
					S"Read Only" );
			}

			//============================================================================
			//	CantSaveFile()
			//============================================================================
			virtual void	CantSaveFile(const char* i_Filename)
			{
				System::String* msg = S"Can't save file: ";
				System::Windows::Forms::MessageBox::Show( msg->Concat( new System::String(i_Filename) ),
					S"Can't save" );
			}

			//============================================================================
			//	CantParseMaterials()
			//============================================================================
			virtual void	CantParseMaterials(const char* i_Filename)
			{
				System::String* msg = S"Can't parse materials: ";
				System::Windows::Forms::MessageBox::Show( msg->Concat( new System::String(i_Filename) ),
					S"Can't parse materials" );
			}

			//============================================================================
			//	GeneralError
			//============================================================================
			virtual void	GeneralError()
			{
				System::Windows::Forms::MessageBox::Show( S"General error.", S"General error" );
			}
		};

		myCallback* l_pInterest = 0;
		myErrorHandler* l_pErrorHandle = 0;
	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		l_pInterest = new myCallback();
		mtrLevel::SetMaterialSelectCallback(l_pInterest);
		mtrLevel::SetModelChangeCallback(l_pInterest);

		l_pErrorHandle = new myErrorHandler();
		mtrErrorHandler::SetErrorHandler( l_pErrorHandle );
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		MaterialDialog::FormInstance = 0;

		mtrLevel::SetMaterialSelectCallback(NULL);
		mtrLevel::SetModelChangeCallback(NULL);
		delete l_pInterest;

		mtrErrorHandler::SetErrorHandler( NULL );
		delete l_pErrorHandle;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowMaterialDialog(bool i_bShow)
	{
		if (i_bShow)
		{
			if (!MaterialDialog::FormInstance)
			{
				MaterialDialog::FormInstance = new MaterialDialog(l_MaterialData);
			}
			MaterialDialog::FormInstance->Show();
		}
		else
		{
			if (MaterialDialog::FormInstance)
				MaterialDialog::FormInstance->Hide();
		}

	}

	
	//--------------------------------------------------------------------
	// When something outside of the material dialog changes the
	// material values, call this to update the dialog to the new data.
	//--------------------------------------------------------------------
	void UpdateMaterialDialog()
	{
		if ((MaterialDialog::FormInstance != 0) && (l_SelIndex >= 0))
		{
			l_MaterialData = mtrLevel::GetMaterialData(l_SelIndex);
			MaterialDialog::FormInstance->UpdateData();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowLightsDialog(bool i_bShow)
	{
		if (i_bShow)
		{
			if (!LightsDialog::FormInstance)
			{
				LightsDialog::FormInstance = new LightsDialog();
			}
			LightsDialog::FormInstance->Show();
		}
		else
		{
			if (LightsDialog::FormInstance)
				LightsDialog::FormInstance->Hide();
		}

	}

}	// end of namespace
