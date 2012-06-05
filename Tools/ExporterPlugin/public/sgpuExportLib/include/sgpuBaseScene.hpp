/****************************************************************************\
**  sgpuBaseScene.hpp
**
**      sgpuBaseScene.hpp defines class for a scene that can be exported
**	to a StudioGPU static geometry file (.gxb)
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_BASESCENE_HPP
#define SGPU_BASESCENE_HPP
#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"


//============================================================================
//============================================================================
struct sgpuBaseSceneImpl;

//============================================================================
//============================================================================
class SGPUEXPORTLIB_API sgpuBaseScene
{
public:
	//========================================================================
	//	default constructor
	//========================================================================
	sgpuBaseScene();

	//========================================================================
	//	destructor
	//========================================================================
	virtual ~sgpuBaseScene();
protected:
	
	//========================================================================
	//	disallowed
	//========================================================================
	sgpuBaseScene( const sgpuBaseScene &i_Other );
	sgpuBaseScene &operator=( const sgpuBaseScene &i_Other );
protected:

	sgpuBaseSceneImpl *m_pBaseImpl;
private:
};

#endif // #ifndef SGPU_BASESCENE_HPP