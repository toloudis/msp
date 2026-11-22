#=============================================================================
#	FastLoad.py
#
#	Can be used to configure the load preferences so that files load
#	quickly for times when you know you will not be doing shadows 
#	or rendering.
#=============================================================================

#-----------------------------------------------------------------------------
# 
#-----------------------------------------------------------------------------
print "In FastLoad.py"

#-----------------------------------------------------------------------------
# Sets load preferences for faster loading when not using shadows and 
# not needing full textures.
#-----------------------------------------------------------------------------
if versionAtLeast("2.8.16.1"):
	mach.setValue("LoadPrefs", "skipAllTextures", True)
	mach.setValue("LoadPrefs", "noDepthMaps", True)
	mach.setValue("LoadPrefs", "computeBasisVectors", False)
	mach.setValue("LoadPrefs", "maxSubdivLevel", 0)
	mach.setValue("LoadPrefs", "skipLoadingHighRes", True)
	mach.setValue("LoadPrefs", "neverLoadSounds", True)
	 
# - set shadows off and low res on also for this mode
mach.setValue("ViewportRenderPrefs", "shadowsOn", False)
mach.setValue("ViewportRenderPrefs", "lowResolution", True)
	