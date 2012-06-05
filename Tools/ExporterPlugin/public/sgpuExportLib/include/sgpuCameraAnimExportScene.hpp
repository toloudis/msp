/****************************************************************************\
**  sgpuCameraAnimExportScene.hpp
**
**      sgpuCameraAnimExportScene.hpp defines class for a scene that can be exported
**	to a StudioGPU static geometry file (.gxb)
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_CAMERAANIMEXPORTSCENE_HPP
#define SGPU_CAMERAANIMEXPORTSCENE_HPP
#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"
#include "sgpuBaseScene.hpp"
#include "sgpuVector.hpp"
#include "sgpuReportProgress.hpp"


//============================================================================
//============================================================================
struct sgpuCameraAnimExportImpl;
class sgpuNode;
class sgpuMaterial;
class sgpuCameraAnimExportScene;
class sgpuVector3;


#define SGPU_CAMERA_FOCAL_LENGTH_DEFAULT  1.0f
#define SGPU_CAMERA_CENTER_OF_INTEREST_DEFAULT 1.0f
#define SGPU_CAMERA_HORIZ_FILM_APERTURE_DEFAULT 1.0f
#define SGPU_CAMERA_VERT_FILM_APERTURE_DEFAULT 1.0f

//============================================================================
//============================================================================
//structure to hold camera animation


struct SGPUEXPORTLIB_API sgpuCameraFrame
{
	sgpuCameraFrame():
		m_FocalLength( SGPU_CAMERA_FOCAL_LENGTH_DEFAULT ),
		m_CenterOfInterest( SGPU_CAMERA_CENTER_OF_INTEREST_DEFAULT ),
		m_HorizFilmAperture( SGPU_CAMERA_HORIZ_FILM_APERTURE_DEFAULT ),
		m_VertFilmAperture( SGPU_CAMERA_VERT_FILM_APERTURE_DEFAULT )
{}
sgpuVector3  m_Translation;
sgpuVector3 m_Rotation;
float m_FocalLength;
float m_CenterOfInterest;
float m_HorizFilmAperture;
float  m_VertFilmAperture;
};

class SGPUEXPORTLIB_API sgpuCameraAnimExportScene : public sgpuBaseScene
{
public:
	//The constructor stores the framerate, startFrame, endFrame and stepFrame
	//that will be exported.
	//It constructs a sgpuVertAnimEstimator privately
	sgpuCameraAnimExportScene(		 
		float i_FrameRate,
		float i_StartFrame,
		float i_EndFrame,
		float i_StepFrame
		);

	//========================================================================
	//	destructor
	//========================================================================
	~sgpuCameraAnimExportScene();
	void SetName( const sgpuString &i_CameraName );
	void SetFrame( int i_FrameIdx, const sgpuCameraFrame &i_CameraFrame );


	//Write the animation for a mesh, for a particular key
	//Uses the (mesh-key , offset) table to  fill up the vertex and normal values
	
	bool WriteAnim(
		const sgpuString &i_FileName,
		sgpuReportProgress *i_pReportProgress 
	);

	int GetNumFrames()const
	{
		return static_cast< int > ( ( ( m_EndFrame - m_StartFrame ) /m_StepFrame  ) + 1 );
	}
	
	float ComputeIthFrame( int i_FrameIdx ) const
	{
		return m_StartFrame + m_StepFrame * i_FrameIdx;
	}

	float GetEstimate() const;

	const float m_FrameRate;
	const float m_StartFrame;
	const float m_EndFrame; 
	const float m_StepFrame;
	sgpuCameraAnimExportImpl *m_pImpl;
private:
	sgpuCameraAnimExportScene( const sgpuCameraAnimExportScene &i_Other );
	sgpuCameraAnimExportScene & operator=( const sgpuCameraAnimExportScene &i_Other );

};


#endif // #ifndef SGPU_VERTEXANIMEXPORTSCENE_HPP