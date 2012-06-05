/****************************************************************************\
**  mdlSplitFragInfo.hpp
**
**      Contains structures for single material fragments.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MDL_SPLITFRAGINFO_HPP
#error mdlSplitFragInfo.hpp multiply included
#endif
#define MDL_SPLITFRAGINFO_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <vector>

//class matMaterial;

struct mdlSplitFragInfo
{	
	mdlSplitFragInfo();

	std::string m_Name;
	std::vector<maPoint3d> m_Vertices;
	std::vector<maVector3d> m_Normals;
	std::vector<maPoint2d> m_UVs;
	std::vector<maFloatRGBA> m_Colors;	
	std::vector<envType::UInt32> m_Indices;
	int m_WeldIndex;	// number of indices before shadow welding is added
	std::vector<int> m_RemapArray;
	//matMaterial* m_Material;
	std::string m_MaterialName;

	struct 
	{
		bool m_bCastsShadow : 1;	// casts shadows
		bool m_bReceivesShadow : 1;	// receives shadows
		bool m_bBumpMap : 1;		// needs texture space info for bump mapping
		bool m_bShadowHull : 1;		// fragment is invisible, but casts shadow
		bool m_bComponentSort : 1;	// components needs to be sorted for transparency
		bool m_bDoubleSided : 1;	// render both sides of triangles
	} m_Flags;
};


				


