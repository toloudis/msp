/*****************************************************************************
**  Depth.sl
**
**      Depth shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// Depth()
//--------------------------------------------------------------------
surface 
Depth()
{
	vector incid = normalize(I);
	color C = color(zcomp(incid),zcomp(incid),zcomp(incid));
	Oi = Os;
	Ci = Oi * Cs * C;
}







