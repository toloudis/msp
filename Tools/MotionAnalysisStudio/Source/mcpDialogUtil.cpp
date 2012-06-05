/*****************************************************************************
**	mcpDialogUtil.cpp
**
**	API for opening dialogs for channel editor
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "StdAfx.h"
#include "mcpDialogUtil.hpp"
#include "mcpErrorHandler.hpp"

#include "MainForm.h"
#include "LightsDialog.h"

#include "mcpSkeleton.hpp"

#include "dbgLog.hpp"

using namespace MotionAnalysisStudio;

namespace mcpDialogUtil
{

	namespace
	{

		class myCallback : public mcpModelChangeCallback
		{
		public:
			virtual void ModelChange()
			{
				
				//if (mcpLevel::HasModel())
				//{
					//int nTargets = mcpLevel::GetNumExpressions();
					////if (nTargets > 0)
					//{
					//	if (ExpressionsDialog::FormInstance == 0)
					//	{
					//		ExpressionsDialog::FormInstance = new ExpressionsDialog();
					//	}
					//	for (int i=0; i<nTargets; i++)
					//		ExpressionsDialog::FormInstance->AddExpression(mcpLevel::GetExpressionName(i).c_str());
					//	ExpressionsDialog::FormInstance->Show();
					//}
				//}
			}
		};

		class myErrorHandler : public mcpErrorHandler
		{
		public:
			//============================================================================
			//	FileDoesntExist()
			//============================================================================
			virtual void	FileDoesntExist(const char* i_Filename)
			{
				MessageBox::Show(String::Format("File doesn't exist: {0}", new String(i_Filename)),
					new String("File Does Not Exist"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	FileReadOnly()
			//============================================================================
			virtual void	FileReadOnly(const char* i_Filename)
			{
				MessageBox::Show(String::Format("File is Read Only: {0}", new String(i_Filename)),
					new String("File is Read Only"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	InvalidFileFormat()
			//============================================================================
			virtual void	InvalidFileFormat(const char* i_Filename)
			{
				MessageBox::Show(String::Format("Invalid File Format: {0}", new String(i_Filename)),
					new String("Invalid File Format"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	CantSaveFile()
			//============================================================================
			virtual void	CantSaveFile(const char* i_Filename)
			{
				MessageBox::Show(String::Format("Cannot Save File: {0}", new String(i_Filename)),
					new String("Cannot Save File"),
					MessageBoxButtons::OK, MessageBoxIcon::Error);
			}

			//============================================================================
			//	GeneralError
			//============================================================================
			virtual void	GeneralError()
			{
				MessageBox::Show(new String("General Error"),
					new String("General Error"),
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
		mcpSkeleton::SetModelChangeCallback(l_pInterest);

		l_pHandler = new myErrorHandler();
		mcpErrorHandler::SetErrorHandler(l_pHandler);
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		//MaterialDialog::FormInstance = 0;

		mcpSkeleton::SetModelChangeCallback(NULL);
		delete l_pInterest;

		mcpErrorHandler::SetErrorHandler(NULL);
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
