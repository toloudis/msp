/*****************************************************************************
**	dcutDocumentChunk.hpp
**
**	 Derived chunk for dcut system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef DCUT_DOCUMENTCHUNK_HPP
#error dcutDocumentChunk.hpp multiply included
#endif
#define DCUT_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef DCUT_DATAPARSER_HPP
#include "Systems/DirectorsCut/Data/dcutDataParser.hpp"
#endif
#ifndef DCUT_DIALOGUTIL_HPP
#include "Systems/DirectorsCut/GUI/dcutDialogUtil.hpp"
#endif
#ifndef DCUT_DATAMGR_HPP
#include "Systems/DirectorsCut/Data/dcutDataMgr.hpp"
#endif


//============================================================================
//============================================================================
class dcutDocumentChunk : 
	public cmmDocumentChunkTemplate<dcutCuesData, 
									dcutCamerasDataParser, 
									dcutDataMgr, 
									dcutDialogUtil>
{
public:
	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
