/*****************************************************************************
**  WriteSubdiv.hpp
**
**     Command to write lod versions of.mx file from selected subdivision 
**	surfaces.  
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef WRITESUBDIV_HPP
#error WriteSubdiv.hpp multiply included
#endif
#define WRITESUBDIV_HPP

#include <maya/MPxCommand.h>

class WriteSubdiv: public MPxCommand
{
public:
	WriteSubdiv() ;
	virtual	~WriteSubdiv() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
	MString	filePath;
	bool shareMaterials;
	int begin_depth, end_depth;
};

