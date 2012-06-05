/****************************************************************************\
**	mainInitDocument.hpp
**
**		Object that initializes the document for our application on
**	constructor and then closes it down on destructor.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_INITDOCUMENT_HPP
#error mainInitDocument.hpp multiply included
#endif
#define MAIN_INITDOCUMENT_HPP

//============================================================================
//============================================================================
class mainInitDocument
{
public:
	//--------------------------------------------------------------------
	// Initializes library packages
	//--------------------------------------------------------------------
	mainInitDocument();

	//--------------------------------------------------------------------
	// DeInitializes library packages
	//--------------------------------------------------------------------
	~mainInitDocument();
};
