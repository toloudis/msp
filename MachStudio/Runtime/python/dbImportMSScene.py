import os
import os.path
import shutil
import mach

def dbGetCamListFromProjectDir():
    cams = []
    
    cam_dir = mach.getProjectDirectory() + "\\Data\\" + mach.getSceneName() + "\\Sets\\General\\Data\\"
    dirList = os.listdir(cam_dir)
    for file in dirList:
        fileUpper = file.upper()
        if( fileUpper.find('CAM') != -1 ):
            cams = cams + [file]
    
    cams.sort()
    
    return cams

def dbCreateCameras():
    camera = mach.createObject("Cameras", "Perspective Camera")
    mach.setValue(camera, "name", "Maya Cam")
    camera = "Maya Cam"

    cams = dbGetCamListFromProjectDir()
    
    shotList = [['', 0.0]]
    
    endTime = 0.0
    
    for cam in cams:
        shot = cam[:-4]
        shotList[-1][0] = shot
        driver = mach.createDriver(camera, "Maya Script")
        mach.setDriverValue(driver, "animationFileName", cam)
        mach.setDriverValue(driver, 'beginTime', shotList[-1][1])

        if(cam != cams[-1]):
            shotList = shotList + [['', mach.getDriverValue(driver, 'endTime')]]
        else:
            endTime = mach.getDriverValue(driver, 'endTime')
    
    mach.setMaximumTime(endTime) 

    return shotList

def dbGetShotListFromCamera(camera):
    drivers = mach.getDriversForChannel("Maya Cam", "position")
    shotList = []
    
    for driver in drivers:
        shot = mach.getDriverValue(driver, 'animationFileName')
        startTime = mach.getDriverValue(driver, 'beginTime')
        shotList = shotList + [[shot[:-4], startTime]]
        
    return shotList

def dbAttachCharacterAnimation( char, shotList, suffix = '' ):
    anims = []
    char = char.upper()
    suffix = str(suffix)
        
    # Get the placed Character
    allChars = mach.getPlacedOfType("Characters")
    charFile = ''
    for tempChar in allChars:
        if( tempChar.upper().find(char) != -1 ):
            charFile = tempChar
        if( tempChar.upper() == char ):
            charFile = tempChar
            char = char[0:char.find('_')].upper()
    
    if( charFile == '' ):
        print "Character not found"
        return 0
    
    # Delete the existing drivers
    existingDrivers = mach.getDriversForChannel(charFile, "animFull")
    for driver in existingDrivers:
        dbUpdateDriverToShotlist(driver)
        
    # Get the cha list attached to the character
    anim_dir = mach.getProjectDirectory() + "\\Data\\" + mach.getSceneName() + "\\Characters\\General\\Models\\"
    dirList = os.listdir(anim_dir)
    for file in dirList:
        charS = char + suffix + ".CHA"
        if( file.upper().find('CHA') != -1 and file.upper().find(charS) != -1 ):
           anims = anims + [file]
    
    for anim in anims:
        charShot = anim[0:11]
        for shot in shotList:
            if(charShot == shot[0]):
                if dbAnimDriverExists(charFile, anim):
                    print (anim + " animation driver already exists. Delete if you'd like to replace")
                else:
                    animDriver = mach.createDriver(charFile, 'Anim-Full')
                    mach.setDriverValue(animDriver, 'animFileName', anim)
                    mach.setDriverValue(animDriver, 'beginTime', shot[1])

    
    return anims, charFile 

def dbAttachAnimationBySelection(suffix = ''):
    selected = mach.getSelection()
    shotList = []

    cams = mach.getPlacedOfType('Cameras')
    MayaCamExists = 0
    for cam in cams:
        if( cam == 'Maya Cam'):
            mach.deleteObject('Maya Cam')
            MayaCamExists = 0
            
    shotList = dbCreateCameras()
    
    for obj in selected:
        if(mach.getType(obj) == "Characters"):
            dbAttachCharacterAnimation( obj, shotList, suffix )
        else:
            print "Object Type selected is not supported by this script."
    
    return 1

def dbAnimDriverExists(char, animFile):
    drivers = mach.getDriversForChannel(char, "animFull") 
    exists = False
    
    for driver in drivers:
        existingFile = mach.getDriverValue(driver, 'animFileName')
        if(existingFile == animFile):
            exists = True
    
    return exists

def dbUpdateDriverToShotlist(driver):
    shotList = dbGetShotListFromCamera('Maya Cam')
    driverAnim = mach.getDriverValue(driver, 'animFileName')
    
    for shot in shotList:
        if( driverAnim.upper().find(shot[0]) != -1 ):
            mach.setDriverValue(driver, 'beginTime', shot[1])

def dbMSFileTroller(directory):
    chaFiles = []
    camFiles = []
    directoryList = os.listdir(directory)

    for file in directoryList:
        tmp = directory + "\\" + file
        if(os.path.isfile(tmp)):
            if(tmp.upper().endswith('CHA')):
                if(tmp not in chaFiles):
                    chaFiles.append(tmp)
            elif(tmp.upper().endswith('CAM')):
                if(tmp not in camFiles):
                    camFiles.append(tmp)
        elif(os.path.isdir(tmp)):
            returnFiles = dbMSFileTroller(tmp)
            chaFiles = chaFiles + returnFiles[0]
            camFiles = camFiles + returnFiles[1]
    
    return chaFiles, camFiles

def dbOrganizeApprovedFiles():
    source = 'S:/PTE01/Yian Studio/Downloads/Approved Files/'
    destination = 'S:/PTE01/Yian Studio/approved/'
    
    dateList = os.listdir(source)
    for date in dateList:
        folderList = os.listdir(source + date + '/')
        for folder in folderList:
            scene = folder[:6]
            if(folder.upper().startswith('C')):
                scene = folder[1:7]
            
            if(not os.path.exists(destination + scene + '0/')):
                os.mkdir(destination + scene + '0/')
            
            src = source + date + '/' + folder
            dst = destination + scene + '0/' + folder
            #print('copying ' + src + ' to ' + dst + '\n')
            
            if(os.path.exists(dst)):
                dst = dst + '_' + date
                
            shutil.copytree(src, dst, symlinks=0)
            