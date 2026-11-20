/*****************************************************************************
**  optxTextureProbe.cpp
**
**      See optxTextureProbe.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXTEXTUREPROBE_HPP
#include "GraphicsOptiX/optx/optxTextureProbe.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GOX_OPTXEMBEDDEDPROGRAMS_HPP
#include "GraphicsOptiX/optx/private/optxEmbeddedPrograms.hpp"
#endif
#ifndef GOX_OPTXTEXTUREPROBEPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxTextureProbeParams.h"
#endif

#include <optix_stack_size.h>

namespace gox
{
	namespace
	{
		struct alignas(OPTIX_SBT_RECORD_ALIGNMENT) EmptyRecord
		{
			char m_Header[OPTIX_SBT_RECORD_HEADER_SIZE];
		};
	}

	optxTextureProbe::optxTextureProbe()
		: m_Module(NULL)
		, m_Groups()
		, m_Pipeline(NULL)
		, m_Sbt()
	{
	}

	optxTextureProbe::~optxTextureProbe()
	{
		Destroy();
	}

	bool optxTextureProbe::Create(std::string& o_Error)
	{
		Destroy();
		const optxEmbeddedProgram* program = optxEmbeddedPrograms::Find("optxTextureProbe");
		if (!program)
		{
			o_Error = "the optxTextureProbe program isn't embedded";
			return false;
		}
		OptixDeviceContext context = optxContext::GetOptix();
		OptixModuleCompileOptions module_options = {};
		module_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
		module_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
		module_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL;
		OptixPipelineCompileOptions pipeline_options = {};
		pipeline_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
		pipeline_options.numPayloadValues = 0;
		pipeline_options.numAttributeValues = 2;
		pipeline_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;
		pipeline_options.pipelineLaunchParamsVariableName = "params";
		pipeline_options.usesPrimitiveTypeFlags = OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE;
		if (OPTIX_CHECK(optixModuleCreate(context, &module_options, &pipeline_options,
										  reinterpret_cast<const char*>(program->m_pIR), program->m_Size, NULL, NULL, &m_Module)))
		{
			m_Module = NULL;
			o_Error = "optxTextureProbe: module creation failed";
			return false;
		}
		OptixProgramGroupDesc descs[3] = {};
		descs[0].kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		descs[0].raygen.module = m_Module;
		descs[0].raygen.entryFunctionName = "__raygen__probe";
		descs[1].kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		descs[1].miss.module = m_Module;
		descs[1].miss.entryFunctionName = "__miss__none";
		descs[2].kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		descs[2].hitgroup.moduleCH = m_Module;
		descs[2].hitgroup.entryFunctionNameCH = "__closesthit__none";
		OptixProgramGroupOptions group_options = {};
		if (OPTIX_CHECK(optixProgramGroupCreate(context, descs, 3, &group_options, NULL, NULL, m_Groups)))
		{
			for (OptixProgramGroup& group : m_Groups)
			{
				group = NULL;
			}
			o_Error = "optxTextureProbe: program group creation failed";
			Destroy();
			return false;
		}
		OptixPipelineLinkOptions link_options = {};
		link_options.maxTraceDepth = 1;
		if (OPTIX_CHECK(optixPipelineCreate(context, &pipeline_options, &link_options, m_Groups, 3, NULL, NULL, &m_Pipeline)))
		{
			m_Pipeline = NULL;
			o_Error = "optxTextureProbe: pipeline creation failed";
			Destroy();
			return false;
		}
		EmptyRecord records[3];
		for (int g = 0; g < 3; ++g)
		{
			if (OPTIX_CHECK(optixSbtRecordPackHeader(m_Groups[g], &records[g])))
			{
				o_Error = "optxTextureProbe: shader binding table couldn't be made";
				Destroy();
				return false;
			}
		}
		if (!m_Records.Upload(records, sizeof(records)))
		{
			o_Error = "optxTextureProbe: shader binding table couldn't be uploaded";
			Destroy();
			return false;
		}
		m_Sbt = OptixShaderBindingTable();
		m_Sbt.raygenRecord = m_Records.Get();
		m_Sbt.missRecordBase = m_Records.Get() + sizeof(EmptyRecord);
		m_Sbt.missRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.missRecordCount = 1;
		m_Sbt.hitgroupRecordBase = m_Records.Get() + 2 * sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordCount = 1;
		return true;
	}

	void optxTextureProbe::Destroy()
	{
		if (m_Pipeline)
		{
			OPTIX_CHECK(optixPipelineDestroy(m_Pipeline));
		}
		for (OptixProgramGroup& group : m_Groups)
		{
			if (group)
			{
				OPTIX_CHECK(optixProgramGroupDestroy(group));
			}
			group = NULL;
		}
		if (m_Module)
		{
			OPTIX_CHECK(optixModuleDestroy(m_Module));
		}
		m_Pipeline = NULL;
		m_Module = NULL;
		m_Sbt = OptixShaderBindingTable();
		m_Records.Free();
		m_Params.Free();
		m_Queries.Free();
		m_Results.Free();
	}

	std::vector<float> optxTextureProbe::Sample(CUdeviceptr i_Textures, unsigned i_Index, const std::vector<Query>& i_Queries)
	{
		std::vector<float> results(i_Queries.size() * 4);
		if (!m_Pipeline || i_Queries.empty() || !m_Queries.Upload(i_Queries.data(), i_Queries.size() * sizeof(Query)) ||
			!m_Results.Reserve(results.size() * sizeof(float)))
		{
			return std::vector<float>();
		}
		optxTextureProbeParams params = {};
		params.m_Textures = i_Textures;
		params.m_Queries = m_Queries.Get();
		params.m_Results = m_Results.Get();
		params.m_Index = i_Index;
		params.m_Count = unsigned(i_Queries.size());
		if (!m_Params.Upload(&params, sizeof(params)) ||
			OPTIX_CHECK(optixLaunch(m_Pipeline, optxContext::GetStream(), m_Params.Get(), sizeof(params), &m_Sbt,
									params.m_Count, 1, 1)) ||
			!m_Results.Download(results.data(), results.size() * sizeof(float)))
		{
			return std::vector<float>();
		}
		return results;
	}
}
