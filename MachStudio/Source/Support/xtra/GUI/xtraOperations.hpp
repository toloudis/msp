/*****************************************************************************
**	xtraOperations.hpp
**
**	Undoable operations related to custom properties
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef XTRA_OPERATIONS_HPP
#error xtraOperations.hpp multiply included
#endif
#define XTRA_OPERATIONS_HPP


//============================================================================
//============================================================================
class xtraPropertyData;
class xtraScriptObject;

//============================================================================
//============================================================================
namespace xtraOperations
{
	//--------------------------------------------------------------------
	//	Prompt user for new custom property of given types
	//--------------------------------------------------------------------
	void  CreateCustomBoolean(xtraScriptObject* i_pObject);
	void  CreateCustomFloat(xtraScriptObject* i_pObject);
	void  CreateCustomColor(xtraScriptObject* i_pObject);
	void  CreateCustomString(xtraScriptObject* i_pObject);
	void  CreateCustomPosition(xtraScriptObject* i_pObject);
	void  CreateCustomOrientation(xtraScriptObject* i_pObject);
	void  CreateCustomTexture(xtraScriptObject* i_pObject);

	//--------------------------------------------------------------------
	//	Create new custom property based on given data.
	//  Returns true if the property was created, may return false if 
	//	name conflicts.
	//--------------------------------------------------------------------
	bool  CreateCustomProperty(xtraScriptObject* i_pObject,
							   const xtraPropertyData& i_Data);

}	// end of namespace
