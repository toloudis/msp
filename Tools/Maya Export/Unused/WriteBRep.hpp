/*****************************************************************************
**  WriteBRep.hpp
**
**     Command to write .mx file from selected meshes.  All
**	meshes will be zero-transformed   
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef WRITEBREP_HPP
#error WriteBRep.hpp multiply included
#endif
#define WRITEBREP_HPP

#include <maya/MPxCommand.h>

class WriteBRep: public MPxCommand
{
public:
	WriteBRep() 
	 : selectedOnly(true),shareMaterials(true)	{};
	virtual	~WriteBRep() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
	MString	filePath;
	bool selectedOnly;
	bool shareMaterials;
};

