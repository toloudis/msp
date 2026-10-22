/*****************************************************************************
**  vkdLinkStages.cpp
**
**      see vkdLinkStages.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "GraphicsVulkan/vkd/private/vkdLinkStages.hpp"

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <spirv-tools/optimizer.hpp>
#include <spirv-reflect/spirv_reflect.h>	// vcpkg's copy, next to its spirv.h

#include <string>
#include <unordered_set>

namespace gvk
{
	namespace
	{
		//	SPIR-V 1.6 (Vulkan 1.3): what DXC's -fspv-target-env=vulkan1.3 makes
		const spv_target_env c_Env = SPV_ENV_VULKAN_1_3;

		//	The locations one interface variable takes: one per array element
		//	and matrix column
		uint32_t location_count(const SpvReflectInterfaceVariable& i_Var)
		{
			uint32_t count = 1;
			for (uint32_t d = 0; d < i_Var.array.dims_count; d++)
			{
				count *= i_Var.array.dims[d];
			}
			if (i_Var.type_description && (i_Var.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX))
			{
				count *= i_Var.numeric.matrix.column_count;
			}
			return count;
		}

		//	Every location of i_Spirv's non-built-in inputs (i_bInputs) or
		//	outputs. false if SPIRV-Reflect can't read it.
		bool locations(const std::vector<uint32_t>& i_Spirv, bool i_bInputs, std::unordered_set<uint32_t>& o_Locations)
		{
			SpvReflectShaderModule module;
			if (spvReflectCreateShaderModule2(SPV_REFLECT_MODULE_FLAG_NO_COPY, i_Spirv.size() * sizeof(uint32_t),
											  i_Spirv.data(), &module) != SPV_REFLECT_RESULT_SUCCESS)
			{
				return false;
			}
			uint32_t count = 0;
			std::vector<SpvReflectInterfaceVariable*> vars;
			if (i_bInputs)
			{
				spvReflectEnumerateInputVariables(&module, &count, NULL);
				vars.resize(count);
				spvReflectEnumerateInputVariables(&module, &count, vars.data());
			}
			else
			{
				spvReflectEnumerateOutputVariables(&module, &count, NULL);
				vars.resize(count);
				spvReflectEnumerateOutputVariables(&module, &count, vars.data());
			}
			for (const SpvReflectInterfaceVariable* var : vars)
			{
				if (var->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
				{
					continue;
				}
				for (uint32_t l = 0; l < location_count(*var); l++)
				{
					o_Locations.insert(var->location + l);
				}
			}
			spvReflectDestroyShaderModule(&module);
			return true;
		}
	}

	std::vector<uint32_t> vkdTrimUnconsumedOutputs(const std::vector<uint32_t>& i_Producer,
												   const std::vector<uint32_t>& i_Consumer)
	{
		//	The pixel shader's declared inputs, read or not: the layer
		//	matches declarations, and an input with no output to match is
		//	a finding of its own
		std::unordered_set<uint32_t> live_locs, written;
		if (!locations(i_Consumer, true, live_locs) || !locations(i_Producer, false, written))
		{
			DBG_LOG("vkdTrimUnconsumedOutputs: SPIRV-Reflect can't read the shaders");
			return i_Producer;
		}
		bool unconsumed = false;
		for (uint32_t location : written)
		{
			unconsumed = unconsumed || live_locs.count(location) == 0;
		}
		if (!unconsumed)
		{
			return i_Producer;
		}

		//	Every built-in SPIRV-Tools would analyze stays (see the header)
		std::unordered_set<uint32_t> live_builtins =
		{
			(uint32_t)SpvBuiltInPointSize, (uint32_t)SpvBuiltInClipDistance, (uint32_t)SpvBuiltInCullDistance
		};
		std::string messages;
		spvtools::Optimizer trim(c_Env);
		trim.SetMessageConsumer([&messages](spv_message_level_t, const char*, const spv_position_t&, const char* i_Message)
		{
			messages += std::string(messages.empty() ? "" : "; ") + i_Message;
		});
		trim.RegisterPass(spvtools::CreateEliminateDeadOutputStoresPass(&live_locs, &live_builtins))
			.RegisterPass(spvtools::CreateRemoveUnusedInterfaceVariablesPass())
			.RegisterPass(spvtools::CreateDeadVariableEliminationPass());
		//	DXC validated the module; the optimizer's validator doesn't
		//	know DXC's -fvk-use-dx-layout block layouts
		spvtools::OptimizerOptions options;
		options.set_run_validator(false);
		std::vector<uint32_t> trimmed;
		if (!trim.Run(i_Producer.data(), i_Producer.size(), &trimmed, options))
		{
			DBG_LOG("vkdTrimUnconsumedOutputs: can't remove the unconsumed outputs (" << messages << ")");
			return i_Producer;
		}
		return trimmed;
	}
}
