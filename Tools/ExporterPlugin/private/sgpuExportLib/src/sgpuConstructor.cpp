/****************************************************************************\
**  sgpuConstructor.cpp
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuSubdivConstructor.hpp"
#include "sgpuNode.hpp"
#include "sgpuSubdivImpl.hpp"
#include "sgpuModelExportScene.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuSubdivConstructorImpl.hpp"
#include "sgpuException.hpp"
#include "Core/It/itStringUtil.hpp"
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>

		



sgpuConstructor::sgpuConstructor( sgpuModelExportScene &i_Scene,  sgpuNode & i_Node  ):
m_Node( i_Node ),
m_Scene( i_Scene )
{
}

sgpuConstructor::~sgpuConstructor()
{ 
}

sgpuConstructor::sgpuConstructor( const sgpuConstructor &i_Other ):
m_Scene( i_Other.m_Scene ),
m_Node(i_Other.m_Node )
{	
	DBG_ASSERT( false, "copy constructor for sgpuConstructor not allowed" );
}
sgpuConstructor &sgpuConstructor::operator=( const sgpuConstructor & i_Other)
{
	DBG_ASSERT( false, "copy constructor for sgpuConstructor not allowed" );
	return *this;
}
