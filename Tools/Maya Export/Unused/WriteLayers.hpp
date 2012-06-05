/*****************************************************************************
**  WriteLayers.hpp
**
**     Command to write .mx file from selected meshes.  All
**	meshes will be zero-transformed   
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef WRITELAYERS_HPP
#error WriteLayers.hpp multiply included
#endif
#define WRITELAYERS_HPP

#include <maya/MPxCommand.h>

class WriteLayers: public MPxCommand
{
public:
	WriteLayers() 
	 : selectedOnly(true),shareMaterials(true)	{};
	virtual	~WriteLayers() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
	MString	filePath;
	bool selectedOnly;
	bool shareMaterials;
};

