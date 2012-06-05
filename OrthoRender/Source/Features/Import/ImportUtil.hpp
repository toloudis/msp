/*****************************************************************************
**	ImportUtil.hpp
**
**		API for Import utilities
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef IMPORTUTILS_HPP
#error ImportUtil.hpp multiply included
#endif
#define IMPORTUTILS_HPP


//============================================================================
//	forward references
//============================================================================
class docDocument;
struct ImportData;


//============================================================================
//============================================================================
namespace ImportUtil
{
	//------------------------------------------------------------------------
	//  AddToMenu() - add Import actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu();

	//------------------------------------------------------------------------
	//	Build a data list from the passed in doc.
	//------------------------------------------------------------------------
	void BuildDataList( docDocument* i_pDoc, ImportData& o_Data );

	//------------------------------------------------------------------------
	//	Import one Document into another
	//		The i_SourceData list are items to REMOVE from the source document
	//	(i.e. not import into the destination).  An empty list will merge
	//	everything.
	//------------------------------------------------------------------------
	void ImportDoc( docDocument* io_pDestDoc, docDocument* io_pSourceDoc, const ImportData& i_SourceData );
}
