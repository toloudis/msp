/*****************************************************************************
**  optxTextureProbe.hpp
**
**      optxTextureProbe samples a texture table's textures in CUDA, as the
**	device programs read them (optxMaterial.h: swizzle, sRGB decoding,
**	filtering, mips): the host side of optx/private/Programs/
**	optxTextureProbe.cu. For tests of texture sharing.
**
**	Create needs optxContext to exist and be current. Sample waits for its
**	launch.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXTEXTUREPROBE_HPP
#error optxTextureProbe.hpp multiply included
#endif
#define GOX_OPTXTEXTUREPROBE_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif

#include <string>
#include <vector>

namespace gox
{
	class optxTextureProbe
	{
		public:
			//	A sample: where (u, v; normalized coordinates) and at which mip level
			struct Query
			{
				float	m_U;
				float	m_V;
				float	m_Level;
				float	m_Unused;
			};

			optxTextureProbe();
			~optxTextureProbe();
			optxTextureProbe(const optxTextureProbe&) = delete;
			optxTextureProbe& operator=(const optxTextureProbe&) = delete;

			bool Create(std::string& o_Error);
			void Destroy();

			//--------------------------------------------------------------------
			//	Sample - texture i_Index of the table at i_Textures (optxTexture
			//	records) at each query: rgba per query; empty on failure
			//--------------------------------------------------------------------
			std::vector<float> Sample(CUdeviceptr i_Textures, unsigned i_Index, const std::vector<Query>& i_Queries);

		private:
			OptixModule					m_Module;
			OptixProgramGroup			m_Groups[3];	// raygen, miss, hit
			OptixPipeline				m_Pipeline;
			cuBuffer					m_Records;
			OptixShaderBindingTable		m_Sbt;
			cuBuffer					m_Params;
			cuBuffer					m_Queries;
			cuBuffer					m_Results;
	};
}
