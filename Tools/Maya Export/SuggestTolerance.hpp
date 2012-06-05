/*****************************************************************************
**  SuggestTolerance.hpp
**
**     Command to write pure vertex animation in world space to .gxb 
**	and .gab files.
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef SUGGESTTOLERANCE_HPP
#error SuggestTolerance.hpp multiply included
#endif
#define SUGGESTTOLERANCE_HPP

#include <maya/MPxCommand.h>

//============================================================================
//============================================================================
class SuggestTolerance : public MPxCommand
{
public:
	SuggestTolerance();
	virtual	~SuggestTolerance() {}

	static void*	creator();

	virtual MStatus	doIt( const MArgList& );
	MStatus safeDoIt(const MArgList &args);

};

