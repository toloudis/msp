# Copyright Studio GPU. 
# primitives-test, ported from the exporter sdk

from sgpuExportLib.sgpuExportLib import *
import math

    
def create_textured_sphere ( mesh, *args, **keywords ):
    keys = keywords.keys()
    radius = keywords.get( "radius", 5.0)
    latDiv = keywords.get( "latDiv", 256 )
    longDiv = keywords.get( "longDiv", 256 )
    latAngleDelta = math.pi / float(latDiv)
    longAngleDelta =  2 * math.pi / float(longDiv)
    numRows = latDiv -1
    numColumns = longDiv
    numVerts = numRows * numColumns + 2
    mesh.SetNumVertices( numVerts )
    print "numColumns = ", numColumns
    ind = 0
    theta = - math.pi  /2 
    for rowNum in range(0, numRows):
        theta += latAngleDelta
        sin_theta = math.sin(theta)
        cos_theta = math.cos(theta)
        for colNum in range(0, numColumns):
            phi = longAngleDelta * float(colNum)
            sin_phi = math.sin(phi)
            cos_phi = math.cos(phi)
            x = cos_phi * cos_theta
            z = -sin_phi * cos_theta
            y = sin_theta
            #print ind, ": x: ", x*radius, " y: ", y*radius, " z: ", z*radius 
            mesh.SetPosition(ind, x*radius, y*radius, z*radius)
            mesh.SetNormal(ind, x, y, z)
            mesh.SetTexCoord(ind, 2 - (phi / math.pi), theta / math.pi)
            ind += 1
    #	The top point's vertex number is (num_rows * num_columns)
    #	The bottom point's vertex number is (num_rows * num_columns) + 1
    topIndex = numRows * numColumns
    botIndex = topIndex + 1
    mesh.SetPosition(topIndex, 0, radius, 0)
    mesh.SetPosition(botIndex, 0, -radius, 0)
    mesh.SetNormal(topIndex, 0,  1.0, 0)
    mesh.SetNormal(botIndex, 0, -1.0, 0)
    mesh.SetTexCoord(topIndex, 0,  0)
    mesh.SetTexCoord(botIndex, 0, 1)
    #
    # make indices for the center strips (if necessary)
    numStrips = numRows - 1
    numIndices = 6 * (numStrips+1) * numColumns
    print "numIndices = ", numIndices
    mesh.SetNumIndices(numIndices)
    ind = 0
    for rowNum in range(0,  numStrips):
        rowIndexBase = rowNum * numColumns * 6
        curIndexBase = rowIndexBase;
        for columnNum in range(0, numColumns):
            nextColumnNum = (columnNum + 1) % numColumns
            v0 = rowNum * numColumns + columnNum
            v1 = (rowNum+1) * numColumns + columnNum
            v2 = (rowNum+1) * numColumns + nextColumnNum
            v3 = rowNum * numColumns + nextColumnNum
            mesh.SetIndex(ind + 0, v0)
            mesh.SetIndex(ind + 1, v3)
            mesh.SetIndex(ind + 2, v2)
            #print  "tri ", ind/3, " v0: ", v0, " v1: ", v3, " v2 ", v2  
            mesh.SetIndex(ind + 3, v2)
            mesh.SetIndex(ind + 4, v1)
            mesh.SetIndex(ind + 5, v0)
            #print  "tri ", ind/3+1, " v0: ", v2, " v1: ", v1, " v2 ", v0 
            ind += 6
    # make the top cap
    topRowBase = ( numRows -1 ) * numColumns
    for columnNum in range(0,  numColumns):
        v0 = topIndex
        v1 = topRowBase + columnNum
        v2 = topRowBase + ((columnNum + 1) % numColumns )
        mesh.SetIndex(ind + 0, topIndex)
        mesh.SetIndex(ind + 1, v1)
        mesh.SetIndex(ind + 2, v2 )
        ind += 3
    # make the bottom cap
    botRowBase = 0
    for columnNum in range(0,  numColumns):
        v0 = botRowBase
        v2 = botRowBase + columnNum
        v1 = botRowBase + (columnNum + 1) % numColumns 
        mesh.SetIndex(ind + 0, botIndex)
        mesh.SetIndex(ind + 1, v1  )
        mesh.SetIndex(ind + 2, v2 )
        ind += 3

def create_textured_cube(mesh,  *args, **kwds  ):
    side = kwds.get( 'side', 10 )
    hs = side * 0.5
    numVertices = 24; # 6 faces, 4 vertices per face
    mesh.SetNumVertices(numVertices);
    #define each face of the cube	
    base = 0
    # front
    mesh.SetPosition(base+0, -hs, -hs, +hs)
    mesh.SetPosition(base+1, +hs, -hs, +hs)
    mesh.SetPosition(base+2, +hs, +hs, +hs)
    mesh.SetPosition(base+3, -hs, +hs, +hs)
    mesh.SetTexCoord(base+0, 0,0)
    mesh.SetTexCoord(base+1, 1,0)
    mesh.SetTexCoord(base+2, 1,1)
    mesh.SetTexCoord(base+3, 0,1)
    mesh.SetNormal(base+0, 0, 0, 1.0)
    mesh.SetNormal(base+1, 0, 0, 1.0)
    mesh.SetNormal(base+2, 0, 0, 1.0)
    mesh.SetNormal(base+3, 0, 0, 1.0)
    base += 4
    # back
    mesh.SetPosition(base+0, -hs, +hs, -hs)
    mesh.SetPosition(base+1, +hs, +hs, -hs)
    mesh.SetPosition(base+2, +hs, -hs, -hs)
    mesh.SetPosition(base+3, -hs, -hs, -hs)
    mesh.SetTexCoord(base+0, 0,0)
    mesh.SetTexCoord(base+1, 1,0)
    mesh.SetTexCoord(base+2, 1,1)
    mesh.SetTexCoord(base+3, 0,1)
    mesh.SetNormal(base+0, 0, 0, -1.0)
    mesh.SetNormal(base+1, 0, 0, -1.0)
    mesh.SetNormal(base+2, 0, 0, -1.0)
    mesh.SetNormal(base+3, 0, 0, -1.0)
    base += 4
    # left
    mesh.SetPosition(base+0, -hs, -hs, +hs)
    mesh.SetPosition(base+1, -hs, +hs, +hs)
    mesh.SetPosition(base+2, -hs, +hs, -hs)
    mesh.SetPosition(base+3, -hs, -hs, -hs)
    mesh.SetTexCoord(base+0, 0,0)
    mesh.SetTexCoord(base+1, 1,0)
    mesh.SetTexCoord(base+2, 1,1)
    mesh.SetTexCoord(base+3, 0,1)
    mesh.SetNormal(base+0, -1.0, 0, 0)
    mesh.SetNormal(base+1, -1.0, 0, 0)
    mesh.SetNormal(base+2, -1.0, 0, 0)
    mesh.SetNormal(base+3, -1.0, 0, 0)
    base += 4
    # right
    mesh.SetPosition(base+0, +hs, -hs, +hs)
    mesh.SetPosition(base+1, +hs, -hs, -hs)
    mesh.SetPosition(base+2, +hs, +hs, -hs)
    mesh.SetPosition(base+3, +hs, +hs, +hs)
    mesh.SetTexCoord(base+0, 0,0)
    mesh.SetTexCoord(base+1, 1,0)
    mesh.SetTexCoord(base+2, 1,1)
    mesh.SetTexCoord(base+3, 0,1)
    mesh.SetNormal(base+0, 1.0, 0, 0)
    mesh.SetNormal(base+1, 1.0, 0, 0)
    mesh.SetNormal(base+2, 1.0, 0, 0)
    mesh.SetNormal(base+3, 1.0, 0, 0)
    base += 4
    # top
    mesh.SetPosition(base+0, -hs, +hs, +hs)
    mesh.SetPosition(base+1, +hs, +hs, +hs)
    mesh.SetPosition(base+2, +hs, +hs, -hs)
    mesh.SetPosition(base+3, -hs, +hs, -hs)
    mesh.SetTexCoord(base+0, 0,0)
    mesh.SetTexCoord(base+1, 1,0)
    mesh.SetTexCoord(base+2, 1,1)
    mesh.SetTexCoord(base+3, 0,1)
    mesh.SetNormal(base+0, 0, 1.0, 0)
    mesh.SetNormal(base+1, 0, 1.0, 0)
    mesh.SetNormal(base+2, 0, 1.0, 0)
    mesh.SetNormal(base+3, 0, 1.0, 0)
    base += 4;
    # bottom
    mesh.SetPosition(base+0, -hs, -hs, +hs)
    mesh.SetPosition(base+1, -hs, -hs, -hs)
    mesh.SetPosition(base+2, +hs, -hs, -hs)
    mesh.SetPosition(base+3, +hs, -hs, +hs)
    mesh.SetTexCoord(base+0, 0,0)
    mesh.SetTexCoord(base+1, 1,0)
    mesh.SetTexCoord(base+2, 1,1)
    mesh.SetTexCoord(base+3, 0,1)
    mesh.SetNormal(base+0, 0, -1.0, 0)
    mesh.SetNormal(base+1, 0, -1.0, 0)
    mesh.SetNormal(base+2, 0, -1.0, 0)
    mesh.SetNormal(base+3, 0, -1.0, 0)
    #indices for the top and bottom
    numIndices = 36; # 6 quad faces = 12 triangles = 36 indices (3 per triangles)
    mesh.SetNumIndices(numIndices);
    ind = 0;
    for face in range(0,  6):
        curVertexBase = face * 4
        mesh.SetIndex(ind + 0, curVertexBase)
        mesh.SetIndex(ind + 1, curVertexBase + 1)
        mesh.SetIndex(ind + 2, curVertexBase + 2)

        mesh.SetIndex(ind + 3, curVertexBase + 2)
        mesh.SetIndex(ind + 4, curVertexBase + 3)
        mesh.SetIndex(ind + 5, curVertexBase)
        ind += 6
            
    
def main():
    scene = sgpuScene()
    root_node = scene.GetRootNode()
    root_node.SetNodeName("RootNode")
    sphere_mat = scene.CreateMaterial( "sphere_mat" )
    sphere_mat.SetDiffuseColor(0.2, 0.4, 0.8);
    sphere_mat.SetSpecularColor(0.8,0.8,0.85);
    sphere_mat.SetShininess(50);
    sphere_node = root_node.AddChildNode()
    sphere_node.SetNodeName( "Sphere" )
    sphere_mesh = sphere_node.CreateTriangleMesh()
    sphere_mesh.SetMeshName( "Sphere_Shape" )
    create_textured_sphere( sphere_mesh, latDiv=256, longDiv=256)
    sphere_mesh.AssignMaterial( sphere_mat )
    cube_root = root_node.AddChildNode()
    cube_root.SetNodeName("CubeRoot")
    cube_trans = sgpuMatrix()
    cube_trans.MakeTranslate(-12.0,0,0)
    cube_root.SetTransformationMatrix(cube_trans)
    #Cube #1
    cube1_node = cube_root.AddChildNode();
    cube1_node.SetNodeName("Cube1")
    cube1_mesh = cube1_node.CreateTriangleMesh()
    cube1_mesh.SetMeshName( "Cube1_Shape" )
    create_textured_cube( cube1_mesh, side=8 )
    cube1_mat = scene.CreateMaterial( "Cube1Mat" )
    #Relative path from .gxb location. Relative paths
    #need to begin with ".". An absolute path would also work.
    cube1_mat.SetDiffuseTexture( ".\\Textures\\Leather_Seat_Center.dds" )
    cube1_mat.SetDiffuseColor(1,1,1) # good to use pure white color when using a texture
    cube1_mesh.AssignMaterial( cube1_mat )
    #Cube #2
    cube2_node = cube_root.AddChildNode()
    cube2_node.SetNodeName( "Cube2" )
    cube2_trans = sgpuMatrix()
    cube2_rot = sgpuMatrix()
    cube2_trans.MakeTranslate(0,8.0,0);
    cube2_rot.MakeRotate( math.pi/4, 1,1,0);
    cube2_node.SetTransformationMatrix(cube2_rot * cube2_trans)
    cube2_mesh = cube2_node.CreateTriangleMesh();
    cube2_mesh.SetMeshName( "Cube2_Shape" )
    create_textured_cube(cube2_mesh, side = 2.0)
    cube2_mat = scene.CreateMaterial( "Cube2Mat" )
    cube2_mat.SetDiffuseColor(0.8, 0.2, 0.1)
    cube2_mesh.AssignMaterial( cube2_mat )
    desc = scene.Describe()
    print desc
    writeRes = scene.WriteScene(r'..\..\data\shape_test.gxb', 'Primitives Test v1.0')
    if( writeRes ):
        print ' written succesfully '
    return 0


def run(args = None):
    if args is not None:
        import sys
        sys.argv = args
    import doctest, testSgpuExportLib
    return doctest.testmod(testSgpuExportLib, verbose=True)

if __name__ == '__main__':
    import sys
    sys.exit(main())

