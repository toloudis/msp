import os
import mach

"""
This function allows the user to create python scripts that will produce replicas of each object
they have selected in mach studio
"""
def createScriptFromObject( ):
    """filename = "python/AvailableObjects.py"
    if not os.path.isfile(filename):
        print "ERROR: AvailableObjects.py does not exist in the python directory"
        return    """
    script_list = []
    for obj in mach.getSelection():
        filename = createFileName( obj )
        writeFile = open(filename, "w")
        writeFile.write("import mach \n\n")
        objectType = mach.getType(obj)
        if objectType == "Objects":
            writeObjectScript(obj, writeFile)
        elif objectType == "Cameras":
            writeCameraScript(obj, writeFile)
        elif objectType == "Point Lights":
            writePtLightScript(obj, writeFile)
        elif objectType == "Projected Lights":
            writePrjLightScript(obj, writeFile)
        script_list.append(filename)
    writeFile.close()

    for fname in script_list:
        execfile(fname)
    

"""
This function takes a prop and writes all the methods to create a copy prop to a script file
"""
def writeObjectScript( io_Object, writeFile ):
    functionName = createFunctionName( io_Object )
    # write to file the createObject command special to Characters
    # this is the only part of the code unique to each object type
    writeFile.write("def " + functionName + "():\n")
    createObjectStart = "\tcurObject = mach.createObject(\"Objects\",\""
    charFileName = mach.getValue(io_Object, "filename")
    charFileName = charFileName.replace("\\", "\\\\")
    createObjectFinish = "\")"
    writeFile.write(createObjectStart + charFileName + createObjectFinish + "\n")

    #write to file all of the properties 
    writeProperties(io_Object, writeFile)
    #writeGetCurrentDirectory( functionName, writeFile )
    writeMenuCommand( io_Object, functionName, writeFile, False )

"""
This function takes a prop and writes all the methods to create a copy camera to a script file
"""
def writeCameraScript( io_Object, writeFile ):
    functionName = createFunctionName( io_Object )

    # write to file the createObject command special to Cameras
    # this is the only part of the code unique to each object type
    writeFile.write("def " + functionName + "():\n")
    createObject = "\tcurObject = mach.createObject(\"Cameras\", \"Camera\")"
    writeFile.write(createObject + "\n")

    #write to file all of the properties
    writeProperties(io_Object, writeFile)
    writeMenuCommand( io_Object, functionName, writeFile, False )


"""
This function takes a prop and writes all the methods to create a copy point light to a script file
"""
def writePtLightScript( io_Object, writeFile ):
    functionName = createFunctionName( io_Object )

    # write to file the createObject command special to Point Lights
    # this is the only part of the code unique to each object type
    writeFile.write("def " + functionName + "():\n")
    createObject = "\tcurObject = mach.createObject(\"Point Lights\", \"Point Light\")" 
    writeFile.write(createObject + "\n")

    #write to file all of the properties 
    writeProperties(io_Object, writeFile)
    writeMenuCommand( io_Object, functionName, writeFile, False )

"""
This function takes a prop and writes all the methods to create a copy projected light to a script file
"""
def writePrjLightScript( io_Object, writeFile ):
    functionName = createFunctionName( io_Object )

    # write to file the createObject command special to Projected Lights
    # this is the only part of the code unique to each object type
    writeFile.write("def " + functionName + "():\n")
    createObject = "\tcurObject = mach.createObject(\"Projected Lights\", \"Projected Lights\")" 
    writeFile.write(createObject + "\n")

    #write to file all of the properties 
    writeProperties(io_Object, writeFile)
    writeMenuCommand( io_Object, functionName, writeFile, False )

"""
This function modifies the object name and appends "ObjectScript.py" to the end
"""
def createFileName( i_Object ):
    userData = mach.getUserDataPath() + "\\PythonObjects\\"
    if not os.path.isdir(userData):
        os.mkdir(userData)
    
    fileName = i_Object + "_ObjectScript.py"
    fileName = fileName.replace("-", "_") #remove dashes, insert underscore
    fileName = fileName.replace(" ", "")  #remove spaces
    fileName = userData + fileName
    return fileName

"""
This function modifies the object name and appends "Script" to the end
"""
def createFunctionName( i_Object ):
    function = i_Object + "Script"
    function = function.replace("-", "_") #remove dashes, insert underscore
    function = function.replace(" ", "")  #remove spaces
    return function

"""
This function loops through each property of the object and writes to file
the method to set a value (mach.setValue) equal to the pre-existing properties
"""
def writeProperties( i_Object, writeFile, tabbed = True ):
    if tabbed:
        setValueStart = "\tmach.setValue("
    else:
        setValueStart = "mach.setValue("
    objString = "curObject"
    for prop in mach.getProperties(i_Object):
        if prop == "filename":
            continue
        propString = "\"" + prop + "\""
        curPropValue = mach.getValue( i_Object, prop )
        if prop == "name":
            curPropValue = curPropValue + "_PyObject"
            
        if str(curPropValue) == curPropValue:
            curPropValue = "\"" + curPropValue + "\""
        writeFile.write(setValueStart + objString + ", " + propString + ", " + str(curPropValue) + ")\n")
        if prop == "name":
            writeFile.write("\tcurObject = " + curPropValue + "\n")

"""
This function writes a function to the Available script file that returns this objects current
working directory.  This is necessary if we want to insure that users cannot load an object that doesn't exist
in the current project directory
"""
def writeGetCurrentDirectory( function, writeFile ):
    directoryFunction = "get" + function + "Directory()"
    writeFile.write("def " + directoryFunction + ":\n")
    directory = "\"" + mach.getProjectDirectory() + "\""
    writeFile.write("\treturn " + directory + "\n")
    writeFile.write("if mach.getProjectDirectory() == " + directoryFunction + ":\n")


"""
This function writes the call that tells Mach Studio to place the newly made script into the Available tab
"""
def writeCreateAvailable( i_Object, functionName, writeFile, indent ):
    createAvailableStart = "mach.createScriptObject(\"Add to Scene: "
    if indent == True:
        writeFile.write("\t" + createAvailableStart + i_Object + "\", " + functionName + ")\n\n")
    else:
        writeFile.write(createAvailableStart + i_Object + "\", " + functionName + ")\n\n")            

"""
This function writes the call that tells Mach Studio to place the newly made script into the Available tab
"""
def writeMenuCommand( i_Object, functionName, writeFile, indent ):
    createMenuStart = "mach.createMenuCommand("
    if indent == True:
        writeFile.write("\t" + createMenuStart + "\"" + i_Object + "\", " + functionName + ")\n\n")      
    else:
        writeFile.write(createMenuStart + "\"" + i_Object + "\", " + functionName + ", \"Create\", \"Python Object\")\n\n")         

mach.createCommand("Copy Object as Python Object", createScriptFromObject)


#--------------------------------------------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------------------------------------------


def copyObject():
    filename = "python/clipboard.py"
    copyFile = open(filename, "w")
    for obj in mach.getSelection():
        objectType = mach.getType(obj)
        if objectType == "Objects":
            createObjectStart = "curObject = mach.createObject(\"Characters\",\""
            charFileName = mach.getValue(io_Object, "filename")
            createObjectFinish = "\")"
            copyFile.write(createObjectStart + charFileName + createObjectFinish + "\n")
        elif objectType == "Cameras":
            createObject = "curObject = mach.createObject(\"Cameras\", \"Camera\")"
            copyFile.write(createObject + "\n")
        elif objectType == "Point Lights":
            createObject = "curObject = mach.createObject(\"Point Lights\", \"Point Light\")" 
            copyFile.write(createObject + "\n")
        elif objectType == "Projected Lights":
            createObject = "curObject = mach.createObject(\"Projected Lights\", \"Projected Light\")" 
            copyFile.write(createObject + "\n")
        writeProperties(obj, copyFile, False);
    copyFile.close()


def pasteObject():
    filename = "python/clipboard.py"
    '''if not os.path.isfile(filename):
        print "ERROR: clipboard.py does not exist; there is no copied object"
        return'''    
    pasteFile = open(filename, "r")
    for pLine in pasteFile:
        exec(pLine)

def copyDriver():
    filename = "python/driverClipboard.py"
    copyFile = open(filename, "w")
    for obj in mach.getSelection():
        
        for driver in mach.getSelectedDrivers():
            writeDriverProperties(driver, copyFile)

def pasteObjectDrivers():
    print "paste drivers"

#Driver copy operations
def writeCreateDriver(driverID, writeFile):
   print "createDriver"


    
"""
This function loops through each driver property of the object and writes to file
the method to set a value (mach.setDriverValue) equal to the pre-existing properties
"""
def writeDriverProperties( driverID, writeFile ):
    setValueStart = "mach.setDriverValue("
    driverString = "curDriver"
    for prop in mach.getDriverProperties(driverId):
        propString = "\"" + prop + "\""
        curPropValue = mach.getDriverValue( driverID, prop )
        if str(curPropValue) == curPropValue:
            curPropValue = "\"" + curPropValue + "\""
        writeFile.write(setValueStart + driverString + ", " + propString + ", " + str(curPropValue) + ")\n")


