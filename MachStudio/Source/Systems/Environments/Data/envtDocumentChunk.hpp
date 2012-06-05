/*****************************************************************************
**	envtDocumentChunk.hpp
**
**	 Derived chunk for envt system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef ENVT_DOCUMENTCHUNK_HPP
#error envtDocumentChunk.hpp multiply included
#endif
#define ENVT_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef ENVT_ENVIRONMENTSDATAPARSER_HPP
#include "Systems/Environments/Data/envtEnvironmentsDataParser.hpp"
#endif
#ifndef ENVT_DIALOGUTIL_HPP
#include "Systems/Environments/GUI/envtDialogUtil.hpp"
#endif
#ifndef ENVT_OBJECTMGR_HPP
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#endif


class envtDocumentChunk : 
	public cmmDocumentChunkTemplate<envtEnvironmentsData, 
									envtEnvironmentsDataParser, 
									envtObjectMgr, 
									envtDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;
};
