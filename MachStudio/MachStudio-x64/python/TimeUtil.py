#=============================================================================
#	TimeUtil.py
#
#	Routines for converting between seconds and frames
#=============================================================================

import mach
import math

#-----------------------------------------------------------------------------
# defines the constant frame rate here, could be related to the internal
# settings of MachStudio through an API function?
#-----------------------------------------------------------------------------
def get_frame_rate():
	return 24.0
	
#-----------------------------------------------------------------------------
# given time in seconds, this returns the nearest frame number
#-----------------------------------------------------------------------------
def convert_seconds_to_frames(seconds):
	return math.floor(0.5 + seconds * get_frame_rate())
	
#-----------------------------------------------------------------------------
# given time as frame number, returns time as seconds
#-----------------------------------------------------------------------------
def convert_frames_to_seconds(frames):
	return (frames / get_frame_rate())
	
#-----------------------------------------------------------------------------
# snaps a time value in seconds to the nearest frame, returned in seconds
#-----------------------------------------------------------------------------
def snap_to_frame(seconds):
	frame = convert_seconds_to_frames(seconds)
	return convert_frames_to_seconds(frame)

#-----------------------------------------------------------------------------
# snap the current timeline time to an exact frame
#-----------------------------------------------------------------------------
def snap_current_time_to_frame():
	time = mach.getCurrentTime()
	mach.setCurrentTime( snap_to_frame(time) )
	
#-----------------------------------------------------------------------------
# snap the begin and end values of the given driver to exact frames
#-----------------------------------------------------------------------------
def snap_driver_to_frame(driver_id):
	beginTime = snap_to_frame(mach.getDriverValue(driver_id, "beginTime"))
	endTime = snap_to_frame(mach.getDriverValue(driver_id, "endTime"))
	mach.setDriverValue(driver_id, "beginTime", beginTime)
	mach.setDriverValue(driver_id, "endTime", endTime)
	
#-----------------------------------------------------------------------------
# snap all selected drivers (unlocked channels only) to exact frames.
#-----------------------------------------------------------------------------
def snap_drivers_to_frames():
	for driver in mach.getSelectedUnlockedDrivers():
		snap_driver_to_frame(driver)
		
#-----------------------------------------------------------------------------
# snap all drivers (including locked channels) for the selected objects
# to exact frames. A variation of this might use mach.getPlacedObjects()
# in order to snap all drivers on all objects.
#-----------------------------------------------------------------------------
def snap_all_drivers_to_frames():
	for obj in mach.getSelection():
		for driver in mach.getDrivers(obj):
			snap_driver_to_frame(driver)
		
#-----------------------------------------------------------------------------
# snap all markers to exact frames
#-----------------------------------------------------------------------------
def snap_markers_to_frames():
	for i in range(mach.getNumMarkers()):
		time = mach.getMarkerValue(i, "time")
		mach.setMarkerValue(i, "time", snap_to_frame(time))

#-----------------------------------------------------------------------------
# snap the current time, all markers and all selected objects to frames
#-----------------------------------------------------------------------------
def snap_all_to_frames():
	snap_current_time_to_frame()
	snap_markers_to_frames()
	snap_all_drivers_to_frames()
	
#-----------------------------------------------------------------------------
# delete all markers - careful!
#-----------------------------------------------------------------------------
def delete_all_markers():
        #must delete in reverse order since each deletion decrements
        #the size of the marker list
	i = mach.getNumMarkers() - 1
	while i >= 0:
                mach.deleteMarker(mach.getMarkerValue(i, "time"))
                i -= 1

#-----------------------------------------------------------------------------
#	Commands that show up in the hot keys
#-----------------------------------------------------------------------------
mach.createCommand("Snap All to Frames", snap_all_to_frames)
mach.createCommand("Snap Drivers Frames", snap_drivers_to_frames)
mach.createCommand("Snap Markers Frames", snap_markers_to_frames)
mach.createCommand("Delete All Markers", delete_all_markers)
