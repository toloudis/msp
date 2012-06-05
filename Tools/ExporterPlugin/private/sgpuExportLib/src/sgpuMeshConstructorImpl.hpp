/****************************************************************************\
**  sgpuMeshConstructorImpl.hpp
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_TRIMESHCONSTRUCTORIMPL_HPP
#define SGPU_TRIMESHCONSTRUCTORIMPL_HPP

#ifndef SGPU_SUBDIVCONSTRUCTOR_HPP
#include "sgpuMeshConstructor.hpp"
#endif 
#include "sgpuUtilsImpl.hpp"
#ifndef MDL_SUBDIVINFO_HPP
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#endif 

#include <vector>
#include <map>


//============================================================================
//============================================================================
struct sgpuMeshConstructorImpl
{
	std::map< int, int> m_FaceRemap;
	std::vector< sgpuVector3 >  m_PositionTemp;
	std::vector< sgpuVector3 > m_NormalTemp;
	std::vector< sgpuVector3 > m_UVTemp;
	std::vector< PolyFace > m_FaceTemp;
	std::map< std::string, sgpuPropertyValue > m_Property;	
	bool m_bVertexAnimation;
	bool GetProperty(const std::string &i_PropertyName, sgpuPropertyValue &o_Property );
	void SetProperty(const std::string &i_PropertyName, const sgpuPropertyValue & i_PropertyValue);
	
	bool IsValidVertexIndex( int i_VIdx )const;
	bool IsValidNormalIndex( int i_NIdx )const;
	bool IsValidUVIndex( int i_UVIdx )const;
	bool IsValidFaceVertex( const sgpuConstructor::FaceVertex & i_FV ) const;
};

#endif // #ifndef SGPU_TRIMESHCONSTRUCTORIMPL_HPP
