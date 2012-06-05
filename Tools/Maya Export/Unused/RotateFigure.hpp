/*****************************************************************************
**  RotateFigure.hpp
**
**     Command to remove pivot points from hierarchical model,
**	should be done before animating.     
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef RotateFigure_HPP
#error RotateFigure.hpp multiply included
#endif
#define RotateFigure_HPP

#include <maya/MPxCommand.h>

class MDagPath;

class RotateFigure: public MPxCommand
{
public:
					RotateFigure();
	virtual			~RotateFigure();
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );

private:
	MStatus			parseArgs(const MArgList& args);
	MStatus			doScan();
	void			printTransformData(const MDagPath& dagPath, bool quiet);

	double m_Angle;
};
