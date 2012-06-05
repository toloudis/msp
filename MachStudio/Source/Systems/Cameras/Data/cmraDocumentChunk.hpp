/*****************************************************************************
**	cmraDocumentChunk.hpp
**
**	 Derived chunk for cmra system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_DOCUMENTCHUNK_HPP
#error cmraDocumentChunk.hpp multiply included
#endif
#define CMRA_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef CMRA_DATAPARSER_HPP
#include "Systems/Cameras/Data/cmraDataParser.hpp"
#endif
#ifndef CMRA_DIALOGUTIL_HPP
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"
#endif
#ifndef CMRA_DATAMGR_HPP
#include "Systems/Cameras/Data/cmraDataMgr.hpp"
#endif


//============================================================================
//============================================================================
class cmraDocumentChunk : 
	public cmmDocumentChunkTemplate<cmraCamerasData, 
									cmraCamerasDataParser, 
									cmraDataMgr, 
									cmraDialogUtil>
{
public:
	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
