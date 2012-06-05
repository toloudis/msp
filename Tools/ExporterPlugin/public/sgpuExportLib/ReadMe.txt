sgpuExportLib Version 1.3.0.0
** Studio GPU
**	Copyright(C) 2009 - All Rights Reserved

This SDK provides includes and DLL for exporting static geometry files in 
the .GXB file format used by Studio GPU applications.

Directories:
 bin - contains DLL
 data - sample program generates .gxb example file here
 data/Textures - contains textures referred to by the sample program
 include - header files for SDK
 obj - temporary directory for sample programs
 samples - example program using SDK generates a .gxb example file

Using the SDK:

Exporting a .GXB file involves constructing an sgpuModelExportScene that contains nodes organized 
into a scene graph. Nodes can contain indexed triangle meshes or subdivision meshes 
or references to other nodes. The sgpuModelExportScene contains a table of uniquely named phong materials 
that can be created and assigned to the meshes. Once the sgpuModelExportScene is constructed, 
it can be written to a file.

Many of the classes in the SDK are handles to internal memory and cannot be 
constructed on their own.  You start by creating an instance of sgpuModelExportScene and then
use that scene to create the objects you need.

A right handed co-ordinate system with Y-up axis i assumed.

Building the scene graph:
From the sgpuModelExportScene instance, you call GetRootNode() to get a handle to the root node
of the scene graph. With that handle you can build the hierarchy by calling AddChildNode()
on that spuNode root node handle and then on the sgpuNodes returned by that call. You can
set the transformations on the nodes with the class sgpuMatrix and the function 
SetTransformationMatrix() on sgpuNode.

Creating triangle meshes:
Each sgpuNode may contain a single mesh, the mesh is created using the call 
CreateNodeContent_TriangleMesh() on the sgpuNode handle. With the sgpuMesh handle that is returned,
you can fill in the vertex and index information for the mesh. Each mesh contains a set of triangular faces. 
The corner of each face are made up of vertices which is a tuple of (position, normal, texture-coordinate).
Since the identity of a vertex is determined the three tuple of (position, normal and texture-coordinate),
it is quite possible that a vertex in the sgpuMesh is different from that in the original 
source mesh (Max/Maya). As an example, consider a cube in the source mesh, which is made up of 
8 positions, 6 normals and 4 texture co-ordinates and 12 triangles. When translated to sgpuMesh, 
this would result in 24 vertices. To help in constructing an sgpuMesh, 
a constructor called sgpuMeshConstructor can be used. sgpuMeshConstructor in the above case of a cube,
will accept 8 positions, 6 normals, and 4 UV-s and accepts 12 faces along with FaceVert-ices, 
A FaceVertex specifies which position, normal and UV will be used for that corner of a face.



Creating subdivision surface:
Each sgpuNode can alternatively, contain, a single sgpuSubdiv 
or the base mesh of a Catmul-Clark subdivision mesh. The base mesh is specified exactly like 
in the case of an sgpuMesh. A helper class called sgpuSubdivConstructor can be used to specify
the base subdiv mesh.

Creating a node reference:
Each node can alternatively contain a reference to another node. This can be used to represent instancing.
For eg: if node A and node B are instances of each other, (ie they share the same geometry and materials,
but have different transformations), and if node B comes after node A in the scene, then node B can contain 
a node reference to node A, which enables the sharing of the geometry between A and B. 
node_B.Create_PathReference( root_node, node_A); will create a path reference to node_A under node_B,
which ensures that the geometry of node-A will be shared by node B. Ypou can set a different transformation to node B,
which results in instancing.


Creating and assigning materials:
The scene contains uniquely named materials that can be shared between meshes. You create
a new material by calling CreateMaterial(material_name) on the sgpuScene instance. A triangular mesh 
or a subdivision mesh can have multiple materials. THe faces of a triangular mesh or a subdivision mesh 
should be sorted by the material assignment. For an sgpuMesh, at the beginning of a set of faces with 
the same material, one should specify sgpuMsh::SetMaterialChange( int i_FaceIndex, sgpuMaterial &i_Material);
The same goes for an sgpuSubdiv also. Again the requirement that an sgpuMesh should have its faces sorted,
with meterials, can be hidden if you use the helper constrctor classes.


Referencing textures:
The sgpuMaterial class contains functions to set a simple phong shader with an optional single 
texture. Textures can be referenced with either an absolute path, or a relative path from the
geometry file. However, relative paths must begin with a "." to signify that the paths are 
relative (i.e. ".\Textures\Wood.dds"). If a single filename is given without path, it will be 
assumed to be in the same directory as the geometry file.

Naming:
The nodes, meshes and materials should all be uniquely named.  





