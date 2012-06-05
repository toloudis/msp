/*****************************************************************************
**	chrDialogUtil.cpp
**
**	API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "chrDialogUtil.hpp"
#include "nonGUI/chrErrorHandler.hpp"
//#include "chrOperations.hpp"

#include "MainForm.h"
#include "ExpressionsDialog.h"
//#include "MorphDialog.h"
#include "LightsDialog.h"

//#include "nonGUI/chrLevel.hpp"

#include "Core/dbg/dbgLog.hpp"


//============================================================================
//============================================================================
using namespace CharacterStudio;


//============================================================================
//============================================================================
namespace chrDialogUtil
{
	namespace
	{
		class myCallback : public chrModelChangeCallback
		{
		public:
			virtual void ModelChange()
			{
				if (ExpressionsDialog::FormInstance != nullptr)
				{
					// Hide makes the form not visible anymore,
					// but keeps the controls around
					//ExpressionsDialog::FormInstance->Hide();

					// Close destroys the controls in the form
					ExpressionsDialog::FormInstance->Close();
				}

				if (chrLevel::HasModel())
				{	
					// Only show the expression dialog, if we have
					// expressions. Otherwise, the user will have to
					// open the dialog through the menu.
					int nTargets = chrLevel::GetNumExpressions();
					if (nTargets > 0)
					{
						ShowExpressionsDialog(true);
					}
				}

				if (MainForm::FormInstance != nullptr)
				{
					MainForm::FormInstance->EnableAnimationUI( chrLevel::CanAnimate() );
					MainForm::FormInstance->EnableSubdivUI( chrLevel::HasSubdivModel() );
				}
			}
		};

		class myErrorHandler : public chrErrorHandler
		{
		public:
			//============================================================================
			//	FileDoesntExist()
			//============================================================================
			virtual void	FileDoesntExist(const char* i_Filename)
			{
				MessageBox::Show(String::Format("File doesn't exist: {0}", gcnew String(i_Filename)),
					gcnew String("File Does Not Exist"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	FileReadOnly()
			//============================================================================
			virtual void	FileReadOnly(const char* i_Filename)
			{
				MessageBox::Show(String::Format("File is Read Only: {0}", gcnew String(i_Filename)),
					gcnew String("File is Read Only"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	InvalidFileFormat()
			//============================================================================
			virtual void	InvalidFileFormat(const char* i_Filename)
			{
				MessageBox::Show(String::Format("Invalid File Format: {0}", gcnew String(i_Filename)),
					gcnew String("Invalid File Format"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	CantSaveFile()
			//============================================================================
			virtual void	CantSaveFile(const char* i_Filename)
			{
				MessageBox::Show(String::Format("Cannot Save File: {0}", gcnew String(i_Filename)),
					gcnew String("Cannot Save File"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	IncompatibleAnimation()
			//============================================================================
			virtual void	IncompatibleAnimation(const char* i_Filename)
			{
				MessageBox::Show(String::Format("IncompatibleAnimation: {0}", gcnew String(i_Filename)), 
					gcnew String("IncompatibleAnimation"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	GeneralError
			//============================================================================
			virtual void	GeneralError()
			{
				MessageBox::Show(gcnew String("General Error"),
					gcnew String("General Error"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		};

		myCallback* l_pInterest = 0;
		myErrorHandler* l_pHandler = 0;
	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		l_pInterest = new myCallback();
		chrLevel::SetModelChangeCallback(l_pInterest);

		l_pHandler = new myErrorHandler();
		chrErrorHandler::SetErrorHandler(l_pHandler);
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		//MaterialDialog::FormInstance = nullptr;

		chrLevel::SetModelChangeCallback(NULL);
		delete l_pInterest;

		chrErrorHandler::SetErrorHandler(NULL);
		delete l_pHandler;
	}


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowLightsDialog(bool i_bShow)
	{
		if (i_bShow)
		{
			if (!LightsDialog::FormInstance)
			{
				LightsDialog::FormInstance = gcnew LightsDialog();
			}
			LightsDialog::FormInstance->Show();
		}
		else
		{
			if (LightsDialog::FormInstance)
				LightsDialog::FormInstance->Hide();
		}

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ShowExpressionsDialog(bool i_bShow)
	{
		if (i_bShow)
		{
			if (!ExpressionsDialog::FormInstance)
			{
				ExpressionsDialog::FormInstance = gcnew ExpressionsDialog();
			}
			ExpressionsDialog::FormInstance->Show();
		}
		else
		{
			if (ExpressionsDialog::FormInstance)
				ExpressionsDialog::FormInstance->Hide();
		}
	}

}	// end of namespace
