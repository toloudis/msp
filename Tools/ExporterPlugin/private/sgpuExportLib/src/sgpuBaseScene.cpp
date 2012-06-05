/****************************************************************************\
**  sgpuBaseScene.cpp
**
**      sgpuBaseScene.hpp defines the sgpuBaseScene class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuBaseScene.hpp"
#include "sgpuException.hpp"
#include "sgpuString.hpp"
#include "sgpuFileWriterLifeTimeKeeper.hpp"
#include "sgpuBaseSceneImpl.hpp"
#include "sgpuUtilsImpl.hpp"
#include "Core/It/itStringUtil.hpp"
#include <map>



//========================================================================
// constructors, destructors, assignment operator
//========================================================================
sgpuBaseScene::sgpuBaseScene()
	:m_pBaseImpl(new sgpuBaseSceneImpl )	
{
}

sgpuBaseScene::~sgpuBaseScene() 										
{ 																
	delete m_pBaseImpl; 											
} 	

//========================================================================
//	disallowed copy constructor
//========================================================================
sgpuBaseScene::sgpuBaseScene( const sgpuBaseScene &i_Mesh ) 					
: m_pBaseImpl(NULL) 
{	
	DBG_ASSERT( false, "copy constructor for sgpuBaseScene not allowed" );
} 	

//========================================================================
//	disallowed assignment operator
//========================================================================
sgpuBaseScene& sgpuBaseScene::operator=(const sgpuBaseScene& i_CopyFrom) 	
{
	DBG_ASSERT( false, "assignment operator for sgpuBaseScene not allowed" );
	return *this;
}

