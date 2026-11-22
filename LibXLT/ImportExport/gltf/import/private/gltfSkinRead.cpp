/****************************************************************************\
**  gltfSkinRead.cpp
**
**      see gltfSkinRead.hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SKINREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkinRead.hpp"
#endif
#ifndef GLTF_MESHIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#endif

#include "ImportExport/gltf/import/private/gltfAccessorUtil.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>

namespace
{
	int find_attribute(const tinygltf::Primitive& i_Prim, const std::string& i_Name)
	{
		std::map<std::string, int>::const_iterator it = i_Prim.attributes.find(i_Name);
		return (it == i_Prim.attributes.end()) ? -1 : it->second;
	}

	size_t accessor_count(const tinygltf::Model& i_Model, int i_Accessor)
	{
		if (i_Accessor < 0 || i_Accessor >= (int)i_Model.accessors.size())
			return 0;
		return i_Model.accessors[i_Accessor].count;
	}

	std::string set_name(const char* i_Prefix, int i_Set)
	{
		std::ostringstream name;
		name << i_Prefix << i_Set;
		return name.str();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfSkinRead
{
	bool ReadInfluences(const tinygltf::Model& i_Model, const tinygltf::Primitive& i_Prim,
						const std::vector<int>& i_BoneOfJoint,
						std::vector<std::vector<gltfInfluence>>& o_Vertices, bool& o_bRenormalized)
	{
		o_Vertices.clear();
		o_bRenormalized = false;
		const size_t numVertices = accessor_count(i_Model, find_attribute(i_Prim, "POSITION"));
		if (numVertices == 0)
			return false;

		o_Vertices.resize(numVertices);
		int set = 0;
		for (;; set++)
		{
			const int joints = find_attribute(i_Prim, set_name("JOINTS_", set));
			const int weights = find_attribute(i_Prim, set_name("WEIGHTS_", set));
			if (joints < 0 || weights < 0)
				break;

			std::vector<float> jointValues, weightValues;
			if (!gltfAccessorUtil::ReadFloats(i_Model, joints, 4, jointValues)
				|| !gltfAccessorUtil::ReadFloats(i_Model, weights, 4, weightValues)
				|| jointValues.size() != numVertices * 4 || weightValues.size() != numVertices * 4)
			{
				o_Vertices.clear();
				return false;
			}

			for (size_t v = 0; v < numVertices; v++)
			{
				for (int c = 0; c < 4; c++)
				{
					const float weight = weightValues[v * 4 + c];
					const int joint = (int)jointValues[v * 4 + c];
					if (weight <= 0 || joint < 0 || joint >= (int)i_BoneOfJoint.size())
						continue;
					gltfInfluence influence = { (uint32_t)i_BoneOfJoint[joint], weight };
					o_Vertices[v].push_back(influence);
				}
			}
		}
		if (set == 0)
		{
			o_Vertices.clear();
			return false;
		}

		for (std::vector<gltfInfluence>& influences : o_Vertices)
		{
			if (influences.empty())
			{
				gltfInfluence influence = { 0, 1 };
				influences.push_back(influence);
				continue;
			}
			float sum = 0;
			for (const gltfInfluence& influence : influences)
				sum += influence.m_Weight;
			if (fabsf(sum - 1) > 1e-3f)
			{
				o_bRenormalized = true;
				for (gltfInfluence& influence : influences)
					influence.m_Weight /= sum;
			}
		}
		return true;
	}

	bool ReadMorphDeltas(const tinygltf::Model& i_Model, const tinygltf::Primitive& i_Prim, size_t i_NumVertices,
						 std::vector<std::vector<maVector3d>>& o_Targets)
	{
		o_Targets.clear();
		for (const std::map<std::string, int>& target : i_Prim.targets)
		{
			std::vector<maVector3d> deltas(i_NumVertices);
			std::map<std::string, int>::const_iterator it = target.find("POSITION");
			if (it != target.end())
			{
				std::vector<float> values;
				if (!gltfAccessorUtil::ReadFloats(i_Model, it->second, 3, values) || values.size() != i_NumVertices * 3)
				{
					o_Targets.clear();
					return false;
				}
				for (size_t v = 0; v < i_NumVertices; v++)
					deltas[v] = maVector3d(values[v * 3], values[v * 3 + 1], values[v * 3 + 2]) * gltfMeshImport::c_UnitScale;
			}
			o_Targets.push_back(deltas);
		}
		return true;
	}

	size_t NumMorphTargets(const tinygltf::Mesh& i_Mesh)
	{
		size_t count = 0;
		for (const tinygltf::Primitive& prim : i_Mesh.primitives)
			count = std::max(count, prim.targets.size());
		return count;
	}

	std::vector<float> DefaultWeights(const tinygltf::Mesh& i_Mesh, const tinygltf::Node& i_Node)
	{
		std::vector<float> weights(NumMorphTargets(i_Mesh), 0.0f);
		const std::vector<double>& given = !i_Node.weights.empty() ? i_Node.weights : i_Mesh.weights;
		for (size_t i = 0; i < weights.size() && i < given.size(); i++)
			weights[i] = (float)given[i];
		return weights;
	}

	std::string MorphTargetKey(int i_Target)
	{
		std::ostringstream key;
		key << "#" << i_Target;
		return key.str();
	}
}
