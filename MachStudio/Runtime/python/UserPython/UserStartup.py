#==============================================================================
#   UserStartup.py
#
#   Gets executed on launch of Mach Studio.
#
#   This file should contain your imports to user-created python scripts
#==============================================================================

#   for debug console
print "Executing UserStartup.py"

#==============================================================================
#
#   The file location of UserStartup.py must be defined in the user's preferences
#   of Mach Studio (Edit->Preferences; under the Python section).  Each
#   custom script created by the user to be included as a Python script must
#   also exist in the same directory as UserStartup.py
#
#   UserStartup.py defines which user-created scripts should be loaded
#   into Mach Studio on start-up.  This is done by importing each script as
#   shown below.  Each script can define commands that can perform a number
#   of operations in Mach Studio and these commands can be set as menu items
#   and hot keys.  See Example.py for a sample script of setting a command to
#   a menu item.
#       
#==============================================================================

#   importing Example.py script for use in Mach Studio
#import Example

import SceneConversion
