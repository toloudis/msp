/*****************************************************************************
**	chtrExpressionOperations.cpp
**
**	Interface for dialogs to change Expression info
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Expressions/chtrExpressionOperations.hpp"

#include "Systems/Character/Expressions/chtrExpressionDef.hpp"
#include "Systems/Character/GUI/chtrDialogUtil.hpp"
#include "Systems/Character/Object/chtrScriptObject.hpp"

#include "Core/Env/envExceptionX.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Tool/gui/guiMessageBox.hpp"

#include <sstream>
#include <iomanip>


//============================================================================
//============================================================================
namespace chtrExpressionOperations
{
	//--------------------------------------------------------------------
	//	Create Single Expression based on given data
	//--------------------------------------------------------------------
	void  CreateSingleExpression(chtrScriptObject* i_pObject,
								const chtrSingleExpressionData& i_Data)
	{
		// TODO needs undo operation

		fsLocator anim_file;
		if (i_Data.m_FileName.GetValue().GetNumNames() > 0)
		{
			anim_file = i_Data.m_FileName.GetValue();
		}

		try
		{
			i_pObject->AddSingleExpression(i_Data.m_Name.GetValue(), anim_file);
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem loading expression, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Expression Error", guiMessageBox::e_OKOnly);
		}	
	}

	//--------------------------------------------------------------------
	//	Create Dual Expression based on given data
	//--------------------------------------------------------------------
	void  CreateDualExpression(chtrScriptObject* i_pObject,
								const chtrDualExpressionData& i_Data)
	{
		// TODO needs undo operation

		fsLocator anim_file1;
		fsLocator anim_file2;
		if (i_Data.m_FileNameLeft.GetValue().GetNumNames() > 0)
		{
			anim_file1 = i_Data.m_FileNameLeft.GetValue();
		}
		if (i_Data.m_FileNameRight.GetValue().GetNumNames() > 0)
		{
			anim_file2 = i_Data.m_FileNameRight.GetValue();
		}
		
		try
		{
			i_pObject->AddDualExpression(i_Data.m_Name.GetValue(), anim_file1, anim_file2);
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem loading expression, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Expression Error", guiMessageBox::e_OKOnly);
		}	
	}

	//--------------------------------------------------------------------
	//	Create Quad Expression based on given data
	//--------------------------------------------------------------------
	void  CreateQuadExpression(chtrScriptObject* i_pObject,
								const chtrQuadExpressionData& i_Data)
	{
		// TODO needs undo operation

		fsLocator anim_file1;
		fsLocator anim_file2;
		fsLocator anim_file3;
		fsLocator anim_file4;
		if (i_Data.m_FileNameLeft.GetValue().GetNumNames() > 0)
		{
			anim_file1 = i_Data.m_FileNameLeft.GetValue();
		}
		if (i_Data.m_FileNameRight.GetValue().GetNumNames() > 0)
		{
			anim_file2 = i_Data.m_FileNameRight.GetValue();
		}
		if (i_Data.m_FileNameUp.GetValue().GetNumNames() > 0)
		{
			anim_file3 = i_Data.m_FileNameUp.GetValue();
		}
		if (i_Data.m_FileNameDown.GetValue().GetNumNames() > 0)
		{
			anim_file4 = i_Data.m_FileNameDown.GetValue();
		}
		
		try
		{
			i_pObject->AddQuadExpression(i_Data.m_Name.GetValue(), anim_file1, anim_file2, anim_file3, anim_file4);
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem loading expression, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Expression Error", guiMessageBox::e_OKOnly);
		}	
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_default_expression_name( chtrScriptObject* i_pChtrObject, std::string& o_Name )
	{
		int num_exp = i_pChtrObject->GetNumExpressions();
		//char name[24];
		//sprintf( name, "Expression %02d", num_exp+1 );
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss.setf(std::ios::fixed, std::ios::floatfield);
		oss << "Expression "<<std::setw(2)<<std::setfill('0')<<num_exp+1;
		std::string name(oss.str());
		o_Name = name;
	}

	//--------------------------------------------------------------------
	//	
	//--------------------------------------------------------------------
	void  CreateSingleExpression(chtrScriptObject* i_pChtrObject)
	{
		//	Set-Up the data
		std::string expression_name;
		create_default_expression_name( i_pChtrObject, expression_name );

		chtrSingleExpressionData data;
		data.m_Name.SetValue( expression_name );

		//	Create the expression
		CreateSingleExpression( i_pChtrObject, data );
	}

	//--------------------------------------------------------------------
	//	
	//--------------------------------------------------------------------
	void  CreateDualExpression(chtrScriptObject* i_pChtrObject)
	{
		//	Set-Up the data
		std::string expression_name;
		create_default_expression_name( i_pChtrObject, expression_name );

		chtrDualExpressionData data;
		data.m_Name.SetValue( expression_name );

		//	Create the expression
		CreateDualExpression( i_pChtrObject, data );
	}

	//--------------------------------------------------------------------
	//	
	//--------------------------------------------------------------------
	void  CreateQuadExpression(chtrScriptObject* i_pChtrObject)
	{
		//	Set-Up the data
		std::string expression_name;
		create_default_expression_name( i_pChtrObject, expression_name );

		chtrQuadExpressionData data;
		data.m_Name.SetValue( expression_name );

		//	Create the expression
		CreateQuadExpression( i_pChtrObject, data );
	}

	//--------------------------------------------------------------------
	//	Delete Expression with given name.
	//--------------------------------------------------------------------
	void  DeleteExpression(chtrScriptObject* i_pObject,
							const std::string& i_ExpressionName)
	{
		// TODO needs undo operation
		DBG_ASSERT(i_pObject->GetExpressionObject() != NULL, "Cannot delete expression.  No Expression Object. - " << i_ExpressionName.c_str() );

		//const bool bDeleteDrivers = true;
		i_pObject->DeleteExpression(i_ExpressionName); //, bDeleteDrivers);
	
		chtrDialogUtil::UpdateListDialog();
	}

}	// end of namespace
