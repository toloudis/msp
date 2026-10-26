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
**	- A skinned mesh is placed at its skeleton, not at its node: glTF
**	  ignores a skinned mesh node's transform (the vertices follow the
**	  joints' world transforms). Its character holds the skeleton root R
**	  (gltfSkeletonRead) with R's local transform, so the object sits at
**	  R's parent B: a child of B's group with an identity transform, or at
**	  the top with identity when R is a scene root. When B is a joint of
**	  another skin (no object), it is a child of the nearest group above B
**	  with B's transform relative to it (one warning). Skinned meshes are
**	  appended after the walk, so their parent groups precede them. When
**	  the skeleton can't be read, the mesh is placed as a plain mesh node
**	  is (and comes in static).
**	- What each object's animations become, and when they play, is
**	  gltfAnimTracks::Decide's (gltfAnimTracks.cpp). A transform-clip mesh
**	  (its transform animation can't be keys) sits at its parent with an
**	  identity transform; its template carries the node's transform.
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
#ifndef GLTF_NODETRANSFORM_HPP
#include "ImportExport/gltf/import/private/gltfNodeTransform.hpp"
#endif
#ifndef GLTF_ANIMREAD_HPP
#include "ImportExport/gltf/import/private/gltfAnimRead.hpp"
#endif
#ifndef GLTF_ANIMTRACKS_HPP
#include "ImportExport/gltf/import/private/gltfAnimTracks.hpp"
#endif
#ifndef GLTF_SKELETONREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkeletonRead.hpp"
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
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <sstream>


namespace
{
	using gltfNodeTransform::Transform;

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
			std::map<std::string, int> named_nodes;
			for (int node : i_Nodes)
			{
				if (!IsNode(node))
					continue;
				const auto inserted = named_nodes.emplace(m_Model.nodes[node].name, node);
				if (!inserted.second && inserted.first->second != node)
					inserted.first->second = -1;
			}
			for (int node : i_Nodes)
			{
				if (!IsNode(node))
					continue;
				std::string path = Segment(node, named_nodes);
				if (!i_ParentPath.empty())
					path = i_ParentPath + "/" + path;
				ReadNode(node, path, i_Parent, i_pJointMatrix);
			}
		}

		//----------------------------------------------------------------
		// the skinned meshes, at their skeleton root's parent (see the top)
		//----------------------------------------------------------------
		void AddSkinnedMeshes()
		{
			for (const SkinnedMesh& mesh : m_SkinnedMeshes)
			{
				int parent = -1;
				Transform transform;
				if (mesh.m_Base >= 0)
				{
					std::map<int, Placement>::const_iterator it = m_Placements.find(mesh.m_Base);
					if (it == m_Placements.end())
					{
						DBG_WARNING("glTF skinned mesh " << mesh.m_Path << ": its skeleton's parent node isn't an object; placed at the top");
					}
					else
					{
						parent = it->second.m_Object;
						if (it->second.m_bBelowJoint)
						{
							DBG_WARNING("glTF skinned mesh " << mesh.m_Path << ": its skeleton's parent is a joint; placed with the joint's transform at the time of import");
							transform = gltfNodeTransform::Decompose(it->second.m_Matrix);
						}
					}
				}
				const int index = Add(gltfSceneObject::e_SkinnedMesh, mesh.m_Node, mesh.m_Path + ":mesh",
									  m_Model.meshes[m_Model.nodes[mesh.m_Node].mesh].name, parent, transform);
				m_Objects[index].m_Mesh = m_Model.nodes[mesh.m_Node].mesh;
				m_Objects[index].m_Skin = m_Model.nodes[mesh.m_Node].skin;
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
		std::string Segment(int i_Node, const std::map<std::string, int>& i_NamedNodes) const
		{
			const std::string& name = m_Model.nodes[i_Node].name;
			if (!name.empty() && i_NamedNodes.at(name) == i_Node)
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
				gltfSkeleton skeleton;
				if (bSkinned && gltfSkeletonRead::Read(m_Model, node.skin, skeleton))
				{
					SkinnedMesh mesh;
					mesh.m_Node = i_Node;
					mesh.m_Path = i_Path;
					mesh.m_Base = skeleton.m_Base;
					m_SkinnedMeshes.push_back(mesh);
				}
				else
				{
					const int index = Add(bSkinned ? gltfSceneObject::e_SkinnedMesh : gltfSceneObject::e_Mesh, i_Node,
										  i_Path + ":mesh", m_Model.meshes[node.mesh].name, i_Parent, i_Transform);
					m_Objects[index].m_Mesh = node.mesh;
					m_Objects[index].m_Skin = bSkinned ? node.skin : -1;
				}
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
				maMatrix4x4 joint = gltfNodeTransform::NodeMatrix(node);
				if (i_pJointMatrix)
					joint = joint * *i_pJointMatrix;
				m_Placements[i_Node] = Placement{ i_Parent, true, joint };
				ReadSiblings(node.children, i_Path, i_Parent, &joint);
			}
			else
			{
				Transform transform;
				if (i_pJointMatrix)
				{
					DBG_WARNING("glTF node " << i_Path << " is below a skin joint; it is placed with the joint's transform at the time of import");
					transform = gltfNodeTransform::Decompose(gltfNodeTransform::NodeMatrix(node) * *i_pJointMatrix);
				}
				else
				{
					transform = gltfNodeTransform::NodeTransform(node);
				}

				if (HasChildren(node))
				{
					const int group = Add(gltfSceneObject::e_Group, i_Node, i_Path, "", i_Parent, transform);
					m_Placements[i_Node] = Placement{ group, false, maMatrix4x4() };
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

		// where an object below a node goes: a group's object, or a joint's
		// nearest group with the joints' transforms between them
		struct Placement
		{
			int m_Object;
			bool m_bBelowJoint;
			maMatrix4x4 m_Matrix;
		};
		struct SkinnedMesh
		{
			int m_Node;
			std::string m_Path;
			int m_Base;				// the skeleton root's parent, -1 at the top
		};

		const tinygltf::Model& m_Model;
		std::vector<gltfSceneObject>& m_Objects;
		std::set<int> m_Joints;
		std::set<int> m_Visiting;
		std::map<int, Placement> m_Placements;
		std::vector<SkinnedMesh> m_SkinnedMeshes;
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
		reader.AddSkinnedMeshes();

		// the materials each mesh uses
		for (gltfSceneObject& object : objects)
		{
			if (object.m_Mesh < 0 || object.m_Mesh >= (int)i_Model.meshes.size())
				continue;
			for (const tinygltf::Primitive& primitive : i_Model.meshes[object.m_Mesh].primitives)
			{
				if (primitive.material >= 0)
					object.m_Materials.push_back(primitive.material);
			}
			std::sort(object.m_Materials.begin(), object.m_Materials.end());
			object.m_Materials.erase(std::unique(object.m_Materials.begin(), object.m_Materials.end()), object.m_Materials.end());
		}

		// keys and clips; a transform-clip mesh sits at its parent
		gltfAnimTracks::Decide(i_Model, objects, gltfAnimTracks::c_SampleRate);
		for (gltfSceneObject& object : objects)
		{
			if (!object.m_bTransformClip)
				continue;
			object.m_Translation = maVector3d();
			object.m_Rotation = maRotation();
			object.m_Scale = maVector3d(1, 1, 1);
			object.m_bUniformScale = true;
		}
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
		o_Objects = *gltfSdkManager::ReadSceneObjectsShared(pModel);
		return true;
	}
}
