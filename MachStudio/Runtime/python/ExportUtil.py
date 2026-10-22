#=============================================================================
#	ExportUtil.py
#
#	Routines for exporting pose information to be read into Maya
#=============================================================================

import mach

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
#def exportPosesForObject(exportFile, obj):
#	anim_drivers = mach.getDriversForChannel(obj, "animFull")
#	for drv in anim_drivers:
#		begin_time = mach.getDriverValue(drv, "beginTime")
#		mach.setCurrentTime(begin_time)
#		mach.updateTime()
#		pos = mach.getValue(obj, "position")
#		rot = mach.getValue(obj, "orientation")
#		animFile = mach.getDriverValue(drv, "animFileName")
#		exportFile.write("addPose('%s', %f, %s, %s )\n" % (animFile, begin_time, str(pos), str(rot)))

def exportPosesForObject(exportFile, obj):
	print "Exporting Poses\n"
	anim_drivers = mach.getDriversForChannel(obj, "animFull")
	print "1\n"
	for drv in anim_drivers:
		begin_time = mach.getDriverValue(drv, "beginTime")
		animFile = mach.getDriverValue(drv, "animFileName")
		exportFile.write("addPose('%s', %f)\n" % (animFile, begin_time))


#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def exportPositionForObject(exportFile, obj):
	print "Exporting Positions\n"
	pos_drivers = mach.getDriversForChannel(obj, "position")
	for drv in pos_drivers:
		begin_time = mach.getDriverValue(drv, "beginTime")
		mach.setCurrentTime(begin_time)
		mach.updateTime()
		pos = mach.getValue(obj, "position")
		exportFile.write("changePosition(%f, %s)\n" % (begin_time, str(pos)))

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def exportRotationForObject(exportFile, obj):
	print "Exporting Orientation\n"
	rot_drivers = mach.getDriversForChannel(obj, "orientation")
	for drv in rot_drivers:
		begin_time = mach.getDriverValue(drv, "beginTime")
		mach.setCurrentTime(begin_time)
		mach.updateTime()
		rot = mach.getValue(obj, "orientation")
		exportFile.write("changeOrientation(%f, %s)\n" % (begin_time, str(rot)))

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def exportVisibilityForObject(exportFile, obj):
	print "Exporting Visibility\n"
	vis_drivers = mach.getDriversForChannel(obj, "visible")
	for drv in vis_drivers:
		begin_time = mach.getDriverValue(drv, "beginTime")
		end_time = mach.getDriverValue(drv, "endTime")
		exportFile.write("changeVisibility(%f, %f)\n" % (begin_time, end_time))

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def exportScene(exportFile):
	exportFile.write("setSceneLength(%f)\n" % mach.getMaximumTime())
	sound_drivers = []
	characters = mach.getPlacedOfType("Characters")
	for obj in characters:
		pos = mach.getValue(obj, "position")
		rot = mach.getValue(obj, "orientation")
		exportFile.write("addCharacter('%s', %s, %s)\n" % (mach.getValue(obj, "filename"), str(pos), str(rot)))
		sound_drivers = sound_drivers + mach.getDriversForChannel(obj, "sound")
		exportPosesForObject(exportFile, obj)
		exportPositionForObject(exportFile, obj)
		exportRotationForObject(exportFile, obj)
		exportVisibilityForObject(exportFile, obj)
	props = mach.getPlacedOfType("Props")
	for obj in props:
		pos = mach.getValue(obj, "position")
		rot = mach.getValue(obj, "orientation")
		exportFile.write("addProp('%s', %s, %s)\n" % (mach.getValue(obj, "filename"), str(pos), str(rot)))
		sound_drivers = sound_drivers + mach.getDriversForChannel(obj, "sound")
		exportPosesForObject(exportFile, obj)
	for drv in sound_drivers:
		exportFile.write("addSound(%s, %f)\n" % (mach.getDriverValue(drv,"soundName"), mach.getDriverValue(drv, "beginTime")))

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def getExportFilename():
	fullpath =  mach.getSceneFilepath()
	#print fullpath
	if (len(fullpath) > 4):
		ext = fullpath[-4:]
		if (ext.lower() == ".mab"):
			fullpath = fullpath[:-4] + ".maa"
	else:
		fullpath = "export.maa"
	return fullpath		
	
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
def doExport():
	exp_filename = getExportFilename()
	exp_file = open(exp_filename, 'w')
	exportScene(exp_file)
	exp_file.close()
	print "Exported to file:", exp_filename


#-----------------------------------------------------------------------------
#	Commands that show up in the hot keys
#-----------------------------------------------------------------------------
#mach.createCommand("Export Poses", doExport)
