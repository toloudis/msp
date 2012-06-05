/*****************************************************************************
**  WriteVerts.hpp
**
**     Command to write vertex animation of meshes.  With argument "-geom"
**	it writes .vtx file and with "-anim" it writes .vta file.   
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef WRITEVERTS_HPP
#error WriteVerts.hpp multiply included
#endif
#define WRITEVERTS_HPP

#include <maya/MPxCommand.h>

class MDagPath;

class WriteVerts: public MPxCommand
{
public:
					WriteVerts();
	virtual			~WriteVerts();
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );

private:
	MStatus			parseArgs(const MArgList& args);
	MStatus			doScan();
	void			printTransformData(const MDagPath& dagPath, bool quiet);
	bool	bDoGeom;
	bool	bDoAnim;
	bool    bShareMaterials;
	MString	filePath;
};
