/*****************************************************************************
**	grupDocumentChunk.hpp
**
**	 Derived chunk for grup system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GRUP_DOCUMENTCHUNK_HPP
#error grupDocumentChunk.hpp multiply included
#endif
#define GRUP_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef GRUP_GROUPSDATAPARSER_HPP
#include "Systems/Groups/Data/grupGroupsDataParser.hpp"
#endif
#ifndef GRUP_DIALOGUTIL_HPP
#include "Systems/Groups/GUI/grupDialogUtil.hpp"
#endif
#ifndef GRUP_OBJECTMGR_HPP
#include "Systems/Groups/Object/grupObjectMgr.hpp"
#endif


class grupDocumentChunk : 
	public cmmDocumentChunkTemplateSimple<grupGroupsData, 
									grupGroupsDataParser, 
									grupObjectMgr, 
									grupDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
