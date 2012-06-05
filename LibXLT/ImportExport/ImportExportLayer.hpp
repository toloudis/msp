/*****************************************************************************
**  ImportExportLayer.hpp
**
**      ImportExportLayer contains the initialization functions
**	for the all packages within the Importer Layer.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef IMPORTEXPORT_LAYER_HPP
#error ImportExportLayer.hpp multiply included
#endif
#define IMPORTEXPORT_LAYER_HPP

class ImportExportLayer
{
	public:

		//------------------------------------------------------------------------
		//	Init
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp
		//------------------------------------------------------------------------
		static void CleanUp() throw();

		//------------------------------------------------------------------------
		//	InitGraphics - should be called after device is created
		//------------------------------------------------------------------------
		static void InitGraphics();

		//------------------------------------------------------------------------
		//	CleanUpGraphics - should be called before device is destroyed
		//------------------------------------------------------------------------
		static void CleanUpGraphics();
};
