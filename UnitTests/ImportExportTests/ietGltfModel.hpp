/****************************************************************************\
**  ietGltfModel.hpp
**
**      Helpers that build glTF models in memory for the tests: an
**	accessor of floats or unsigned bytes, each in its own buffer and
**	buffer view.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "ImportExport/gltf/import/private/tiny_gltf.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace iet
{
	inline int AddAccessor(tinygltf::Model& io_Model, const void* i_pData, size_t i_Bytes, size_t i_Count,
						   int i_ComponentType, int i_Type)
	{
		tinygltf::Buffer buffer;
		buffer.data.resize(i_Bytes);
		if (i_Bytes)
			std::memcpy(&buffer.data[0], i_pData, i_Bytes);
		io_Model.buffers.push_back(buffer);

		tinygltf::BufferView view;
		view.buffer = (int)io_Model.buffers.size() - 1;
		view.byteLength = i_Bytes;
		io_Model.bufferViews.push_back(view);

		tinygltf::Accessor accessor;
		accessor.bufferView = (int)io_Model.bufferViews.size() - 1;
		accessor.componentType = i_ComponentType;
		accessor.type = i_Type;
		accessor.count = i_Count;
		io_Model.accessors.push_back(accessor);
		return (int)io_Model.accessors.size() - 1;
	}
}

// an accessor of i_Values (TINYGLTF_TYPE_SCALAR, _VEC3, _MAT4, ...); returns its index
inline int ietAddFloatAccessor(tinygltf::Model& io_Model, const std::vector<float>& i_Values, int i_Type)
{
	const int ncomp = tinygltf::GetNumComponentsInType((uint32_t)i_Type);
	return iet::AddAccessor(io_Model, i_Values.data(), i_Values.size() * sizeof(float), i_Values.size() / ncomp,
							TINYGLTF_COMPONENT_TYPE_FLOAT, i_Type);
}

// an accessor of unsigned bytes (JOINTS_n); returns its index
inline int ietAddUByteAccessor(tinygltf::Model& io_Model, const std::vector<uint8_t>& i_Values, int i_Type)
{
	const int ncomp = tinygltf::GetNumComponentsInType((uint32_t)i_Type);
	return iet::AddAccessor(io_Model, i_Values.data(), i_Values.size(), i_Values.size() / ncomp,
							TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, i_Type);
}

// a KHR_animation_pointer channel on animation i_Animation, through its sampler i_Sampler
inline void ietAddPointerChannel(tinygltf::Model& io_Model, int i_Animation, const std::string& i_Pointer, int i_Sampler)
{
	tinygltf::Value::Object pointer;
	pointer["pointer"] = tinygltf::Value(i_Pointer);
	tinygltf::AnimationChannel channel;
	channel.sampler = i_Sampler;
	channel.target_path = "pointer";
	channel.target_extensions["KHR_animation_pointer"] = tinygltf::Value(pointer);
	io_Model.animations[i_Animation].channels.push_back(channel);
}
