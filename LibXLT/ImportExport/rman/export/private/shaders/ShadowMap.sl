/*****************************************************************************
**  ShadowMap.sl
**
**      Shadows shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// ShadowMap()
//--------------------------------------------------------------------
surface 
ShadowMap(
		float UScale = 0;
		float VScale = 0;
		float UTrans = 0;
		float VTrans = 0;
		float UVAngle = 0;
		float g_transparency = 0;
		string transMap = "";
		float bRenderTransparent = 1;
		float objVisible = 1;
		float castsShadow = 1;
		)
{

	matrix uvTransform = MakeUVTransform( UScale, VScale, UTrans, VTrans, UVAngle );

	point st_point = transform( uvTransform, point(s,t,0) );
	float ss = xcomp(st_point);
	float tt = ycomp(st_point);

	ss = ss - floor(ss);
	tt = tt - floor(tt);

	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;		
	Oi = Tex2DCombine( transMap , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency * objVisible * castsShadow;
	Ci = 0;
}







