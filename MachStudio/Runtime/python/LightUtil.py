#=============================================================================
#	LightUtil.py
#
#	Routines for manipulating Light properties
#=============================================================================

import mach

'''
  Duplicates light object and offsets the position and target point by the offset values.
'''
def duplicateLightOffset( obj, offX, offY, offZ ):
    curPos = ()
    curTarget = ()

    #first duplicate the object
    new_obj = mach.duplicateObject(obj)

    #grab position and target point values
    curPos = mach.getValue(new_obj, "position")
    curTarget = mach.getValue(new_obj, "targetPoint")

    #offset the position and target point the same amount
    mach.setValue( new_obj, "position", (curPos[0] + offX, curPos[1] + offY, curPos[2] + offZ) )
    mach.setValue( new_obj, "targetPoint", (curTarget[0] + offX, curTarget[1]+ offY, curTarget[2] + offZ) )

    #update the new driver values
    for driver in mach.getDriversForChannel( new_obj, "target" ):
        curTarget = mach.getDriverValue(driver, "position")
        mach.setDriverValue( driver, "position", (curTarget[0] + offX, curTarget[1] + offY, curTarget[2] + offZ) )
    for driver in mach.getDriversForChannel( new_obj, "position" ):
        curPos = mach.getDriverValue(driver, "position")
        mach.setDriverValue( driver, "position", (curPos[0] + offX, curPos[1]+ offY, curPos[2] + offZ) )
    

'''
  Duplicates a group of light objects and offsets the position and target points by the offset values.
'''
def duplicateLightGroupOffset( group, offX, offY, offZ ):
    objInGroup = []
    objInGroup = mach.getObjectsInGroup(group)
    for obj in objInGroup:
        duplicateLightOffset( obj, offX, offY, offZ )

'''
  Duplicates a selection of light objects and offsets the position and target points by the offset values.
'''
def duplicateSelectedLightOffset( offX, offY, offZ ):
    objSelected = []
    objSelected = mach.getSelection()
    for obj in objSelected:
        duplicateLightOffset( obj, offX, offY, offZ )

'''
This function will toggle the enabled property of all lights in the placed object
section
'''
def toggleAllLights():
    for obj in mach.getPlacedObjects():
        objectType = mach.getType(obj)
        if objectType == "Point Lights" or objectType == "Projected Lights":
            toggleEnable(obj)
'''
This function will toggle the enabled property of all lights that are currently
selected
'''
def toggleLight():
    for obj in mach.getSelection():
        objectType = mach.getType(obj)
        if objectType == "Point Lights" or objectType == "Projected Lights":
            toggleEnable(obj)
'''
This function will toggle the manipMode property of all lights that are currently
selected
'''            
def toggleLightManip():
    for obj in mach.getSelection():
        objectType = mach.getType(obj)
        #only projected lights have the manipMode property
        if objectType == "Projected Lights":
            toggleManip(obj)
            
def toggleEnable(lightObject):
    if mach.getValue(lightObject, "enabled") == True:
        mach.setValue(lightObject, "enabled", False)
    else:
        mach.setValue(lightObject, "enabled", True)

def toggleManip(lightObject):
    if mach.getValue(lightObject, "manipMode") == "Light":
        mach.setValue(lightObject, "manipMode", "Target")
    elif mach.getValue(lightObject, "manipMode") == "Target":
        mach.setValue(lightObject, "manipMode", "Together")
    else:
        mach.setValue(lightObject, "manipMode", "Light")


        
mach.createCommand("Toggle Enable All Lights", toggleAllLights)
mach.createCommand("Toggle Enable Selected Lights", toggleLight)

