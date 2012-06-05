/****************************************************************************\
**  ShadeUtil.hpp
**
**      ShadeUtil contains functions for pre-lighting vertices
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SHADEUTIL_HPP
#error ShadeUtil.hpp multiply included
#endif
#define SHADEUTIL_HPP


class MFnMesh;

//============================================================================
//============================================================================
namespace ShadeUtil
{
	//========================================================================
	//  Color vertices based on ambient occlusion term
	//========================================================================
	void ComputeAmbientOcclusion(MFnMesh &mesh);

}

