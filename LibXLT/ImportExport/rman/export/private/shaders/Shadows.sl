/*****************************************************************************
**  Shadows.sl
**
**      Shadows shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// Shadows()
//--------------------------------------------------------------------
surface 
Shadows(
		string normalMap = "";
		float normalMapScale = 0;
		float g_DisplacementEnabled = 0;
		float g_DisplacementNormalScale = 0;
		float UScale = 0;
		float VScale = 0;
		float UTrans = 0;
		float VTrans = 0;
		float UVAngle = 0;
		float g_transparency = 1;
		string transparencyMap = "";
		float bRenderTransparent = 1;
		output float bReceivesShadow = 1;
		)
{

	matrix uvTransform = MakeUVTransform( UScale, VScale, UTrans, VTrans, UVAngle );

	point st_point = transform( uvTransform, point(s,t,0) );
	float ss = xcomp(st_point);
	float tt = ycomp(st_point);

	ss = ss - floor(ss);
	tt = tt - floor(tt);

	normal Nf = GetBumpNormal(normalMap, normalMapScale, ss, tt);

	color C = 0;

	illuminance( P ) {
		float bEnableLight = 1;
		lightsource("bEnableLight",bEnableLight);
		float cosine = saturate(normalize(L).Nf);
		C += Cl * cosine * bEnableLight;
	}
	
	float doTransparency = ( g_transparency<1 && bRenderTransparent==0 ) ? 0 : 1;
		
	Oi = Tex2DCombineOpacity( transparencyMap , color(g_transparency,g_transparency,g_transparency), ss, tt ) * doTransparency;
	Ci = Oi * Cs * (1-C);
}







