#=====================================================================================
#    VideoLog.py
#
#    Routines to dump the video statistics onto a log file.
#=====================================================================================

import mach

def videoLog():
	mach.logVideoStats()

mach.createCommand("Log Video Statistics", videoLog)