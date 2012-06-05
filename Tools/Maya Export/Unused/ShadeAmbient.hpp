/*****************************************************************************
**  ShadeAmbient.hpp
**
**     Command to compute ambient occlusion term at each vertex,
**	putting value in color per vertex.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SHADEAMBIENT_HPP
#error ShadeAmbient.hpp multiply included
#endif
#define SHADEAMBIENT_HPP

#include <maya/MPxCommand.h>

class ShadeAmbient: public MPxCommand
{
public:
	ShadeAmbient() 
	{
	};
	virtual	~ShadeAmbient() {}
	static void*	creator();
	virtual MStatus	doIt( const MArgList& );
private:
	virtual MStatus	ParseArgs( const MArgList& );
};
