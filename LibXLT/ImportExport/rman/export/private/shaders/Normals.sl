/*****************************************************************************
**  Normals.sl
**
**      Normals shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// Normals()
//--------------------------------------------------------------------
surface 
Normals(
		string normalMap = "";
		float normalMapScale = 0;
		float g_DisplacementEnabled = 0;
		float g_DisplacementNormalScale = 0;
		float UScale = 0;
		float VScale = 0;
		float UTrans = 0;
		float VTrans = 0;
		float UVAngle = 0;
		float camMat_Sub0 = 1;
		float camMat_Sub1 = 0;
		float camMat_Sub2 = 0;
		float camMat_Sub3 = 0;
		float camMat_Sub4 = 0;
		float camMat_Sub5 = 1;
		float camMat_Sub6 = 0;
		float camMat_Sub7 = 0;
		float camMat_Sub8 = 0;
		float camMat_Sub9 = 0;
		float camMat_Sub10 = 1;
		float camMat_Sub11 = 0;
		float camMat_Sub12 = 0;
		float camMat_Sub13 = 0;
		float camMat_Sub14 = 0;
		float camMat_Sub15 = 1;
		)
{
	matrix uvTransform = MakeUVTransform( UScale, VScale, UTrans, VTrans, UVAngle );

	point st_point = transform( uvTransform, point(s,t,0) );
	float ss = xcomp(st_point);
	float tt = ycomp(st_point);

	ss = ss - floor(ss);
	tt = tt - floor(tt);

	normal Nf = GetBumpNormal(normalMap, normalMapScale, ss, tt);

	matrix camMat = matrix( camMat_Sub0, camMat_Sub1, camMat_Sub2, camMat_Sub3,
							camMat_Sub4, camMat_Sub5, camMat_Sub6, camMat_Sub7,
							camMat_Sub8, camMat_Sub9, camMat_Sub10, camMat_Sub11,
							camMat_Sub12, camMat_Sub13, camMat_Sub14, camMat_Sub15);

	matrix identity = matrix(1,0,0,0,
							 0,1,0,0,
							 0,0,1,0,
							 0,0,0,1);	
	matrix worldMat = transform("world",identity);

	normal Nw = ntransform(worldMat,-Nf);
	normal Nc = ntransform(camMat,Nw);
	Nc = normalize(Nc);
	color C = color( xcomp(Nc),ycomp(Nc),zcomp(Nc) );
	Oi = Os;
	Ci = Oi * Cs * C;
}







