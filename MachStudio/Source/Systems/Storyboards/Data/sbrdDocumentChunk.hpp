/*****************************************************************************
**	sbrdDocumentChunk.hpp
**
**	 Derived chunk for the sbrds system.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_DOCUMENTCHUNK_HPP
#error sbrdDocumentChunk.hpp multiply included
#endif
#define SBRD_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef SBRD_DATAPARSER_HPP
#include "Systems/Storyboards/Data/sbrdDataParser.hpp"
#endif
#ifndef SBRD_DIALOGUTIL_HPP
#include "Systems/Storyboards/GUI/sbrdDialogUtil.hpp"
#endif
#ifndef SBRD_OBJECTMGR_HPP
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#endif



//============================================================================
//============================================================================
class sbrdDocumentChunk : 
	public cmmDocumentChunkTemplate<sbrdListData, 
									sbrdDataParser, 
									sbrdObjectMgr, 
									sbrdDialogUtil>
{
public:
	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

	//--------------------------------------------------------------------
	//  Read chunk data
	//--------------------------------------------------------------------
	virtual void  Read(	chReader &i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size);
};
