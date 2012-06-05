/*****************************************************************************
**	propDocumentChunk.hpp
**
**	 Derived chunk for the props system.
**
**	StudioGPU
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

class chtrCharactersData;

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

	//--------------------------------------------------------------------
	// Convert prop information in this chunk to chtr data
	// and *append* the info to the given list.
	// Returns true if there were props converted.
	//--------------------------------------------------------------------
	static bool ConvertPropsToCharacters(chtrCharactersData &io_ChtrList);
	bool ConvertPropsToCharactersImpl(chtrCharactersData &io_ChtrList);

	//--------------------------------------------------------------------
	//  Read chunk data
	//--------------------------------------------------------------------
	virtual void  Read(chReader &i_Reader,
					   chDefs::Version i_Version,
					   chDefs::Size i_Size);
};
