/****************************************************************************\
**  sgpuSubdivConstructorImpl.hpp
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_SUBDIVCONSTRUCTORIMPL_HPP
#define SGPU_SUBDIVCONSTRUCTORIMPL_HPP

#ifndef SGPU_SUBDIVCONSTRUCTOR_HPP
#include "SgpuSubdivConstructor.hpp"
#endif 

#ifndef MDL_SUBDIVINFO_HPP
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#endif 
#include "sgpuUtilsImpl.hpp"

#include <vector>
#include <map>


//============================================================================
//============================================================================
struct sgpuSubdivConstructorImpl
{
	std::map< int, int> m_FaceRemap;
	std::vector< sgpuVector3 >  m_PositionTemp;
	std::vector< sgpuVector3 > m_UVTemp;
	std::vector< PolyFace > m_FaceTemp;	
	std::map< std::string, sgpuPropertyValue > m_Property;	
	bool	m_bVertexAnimation;
	bool GetProperty(const std::string &i_PropertyName, sgpuPropertyValue &o_Property );
	void SetProperty(const std::string &i_PropertyName, const sgpuPropertyValue & i_PropertyValue);
	
	bool IsValidFaceVertex( const sgpuConstructor::FaceVertex & i_FV ) const;
	bool IsValidVertexIndex( int i_VIdx );
	bool IsValidUVIndex( int i_UVIdx );
};

#endif // #ifndef SGPU_SUBDIVCONSTRUCTORIMPL_HPP
