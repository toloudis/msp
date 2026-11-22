#==============================================================================
#   Example.py
#
#   This script will outline a simple approach to creating a command using
#   the mach python module.  This command will then be set to show up in
#   Mach Studio's "Scripts" menu as a command option, which will also
#   allow users to set the command as a hot key in Preferences->Hot Keys.
#
#   You can uncomment the code below to try it out.
#
#==============================================================================

#------------------------------------------------------------------------------
#   The mach module gives access to python commands that will execute
#   a number of different Mach Studio operations, such as returning the
#   name of an object or setting the position of an object.
#------------------------------------------------------------------------------
#import mach

#------------------------------------------------------------------------------
# printSelectedProperties - This will print each selected object's properties
#   and the values of those properties.  Anytime there is a print operation,
#   the result will appear in the "Python Dialog" which is accessible through
#   the menu item Scripts->Python Dialog.
#------------------------------------------------------------------------------
#def printSelectedProperties():
#    for obj in mach.getSelection():
#       print "***", obj
#       for p in mach.getProperties(obj):
#          print p, mach.getValue(obj, p)

#------------------------------------------------------------------------------
# mach.createCommand will place a python function in the "Scripts" menu of
#   Mach Studio.  The first parameter is the text that will appear in the "Scripts"
#   menu and the second parameter is the function that will be executed.
#
#   One thing to note: functions that are executed via the createCommand
#   operation must not have any parameters.
#
#   Using the mach.createCommand function also places the script command into
#   the Hot Keys preferences (Edit->Preferences->Hot Keys, under the "Python"
#   section).  Here users can define a key press that will execute the custom
#   script.
#------------------------------------------------------------------------------
#mach.createCommand("Print Property Values", printSelectedProperties)
