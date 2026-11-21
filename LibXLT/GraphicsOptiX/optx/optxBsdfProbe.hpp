/*****************************************************************************
**  optxBsdfProbe.hpp
**
**      optxBsdfProbe evaluates and samples the BRDF the tracing programs
**	use (optx/private/Programs/optxBsdf.h) in CUDA, for given material
**	values and directions: the host side of optx/private/Programs/
**	optxBsdfProbe.cu (what each query gives in optxBsdfProbeParams.h).
**	For tests of the BRDF.
**
**	Create needs optxContext to exist and be current. Run waits for its
**	launch.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXBSDFPROBE_HPP
#error optxBsdfProbe.hpp multiply included
#endif
#define GOX_OPTXBSDFPROBE_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXBSDFPROBEPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxBsdfProbeParams.h"
#endif

#include <string>
#include <vector>

namespace gox
{
	class optxBsdfProbe
	{
		public:
			optxBsdfProbe();
			~optxBsdfProbe();
			optxBsdfProbe(const optxBsdfProbe&) = delete;
			optxBsdfProbe& operator=(const optxBsdfProbe&) = delete;

			bool Create(std::string& o_Error);
			void Destroy();

			//--------------------------------------------------------------------
			//	Run - one result per query; empty on failure
			//--------------------------------------------------------------------
			std::vector<optxBsdfResult> Run(const std::vector<optxBsdfQuery>& i_Queries);

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
