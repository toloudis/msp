/*****************************************************************************
**  WriteXForms.hpp
**
**     Command to write hierarchical file.  With argument "-geom"
**	it writes .mhx file and with "-anim" it writes .mha file.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef WRITEXFORMS_HPP
#error WriteXForms.hpp multiply included
#endif
#define WRITEXFORMS_HPP

#include <maya/MPxCommand.h>

class MDagPath;

class WriteXforms: public MPxCommand
{
public:
					WriteXforms();
	virtual			~WriteXforms();
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
