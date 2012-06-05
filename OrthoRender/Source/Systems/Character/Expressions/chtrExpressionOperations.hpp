/*****************************************************************************
**	chtrExpressionOperations.hpp
**
**	Interface for dialogs to change Expression info
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_EXPRESSION_OPERATIONS_HPP
#error chtrExpressionOperations.hpp multiply included
#endif
#define CHTR_EXPRESSION_OPERATIONS_HPP

#include <string>


//============================================================================
//	Forward References
//============================================================================
class chtrScriptObject;


//============================================================================
//============================================================================
namespace chtrExpressionOperations
{
	//--------------------------------------------------------------------
	//	Create a new expression
	//--------------------------------------------------------------------
	void  CreateSingleExpression(chtrScriptObject* i_pChtrObject);
	void  CreateDualExpression(chtrScriptObject* i_pChtrObject);
	void  CreateQuadExpression(chtrScriptObject* i_pChtrObject);

	//--------------------------------------------------------------------
	//	Delete Expression with given name.
	//--------------------------------------------------------------------
	void  DeleteExpression(chtrScriptObject* i_pChtrObject,
							const std::string& i_ExpressionName);

}	// end of namespace
