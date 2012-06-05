/*****************************************************************************
**	propDocumentChunk.hpp
**
**	 Derived chunk for the props system.
**
**	Extra Large Technology
**	Copyright(C) 2004-6 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_DOCUMENTCHUNK_HPP
#error propDocumentChunk.hpp multiply included
#endif
#define PROP_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef PROP_DATAPARSER_HPP
#include "Systems/Props/Data/propDataParser.hpp"
#endif
#ifndef PROP_DIALOGUTIL_HPP
#include "Systems/Props/GUI/propDialogUtil.hpp"
#endif
#ifndef PROP_OBJECTMGR_HPP
#include "Systems/Props/Object/propObjectMgr.hpp"
#endif

//============================================================================
//============================================================================
class propDocumentChunk : 
	public cmmDocumentChunkTemplate<propPropsData, 
									propDataParser, 
									propObjectMgr, 
									propDialogUtil>
{
public:
	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
