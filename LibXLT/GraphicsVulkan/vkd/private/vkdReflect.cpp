/*****************************************************************************
**  vkdReflect.cpp
**
**      see vkdReflect.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "GraphicsVulkan/vkd/vkdReflect.hpp"
#include "GraphicsVulkan/vkd/vkdBindings.hpp"

#include <spirv-reflect/spirv_reflect.h>	// vcpkg's copy, next to its spirv.h

#include <cctype>
#include <cstring>

namespace gvk
{
	namespace
	{
		uint32_t round16(uint32_t i_Size)
		{
			return (i_Size + 15) & ~15u;
		}

		uint32_t array_elements(const SpvReflectArrayTraits& i_Array)
		{
			uint32_t elements = 1;
			for (uint32_t d = 0; d < i_Array.dims_count; d++)
			{
				elements *= i_Array.dims[d];
			}
			return elements;
		}

		//	"type.Foo" (DXC's name for the type of cbuffer/struct Foo) -> "Foo"
		std::string type_name(const char* i_pName)
		{
			std::string name = i_pName ? i_pName : "";
			if (name.compare(0, 5, "type.") == 0)
			{
				name = name.substr(5);
			}
			return name;
		}

		vkdShaderType to_type(const SpvReflectBlockVariable& i_Var)
		{
			const SpvReflectTypeDescription* td = i_Var.type_description;
			const SpvReflectTypeFlags flags = td ? td->type_flags : 0;

			vkdShaderType type;
			type.m_Elements = (flags & SPV_REFLECT_TYPE_FLAG_ARRAY) ? array_elements(i_Var.array) : 0;

			if (flags & SPV_REFLECT_TYPE_FLAG_STRUCT)
			{
				type.m_Class = fx_SVC_STRUCT;
				type.m_Type = fx_SVT_VOID;
				const SpvReflectTypeDescription* st = td->struct_type_description ? td->struct_type_description : td;
				type.m_Name = type_name(st->type_name);
				type.m_Rows = 1;
				uint32_t columns = 0;
				for (uint32_t m = 0; m < i_Var.member_count; m++)
				{
					vkdShaderType::Member member;
					member.m_Name = i_Var.members[m].name ? i_Var.members[m].name : "";
					member.m_Offset = i_Var.members[m].offset;
					member.m_Type = to_type(i_Var.members[m]);
					const uint32_t count = member.m_Type.m_Elements ? member.m_Type.m_Elements : 1;
					columns += member.m_Type.m_Rows * member.m_Type.m_Columns * count;
					type.m_Members.push_back(member);
				}
				type.m_Columns = columns;
				return type;
			}

			const char* base = "float";
			if (flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
			{
				type.m_Type = fx_SVT_FLOAT;
			}
			else if (flags & SPV_REFLECT_TYPE_FLAG_INT)
			{
				type.m_Type = i_Var.numeric.scalar.signedness ? fx_SVT_INT : fx_SVT_UINT;
				base = i_Var.numeric.scalar.signedness ? "int" : "uint";
			}
			else if (flags & SPV_REFLECT_TYPE_FLAG_BOOL)
			{
				type.m_Type = fx_SVT_BOOL;
				base = "bool";
			}

			if (flags & SPV_REFLECT_TYPE_FLAG_MATRIX)
			{
				//	DXC: HLSL RxC -> R SPIR-V columns of C components;
				//	HLSL column_major -> SPIR-V RowMajor
				type.m_Rows = i_Var.numeric.matrix.column_count;
				type.m_Columns = i_Var.numeric.matrix.row_count;
				type.m_Class = (i_Var.decoration_flags & SPV_REFLECT_DECORATION_ROW_MAJOR) ? fx_SVC_MATRIX_COLUMNS : fx_SVC_MATRIX_ROWS;
				type.m_Name = std::string(base) + std::to_string(type.m_Rows) + "x" + std::to_string(type.m_Columns);
			}
			else if (flags & SPV_REFLECT_TYPE_FLAG_VECTOR)
			{
				type.m_Class = fx_SVC_VECTOR;
				type.m_Rows = 1;
				type.m_Columns = i_Var.numeric.vector.component_count;
				type.m_Name = std::string(base) + std::to_string(type.m_Columns);
			}
			else
			{
				type.m_Class = fx_SVC_SCALAR;
				type.m_Rows = 1;
				type.m_Columns = 1;
				type.m_Name = base;
			}
			return type;
		}

		bool add_constant_buffer(const SpvReflectDescriptorBinding& i_Binding, vkdShaderReflection& o_Reflection)
		{
			vkdShaderBuffer buffer;
			buffer.m_Name = (i_Binding.name && *i_Binding.name) ? i_Binding.name
							: type_name(i_Binding.type_description ? i_Binding.type_description->type_name : "");
			buffer.m_Slot = i_Binding.binding;
			uint32_t end = 0;
			for (uint32_t m = 0; m < i_Binding.block.member_count; m++)
			{
				const SpvReflectBlockVariable& member = i_Binding.block.members[m];
				vkdShaderVariable var;
				var.m_Name = member.name ? member.name : "";
				var.m_Offset = member.offset;
				var.m_Type = to_type(member);
				var.m_bColumnMajor = (var.m_Type.m_Class == fx_SVC_MATRIX_COLUMNS);
				const uint32_t element = var.m_Type.GetElementSize();
				var.m_Size = var.m_Type.m_Elements > 0 ? (var.m_Type.m_Elements - 1) * member.array.stride + element : element;
				if (var.m_Offset + var.m_Size > end)
				{
					end = var.m_Offset + var.m_Size;
				}
				buffer.m_Variables.push_back(var);
			}
			buffer.m_Size = round16(end);
			o_Reflection.m_ConstantBuffers.push_back(buffer);
			return true;
		}

		//	"in.var.TEXCOORD12" -> ("TEXCOORD", 12)
		void parse_semantic(const char* i_pName, std::string& o_Semantic, uint32_t& o_Index)
		{
			std::string name = i_pName ? i_pName : "";
			if (name.compare(0, 7, "in.var.") == 0)
			{
				name = name.substr(7);
			}
			size_t digits = name.size();
			while (digits > 0 && isdigit((unsigned char)name[digits - 1]))
			{
				digits--;
			}
			o_Semantic = name.substr(0, digits);
			o_Index = digits < name.size() ? (uint32_t)atoi(name.c_str() + digits) : 0;
		}
	}

	uint32_t vkdShaderType::GetElementSize() const
	{
		switch (m_Class)
		{
			case fx_SVC_SCALAR:
			case fx_SVC_VECTOR:
				return m_Columns * 4;
			case fx_SVC_MATRIX_ROWS:
				return (m_Rows - 1) * 16 + m_Columns * 4;
			case fx_SVC_MATRIX_COLUMNS:
				return (m_Columns - 1) * 16 + m_Rows * 4;
			case fx_SVC_STRUCT:
			{
				uint32_t size = 0;
				for (const Member& m : m_Members)
				{
					uint32_t memberSize = m.m_Type.GetElementSize();
					if (m.m_Type.m_Elements > 0)
					{
						memberSize += (m.m_Type.m_Elements - 1) * ((memberSize + 15) & ~15u);
					}
					if (m.m_Offset + memberSize > size)
					{
						size = m.m_Offset + memberSize;
					}
				}
				return size;
			}
			default:
				return 0;
		}
	}

	bool vkdReflect(const void* i_pSpirv, size_t i_Size, vkdShaderReflection& o_Reflection, std::string& o_Error)
	{
		o_Reflection = vkdShaderReflection();
		SpvReflectShaderModule module;
		if (spvReflectCreateShaderModule(i_Size, i_pSpirv, &module) != SPV_REFLECT_RESULT_SUCCESS)
		{
			o_Error = "not a SPIR-V module SPIRV-Reflect can read";
			return false;
		}

		bool ok = true;
		o_Reflection.m_Stage = (VkShaderStageFlagBits)module.shader_stage;
		for (uint32_t c = 0; c < module.capability_count; c++)
		{
			o_Reflection.m_Capabilities.push_back((uint32_t)module.capabilities[c].value);
		}

		uint32_t count = 0;
		spvReflectEnumerateDescriptorBindings(&module, &count, NULL);
		std::vector<SpvReflectDescriptorBinding*> bindings(count);
		spvReflectEnumerateDescriptorBindings(&module, &count, bindings.data());
		for (const SpvReflectDescriptorBinding* b : bindings)
		{
			const std::string name = b->name ? b->name : "";
			if (b->set != 0)
			{
				o_Error = name + ": descriptor set " + std::to_string(b->set) + " (expected 0)";
				ok = false;
				break;
			}
			if (b->binding < vkdBindings::c_TextureBinding)
			{
				if (b->descriptor_type != SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
					o_Error = name + ": binding " + std::to_string(b->binding) + " is not a constant buffer";
					ok = false;
					break;
				}
				add_constant_buffer(*b, o_Reflection);
				continue;
			}

			vkdShaderResource resource;
			resource.m_Name = name;
			resource.m_Count = b->count;
			if (b->binding < vkdBindings::c_SamplerBinding)
			{
				resource.m_Kind = vkdShaderResource::e_Texture;
				resource.m_Slot = b->binding - vkdBindings::c_TextureBinding;
			}
			else if (b->binding < vkdBindings::c_UavBinding)
			{
				resource.m_Kind = vkdShaderResource::e_Sampler;
				resource.m_Slot = b->binding - vkdBindings::c_SamplerBinding;
			}
			else
			{
				resource.m_Kind = vkdShaderResource::e_Uav;
				resource.m_Slot = b->binding - vkdBindings::c_UavBinding;
			}
			switch (b->descriptor_type)
			{
				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					resource.m_Dim = (int)b->image.dim;
					resource.m_bArrayed = b->image.arrayed != 0;
					resource.m_bMultisampled = b->image.ms != 0;
					resource.m_bBuffer = b->image.dim == SpvDimBuffer;
					break;
				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					resource.m_bBuffer = true;
					break;
				default:
					break;
			}
			o_Reflection.m_Resources.push_back(resource);
		}

		if (ok && module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
		{
			spvReflectEnumerateInputVariables(&module, &count, NULL);
			std::vector<SpvReflectInterfaceVariable*> inputs(count);
			spvReflectEnumerateInputVariables(&module, &count, inputs.data());
			for (const SpvReflectInterfaceVariable* v : inputs)
			{
				if (v->built_in != -1 || (v->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN))
				{
					continue;
				}
				vkdShaderInput input;
				parse_semantic((v->semantic && *v->semantic) ? v->semantic : v->name, input.m_Semantic, input.m_Index);
				input.m_Location = v->location;
				input.m_Format = (VkFormat)v->format;
				o_Reflection.m_Inputs.push_back(input);
			}
		}

		if (ok && module.shader_stage == SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT)
		{
			spvReflectEnumerateOutputVariables(&module, &count, NULL);
			std::vector<SpvReflectInterfaceVariable*> outputs(count);
			spvReflectEnumerateOutputVariables(&module, &count, outputs.data());
			for (const SpvReflectInterfaceVariable* v : outputs)
			{
				if (v->built_in == -1 && !(v->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) &&
					v->location + 1 > o_Reflection.m_ColorOutputs)
				{
					o_Reflection.m_ColorOutputs = v->location + 1;
				}
			}
		}

		spvReflectDestroyShaderModule(&module);
		return ok;
	}
}
