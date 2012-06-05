/*****************************************************************************
**	trfnDocumentChunk.hpp
**
**	 Derived chunk for trfn system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_DOCUMENTCHUNK_HPP
#error trfnDocumentChunk.hpp multiply included
#endif
#define TRFN_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif
#ifndef TRFN_TRANSFORMSDATAPARSER_HPP
#include "Systems/Transforms/Data/trfnTransformsDataParser.hpp"
#endif
#ifndef TRFN_OBJECTMGR_HPP
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"
#endif
#ifndef TRFN_DIALOGDATAUTIL_HPP
#include "Systems/Transforms/GUI/trfnDialogDataUtil.hpp"
#endif 

//============================================================================
//============================================================================
class trfnDocumentChunk : 
	public cmmDocumentChunkTemplate<trfnTransformsData, 
									trfnTransformsDataParser, 
									trfnObjectMgr, 
									trfnDialogDataUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
