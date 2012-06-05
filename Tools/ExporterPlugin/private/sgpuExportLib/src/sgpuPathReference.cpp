/****************************************************************************\
**  sgpuPathReference.cpp
**
**      sgpuPathReference.hpp defines the sgpuPathReference class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuPathReference.hpp"
#include "sgpuPathReferenceImpl.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuMeshImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"

#include "Core/It/itStringUtil.hpp"
#include <sstream>
#include <map>


sgpuPathReference::sgpuPathReference()
:m_pImpl( new sgpuPathReferenceImpl )	
{
	m_NodeContentType = ePathReference;
} 	

sgpuPathReference::sgpuPathReference( const sgpuPathReference &i_Other ) 					
: m_pImpl(new sgpuPathReferenceImpl(*i_Other.m_pImpl)) { } 				

sgpuPathReference::~sgpuPathReference() 										
{ 																
	delete m_pImpl; 											
} 	

sgpuPathReference& sgpuPathReference::operator=(const sgpuPathReference& i_CopyFrom) 	
{ 		
	sgpuNodeContent::operator=( i_CopyFrom );
	if (&i_CopyFrom != this) 									
	{ 															
		delete m_pImpl; 										
		m_pImpl = new sgpuPathReferenceImpl(*i_CopyFrom.m_pImpl); 			
	} 															
	return *this; 												
}
void sgpuPathReference::PushPathComponent( const sgpuString &i_PathComponent )
{
	m_pImpl->m_InstanceInfo->m_Path.push_back( i_PathComponent.m_pImpl->m_Data );
}

sgpuString sgpuPathReference::PopPathComponent()
{
	if( m_pImpl->m_InstanceInfo->m_Path.size() > 0)
	{
		std::string retVal = m_pImpl->m_InstanceInfo->m_Path.back();
		m_pImpl->m_InstanceInfo->m_Path.pop_back();
		return sgpuString( retVal.c_str() );
	}
	std::stringstream ss;
	ss << "path is empty ";
	throw sgpuException( sgpuString( ss.str().c_str() ) );
}

int sgpuPathReference::GetNumPathComponents()const
{
	return static_cast<int> ( m_pImpl->m_InstanceInfo->m_Path.size() );
}

sgpuString sgpuPathReference::GetPathComponent( int i_nPath )const
{
	if(i_nPath <  m_pImpl->m_InstanceInfo->m_Path.size() )
	{
		const std::string &retVal = m_pImpl->m_InstanceInfo->m_Path[ i_nPath ];
		return sgpuString( retVal.c_str() );
	}
	std::stringstream ss;
	ss << "path is empty ";
	throw sgpuException( sgpuString( ss.str().c_str() ) );
}

bool sgpuPathReference::operator== (const sgpuPathReference & other )const
{
	bool bVal = m_pImpl->m_InstanceInfo->m_Path.size() == other.m_pImpl->m_InstanceInfo->m_Path.size();
	if (!bVal) return false;
	
	for( int i=0; ( i < m_pImpl->m_InstanceInfo->m_Path.size() ) && bVal ; ++i )
	{
		const std::string &sPathComponentThis = m_pImpl->m_InstanceInfo->m_Path[ i ];
		const std::string &sPathComponentOther = other.m_pImpl->m_InstanceInfo->m_Path[ i ];
		bVal = bVal && sPathComponentThis == sPathComponentOther;
	}
	return bVal;
}