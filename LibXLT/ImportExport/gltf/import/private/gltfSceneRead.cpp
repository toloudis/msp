/****************************************************************************\
**  gltfSceneRead.cpp
**
**      see gltfSceneRead.hpp
**
**	The rule: which objects a glTF node tree becomes
**
**	- A node with children becomes an e_Group (identity: its path) carrying
**	  the node's transform; its own mesh, light and camera become child
**	  objects of that group with identity transforms, so the children keep
**	  following it.
**	- A node without children becomes one object per content it has
**	  (mesh, light, camera), each with the node's transform; a node with
**	  nothing is skipped.
**	- A mesh node with a skin is an e_SkinnedMesh. Nodes listed in any
**	  skin's joints are not objects: their non-joint descendants with
**	  content attach to the nearest group above, their transforms composed
**	  through the joints (one warning per such node).
**	- Identity: path segments from the scene root joined with '/'. A
**	  segment is the node's name, or "#<node index>" when the name is empty
**	  or a sibling (same parent, or another scene root) has the same name.
**	  Content objects append ":mesh", ":light" or ":camera" to their node's
**	  path ("Car/Body:mesh"); groups have the bare path. The identity is
**	  kept apart from any file path.
**	- Transforms: TRS as given (translation x 100: metres to centimetres),
**	  or a matrix decomposed (translation x 100, scale = the basis vectors'
**	  lengths, rotation from the normalized basis).
**
**	Light and camera values
**
**	- Point and spot intensity stays in candela, with a quadratic falloff
**	  c_QuadraticFalloff = 1e-4 (1 / (100 cm per m)^2) and no constant or
**	  linear term, so MachStudio's attenuation 1 / (q * d^2), d in cm,
**	  gives cd / d^2 with d in metres: what glTF specifies and the Khronos
**	  sample viewer draws (Lighting.hlsli's attenuation). MachStudio's
**	  default falloff is constant, (1, 0, 0).
**	- Directional intensity is lux, used as is, with a constant falloff.
**	- Range: metres to centimetres; 0 (absent) is no limit.
**	- Spot angles: glTF gives half-angles in radians; the projected light
**	  takes the full inner cone (m_Angle) and the full outer cone minus
**	  the inner (m_Penumbra) in degrees (prjltProjectedLightObject sets
**	  the light's inner angle to m_Angle and its outer to m_Angle +
**	  m_Penumbra), so m_Angle = 2 * inner and m_Penumbra = 2 * (outer -
**	  inner).
**	- Files exported with physical units (Blender's default, about 683 lm
**	  per W) come out as bright as the sample viewer shows them; that is
**	  expected.
**	- Cameras: yfov radians to degrees; clip planes and orthographic
**	  half-extents metres to centimetres; aspect and zfar 0 when absent.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SCENEREAD_HPP
#include "ImportExport/gltf/import/private/gltfSceneRead.hpp"
#endif
#ifndef GLTF_SDKMANAGER_HPP
#include "ImportExport/gltf/import/private/gltfSdkManager.hpp"
#endif
#ifndef GLTF_MESHIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/Fs/fsFileUtil.hpp"
#endif
#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#ifndef MA_MATRIX3X3_HPP
#include "Core/ma/maMatrix3x3.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#include <cmath>
#include <set>
#include <sstream>


namespace
{
	struct Transform
	{
		maVector3d m_Translation;
		maRotation m_Rotation;
		maVector3d m_Scale = maVector3d(1, 1, 1);
	};

	//--------------------------------------------------------------------
	// the node's local matrix (row vectors, as ours; centimetres)
	//--------------------------------------------------------------------
	maMatrix4x4 node_matrix(const tinygltf::Node& i_Node)
	{
		maMatrix4x4 m;
		if (i_Node.matrix.size() == 16)
		{
			// glTF matrices are column-major with column vectors, which has the
			// same memory layout as our row-major, row-vector matrices
			for (int i = 0; i < 16; ++i)
				m.Ptr()[i] = (float)i_Node.matrix[i];
			for (int i = 12; i < 15; ++i)
				m.Ptr()[i] *= gltfMeshImport::c_UnitScale;
			return m;
		}

		maVector3d scale(1, 1, 1), trans;
		maRotation rot;
		if (i_Node.scale.size() == 3)
			scale = maVector3d((float)i_Node.scale[0], (float)i_Node.scale[1], (float)i_Node.scale[2]);
		if (i_Node.rotation.size() == 4)
			rot = maRotation((float)i_Node.rotation[0], (float)i_Node.rotation[1], (float)i_Node.rotation[2], (float)i_Node.rotation[3]);
		if (i_Node.translation.size() == 3)
			trans = maVector3d((float)i_Node.translation[0], (float)i_Node.translation[1], (float)i_Node.translation[2]) * gltfMeshImport::c_UnitScale;

		m.MakeScale(scale.m_X, scale.m_Y, scale.m_Z);
		m *= rot.GetMatrix();
		m.TranslateBy(trans.m_X, trans.m_Y, trans.m_Z);
		return m;
	}

	//--------------------------------------------------------------------
	// translation, rotation and scale of a matrix: scale is the basis
	// vectors' lengths (one negated for a mirror), rotation the
	// normalized basis
	//--------------------------------------------------------------------
	Transform decompose(const maMatrix4x4& i_Matrix)
	{
		Transform t;
		const float* p = i_Matrix.GetPtr();
		t.m_Translation = maVector3d(p[12], p[13], p[14]);

		maVector3d rows[3];
		float scale[3];
		for (int r = 0; r < 3; r++)
		{
			rows[r] = maVector3d(p[r * 4], p[r * 4 + 1], p[r * 4 + 2]);
			scale[r] = rows[r].Length();
		}
		const float det = rows[0].m_X * (rows[1].m_Y * rows[2].m_Z - rows[1].m_Z * rows[2].m_Y)
						- rows[0].m_Y * (rows[1].m_X * rows[2].m_Z - rows[1].m_Z * rows[2].m_X)
						+ rows[0].m_Z * (rows[1].m_X * rows[2].m_Y - rows[1].m_Y * rows[2].m_X);
		if (det < 0)
			scale[0] = -scale[0];

		maMatrix3x3 rot;
		for (int r = 0; r < 3; r++)
		{
			const maVector3d axis = (scale[r] != 0) ? rows[r] * (1.0f / scale[r]) : maVector3d();
			rot(r, 0) = axis.m_X;
			rot(r, 1) = axis.m_Y;
			rot(r, 2) = axis.m_Z;
		}
		t.m_Rotation.SetValue(rot);
		t.m_Scale = maVector3d(scale[0], scale[1], scale[2]);
		return t;
	}

	//--------------------------------------------------------------------
	// the node's transform: TRS as given, else its matrix decomposed
	//--------------------------------------------------------------------
	Transform node_transform(const tinygltf::Node& i_Node)
	{
		if (i_Node.matrix.size() == 16)
			return decompose(node_matrix(i_Node));

		Transform t;
		if (i_Node.scale.size() == 3)
			t.m_Scale = maVector3d((float)i_Node.scale[0], (float)i_Node.scale[1], (float)i_Node.scale[2]);
		if (i_Node.rotation.size() == 4)
			t.m_Rotation = maRotation((float)i_Node.rotation[0], (float)i_Node.rotation[1], (float)i_Node.rotation[2], (float)i_Node.rotation[3]);
		if (i_Node.translation.size() == 3)
			t.m_Translation = maVector3d((float)i_Node.translation[0], (float)i_Node.translation[1], (float)i_Node.translation[2]) * gltfMeshImport::c_UnitScale;
		return t;
	}

	//--------------------------------------------------------------------
	// a KHR_lights_punctual light in MachStudio's units (see the top)
	//--------------------------------------------------------------------
	gltfLightDesc read_light(const tinygltf::Light& i_Light)
	{
		gltfLightDesc d;
		if (i_Light.type == "spot")
			d.m_Type = gltfLightDesc::e_Spot;
		else if (i_Light.type == "directional")
			d.m_Type = gltfLightDesc::e_Directional;
		else if (i_Light.type != "point")
			DBG_WARNING("glTF light " << i_Light.name << " has unknown type " << i_Light.type << "; read as a point light");

		if (i_Light.color.size() == 3)
		{
			for (int i = 0; i < 3; i++)
				d.m_Color[i] = (float)i_Light.color[i];
		}
		d.m_Intensity = (float)i_Light.intensity;

		if (d.m_Type == gltfLightDesc::e_Directional)
		{
			d.m_Falloff = maPoint3d(1, 0, 0);
		}
		else
		{
			d.m_Falloff = maPoint3d(0, 0, gltfSceneRead::c_QuadraticFalloff);
			d.m_Range = (float)i_Light.range * gltfMeshImport::c_UnitScale;
		}

		if (d.m_Type == gltfLightDesc::e_Spot)
		{
			const float inner = (float)i_Light.spot.innerConeAngle, outer = (float)i_Light.spot.outerConeAngle;
			d.m_Angle = 2 * inner * maConstants::c_fRadToAngle;
			d.m_Penumbra = 2 * (outer - inner) * maConstants::c_fRadToAngle;
		}
		return d;
	}

	//--------------------------------------------------------------------
	// a glTF camera in MachStudio's units (see the top)
	//--------------------------------------------------------------------
	gltfCameraDesc read_camera(const tinygltf::Camera& i_Camera)
	{
		gltfCameraDesc d;
		if (i_Camera.type == "orthographic")
		{
			d.m_bOrthographic = true;
			d.m_XMag = (float)i_Camera.orthographic.xmag * gltfMeshImport::c_UnitScale;
			d.m_YMag = (float)i_Camera.orthographic.ymag * gltfMeshImport::c_UnitScale;
			d.m_Near = (float)i_Camera.orthographic.znear * gltfMeshImport::c_UnitScale;
			d.m_Far = (float)i_Camera.orthographic.zfar * gltfMeshImport::c_UnitScale;
		}
		else
		{
			d.m_YFov = (float)i_Camera.perspective.yfov * maConstants::c_fRadToAngle;
			d.m_Aspect = (float)i_Camera.perspective.aspectRatio;
			d.m_Near = (float)i_Camera.perspective.znear * gltfMeshImport::c_UnitScale;
			d.m_Far = (float)i_Camera.perspective.zfar * gltfMeshImport::c_UnitScale;
		}
		return d;
	}

	bool is_uniform(const maVector3d& i_Scale)
	{
		const float c_Tolerance = 1e-4f;
		return fabsf(i_Scale.m_X - i_Scale.m_Y) <= c_Tolerance
			&& fabsf(i_Scale.m_Y - i_Scale.m_Z) <= c_Tolerance
			&& fabsf(i_Scale.m_X - i_Scale.m_Z) <= c_Tolerance;
	}

	//--------------------------------------------------------------------
	// SceneReader - walks one scene's node tree
	//--------------------------------------------------------------------
	class SceneReader
	{
	public:
		SceneReader(const tinygltf::Model& i_Model, std::vector<gltfSceneObject>& o_Objects)
			: m_Model(i_Model), m_Objects(o_Objects)
		{
			for (const tinygltf::Skin& skin : m_Model.skins)
				m_Joints.insert(skin.joints.begin(), skin.joints.end());
		}

		void ReadSiblings(const std::vector<int>& i_Nodes, const std::string& i_ParentPath, int i_Parent,
						  const maMatrix4x4* i_pJointMatrix)
		{
			for (int node : i_Nodes)
			{
				if (!IsNode(node))
					continue;
				std::string path = Segment(node, i_Nodes);
				if (!i_ParentPath.empty())
					path = i_ParentPath + "/" + path;
				ReadNode(node, path, i_Parent, i_pJointMatrix);
			}
		}

	private:
		bool IsNode(int i_Node) const
		{
			return i_Node >= 0 && i_Node < (int)m_Model.nodes.size();
		}

		//----------------------------------------------------------------
		// the node's path segment: its name, unless empty or shared by a sibling
		//----------------------------------------------------------------
		std::string Segment(int i_Node, const std::vector<int>& i_Siblings) const
		{
			const std::string& name = m_Model.nodes[i_Node].name;
			bool bUnique = !name.empty();
			for (int sibling : i_Siblings)
			{
				if (bUnique && sibling != i_Node && IsNode(sibling) && m_Model.nodes[sibling].name == name)
					bUnique = false;
			}
			if (bUnique)
				return name;

			std::ostringstream segment;
			segment << "#" << i_Node;
			return segment.str();
		}

		std::string ObjectName(int i_Node, const std::string& i_ContentName) const
		{
			const std::string& name = m_Model.nodes[i_Node].name;
			if (!name.empty())
				return name;
			if (!i_ContentName.empty())
				return i_ContentName;

			std::ostringstream fallback;
			fallback << "Node_" << i_Node;
			return fallback.str();
		}

		int Add(gltfSceneObject::Kind i_Kind, int i_Node, const std::string& i_Identity,
				const std::string& i_ContentName, int i_Parent, const Transform& i_Transform)
		{
			gltfSceneObject object;
			object.m_Kind = i_Kind;
			object.m_Identity = i_Identity;
			object.m_Name = ObjectName(i_Node, i_ContentName);
			object.m_Node = i_Node;
			object.m_Parent = i_Parent;
			object.m_Translation = i_Transform.m_Translation;
			object.m_Rotation = i_Transform.m_Rotation;
			object.m_Scale = i_Transform.m_Scale;
			object.m_bUniformScale = is_uniform(i_Transform.m_Scale);
			m_Objects.push_back(object);
			return (int)m_Objects.size() - 1;
		}

		//----------------------------------------------------------------
		// one object per content the node has, all with i_Transform
		//----------------------------------------------------------------
		void AddContent(int i_Node, const std::string& i_Path, int i_Parent, const Transform& i_Transform)
		{
			const tinygltf::Node& node = m_Model.nodes[i_Node];
			if (node.mesh >= 0 && node.mesh < (int)m_Model.meshes.size())
			{
				const bool bSkinned = node.skin >= 0 && node.skin < (int)m_Model.skins.size();
				const int index = Add(bSkinned ? gltfSceneObject::e_SkinnedMesh : gltfSceneObject::e_Mesh, i_Node,
									  i_Path + ":mesh", m_Model.meshes[node.mesh].name, i_Parent, i_Transform);
				m_Objects[index].m_Mesh = node.mesh;
				m_Objects[index].m_Skin = bSkinned ? node.skin : -1;
			}
			if (node.light >= 0 && node.light < (int)m_Model.lights.size())
			{
				const int index = Add(gltfSceneObject::e_Light, i_Node, i_Path + ":light",
									  m_Model.lights[node.light].name, i_Parent, i_Transform);
				m_Objects[index].m_Light = node.light;
				m_Objects[index].m_LightDesc = read_light(m_Model.lights[node.light]);
			}
			if (node.camera >= 0 && node.camera < (int)m_Model.cameras.size())
			{
				const int index = Add(gltfSceneObject::e_Camera, i_Node, i_Path + ":camera",
									  m_Model.cameras[node.camera].name, i_Parent, i_Transform);
				m_Objects[index].m_Camera = node.camera;
				m_Objects[index].m_CameraDesc = read_camera(m_Model.cameras[node.camera]);
			}
		}

		bool HasContent(const tinygltf::Node& i_Node) const
		{
			return (i_Node.mesh >= 0 && i_Node.mesh < (int)m_Model.meshes.size())
				|| (i_Node.light >= 0 && i_Node.light < (int)m_Model.lights.size())
				|| (i_Node.camera >= 0 && i_Node.camera < (int)m_Model.cameras.size());
		}

		bool HasChildren(const tinygltf::Node& i_Node) const
		{
			for (int child : i_Node.children)
			{
				if (IsNode(child))
					return true;
			}
			return false;
		}

		//----------------------------------------------------------------
		// i_pJointMatrix: the skipped joints' transforms between this node
		// and its object parent, or NULL
		//----------------------------------------------------------------
		void ReadNode(int i_Node, const std::string& i_Path, int i_Parent, const maMatrix4x4* i_pJointMatrix)
		{
			// guard against cycles in a malformed file
			if (!m_Visiting.insert(i_Node).second)
			{
				DBG_WARNING("glTF node " << i_Node << " is its own ancestor; not read again");
				return;
			}

			const tinygltf::Node& node = m_Model.nodes[i_Node];
			if (m_Joints.count(i_Node))
			{
				// not an object: pass its transform down to the objects below it
				maMatrix4x4 joint = node_matrix(node);
				if (i_pJointMatrix)
					joint = joint * *i_pJointMatrix;
				ReadSiblings(node.children, i_Path, i_Parent, &joint);
			}
			else
			{
				Transform transform;
				if (i_pJointMatrix)
				{
					DBG_WARNING("glTF node " << i_Path << " is below a skin joint; it is placed with the joint's transform at the time of import");
					transform = decompose(node_matrix(node) * *i_pJointMatrix);
				}
				else
				{
					transform = node_transform(node);
				}

				if (HasChildren(node))
				{
					const int group = Add(gltfSceneObject::e_Group, i_Node, i_Path, "", i_Parent, transform);
					AddContent(i_Node, i_Path, group, Transform());
					ReadSiblings(node.children, i_Path, group, NULL);
				}
				else if (HasContent(node))
				{
					AddContent(i_Node, i_Path, i_Parent, transform);
				}
			}

			m_Visiting.erase(i_Node);
		}

		const tinygltf::Model& m_Model;
		std::vector<gltfSceneObject>& m_Objects;
		std::set<int> m_Joints;
		std::set<int> m_Visiting;
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfSceneRead
{
	std::vector<gltfSceneObject> ReadScene(const tinygltf::Model& i_Model)
	{
		std::vector<gltfSceneObject> objects;
		if (i_Model.scenes.empty())
			return objects;

		int scene = i_Model.defaultScene;
		if (scene < 0 || scene >= (int)i_Model.scenes.size())
			scene = 0;

		SceneReader reader(i_Model, objects);
		reader.ReadSiblings(i_Model.scenes[scene].nodes, "", -1, NULL);
		return objects;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfSceneFile
{
	bool ReadObjects(const fsLocator& i_File, std::vector<gltfSceneObject>& o_Objects)
	{
		o_Objects.clear();
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_File, filename);
		std::shared_ptr<const tinygltf::Model> pModel = gltfSdkManager::LoadSceneShared(filename.c_str());
		if (!pModel)
			return false;
		o_Objects = gltfSceneRead::ReadScene(*pModel);
		return true;
	}
}
