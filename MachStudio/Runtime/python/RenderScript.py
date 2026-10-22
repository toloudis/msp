#******************************************************************************
# renderScript.py
#
#   Users can use this script to modify capture settings on the current scene.
#   Flags can be entered in any order and must be followed by the new value for
#   that flag's property
#
#******************************************************************************

import mach
import sys


"""
- Possible Capture Prefs to modify (as of v1.4)-
['captureFormat', 'compressCode', 'moviePerCaptureDriver', 'captureAllCameras',
'cameras', 'treatCaptureDriversAsUniqueCameras', 'resolutions', 'pixelAspectRatio',
'width', 'height', 'captureSampling', 'jitteredSampling', 'filterWidth', 'filterType',
'globalShadowQuality', 'useSceneFilenameInFilename', 'useCameraNameInFilename',
'useRenderLayerNameInFilename', 'useCompressionTypeInFilename', 'counterDigits',
'captureFPS', 'useSceneFilenameAsDirectory', 'useCameraNameAsDirectory',
'useRenderLayerNameAsDirectory', 'useResolutionAsDirectory', 'outputDirectoryRoot',
'showRenderProgressDialog', 'leadInSeconds', 'leadOutSeconds', 'postRenderEmailSend',
'postRenderEmailSendAddresses', 'postRenderCommandOrPythonFileToExecute',
'postRenderCommandToExecute', 'resume', 'renderPosScene', 'renderPosCamera',
'renderPosRenderLayer', 'renderPosTime', 'renderPosSaveFile', 'deleteResumePoint',
'useCaptureDrivers', 'startTime', 'endTime', 'displayTimeCode', 'outputTitleCard',
'smoothing', 'useLayerVisibility', 'renderedFiles']


- Possible Render Layer Prefs to modify (as of v1.4)
['Render Type', 'HDR Layer', 'Hardware Antialiasing', 'Capture Tonemapped Pixels',
'Hardware Tessellation', 'Multipass Lighting', 'Use Normals', 'Enable Invisible Objects Mask',
'Low Resolution', 'Render DOF', 'Render Glow', 'Render Outline', 'Render Environments',
'Render Lit', 'Render Transparent', 'Enable Diffuse', 'Enable Specular', 'Enable Shadows',
'Enable Invisible Objects Cast Shadows', 'Render Reflections', 'Transparency Mode', 'Enable AO',
'Sampling Preset', 'Num Steps', 'Num Dirs', 'Enable Blur', 'Enable Multiple Depths', 'Num Depth Layers']
"""

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def ParseCommands():
    cmdList = sys.argv
    i = 0
    while i < len(cmdList):
        # -f load a file
        if cmdList[i] == "-f":
            #grab the scene name (if spaces in path, use quotes (") around path)
            i += 1
            val = cmdList[i]
            print "opening scene: ", str(val)
            mach.openScene(val)
            
        # -fs [int] frame start
        elif cmdList[i] == "-fs":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Frame( "startTime", val )
            
        # -fe [int] frame end
        elif cmdList[i] == "-fe":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Frame( "endTime", val )
            
        # -rd [string] root render directory
        elif cmdList[i] == "-rd":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_String( "outputDirectoryRoot", val )
            
        # -rx [int] render width
        elif cmdList[i] == "-rx":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Int( "width", val )
            
        # -ry [int] render height
        elif cmdList[i] == "-ry":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Int( "height", val )
            
        # -cam [string] [boolean] switch select camera by name (capture cameras option)
        elif cmdList[i] == "-cam":
            #grab the value
            i += 1
            camString = cmdList[i]
            camList = camString.split(",")  #find a comma to see if this is a list of layers
            camSingle = camString.split(":")
            if len(camList) > 1:
                ModifyCapturePref_CameraList(camList)
            elif len(camSingle) == 2:
                ModifyCapturePref_Camera( camSingle[0], camSingle[1] )
            else:
                camName = camString  #camera name
                i += 1
                camVal = cmdList[i]   #camera selected value
                ModifyCapturePref_Camera( camName, camVal )
            
        # -ff [string: limited list] file format
        elif cmdList[i] == "-ff":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_String( "captureFormat", val.upper() )
            
        # -rs [int] render sampling
        elif cmdList[i] == "-rs":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Int( "captureSampling", val )
            
        # -js [boolean] jitterd sampling
        elif cmdList[i] == "-js":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Boolean( "jitteredSampling", val )
            
        # -fw [float] filter width
        elif cmdList[i] == "-fw":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Float( "filterWidth", val )
            
        # -ft [string: list] filter type
        elif cmdList[i] == "-ft":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_String( "filterType", val )
            
        # -sm [boolean] smoothing
        elif cmdList[i] == "-sm":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Boolean( "smoothing", val )
            
        # -tc [boolean] display time code
        elif cmdList[i] == "-tc":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Boolean( "displayTimeCode", val )
            
        # -tcd [boolean] output title card
        elif cmdList[i] == "-tcd":
            #grab the value
            i += 1
            val = cmdList[i]
            ModifyCapturePref_Boolean( "outputTitleCard", val )

        # -rt [renderLayerIndex] [string] Render Type
        elif cmdList[i] == "-rt":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_String(rLayerList, "Render Type")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_String(rLayerSingle[0], "Render Type", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_String(rLayerIndex, "Render Type", val)
                
        # -ha [renderLayerIndex] [boolean] Hardware Antialiasing
        elif cmdList[i] == "-ha":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Hardware Antialiasing")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Hardware Antialiasing", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Hardware Antialiasing", val)
        
        # -ao [renderLayerIndex] [boolean] Enable AO
        elif cmdList[i] == "-ao":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable AO")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable AO", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable AO", val)
                
        # -tm [renderLayerIndex] [string] transparency mode
        elif cmdList[i] == "-tm":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_String(rLayerList, "Transparency Mode")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_String(rLayerSingle[0], "Transparency Mode", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_String(rLayerIndex, "Transparency Mode", val)
                
        # -aopre [renderLayerIndex] [string] Sampling Preset
        elif cmdList[i] == "-aopre":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_String(rLayerList, "Sampling Preset")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_String(rLayerSingle[0], "Sampling Preset", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_String(rLayerIndex, "Sampling Preset", val)
                
        # -tp [renderLayerIndex] [boolean] capture tonemapped pixels
        elif cmdList[i] == "-tp":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Capture Tonemapped Pixels")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Capture Tonemapped Pixels", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Capture Tonemapped Pixels", val)

        # -ht [renderLayerIndex] [boolean] Hardware Tessellation
        elif cmdList[i] == "-ht":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Hardware Tessellation")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Hardware Tessellation", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Hardware Tessellation", val)

        # -ml [renderLayerIndex] [boolean] Multipass Lighting
        elif cmdList[i] == "-ml":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Multipass Lighting")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Multipass Lighting", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Multipass Lighting", val)

        # -nml [renderLayerIndex] [boolean] Use Normals
        elif cmdList[i] == "-nml":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Use Normals")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Use Normals", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Use Normals", val)
                
        # -invmask [renderLayerIndex] [boolean] Enable Invisible Objects Mask
        elif cmdList[i] == "-invmask":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable Invisible Objects Mask")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable Invisible Objects Mask", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable Invisible Objects Mask", val)

        # -lo [renderLayerIndex] [boolean] Low Resolution
        elif cmdList[i] == "-lo":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Low Resolution")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Low Resolution", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Low Resolution", val)

        # -aoblur [renderLayerIndex] [boolean] Enable AO blur
        elif cmdList[i] == "-aoblur":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable Blur")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable Blur", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable Blur", val)

        # -aodepth [renderLayerIndex] [boolean] Multiple Depth
        elif cmdList[i] == "-aodepth":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable Multiple Depths")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable Multiple Depths", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable Multiple Depths", val)

        # -aonum [renderLayerIndex] [boolean] Num Depth Layers
        elif cmdList[i] == "-aonum":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Int(rLayerList, "Num Depth Layers")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Int(rLayerSingle[0], "Num Depth Layers", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Int(rLayerIndex, "Num Depth Layers", val)
                
        # -pdof [renderLayerIndex] [boolean] Render DOF
        elif cmdList[i] == "-pdof":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Render DOF")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Render DOF", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Render DOF", val)

        # -pglow [renderLayerIndex] [boolean] Render Glow
        elif cmdList[i] == "-pglow":
           #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Render Glow")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Render Glow", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Render Glow", val)
            
        # -pout [renderLayerIndex] [boolean] Render Outline
        elif cmdList[i] == "-pout":
           #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Render Outline")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Render Outline", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Render Outline", val)

        # -penv [renderLayerIndex] [boolean] Render Environments
        elif cmdList[i] == "-penv":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Render Environments")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Render Environments", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Render Environments", val)

        # -plit [renderLayerIndex] [boolean] Render Lit
        elif cmdList[i] == "-plit":
           #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Render Lit")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Render Lit", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Render Lit", val)
            
        # -ptrans [renderLayerIndex] [boolean] Render Transparent
        elif cmdList[i] == "-ptrans":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Render Transparent")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Render Transparent", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Render Transparent", val)
            
        # -pdiff [renderLayerIndex] [boolean] Enable Diffuse
        elif cmdList[i] == "-pdiff":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable Diffuse")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable Diffuse", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable Diffuse", val)

        # -pspec [renderLayerIndex] [boolean] Enable Specular
        elif cmdList[i] == "-pspec":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable Specular")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable Specular", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable Specular", val)

        # -pshad [renderLayerIndex] [boolean] Enable Shadows
        elif cmdList[i] == "-pshad":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable Shadows")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable Shadows", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable Shadows", val)

        # -pinvis [renderLayerIndex] [boolean] Enable Invisible Objects Cast Shadows
        elif cmdList[i] == "-pinvis":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Enable Invisible Objects Cast Shadows")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Enable Invisible Objects Cast Shadows", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Enable Invisible Objects Cast Shadows", val)
            
        # -prefl [renderLayerIndex] [boolean] Enable Reflections
        elif cmdList[i] == "-prefl":
            #grab the value
            i += 1
            rLayerString = cmdList[i]
            rLayerList = rLayerString.split(",")  #find a comma to see if this is a list of layers
            rLayerSingle = rLayerString.split(":")
            if len(rLayerList) > 1:
                ModifyRenderLayerPrefList_Boolean(rLayerList, "Render Reflections")
            elif len(rLayerSingle) == 2:
                ModifyRenderLayerPref_Boolean(rLayerSingle[0], "Render Reflections", rLayerSingle[1])
            else:
                i += 1
                val = cmdList[i]
                rLayerIndex = rLayerString
                ModifyRenderLayerPref_Boolean(rLayerIndex, "Render Reflections", val)
            
        #go to next flag
        i += 1


#------------------------------------------------------------------------------
# Given the property and the string value from the command line, modify the
# appropriate property with an integer
#------------------------------------------------------------------------------
def ModifyCapturePref_Int( i_PropertyName, i_Value ):
    #initialize integer value
    val = 0

    #try to convert the string to an integer, if it cannot convert, print an error
    try:
        val = int(i_Value)
    except (ValueError, IndexError):
        print "ERROR: invalid INT value: ", i_Value, \
              " for Property: ", i_PropertyName, ", value will default to 0." 

    #now set the value of the property with the integer value
    mach.setValue("CapturePrefs", i_PropertyName, val)
    print "Setting Capture Pref property, ", i_PropertyName, " to the value, ", str(val)


#------------------------------------------------------------------------------
# Given the property and the string value from the command line, modify the
# appropriate property with a float
#------------------------------------------------------------------------------
def ModifyCapturePref_Float( i_PropertyName, i_Value ):
    #initialize integer value
    val = float(0.0)

    #try to convert the string to an integer, if it cannot convert, print an error
    try:
        val = float(i_Value)
    except (ValueError, IndexError):
        print "ERROR: invalid FLOAT value: ", i_Value, \
              " for Property: ", i_PropertyName, ", value will default to 0."

    #now set the value of the property with the float value
    mach.setValue("CapturePrefs", i_PropertyName, val)
    print "Setting Capture Pref property, ", i_PropertyName, " to the value, ", str(val)
    

#------------------------------------------------------------------------------
# Given the property and the string value from the command line, modify the
# appropriate property with a boolean
#------------------------------------------------------------------------------
def ModifyCapturePref_Boolean( i_PropertyName, i_Value ):
    #initialize boolean value
    val = False;

    #get the command line boolean as a lowercase string
    str_val = str(i_Value).lower()

    #check the value of the boolean as a string or a numerical true/false
    if str_val == "true" or str_val == "1" or str_val == "yes":
       val = True
    elif str_val == "false" or str_val == "0" or str_val == "no":
        val = False
    else:
        print "ERROR: invalid BOOLEAN value: ", i_Value, \
              " for Property: ", i_PropertyName, ", value will default to False."

    #now set the value of the property with the boolean value
    mach.setValue("CapturePrefs", i_PropertyName, val)
    print "Setting Capture Pref property, ", i_PropertyName, " to the value, ", str(val)


#------------------------------------------------------------------------------
# Given the property and the string value from the command line, modify the
# appropriate property with a string
#------------------------------------------------------------------------------
def ModifyCapturePref_String( i_PropertyName, i_Value ):
    #i_Value is already a string, so no conversion necessary
    #so we just set the value of the property with the string value
    mach.setValue("CapturePrefs", i_PropertyName, i_Value)
    print "Setting Capture Pref property, ", i_PropertyName, " to the value, ", str(i_Value)


#------------------------------------------------------------------------------
# Changing the camera settings involves manipulation of the camera list
#------------------------------------------------------------------------------
def ModifyCapturePref_Camera( i_CameraName, i_Value ):
    #initialize boolean value
    val = False;
    #get the command line boolean as a lowercase string
    str_val = str(i_Value).lower()

    #check the value of the boolean as a string or a numerical true/false
    if str_val == "true" or str_val == "1" or str_val == "yes":
       val = True
    elif str_val == "false" or str_val == "0" or str_val == "no":
        val = False
    else:
        print "ERROR: invalid BOOLEAN value: ", i_Value, \
              " for Camera: ", i_CameraName, ", value will default to False."

    #get the initial list of the cameras
    camList = mach.getValue("CapturePrefs", "cameras")

    camVal = int(val) #returns 0 or 1
    newPair = (i_CameraName, camVal)  #camera name, value pair
    i = 0

    #iterate through the camera list and find the matching camera
    while i < len(camList):
        pair = camList[i]
        if i_CameraName == pair[0]:
            del camList[i]
            camList.insert(i, newPair)
            mach.setValue("CapturePrefs", "cameras", camList)
            print "Setting Capture Camera, ", i_CameraName, " to the selected value, ", str(camVal)
            return
        i += 1
        
    #didn't find camera name, so just append new camera value to the list
    camList.append(newPair)
    mach.setValue("CapturePrefs", "cameras", camList)
    print "Setting Capture Camera, ", i_CameraName, " to the selected value, ", str(camVal)

#------------------------------------------------------------------------------
# Given a list of render layer index/boolean pairs,
# set the boolean value for the render layer render pref 
#------------------------------------------------------------------------------
def ModifyCapturePref_CameraList( i_CamList ):
    #for each camera, set whether the cam is active or not
    for cam in i_CamList:
        cam = cam.strip()  #get rid of any whitespace
        cam_pair = cam.split(":")  #get the pair separated by ':'
        cam_name = cam_pair[0].strip()
        cam_val = cam_pair[1].strip()

        #now that we have the pair values, change the value for the current camera
        ModifyCapturePref_Camera(cam_name, cam_val)
        

#------------------------------------------------------------------------------
# Start and end frames must be divided by the frame count before their
# value can be changed
#------------------------------------------------------------------------------
def ModifyCapturePref_Frame( i_PropertyName, i_Value ):
    #initialize integer value
    val = 0
    frame_val = 0.0
    if i_Value == "endTime":
        frame_val = -1.0
    
    #try to convert the string to an integer, if it cannot convert, print an error
    try:
        val = int(i_Value)
    except (ValueError, IndexError):
        print "ERROR: invalid INT value: ", i_Value, \
              " for Property: ", i_PropertyName, ", value will default to ", str(frame_val) 

    fps = mach.getValue("CapturePrefs", "captureFPS")
    frame_val = float(val) / float(fps)
    
    #now set the value of the property with the integer value
    mach.setValue("CapturePrefs", "useCaptureDrivers", False) #turn off use capture drivers flag
    mach.setValue("CapturePrefs", i_PropertyName, frame_val)
    print "Setting Capture frame, ", i_PropertyName, " to the value, ", str(val), " / ", str(fps), " = ", str(frame_val)


#------------------------------------------------------------------------------
# Given the index of a render layer, return the name of that layer
#------------------------------------------------------------------------------
def GetLayerByIndex( i_RenderLayerIndex ):
    index = 0
    #try to convert the string to an integer, if it cannot convert, print an error
    try:
        index = int(i_RenderLayerIndex)
    except (ValueError, IndexError):
        print "ERROR: invalid INT value: ", i_RenderLayerIndex, \
              " 0 will be used as the default index"

    layerList = mach.getRenderLayers()
    if index >= len(layerList):
        print "ERROR: index out of bounds: ", str(index)
        return ""
    
    return str(layerList[index])
    

#------------------------------------------------------------------------------
# Set a boolean value for a render layer render pref value
#------------------------------------------------------------------------------
def ModifyRenderLayerPref_Boolean( i_RenderLayerIndex, i_PropertyName, i_Value ):
    #first we need to retrieve the render layer at the index given
    renderLayer = GetLayerByIndex(i_RenderLayerIndex)
    
    val = False
    #get the command line boolean as a lowercase string
    str_val = str(i_Value).lower()

    #check the value of the boolean as a string or a numerical true/false
    if str_val == "true" or str_val == "1" or str_val == "yes":
       val = True
    elif str_val == "false" or str_val == "0" or str_val == "no":
        val = False
    else:
        print "ERROR: invalid BOOLEAN value: ", i_Value, \
              " for Property: ", i_PropertyName, ", value will default to False."

    print "Setting Render Layer Pref, ", i_PropertyName, " to the value, ", str(val), " for layer, ", str(renderLayer)
    mach.setRenderLayerPref(renderLayer, i_PropertyName, val)

#------------------------------------------------------------------------------
# Given a list of render layer index/boolean pairs,
# set the boolean value for the render layer render pref 
#------------------------------------------------------------------------------
def ModifyRenderLayerPrefList_Boolean( i_RenderLayerList, i_PropertyName ):
    #for each layered pair, find the pair values and set the pref
    for layer in i_RenderLayerList:
        layer = layer.strip()  #get rid of any whitespace
        layer_pair = layer.split(":")  #get the pair separated by ':'
        layer_index = layer_pair[0].strip()
        pref_val = layer_pair[1].strip()

        #now that we have the pair values, change the value of the property for the current layer
        ModifyRenderLayerPref_Boolean(layer_index, i_PropertyName, pref_val)

#------------------------------------------------------------------------------
# Set a int value for a render layer render pref value
#------------------------------------------------------------------------------
def ModifyRenderLayerPref_Int( i_RenderLayerIndex, i_PropertyName, i_Value ):
    #first we need to retrieve the render layer at the index given
    renderLayer = GetLayerByIndex(i_RenderLayerIndex)
    
    #initialize integer value
    val = 0

    #try to convert the string to an integer, if it cannot convert, print an error
    try:
        val = int(i_Value)
    except (ValueError, IndexError):
        print "ERROR: invalid INT value: ", i_Value, \
              " for Property: ", i_PropertyName, ", value will default to 0."

    #limit the number of depth layers
    if i_PropertyName == "Num Depth Layers":
        if val > 4:
            val = 4
        elif val < 0:
            val = 0

    print "Setting Render Layer Pref, ", i_PropertyName, " to the value, ", str(val), " for layer, ", str(renderLayer)
    mach.setRenderLayerPref(renderLayer, i_PropertyName, val)

#------------------------------------------------------------------------------
# Given a list of render layer index/int pairs,
# set the int value for the render layer render pref 
#------------------------------------------------------------------------------
def ModifyRenderLayerPrefList_Int( i_RenderLayerList, i_PropertyName ):
    #for each layered pair, find the pair values and set the pref
    for layer in i_RenderLayerList:
        layer = layer.strip()  #get rid of any whitespace
        layer_pair = layer.split(":")  #get the pair separated by ':'
        layer_index = layer_pair[0].strip()
        pref_val = layer_pair[1].strip()

        #now that we have the pair values, change the value of the property for the current layer
        ModifyRenderLayerPref_Int(layer_index, i_PropertyName, pref_val)

#------------------------------------------------------------------------------
# Given the render type flag, return the full string for the render type
#------------------------------------------------------------------------------
def GetRenderType( i_TagValue ):
    i_TagValue = i_TagValue.lower()
    if i_TagValue == "hdr":
        return "Default HDR"
    elif i_TagValue == "ao":
        return "AO Only"
    elif i_TagValue == "db":
        return "Depth Buffer"
    elif i_TagValue == "sm":
        return "Shadow Mask"
    elif i_TagValue == "i":
        return "Illumination Only"
    elif i_TagValue == "n":
        return "Normals"
    elif i_TagValue == "dm":
        return "Dirty Matte"
    elif i_TagValue == "w":
        return "Wireframe"
    elif i_TagValue == "r":
        return "Reflections Only"
    elif i_TagValue == "vm":
        return "Velocity Map"
    else:
        print i_TagValue + " is not a valid string for the render type"
        return ""

#------------------------------------------------------------------------------
# Given the transparency mode flag, return the full string for the mode
#------------------------------------------------------------------------------
def GetTransparencyMode( i_TagValue ):
    i_TagValue = i_TagValue.lower()
    if i_TagValue == "0":
        return "Approximate"
    elif i_TagValue == "1":
        return "Full"
    else:
        print i_TagValue + " is not a valid flag for the transparency mode"
        return ""

#------------------------------------------------------------------------------
# Given the ao sampling flag, return the full string for the ao sampling preset
#------------------------------------------------------------------------------
def GetAOSampling( i_TagValue ):
    i_TagValue = i_TagValue.lower()
    if i_TagValue == "0":
        return "Low"
    elif i_TagValue == "1":
        return "Medium"
    elif i_TagValue == "2":
        return "High"
    else:
        return "Custom"
        
#------------------------------------------------------------------------------
# Set a string value for a render layer render pref value
#------------------------------------------------------------------------------
def ModifyRenderLayerPref_String( i_RenderLayerIndex, i_PropertyName, i_Value ):
    #first we need to retrieve the render layer at the index given
    renderLayer = GetLayerByIndex(i_RenderLayerIndex)

    val = i_Value
    if i_PropertyName == "Render Type":
        val = GetRenderType(i_Value)
    elif i_PropertyName == "Transparency Mode":
        val = GetTransparencyMode(i_Value)
    elif i_PropertyName == "Sampling Preset":
        val = GetAOSampling(i_Value)
        
    print "Setting Render Layer Pref, ", i_PropertyName, " to the value, ", str(val), " for layer, ", str(renderLayer)
    mach.setRenderLayerPref(renderLayer, i_PropertyName, val)

#------------------------------------------------------------------------------
# Given a list of render layer index/string pairs,
# set the string value for the render layer render pref 
#------------------------------------------------------------------------------
def ModifyRenderLayerPrefList_String( i_RenderLayerList, i_PropertyName ):
    #for each layered pair, find the pair values and set the pref
    for layer in i_RenderLayerList:
        layer = layer.strip()  #get rid of any whitespace
        layer_pair = layer.split(":")  #get the pair separated by ':'
        layer_index = layer_pair[0].strip()
        pref_val = layer_pair[1].strip()

        #now that we have the pair values, change the value of the property for the current layer
        ModifyRenderLayerPref_String(layer_index, i_PropertyName, pref_val)
        
ParseCommands()
mach.render()
