/****************************************************************************\
**  gltfAccessorUtil.hpp
**
**      gltfAccessorUtil reads typed data out of glTF accessors (handling
**	byte strides, normalized integer types and sparse storage) and
**	converts primitive index lists into triangle lists.
**
**	Only depends on tiny_gltf and the standard library so it can be
**	tested outside of the LibXLT build.
\****************************************************************************/
#pragma once

#include "tiny_gltf.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>


namespace gltfAccessorUtil
{
	namespace detail
	{
		//--------------------------------------------------------------------
		// Read one component of the given type from raw memory as a float.
		// Integer types are mapped to [0,1] or [-1,1] if i_bNormalized.
		//--------------------------------------------------------------------
		inline float read_component(const unsigned char* i_pData, int i_ComponentType, bool i_bNormalized)
		{
			switch (i_ComponentType)
			{
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
			{
				float f;
				std::memcpy(&f, i_pData, sizeof(f));
				return f;
			}
			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
			{
				double d;
				std::memcpy(&d, i_pData, sizeof(d));
				return (float)d;
			}
			case TINYGLTF_COMPONENT_TYPE_BYTE:
			{
				int8_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				if (!i_bNormalized) return (float)v;
				float f = (float)v / 127.0f;
				return (f < -1.0f) ? -1.0f : f;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			{
				uint8_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				return i_bNormalized ? (float)v / 255.0f : (float)v;
			}
			case TINYGLTF_COMPONENT_TYPE_SHORT:
			{
				int16_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				if (!i_bNormalized) return (float)v;
				float f = (float)v / 32767.0f;
				return (f < -1.0f) ? -1.0f : f;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			{
				uint16_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				return i_bNormalized ? (float)v / 65535.0f : (float)v;
			}
			case TINYGLTF_COMPONENT_TYPE_INT:
			{
				int32_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				return (float)v;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			{
				uint32_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				return (float)v;
			}
			}
			return 0.0f;
		}

		//--------------------------------------------------------------------
		// Read one unsigned integer index of the given type from raw memory.
		// Returns false for component types that are not valid for indices.
		//--------------------------------------------------------------------
		inline bool read_index(const unsigned char* i_pData, int i_ComponentType, uint32_t& o_Index)
		{
			switch (i_ComponentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			{
				uint8_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				o_Index = v;
				return true;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			{
				uint16_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				o_Index = v;
				return true;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			{
				uint32_t v;
				std::memcpy(&v, i_pData, sizeof(v));
				o_Index = v;
				return true;
			}
			}
			return false;
		}

		//--------------------------------------------------------------------
		// Locate i_Count elements of i_ElementSize bytes, i_Stride apart,
		// starting i_ByteOffset into the given buffer view. Returns NULL
		// if anything is out of range.
		//--------------------------------------------------------------------
		inline const unsigned char* locate(const tinygltf::Model& i_Model, int i_BufferView,
										   size_t i_ByteOffset, size_t i_Count,
										   size_t i_Stride, size_t i_ElementSize)
		{
			if (i_BufferView < 0 || i_BufferView >= (int)i_Model.bufferViews.size())
				return NULL;
			const tinygltf::BufferView& bv = i_Model.bufferViews[i_BufferView];
			if (bv.buffer < 0 || bv.buffer >= (int)i_Model.buffers.size())
				return NULL;
			const tinygltf::Buffer& buffer = i_Model.buffers[bv.buffer];

			if (i_Count == 0)
				return NULL;
			const size_t last_byte = i_ByteOffset + i_Stride * (i_Count - 1) + i_ElementSize;
			if (last_byte > bv.byteLength)
				return NULL;
			if (bv.byteOffset + bv.byteLength > buffer.data.size())
				return NULL;

			return &buffer.data[0] + bv.byteOffset + i_ByteOffset;
		}

		//--------------------------------------------------------------------
		// Read an integer index list (used by sparse accessors)
		//--------------------------------------------------------------------
		inline bool read_index_list(const tinygltf::Model& i_Model, int i_BufferView,
									size_t i_ByteOffset, int i_ComponentType, size_t i_Count,
									std::vector<uint32_t>& o_Indices)
		{
			const int csize = tinygltf::GetComponentSizeInBytes((uint32_t)i_ComponentType);
			if (csize <= 0)
				return false;
			const unsigned char* pData = locate(i_Model, i_BufferView, i_ByteOffset, i_Count, csize, csize);
			if (!pData)
				return false;
			o_Indices.resize(i_Count);
			for (size_t i = 0; i < i_Count; ++i)
			{
				if (!read_index(pData + i * csize, i_ComponentType, o_Indices[i]))
					return false;
			}
			return true;
		}
	}	// end of namespace detail

	//------------------------------------------------------------------------
	// ReadFloats - read accessor i_Accessor into o_Values as tightly packed
	// floats, i_NumComponents per element. The accessor must have exactly
	// i_NumComponents components (e.g. 3 for VEC3). Returns false (and
	// leaves o_Values empty) if the accessor is missing or malformed.
	//------------------------------------------------------------------------
	inline bool ReadFloats(const tinygltf::Model& i_Model, int i_Accessor,
						   int i_NumComponents, std::vector<float>& o_Values)
	{
		o_Values.clear();
		if (i_Accessor < 0 || i_Accessor >= (int)i_Model.accessors.size())
			return false;

		const tinygltf::Accessor& a = i_Model.accessors[i_Accessor];
		const int ncomp = tinygltf::GetNumComponentsInType((uint32_t)a.type);
		const int csize = tinygltf::GetComponentSizeInBytes((uint32_t)a.componentType);
		if (ncomp != i_NumComponents || csize <= 0 || a.count == 0)
			return false;

		std::vector<float> values(a.count * ncomp, 0.0f);

		// An accessor with no buffer view is all zeros (possibly with sparse data on top)
		if (a.bufferView >= 0)
		{
			const tinygltf::BufferView& bv = i_Model.bufferViews[a.bufferView];
			const int stride = a.ByteStride(bv);
			if (stride <= 0)
				return false;
			const unsigned char* pData = detail::locate(i_Model, a.bufferView, a.byteOffset,
														a.count, stride, (size_t)ncomp * csize);
			if (!pData)
				return false;

			for (size_t i = 0; i < a.count; ++i)
			{
				const unsigned char* pElem = pData + i * stride;
				for (int c = 0; c < ncomp; ++c)
					values[i * ncomp + c] = detail::read_component(pElem + c * csize, a.componentType, a.normalized);
			}
		}
		else if (!a.sparse.isSparse)
		{
			return false;
		}

		if (a.sparse.isSparse && a.sparse.count > 0)
		{
			std::vector<uint32_t> sparse_indices;
			if (!detail::read_index_list(i_Model, a.sparse.indices.bufferView, a.sparse.indices.byteOffset,
										 a.sparse.indices.componentType, a.sparse.count, sparse_indices))
				return false;

			// sparse values are tightly packed with the accessor's component type
			const size_t elem_size = (size_t)ncomp * csize;
			const unsigned char* pValues = detail::locate(i_Model, a.sparse.values.bufferView,
														  a.sparse.values.byteOffset, a.sparse.count,
														  elem_size, elem_size);
			if (!pValues)
				return false;

			for (int s = 0; s < a.sparse.count; ++s)
			{
				const uint32_t target = sparse_indices[s];
				if (target >= a.count)
					return false;
				const unsigned char* pElem = pValues + s * elem_size;
				for (int c = 0; c < ncomp; ++c)
					values[target * ncomp + c] = detail::read_component(pElem + c * csize, a.componentType, a.normalized);
			}
		}

		o_Values.swap(values);
		return true;
	}

	//------------------------------------------------------------------------
	// ReadIndices - read a SCALAR unsigned integer accessor (the indices
	// of a primitive). Returns false if the accessor is malformed.
	//------------------------------------------------------------------------
	inline bool ReadIndices(const tinygltf::Model& i_Model, int i_Accessor,
							std::vector<uint32_t>& o_Indices)
	{
		o_Indices.clear();
		if (i_Accessor < 0 || i_Accessor >= (int)i_Model.accessors.size())
			return false;

		const tinygltf::Accessor& a = i_Model.accessors[i_Accessor];
		if (a.type != TINYGLTF_TYPE_SCALAR)
			return false;

		// Indices can (rarely) be sparse; going through ReadFloats would lose
		// precision above 2^24, so read the plain case directly.
		if (!a.sparse.isSparse && a.bufferView >= 0)
		{
			const tinygltf::BufferView& bv = i_Model.bufferViews[a.bufferView];
			const int csize = tinygltf::GetComponentSizeInBytes((uint32_t)a.componentType);
			const int stride = a.ByteStride(bv);
			if (csize <= 0 || stride <= 0)
				return false;
			const unsigned char* pData = detail::locate(i_Model, a.bufferView, a.byteOffset, a.count, stride, csize);
			if (!pData)
				return false;
			o_Indices.resize(a.count);
			for (size_t i = 0; i < a.count; ++i)
			{
				if (!detail::read_index(pData + i * stride, a.componentType, o_Indices[i]))
				{
					o_Indices.clear();
					return false;
				}
			}
			return true;
		}

		std::vector<float> values;
		if (!ReadFloats(i_Model, i_Accessor, 1, values))
			return false;
		o_Indices.resize(values.size());
		for (size_t i = 0; i < values.size(); ++i)
			o_Indices[i] = (uint32_t)values[i];
		return true;
	}

	//------------------------------------------------------------------------
	// IsTriangleMode - true for primitive modes that produce triangles
	//------------------------------------------------------------------------
	inline bool IsTriangleMode(int i_Mode)
	{
		return i_Mode == TINYGLTF_MODE_TRIANGLES ||
			   i_Mode == TINYGLTF_MODE_TRIANGLE_STRIP ||
			   i_Mode == TINYGLTF_MODE_TRIANGLE_FAN;
	}

	//------------------------------------------------------------------------
	// Triangulate - convert the vertex index list of a primitive into a
	// plain triangle list, preserving glTF's counter-clockwise front faces.
	// Degenerate strip/fan triangles are dropped.
	//------------------------------------------------------------------------
	inline void Triangulate(int i_Mode, const std::vector<uint32_t>& i_Indices,
							std::vector<uint32_t>& o_Triangles)
	{
		o_Triangles.clear();
		const size_t n = i_Indices.size();
		if (i_Mode == TINYGLTF_MODE_TRIANGLES)
		{
			const size_t ntri = n / 3;
			o_Triangles.assign(i_Indices.begin(), i_Indices.begin() + ntri * 3);
		}
		else if (i_Mode == TINYGLTF_MODE_TRIANGLE_STRIP)
		{
			for (size_t i = 2; i < n; ++i)
			{
				uint32_t a = i_Indices[i - 2], b = i_Indices[i - 1], c = i_Indices[i];
				if (a == b || b == c || a == c)
					continue;
				// every other triangle in a strip has reversed winding
				if (i % 2 == 0) { o_Triangles.push_back(a); o_Triangles.push_back(b); }
				else			{ o_Triangles.push_back(b); o_Triangles.push_back(a); }
				o_Triangles.push_back(c);
			}
		}
		else if (i_Mode == TINYGLTF_MODE_TRIANGLE_FAN)
		{
			for (size_t i = 2; i < n; ++i)
			{
				uint32_t a = i_Indices[0], b = i_Indices[i - 1], c = i_Indices[i];
				if (a == b || b == c || a == c)
					continue;
				o_Triangles.push_back(a);
				o_Triangles.push_back(b);
				o_Triangles.push_back(c);
			}
		}
	}

}	// end of namespace
