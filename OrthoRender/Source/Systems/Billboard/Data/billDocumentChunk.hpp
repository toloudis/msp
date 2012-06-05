/*****************************************************************************
**	billDocumentChunk.hpp
**
**	 Derived chunk for the bills system.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_DOCUMENTCHUNK_HPP
#error billDocumentChunk.hpp multiply included
#endif
#define BILL_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef BILL_DATAPARSER_HPP
#include "Systems/Billboard/Data/billDataParser.hpp"
#endif
#ifndef BILL_DIALOGUTIL_HPP
#include "Systems/Billboard/GUI/billDialogUtil.hpp"
#endif
#ifndef BILL_OBJECTMGR_HPP
#include "Systems/Billboard/Object/billObjectMgr.hpp"
#endif



//============================================================================
//============================================================================
class billDocumentChunk : 
	public cmmDocumentChunkTemplate<billListData, 
									billDataParser, 
									billObjectMgr, 
									billDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
