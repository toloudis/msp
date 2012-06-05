/****************************************************************************\
**  sgpuVertexAnimExportScene.hpp
**
**      sgpuVertexAnimExportScene.hpp defines class for a scene that can be exported
**	to a StudioGPU static geometry file (.gxb)
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_VERTEXANIMEXPORTSCENE_HPP
#define SGPU_VERTEXANIMEXPORTSCENE_HPP
#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"
#include "sgpuBaseScene.hpp"
#include "sgpuReportProgress.hpp"


//============================================================================
//============================================================================
struct sgpuVertexAnimEstimatorImpl;
class sgpuNode;
class sgpuMaterial;
class sgpuVertexAnimExportScene;
class sgpuVector3;
//============================================================================
//============================================================================


class SGPUEXPORTLIB_API sgpuVertexAnimEstimator
{
private:
	sgpuVertexAnimEstimator(
		float i_FrameRate, 
		float i_StartFrame, 
		float i_EndFrame, 
		float i_StepFrame);
public:
	~sgpuVertexAnimEstimator()
	{}
	int AddEstimate( 
		const sgpuString &i_MeshName, 
		bool i_bSubdiv,
		int i_NumVerts, 
		int i_NumNormals);
	float GetEstimate();
	float ComputeIthFrame( int i_FrameIdx ) const
	{
		return m_StartFrame + m_StepFrame * i_FrameIdx;
	}
	int GetNumFrames()const
	{
		return static_cast< int > ( ( ( m_EndFrame - m_StartFrame ) /m_StepFrame  ) + 1 );
	}
	int GetNumMeshesInEstimate() const;

	void GetDetailOfIthMesh( int i_MeshIdxInEstimate, 
		sgpuString &o_MeshName, 
		int &o_BeginOffsetInAnimFile, 
		int &o_EndOffsetInAnimFile ); 
friend sgpuVertexAnimExportScene;

public:
	const float m_FrameRate;
	const float m_StartFrame;
	const float m_EndFrame; 
	const float m_StepFrame;
private:
	sgpuVertexAnimEstimatorImpl *m_pImpl;
private:
	sgpuVertexAnimEstimator( const sgpuVertexAnimEstimator &i_Other );
	sgpuVertexAnimEstimator & operator=( const sgpuVertexAnimEstimator &i_Other );
};



class SGPUEXPORTLIB_API sgpuVertexAnimExportScene : public sgpuBaseScene
{
public:
	//The constructor stores the framerate, startFrame, endFrame and stepFrame
	//that will be exported.
	//It constructs a sgpuVertAnimEstimator privately
	sgpuVertexAnimExportScene(		 
		float i_FrameRate,
		float i_StartFrame,
		float i_EndFrame,
		float i_StepFrame
		);

	//========================================================================
	//	destructor
	//========================================================================
	~sgpuVertexAnimExportScene();

	sgpuVertexAnimEstimator & GetEstimator()
	{
		return m_Estimator;
	}

	//Before begining writing the gab file
	//the private sgpuVertexAnimEstimator is checked to see it falls 
	//below 2GByte limit.
	//start writing some introductory chunks
	//Also, since we know because of the estimator,
	//where each key of a mesh should go,
	//we can construct a (mesh-key, offset) table.
	bool OpenAndBeginWriting ( sgpuString & filename  );
	//Write the animation for a mesh, for a particular key
	//Uses the (mesh-key , offset) table to  fill up the vertex and normal values
	bool WriteAnim( 
		int i_IthGeometry,
		const sgpuString &i_MeshName, 
		int i_JthKey, 
		bool bSubdiv, 
		int i_NumVerts, 
		int i_NumNormals, 
		const sgpuVector3 *i_pPositions, 
		const sgpuVector3 *i_pNormals,
		sgpuReportProgress *i_pReportProgress 
		);
	
	//close the file
	bool CloseAndEndWriting();

	sgpuVertexAnimEstimator m_Estimator;
private:
	sgpuVertexAnimExportScene( const sgpuVertexAnimExportScene &i_Other );
	sgpuVertexAnimExportScene & operator=( const sgpuVertexAnimExportScene &i_Other );
};

#endif // #ifndef SGPU_VERTEXANIMEXPORTSCENE_HPP