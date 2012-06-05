/*****************************************************************************
**	vtxStreamCompression.hpp
**
**		Uses an extrapolating deltas compression algorithm on a stream of 
**	vertex animation frames.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_STREAMCOMPRESSION_HPP
#error vtxStreamCompression.hpp multiply included
#endif
#define VTX_STREAMCOMPRESSION_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 
#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 
#ifndef VTX_DELTAENCODER_HPP
#include "Graphics/vtx/vtxDeltaEncoder.hpp"
#endif 
#ifndef VTX_VERTEXFRAME_HPP
#include "Graphics/vtx/vtxVertexFrame.hpp"
#endif 

#include <list>


//============================================================================
//============================================================================
class chWriter;
class vtxGeometryCacheWriter;


//============================================================================
//============================================================================
class vtxStreamCompression
{
public:
	//--------------------------------------------------------------------
	// Static functions
	//--------------------------------------------------------------------
	static shared_ptr<vtxStreamCompression> CreateLosslessCompressStream();
	static shared_ptr<vtxStreamCompression> CreateToleranceCompressStream();

	//--------------------------------------------------------------------
	// Constructor requires a geometry cache to which to write data
	// when necessary.
	// i_SyncInterval defines how often a new reference frame 
	//	needs to be written.
	//--------------------------------------------------------------------
	vtxStreamCompression(const std::string &i_SurfaceName,
						  int i_SyncInterval,
						  vtxGeometryCacheWriter &io_GeometryCache,
						  const std::string &i_EncoderString,
						  shared_ptr<vtxDeltaEncoder> &io_DeltaEncoder,
						  shared_ptr<vtxDeltaEncoder> &io_NormalEncoder);

	//--------------------------------------------------------------------
	// Compare this frame with the current stream data and write 
	//	animation data to the geometry cache when needed.
	//--------------------------------------------------------------------
	void SubmitFrame(float i_Time, const vtxVertexFrame &i_Frame);

	//--------------------------------------------------------------------
	// Animation data is finished, flush the current stream of 
	//	animation data to the geometry cache.
	//--------------------------------------------------------------------
	void Finish();

	//--------------------------------------------------------------------
	// Write out frame information, including references into 
	// geometry cache.
	//--------------------------------------------------------------------
	void WriteCompressedAnimationData(chWriter &o_Writer);

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_frames(chWriter &o_Writer);
	void write_deltas();

private:
	std::string m_SurfaceName;
	std::string m_EncoderString;
	vtxGeometryCacheWriter &m_GeometryCache;
	int m_SyncInterval;
	int m_FrameCount;

	vtxVertexFrame m_CurrentFrame;
	vtxVertexFrame m_LastFrames[2];
	int m_LastFrameIndex;
	envType::Int64 m_RefFrameOffset;

	std::vector<maPoint3d> m_Deltas;
	shared_ptr<vtxDeltaEncoder> m_DeltaEncoder;
	bool m_bHaveNormals;
	std::vector<maPoint3d> m_NormalDeltas;
	shared_ptr<vtxDeltaEncoder> m_NormalEncoder;

	struct sFrameData
	{
		envType::Float32 m_Time;
		envType::Int64 m_ReferenceFrame;
		envType::Int64 m_DeltaBlock;
		envType::Int64 m_NormalsBlock;
		envType::Int32 m_DeltaOffset;
		maAxisBox m_BBox;
		bool m_bStatic;

		sFrameData() : m_Time(0), m_ReferenceFrame(0), m_DeltaBlock(-1), m_NormalsBlock(-1), m_DeltaOffset(0), m_bStatic(false) {}
	};
	std::list<sFrameData> m_FrameInfoList;
	std::list<sFrameData> m_DeltasList;
};
