/****************************************************************************\
**  mdlFragInfo.hpp
**
**      Contains structures for passing around fragment data.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MDL_FRAGINFO_HPP
#error mdlFragInfo.hpp multiply included
#endif
#define MDL_FRAGINFO_HPP

//#ifndef ENT_FRAGINFOSINK_HPP
//#include "Graphics/ent/entFragInfoSink.hpp"
//#endif

//#ifndef MDL_MATINFO_HPP
//#include "Graphics/mdl/mdlMatInfo.hpp"
//#endif
//#ifndef MDL_MATERIALINFO_HPP
//#include "Graphics/mdl/mdlMaterialInfo.hpp"
//#endif 

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif


#include <vector>
#include <map>


class  mdlFragInfo //: public entFragInfo
{
public:
	mdlFragInfo();

	//------------------------------------------------------------------------
	//	AddGeometry causes the geometry information from i_ToAdd to be added
	//	to this.  All indices will be modified properly, etc.
	//------------------------------------------------------------------------
	//void AddGeometry(const mdlFragInfo& i_ToAdd);

	//------------------------------------------------------------------------
	//	GetIndexRange - Get start index and num indices for given
	//	material index
	//------------------------------------------------------------------------
	void GetIndexRange( int i_MatNum,
						int& o_FirstIndex,
						int& o_NumIndices) const;

	std::string m_Name;
	std::vector<maPoint3d> m_Vertices;
	std::vector<maVector3d> m_Normals;
	std::vector<maPoint2d> m_UVs;
	std::vector<maFloatRGBA> m_Colors;
	int m_NumOrigVertices;
	int m_NumOrigNormals;
	std::multimap<int, int> m_VertexRemap;
	std::multimap<int, int> m_NormalRemap;
	std::vector<envType::UInt32> m_Indices;

	//std::vector< shared_ptr<mdlMatInfo> > m_Materials;
	std::vector< std::string > m_MaterialNames;
	
	std::vector<int> m_MaterialChanges;
	struct
	{
		bool m_bCastsShadow : 1;	// casts shadows
		bool m_bReceivesShadow : 1;	// receives shadows
		bool m_bBumpMap : 1;		// needs texture space info for bump mapping
		bool m_bShadowHull : 1;		// fragment is invisible, but casts shadow
		bool m_bDoubleSided : 1;	// render both sides of mesh
		bool m_bTriangleSort : 1;	// triangles needs to be sorted for transparency
		bool m_bVertexAnimation : 1;// this mesh will undergo vertex animation ("cloth")
	} m_Flags;
	int m_ResolutionLevel;		// 0 - all res, 1 - low res, 2 - high res
	std::vector<envType::UInt32> m_WeldIndices;
};





