import mach
import os

#---------------------------------------
# uses mach util for clear selection to allow for hot key of command
#---------------------------------------
def clearSelected():
    mach.clearSelection()

'''
This function will toggle the manipMode property of all lights that are currently
selected
'''            
def toggleManip():
    for obj in mach.getSelection():
        objectType = mach.getType(obj)
        #only projected lights have the manipMode property
        if objectType == "Projected Lights":
            togglePjltManip(obj)
        if objectType == "Cameras":
            toggleCamManip(obj)

def togglePjltManip(i_Object):
    if mach.getValue(i_Object, "manipMode") == "Light":
        mach.setValue(i_Object, "manipMode", "Target")
    elif mach.getValue(i_Object, "manipMode") == "Target":
        mach.setValue(i_Object, "manipMode", "Together")
    else:
        mach.setValue(i_Object, "manipMode", "Light")

def toggleCamManip(i_Object):
    if mach.getValue(i_Object, "manipMode") == "Eye":
        mach.setValue(i_Object, "manipMode", "Target")
    elif mach.getValue(i_Object, "manipMode") == "Target":
        mach.setValue(i_Object, "manipMode", "Together")
    else:
        mach.setValue(i_Object, "manipMode", "Eye")


#------------------------------------------------------------------------------
# toggleOclussion will toggle the "Receive Occlusion" property for any selected
# character or prop object
#------------------------------------------------------------------------------
def toggleObjectOcclusion():
    for obj in mach.getSelection():
        objectType = mach.getType(obj)
        if objectType == "Objects":
            toggleOcclusion(obj)


def toggleOcclusion(i_Object):
    if mach.getValue(i_Object, "receiveOcclusion") == True:
        mach.setValue(i_Object, "receiveOcclusion", False)
    else:
        mach.setValue(i_Object, "receiveOcclusion", True)
    
mach.createCommand("Toggle Manip Mode", toggleManip)
mach.createCommand("Toggle Receive Occlusion", toggleObjectOcclusion)



#------------------------------------------------------------------------------
# Go through each object's materials and change the extension of their texture maps
#------------------------------------------------------------------------------
def replaceTextureMapExt(i_CurrentExt, i_NewExt, i_NewDirectory):
    for obj in mach.getPlacedObjects():
        objectType = mach.getType(obj)
        if objectType == "Objects":
            changeMatExt(obj, i_CurrentExt, i_NewExt, i_NewDirectory)

#------------------------------------------------------------------------------
# do the extension changing operation for the given object
#------------------------------------------------------------------------------
def changeMatExt(i_Object, i_CurrentExt, i_NewExt, i_NewDirectory):
    cur_ext = "." + i_CurrentExt
    new_ext = "." + i_NewExt
    map_string = ""
    dir_string = i_NewDirectory + "\\"
    
    for mat in mach.getAllMaterials(i_Object):
        for prop in mach.getMaterialProperties(i_Object, mat):
            if prop.find("Map") != -1:
                map_string = mach.getMaterialValue(i_Object, mat, prop)
                map_string = str(map_string)
                #we want to make sure the map string isn't a float or int
                try:
                    int_string = int(map_string)
                except (ValueError, IndexError):
                    map_string = getDirectoryFile(map_string)
                    if map_string == "":
                        continue
                    map_string = dir_string + map_string
                    map_string = map_string.replace(cur_ext, new_ext)
                    if os.path.exists(map_string):
                        mach.setMaterialValue(i_Object, mat, prop, map_string)


def getDirectoryFile(i_Path):
    path = i_Path.split("\\")
    return path[-1]


#------------------------------------------------------------------------------
#  Duplicate a group with a position and orientation offset a given number of times
#------------------------------------------------------------------------------
def duplicateGroup(i_NumGroups,i_PositionOffset, i_OrientationOffset):
    cur_group = []
    next_group = []
    cur_group = mach.getSelection()
    group_count = 0

    while group_count < i_NumGroups:
        next_group = duplicateCurrentGroup(cur_group, i_PositionOffset, i_OrientationOffset)
        cur_group = next_group
        group_count += 1
            
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def duplicateCurrentGroup(i_CurGroup, i_PositionOffset, i_OrientationOffset):
    dup_obj = ""
    next_group = []
    group_dict = {}
    for obj in i_CurGroup:
        dup_obj = mach.duplicateObject(obj)
        group_dict[obj] = dup_obj
        rePositionObject(dup_obj, i_PositionOffset)
        reOrientObject(dup_obj, i_OrientationOffset)
        next_group.append(dup_obj)

    #now update all attach drivers to the duplicated objects    
    for dup in next_group:
        reApplyAttachDrivers(dup, group_dict)
        
    return next_group

#------------------------------------------------------------------------------
# apply the position offset to the 
#------------------------------------------------------------------------------
def rePositionObject(i_Object, i_PositionOffset):
    old_pos = mach.getValue(i_Object, "position")
    new_pos = (old_pos[0] + i_PositionOffset[0], old_pos[1] + i_PositionOffset[1], old_pos[2] + i_PositionOffset[2])
    
    mach.setValue(i_Object, "position", new_pos)


#------------------------------------------------------------------------------
# Apply the orientation offset to the duplicated objects
#------------------------------------------------------------------------------
def reOrientObject(i_Object, i_OrientationOffset):
    obj_type = mach.getType(i_Object)
    if obj_type == "Objects":
        old_ori = mach.getValue(i_Object, "orientation")
        new_ori = (old_ori[0] + i_OrientationOffset[0], old_ori[1] + i_OrientationOffset[1], old_ori[2] + i_OrientationOffset[2])
        mach.setValue(i_Object, "orientation", new_ori)
    elif obj_type == "Projected Lights":
        old_pitch = mach.getValue(i_Object, "pitch")
        old_yaw  = mach.getValue(i_Object, "yaw")
        old_tilt  = mach.getValue(i_Object, "tilt")
        mach.setValue(i_Object, "pitch", old_pitch + i_OrientationOffset[0])
        mach.setValue(i_Object, "yaw", old_yaw + i_OrientationOffset[1])
        mach.setValue(i_Object, "tilt", old_tilt + i_OrientationOffset[2])
    elif obj_type == "Cameras":
        old_pitch = mach.getValue(i_Object, "pitch")
        old_yaw  = mach.getValue(i_Object, "yaw")
        old_tilt  = mach.getValue(i_Object, "tilt")
        mach.setValue(i_Object, "pitch", old_pitch + i_OrientationOffset[0])
        mach.setValue(i_Object, "yaw", old_yaw + i_OrientationOffset[1])
        mach.setValue(i_Object, "tilt", old_tilt + i_OrientationOffset[2])


#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def reApplyAttachDrivers(i_Object, i_AttachDictionary):
    objDrivers = mach.getDrivers(i_Object)
    for driver in objDrivers:
        driverName = mach.getDriverName(driver)
        if driverName == "Attach":
            prevAttach = mach.getDriverValue(driver, "objectName")
            if prevAttach in i_AttachDictionary:
                newAttach = i_AttachDictionary[prevAttach]
                mach.setDriverValue(driver, "objectName", newAttach)
            
































        
