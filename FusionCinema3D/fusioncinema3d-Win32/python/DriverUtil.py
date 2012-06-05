#=============================================================================
#	DriverUtil.py
#
#	Routines for selecting and manipulating drivers
#=============================================================================

import mach
import math


#-----------------------------------------------------------------------------
# set blend type for all selected unlocked drivers
#-----------------------------------------------------------------------------
def set_blend_type(blend):
	for driver in mach.getSelectedUnlockedDrivers():
		mach.setDriverValue(driver, "blendType", blend)

#-----------------------------------------------------------------------------
# toggles the blend type for all selected unlocked drivers		
#-----------------------------------------------------------------------------
def toggle_blend_type():
        for driver in mach.getSelectedUnlockedDrivers():
                if mach.getDriverValue(driver, "blendType") == "No Blending":
                        mach.setDriverValue(driver, "blendType", "Previous")
                elif mach.getDriverValue(driver, "blendType") == "Previous":
                        mach.setDriverValue(driver, "blendType", "Blend Time")
                elif mach.getDriverValue(driver, "blendType") == "Blend Time":
                        mach.setDriverValue(driver, "blendType", "Overwrite")
                elif mach.getDriverValue(driver, "blendType") == "Overwrite":
                        mach.setDriverValue(driver, "blendType", "Smooth Blend")
                elif mach.getDriverValue(driver, "blendType") == "Smooth Blend":
                        mach.setDriverValue(driver, "blendType", "No Blending")
	
#-----------------------------------------------------------------------------
# variation of set blend type using smooth, could be hooked up to hot key
#-----------------------------------------------------------------------------
def set_blend_type_smooth():
	set_blend_type("Smooth Blend")
	
#-----------------------------------------------------------------------------
# variation of set blend type using no blend, could be hooked up to hot key
#-----------------------------------------------------------------------------
def set_blend_type_No_Blending():
	set_blend_type("No Blending")

#-----------------------------------------------------------------------------
# variation of set blend type using previous, could be hooked up to hot key
#-----------------------------------------------------------------------------
def set_blend_type_Previous():
	set_blend_type("Previous")

#-----------------------------------------------------------------------------
# variation of set blend type using blend time, could be hooked up to hot key
#-----------------------------------------------------------------------------
def set_blend_type_Blend_Time():
	set_blend_type("Blend Time")

#-----------------------------------------------------------------------------
# variation of set blend type using overwrite, could be hooked up to hot key
#-----------------------------------------------------------------------------
def set_blend_type_Overwrite():
	set_blend_type("Overwrite")
	
#-----------------------------------------------------------------------------
# set blend type for all selected unlocked drivers
#-----------------------------------------------------------------------------
#def delete_drivers_outside_selectedcapture():
#	for driver in mach.getDrivers():
#		capture_beginTime = 0
#		capture_endTime = 99999
#		driver_beginTime = mach.getDriverValue(driver, "beginTime"))
#		driver_endTime = mach.getDriverValue(driver, "endTime"))
#		if (   (driver_endTime <= capture_beginTime)
#				|| (driver_beginTime >= capture_endTime)
#				|| (   (driver_beginTime < capture_endTime)
#				    && (driver_beginTime >= capture_beginTime))
#				|| (   (driver_endTime < capture_endTime)
#				    && (driver_endTime >= capture_beginTime))
#				|| (   (driver_beginTime < capture_beginTime)
#				    && (driver_endTime > capture_endTime))
#				)
#				mach.DeleteDriver(driver);
	
	
#-----------------------------------------------------------------------------
# select all drivers at or around the given time
#-----------------------------------------------------------------------------
def select_drivers_at_time(time, range):
	if len(mach.getSelection()) == 0:
		print "No object is selected"
		return
	
	is_driver_found = False
	for obj in mach.getSelection():
		for driver in mach.getDrivers(obj):
			driver_beginTime = mach.getDriverValue(driver, "beginTime")
			driver_endTime = mach.getDriverValue(driver, "endTime")
			if ( (driver_beginTime - range < time) and (driver_endTime + range > time) ):
				mach.appendSelectDriver(driver)
				is_driver_found = True;
	
	if is_driver_found == False:
		print "No driver is found"
			

#-----------------------------------------------------------------------------
# select all drivers at or around the current time
#-----------------------------------------------------------------------------
def select_drivers_at_current_time():
	# do you want to clear the selection before adding these drivers?
	# or do you want to be able to keep appending selections with this command?
	mach.clearSelectedDrivers()
	# get all drivers within a half frame of the current time
	range = 2.0 / 24.0
	select_drivers_at_time( mach.getCurrentTime(), range )
	
	
#-----------------------------------------------------------------------------
#	Commands that show up in the hot keys
#-----------------------------------------------------------------------------
mach.createCommand("Toggle Blend Type", toggle_blend_type)
mach.createCommand("Set Smooth Blend", set_blend_type_smooth)
mach.createCommand("Set No Blending", set_blend_type_No_Blending)
mach.createCommand("Select Drivers at Time", select_drivers_at_current_time)



#-------------------------------------------------------------------------
#Functions that will Join or Split selected Camera drivers
#-------------------------------------------------------------------------

#this function will split a single Camera key driver or will join 2 drivers (position and target)
def splitOrJoinCameraDrivers():
    for obj in mach.getSelection():
        #first make sure selected object is a camera
        if mach.getType(obj) != "Cameras":
            print "ERROR: The selected object is not a camera"
            return
        if len(mach.getSelectedDrivers()) == 1:
            splitCameraDrivers(obj)
        elif len(mach.getSelectedDrivers()) == 2:
            joinCameraDrivers(obj)
        else:
            print "ERROR: Select a driver to split or 2 drivers to join"
            return



def joinCameraDrivers(obj):
    #set flags that will ensure that each type of driver is present
    gotPosDriver = False
    gotTarDriver = False
    posId = 0L
    tarId = 0L
    for driver in mach.getSelectedDrivers():
        #check each name to make sure we have a Position driver and a Target driver
        driverName = mach.getDriverName(driver)
        if driverName == "Position":
            gotPosDriver = True
            posId = driver
        if driverName == "Target":
            gotTarDriver = True
            tarId = driver
    #if we have both drivers available take both ids and make a new driver
    if gotPosDriver == True and gotTarDriver == True:
        createKeyDriver(obj, posId, tarId)
        deleteOldDriver()
    else:
        print "ERROR: Two selected drivers must be Position and Target types"
        
#This function will remove the link between the target/position driver of a camera
def splitCameraDrivers(obj):
    #iterate through the selected drivers of the object and determine
    #if the driver is a "Camera Key" driver
    for driver in mach.getSelectedDrivers():
        oldId = driver
        #if the driver is a "Camera Key" driver, then we now
        #must create 2 new drivers, 1 for position the other for target
        #and then we must delete the original driver from the timeline
        driverName = mach.getDriverName(oldId)
        if driverName == "Key":
            createPosDriver(obj, oldId)
            createTarDriver(obj, oldId)
            deleteOldDriver()
        else:
            print "ERROR: Cannot split a camera driver of type: " + driverName

#Create the position driver as type "Static Position" and set the position value to the original position value
def createPosDriver(obj, oldId):
    driverName = "Static Position"
    newId = mach.createDriver(obj, driverName)
    newId = long(newId)
    copyProperties(oldId, newId)
    mach.setDriverValue( newId, "position", mach.getDriverValue(oldId, "keyPosition") )

#Create the target driver as type "Static Target" and set the position value to the original target value
def createTarDriver(obj, oldId):
    driverName = "Static Target"
    newId = mach.createDriver(obj, driverName)
    newId = long(newId)
    copyProperties(oldId, newId)
    mach.setDriverValue( newId, "position", mach.getDriverValue(oldId, "keyTarget") )


#create key driver as type "Camera Key" and set the keyPosition and keyTarget to the value of the original driver
def createKeyDriver(obj, posId, tarId):
    driverName = "Camera Key"
    newId = mach.createDriver(obj, driverName)
    newId = long(newId)
    copyProperties(posId, newId)
    mach.setDriverValue( newId, "keyPosition", mach.getDriverValue(posId, "position") )
    mach.setDriverValue( newId, "keyTarget", mach.getDriverValue(tarId, "position") )
    
#delete the original driver
def deleteOldDriver():
    mach.execCommand("deleteDriver")

#Copy all base properties, do not copy position and target values yet
def copyProperties(oldId, newId):
    for prop in mach.getDriverProperties(oldId):
        if prop == "keyPosition":
            continue
        if prop == "keyTarget":
            continue
        if prop == "position":
            continue
        mach.setDriverValue( newId, prop, mach.getDriverValue(oldId, prop) )


#mach.createCommand("Split or Join Camera drivers", splitOrJoinCameraDrivers)

#mach.createScriptObject("Split or Join Camera drivers", splitOrJoinCameraDrivers)
