/*****************************************************************************
**	lyrsDocumentChunk.hpp
**
**	 Derived chunk for lyrs system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef LYRS_DOCUMENTCHUNK_HPP
#error lyrsDocumentChunk.hpp multiply included
#endif
#define LYRS_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif
#ifndef LYRS_LAYERSDATAPARSER_HPP
#include "Systems/Layers/Data/lyrsLayersDataParser.hpp"
#endif
#ifndef LYRS_DIALOGUTIL_HPP
#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"
#endif
#ifndef LYRS_OBJECTMGR_HPP
#include "Systems/Layers/Object/lyrsObjectMgr.hpp"
#endif


//============================================================================
//============================================================================
class lyrsDocumentChunk : 
	public cmmDocumentChunkTemplateSimple<lyrsLayersData, 
									lyrsLayersDataParser, 
									lyrsObjectMgr, 
									lyrsDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
