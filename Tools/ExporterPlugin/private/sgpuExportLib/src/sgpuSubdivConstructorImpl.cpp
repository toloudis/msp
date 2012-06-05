/****************************************************************************\
**  sgpuSubdivConstructorImpl.cpp
**
**    
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuSubdivConstructorImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuPropertyValue.hpp"





bool sgpuSubdivConstructorImpl::IsValidVertexIndex( int i_VIdx )
{
	return i_VIdx >= 0 && i_VIdx < m_PositionTemp.size();
}

bool sgpuSubdivConstructorImpl::IsValidUVIndex( int i_UVIdx )
{
	return i_UVIdx < static_cast< int > (  m_UVTemp.size() );
}

bool sgpuSubdivConstructorImpl::IsValidFaceVertex( const sgpuConstructor::FaceVertex & i_FV ) const
{
	return i_FV.m_VertexId > -1 ;
}



void  sgpuSubdivConstructorImpl::SetProperty(const std::string &i_PropertyName, const sgpuPropertyValue & i_PropertyValue)
{
	std::map< std::string, sgpuPropertyValue >::value_type p( i_PropertyName, i_PropertyValue );
	m_Property[ i_PropertyName ] = i_PropertyValue;

}

bool sgpuSubdivConstructorImpl::GetProperty(const std::string &i_PropertyName, sgpuPropertyValue &o_Property )
{
	bool bRetVal = false;
	std::map< std::string, sgpuPropertyValue >::iterator pit;
	pit = m_Property.find( i_PropertyName );
	if( pit != m_Property.end() )
	{
		o_Property = pit->second;
		bRetVal = true;
	}
	return bRetVal;
}
