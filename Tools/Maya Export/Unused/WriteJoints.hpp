/*****************************************************************************
**  WriteJoints.hpp
**
**     Command to write single-skin file.  With argument "-geom"
**	it writes .jnx file and with "-anim" it writes .jna file.
**	New flag "-subanim" writes out joint animation from 
**	selected joint and below in hierarchy only.
**
**		Note: Animation data should be baked, using MEL
**	bakeResults command.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef WRITEJOINTS_HPP
#error WriteJoints.hpp multiply included
#endif
#define WRITEJOINTS_HPP

#include <maya/MPxCommand.h>

class MDagPath;

class WriteJoints: public MPxCommand
{
public:
					WriteJoints();
	virtual			~WriteJoints();
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );

private:
	MStatus			parseArgs(const MArgList& args);
	MStatus			doScan();
	MStatus			doSubAnim();
	void			printTransformData(const MDagPath& dagPath, bool quiet);
	bool	bDoGeom;
	bool	bDoAnim;
	bool	bDoSubAnim;
	MString	filePath;
};
