/*****************************************************************************
**  BakeAnim.hpp
**
**     Command to bake results of ik simulation into each joint at
**	each frame.  This is needed before outputting single skin animation. 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef BAKEANIM_HPP
#error BakeAnim.hpp multiply included
#endif
#define BAKEANIM_HPP

#include <maya/MPxCommand.h>

class BakeAnim: public MPxCommand
{
public:
	BakeAnim() 
	{
		sample = 1;
	};
	virtual	~BakeAnim() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
	int		sample;
};
