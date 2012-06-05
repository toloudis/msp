/*****************************************************************************
**  RemovePivots.hpp
**
**     Command to remove pivot points from hierarchical model,
**	should be done before animating.     
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef REMOVEPIVOTS_HPP
#error RemovePivots.hpp multiply included
#endif
#define REMOVEPIVOTS_HPP

#include <maya/MPxCommand.h>

class MDagPath;

class RemovePivots: public MPxCommand
{
public:
					RemovePivots();
	virtual			~RemovePivots();
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );

private:
	MStatus			parseArgs(const MArgList& args);
	MStatus			doScan();
	void			printTransformData(const MDagPath& dagPath, bool quiet);
};
