/*****************************************************************************
**	chtrDocumentChunk.hpp
**
**	 Derived chunk for the characters system.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_DOCUMENTCHUNK_HPP
#error chtrDocumentChunk.hpp multiply included
#endif
#define CHTR_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef CHTR_DATAPARSER_HPP
#include "Systems/Character/Data/chtrDataParser.hpp"
#endif
#ifndef CHTR_DIALOGUTIL_HPP
#include "Systems/Character/GUI/chtrDialogUtil.hpp"
#endif
#ifndef CHTR_OBJECTMGR_HPP
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#endif



//============================================================================
//============================================================================
class chtrDocumentChunk : 
	public cmmDocumentChunkTemplate<chtrCharactersData, 
									chtrDataParser, 
									chtrObjectMgr, 
									chtrDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  Clear document data back to initial state
	//--------------------------------------------------------------------
	virtual void  Clear();

	//--------------------------------------------------------------------
	//  Read chunk data
	//--------------------------------------------------------------------
	virtual void  Read(chReader &i_Reader,
					   chDefs::Version i_Version,
					   chDefs::Size i_Size);

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;
};
