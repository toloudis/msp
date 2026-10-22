import mach

#print all materials for each selected object
def listObjectMaterials():
    for obj in mach.getSelection():
        mtrlList = mach.getAllMaterials(obj)
        printList( obj, mtrlList, "Materials" )

#print all materials for each selected object
def listObjectSurfaces():
    for obj in mach.getSelection():
        fgmtList = mach.getAllSurfaces(obj)
        printList( obj, fgmtList, "Surfaces" )


        
#print all materials for each selected object
def printList( i_Object, curList, listType ):
    print "*** " + listType + " for object: " + i_Object + " ***"
    for value in curList:
        print value

#list an object's texture files for each material
def listMaterialTextures():
    for obj in mach.getSelection():
        mtrlList = mach.getAllMaterials(obj)
        print "Model: " + str(mach.getValue(obj, "filename"))
        printTextureList(obj, mtrlList)

#formats the output of the texture list
def printTextureList( i_Object, MatList):
    for mtrl in MatList:
        print "   Material: " + mtrl
        PropList = mach.getMaterialProperties(i_Object, mtrl)
        for prop in PropList:
            if prop.find("Texture") != -1:
                printTextureValue(i_Object, mtrl, prop)

#gets the texture file name and prints it out
def printTextureValue(i_Object, i_Mtrl, i_Prop):
    texValue = str(mach.getMaterialValue(i_Object, i_Mtrl, i_Prop))
    if texValue != "":
        print "\t" + i_Prop + ": " + texValue

mach.createCommand("List Material Textures", listMaterialTextures)
