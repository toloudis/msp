#=============================================================================
#	CaptureUtil.py
#
#	Routines for preparing for different styles of capture
#=============================================================================

import mach

#-----------------------------------------------------------------------------
#	FUNCTION: full capture settings DEFAULTS - all full settings set the
#	values to these FIRST.  Each variation can override them.
#-----------------------------------------------------------------------------
def setFullCaptureDefaults():
	# Uncomment + Set the appropriate capture flags after dialog pops up:
	#
	mach.setValue("CapturePrefs", "width", 852)
	mach.setValue("CapturePrefs", "height", 480)
	mach.setValue("CapturePrefs", "captureFormat", "MOV")
	mach.setValue("CapturePrefs", "compressCode", "RAW")
	mach.setValue("CapturePrefs", "motionSamplesPerFrame", 4)
	mach.setValue("CapturePrefs", "captureSampling", 1)
	mach.setValue("CapturePrefs", "jitteredSampling", True)
	mach.setValue("CapturePrefs", "subdivLevel", 3)
	mach.setValue("CapturePrefs", "useMarkerTimes", True)
	mach.setValue("CapturePrefs", "endTime", 0.0)
	mach.setValue("CapturePrefs", "startTime", 0.0)
	mach.setValue("CapturePrefs", "captureFPS", 24.0)
	mach.setValue("CapturePrefs", "outputDirectory", "")
	mach.setValue("CapturePrefs", "leadInSeconds", 0.0)
	mach.setValue("CapturePrefs", "leadOutSeconds", 0.0)
	mach.setValue("CapturePrefs", "soundFile", "")
	mach.setValue("CapturePrefs", "displayTimeCode", False)
	mach.setValue("CapturePrefs", "captureAllCameras", True)
	mach.setValue("CapturePrefs", "useCameraNameInFilename", True)
	mach.setValue("CapturePrefs", "useSceneFilenameInFilename", True)
	mach.setValue("CapturePrefs", "useCompressionTypeInFilename", True)
	mach.setValue("CapturePrefs", "useRenderDriverAsDirAndFname", True)
	mach.setValue("CapturePrefs", "useCameraNameAsDirectory", True)
	mach.setValue("CapturePrefs", "useResolutionAsDirectory", True)
	mach.setValue("CapturePrefs", "useSceneFilenameAsDirectory", True)
	mach.setValue("CapturePrefs", "outputTitleCard", True)
	mach.setValue("CapturePrefs", "counterDigits", 4)
	mach.setValue("CapturePrefs", "postRenderEmailSend", False)
	mach.setValue("CapturePrefs", "postRenderEmailSendAddresses", "")
	mach.setValue("CapturePrefs", "postRenderCommandOrPythonFileToExecute", False)
	mach.setValue("CapturePrefs", "postRenderCommandToExecute", "")
	mach.setValue("CapturePrefs", "autoGenerateSubDirectories", True)
	mach.setValue("CapturePrefs", "showRenderProgressDialog", True)
	
	# Uncomment + Set the rendering flags for capture here:
	mach.setValue("RenderFullPrefs", "renderer", "HDR")
	mach.setValue("RenderFullPrefs", "shadowsOn", True)
	mach.setValue("RenderFullPrefs", "renderDOF", True)
	mach.setValue("RenderFullPrefs", "renderFur", True)
	mach.setValue("RenderFullPrefs", "renderGlow", True)
	mach.setValue("RenderFullPrefs", "renderMatte", False)
	mach.setValue("RenderFullPrefs", "renderTransparent", True)
	mach.setValue("RenderFullPrefs", "renderReflections", True)
	mach.setValue("RenderFullPrefs", "renderAmbient", True)
	mach.setValue("RenderFullPrefs", "renderLit", True)
	mach.setValue("RenderFullPrefs", "furQuality", 1)
	mach.setValue("RenderFullPrefs", "blueShift", True)
	mach.setValue("RenderFullPrefs", "toneMap", True)
	return
# END OF setFullCaptureDefaults()

#-----------------------------------------------------------------------------
#	FUNCTION: full capture settings
#-----------------------------------------------------------------------------
def doFullCapture_16_9():
	# Pop up the capture dialog, switching to mode capture
	mach.execCommand("render")

	setFullCaptureDefaults()

	# set the custom values here
	#mach.setValue("CapturePrefs", "width", 852)
	#mach.setValue("CapturePrefs", "height", 480)
	
	print "Done setup of full capture, ready to render."
	return
# END OF doFullCapture()

#-----------------------------------------------------------------------------
#	FUNCTION: full capture settings
#-----------------------------------------------------------------------------
def doFullCapture_4_3():
	# Pop up the capture dialog, switching to mode capture
	mach.execCommand("render")

	setFullCaptureDefaults()

	# set the custom values here
	mach.setValue("CapturePrefs", "width", 720)
	mach.setValue("CapturePrefs", "height", 480)
	
	print "Done setup of full capture, ready to render."
	return
# END OF doFullCapture()


#-----------------------------------------------------------------------------
#	FUNCTION: fast capture settings DEFAULTS - all fast settings set the
#	values to these FIRST.  Each variation can override them.
#-----------------------------------------------------------------------------
def setFastCaptureDefaults():
	# Uncomment + Set the appropriate capture flags after dialog pops up:
	#
	mach.setValue("CapturePrefs", "width", 852)
	mach.setValue("CapturePrefs", "height", 480)
	mach.setValue("CapturePrefs", "captureFormat", "MOV")
	mach.setValue("CapturePrefs", "compressCode", "jpeg")
	mach.setValue("CapturePrefs", "motionSamplesPerFrame", 1)
	mach.setValue("CapturePrefs", "captureSampling", 1)
	mach.setValue("CapturePrefs", "jitteredSampling", False)
	mach.setValue("CapturePrefs", "subdivLevel", 0)
	mach.setValue("CapturePrefs", "useMarkerTimes", True)
	mach.setValue("CapturePrefs", "endTime", 0.0)
	mach.setValue("CapturePrefs", "startTime", 0.0)
	mach.setValue("CapturePrefs", "captureFPS", 24.0)
	mach.setValue("CapturePrefs", "outputDirectory", "")
	mach.setValue("CapturePrefs", "leadInSeconds", 0.0)
	mach.setValue("CapturePrefs", "leadOutSeconds", 0.0)
	mach.setValue("CapturePrefs", "soundFile", "")
	mach.setValue("CapturePrefs", "displayTimeCode", True)
	mach.setValue("CapturePrefs", "captureAllCameras", True)
	mach.setValue("CapturePrefs", "useCameraNameInFilename", True)
	mach.setValue("CapturePrefs", "useSceneFilenameInFilename", True)
	mach.setValue("CapturePrefs", "useCompressionTypeInFilename", True)
	mach.setValue("CapturePrefs", "useRenderDriverAsDirAndFname", True)
	mach.setValue("CapturePrefs", "useCameraNameAsDirectory", True)
	mach.setValue("CapturePrefs", "useResolutionAsDirectory", True)
	mach.setValue("CapturePrefs", "useSceneFilenameAsDirectory", True)
	mach.setValue("CapturePrefs", "outputTitleCard", True)
	mach.setValue("CapturePrefs", "counterDigits", 4)
	mach.setValue("CapturePrefs", "postRenderEmailSend", False)
	mach.setValue("CapturePrefs", "postRenderEmailSendAddresses", "")
	mach.setValue("CapturePrefs", "postRenderCommandOrPythonFileToExecute", False)
	mach.setValue("CapturePrefs", "postRenderCommandToExecute", "")
	mach.setValue("CapturePrefs", "autoGenerateSubDirectories", True)
	mach.setValue("CapturePrefs", "showRenderProgressDialog", True)
	
	# Uncomment + Set the rendering flags for capture here:
	mach.setValue("RenderFullPrefs", "renderer", "2.0 Default")
	mach.setValue("RenderFullPrefs", "shadowsOn", False)
	mach.setValue("RenderFullPrefs", "renderDOF", False)
	mach.setValue("RenderFullPrefs", "renderFur", False)
	mach.setValue("RenderFullPrefs", "renderGlow", False)
	mach.setValue("RenderFullPrefs", "renderMatte", False)
	mach.setValue("RenderFullPrefs", "renderTransparent", True)
	mach.setValue("RenderFullPrefs", "renderReflections", False)
	mach.setValue("RenderFullPrefs", "renderAmbient", True)
	mach.setValue("RenderFullPrefs", "renderLit", True)
	mach.setValue("RenderFullPrefs", "furQuality", 1)
	mach.setValue("RenderFullPrefs", "blueShift", True)
	mach.setValue("RenderFullPrefs", "toneMap", True)
	return
# END OF setFastCaptureDefaults()

#-----------------------------------------------------------------------------
# FUNCTION: Fast capture settings
#-----------------------------------------------------------------------------
def doFastCapture():
	# Pop up the capture dialog, switching to mode capture
	mach.execCommand("render")
	
	setFastCaptureDefaults()

	# set the custom values here
	#	mach.setValue("CapturePrefs", "width", 640)
	#	mach.setValue("CapturePrefs", "height", 480)
	
	print "Done setup of fast capture, ready to render."
	return
# END OF doFastCapture()


#-----------------------------------------------------------------------------
#	create the commands that show up in the hot keys
#-----------------------------------------------------------------------------
#mach.createCommand("Capture - Full 16x9", doFullCapture_16_9)
#mach.createCommand("Capture - Full 4x3", doFullCapture_4_3)
#mach.createCommand("Capture - Fast", doFastCapture)
