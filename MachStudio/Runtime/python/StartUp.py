#=============================================================================
#	StartUp.py
#
#	Gets executed at start-up on launch of Mach Studio
#=============================================================================

#-----------------------------------------------------------------------------
# 
#-----------------------------------------------------------------------------
print "In StartUp.py"

#-----------------------------------------------------------------------------
# Compare versions to see if certain mach commands are available
#-----------------------------------------------------------------------------

def versionAtLeast(a):
 	split1 = a.split(".")
 	split2 = mach.getVersion().split(".")
 	for i in range(min(len(split1), len(split2))):
 		if (int(split1[i]) < int(split2[i])):
 			return True
 		elif (int(split1[i]) > int(split2[i])):
 			return False
 		
 	return True

#-----------------------------------------------------------------------------
# Shortcuts for object creation
#-----------------------------------------------------------------------------
def createPointLight():
 	return mach.createObject("Point Lights", "Point Light")

def createCamera():
 	return mach.createObject("Cameras", "Camera")
 	
def createProjectedLight():
 	return mach.createObject("Projected Lights", "Projected Light")
 	
#-----------------------------------------------------------------------------
# Print properties of selected objects
#-----------------------------------------------------------------------------
def printSelectedProperties():
	for obj in mach.getSelection():
	   print "***", obj
	   for p in mach.getProperties(obj):
		  print p, mach.getValue(obj, p)
		  
#-----------------------------------------------------------------------------
# Print load preferences in the syntax in order to set the values
#-----------------------------------------------------------------------------
def printLoadPreferences():
   for p in mach.getProperties("LoadPrefs"):
      print 'mach.setValue("LoadPrefs", "' + p + '", ', mach.getValue("LoadPrefs", p), ')'
		  
#-----------------------------------------------------------------------------
#Example of how to hook up a Python command to a menu and hotkey
#-----------------------------------------------------------------------------
#def sayHello():
#	print "Hello command!"
#
#mach.createCommand("Say hello", sayHello)


#-----------------------------------------------------------------------------
# Example for importing another python script from the same "MachStudio/python" directory
# Functions from this script would then be in the "ImportMe" module, so that you
# might call a function like: ImportMe.imported()
#-----------------------------------------------------------------------------
#import ImportMe

#-----------------------------------------------------------------------------
# ExportUtil defines the routines for exporting poses to Maya
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	import ExportUtil

#-----------------------------------------------------------------------------
# CaptureUtil defines the routines for preparing capture preferences
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	import CaptureUtil

#-----------------------------------------------------------------------------
# TimeUtil defines routines for seconds versus frames representation of time
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	import TimeUtil
	
#-----------------------------------------------------------------------------
# DriverUtil defines routines for manipulating drivers
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	import DriverUtil

#-----------------------------------------------------------------------------
# LightUtil defines routines for duplicating Light objects and offsetting their new
# position
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	from LightUtil import *

#-----------------------------------------------------------------------------
# PythonObjectUtil defines routines for extracting data from objects and create
# scripts that will be thrown in the available tab
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	import PythonObjUtil

#-----------------------------------------------------------------------------
# ObjectInfo retrieves all of the materials and surfaces for an object
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	from ObjectInfo import *

#-----------------------------------------------------------------------------
# ObjectCommands set of functions that do misc. commands on objects
#-----------------------------------------------------------------------------
if versionAtLeast("0.9.0.5"):
	from ObjectCommands import *

#-----------------------------------------------------------------------------
# ObjectCommands set of functions that do misc. commands on objects
#-----------------------------------------------------------------------------
if versionAtLeast("1.3.3.0"):
	from VideoLog import *

#-----------------------------------------------------------------------------
# Populate all render layers will provide thousands of render layer configurations
#-----------------------------------------------------------------------------
if versionAtLeast("1.3.8.0"):
	from PopulateAllLayers import *



