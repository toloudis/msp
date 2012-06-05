/****************************************************************************\
**  sgpuNodeContent.cpp
**
**      sgpuNodeContent.hpp defines the sgpuNodeContent class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuNodeContent.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuException.hpp"
#include "Core/It/itStringUtil.hpp"
#include <sstream>
#include <map>



sgpuNodeContent::sgpuNodeContent():m_NodeContentType( eNone )
{
} 	

sgpuNodeContent::sgpuNodeContent( const sgpuNodeContent &i_Other ) 					
: m_NodeContentType( i_Other.m_NodeContentType )
{ } 				

sgpuNodeContent::~sgpuNodeContent() 										
{ 	 											
} 	

sgpuNodeContent& sgpuNodeContent::operator=(const sgpuNodeContent& i_CopyFrom) 	
{ 		
	m_NodeContentType = i_CopyFrom.m_NodeContentType;
	return *this; 												
}
