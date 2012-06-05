/*****************************************************************************
**	prtclDocumentChunk.hpp
**
**	 Derived chunk for the prtcls system.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_DOCUMENTCHUNK_HPP
#error prtclDocumentChunk.hpp multiply included
#endif
#define PRTCL_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef PRTCL_DATAPARSER_HPP
#include "Systems/Particles/Data/prtclDataParser.hpp"
#endif
#ifndef PRTCL_DIALOGUTIL_HPP
#include "Systems/Particles/GUI/prtclDialogUtil.hpp"
#endif
#ifndef PRTCL_OBJECTMGR_HPP
#include "Systems/Particles/Object/prtclObjectMgr.hpp"
#endif


//============================================================================
//============================================================================
class prtclDocumentChunk : 
	public cmmDocumentChunkTemplate<prtclParticlesData, 
									prtclDataParser, 
									prtclObjectMgr, 
									prtclDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
