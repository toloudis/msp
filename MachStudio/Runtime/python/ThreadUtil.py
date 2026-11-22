#******************************************************************************
#  ThreadUtil.py
#       Will hold various functions that will allow 3rd party apps which require
#       their own thread to send an event request to MachStudio
#******************************************************************************
import mach
import wx

#------------------------------------------------------------------------------
# Instead of calling app.MainLoop() to open a new wx.App window, call runWxApp
# to create a link between the wx window and MSP's main thread.
# This function takes in the app and the frame/window used as the app's interface
#------------------------------------------------------------------------------
def runWxApp(app, window):
        def processWxEvents():
                while event_loop.Pending():
                        event_loop.Dispatch()
                        app.ProcessPendingEvents()

        #define a custom close event that removes the processWxEvents function from our
        #callback list
        def onClose(event):
                window.Show(False)
                window.Destroy()
                processWxEvents()
                mach.removeEventCallback(processWxEvents)
        window.Bind(wx.EVT_CLOSE, onClose)

        #add processWxEvents to our callback list
        event_loop = wx.EventLoop()
        wx.EventLoop.SetActive(event_loop)
        mach.addEventCallback(processWxEvents)
