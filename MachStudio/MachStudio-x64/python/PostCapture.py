#=============================================================================
#	PostCapture.py
#
#	Execute this file after rendering has completed.
#
#
#	2007-07-31 rjk
#							-	grab list of files and copy them to the appropriate folder(s)
#								on the server
#							-	write CSV file with information for render report
#
#	TO DO
#		- dialog asking user to copy yes or no
#		- send email
#		- 
#=============================================================================
#from Tkinter import *
#from tkMessageBox import *
#import tkFileDialog
#import sys,glob, smtplib, MimeWriter, base64, StringIO
#import os
#import Tkinter
import os
import shutil
#import MimeWriter
#import StringIO
#import smtplib
import csv
import datetime


#  File "<string>", line 1, in <module>
#  File "C:\projects\MachStudio\python\PostCapture.py", line 8, in <module>
#    import Tkinter
#  File "C:\Python25\Lib\lib-tk\Tkinter.py", line 37, in <module>
#    import FixTk # Attempt to configure Tcl/Tk without requiring PATH
#  File "C:\Python25\Lib\lib-tk\FixTk.py", line 24, in <module>
#    import _tkinter
#ImportError: No module named _tkinter

#=============================================================================
#	return the user name
#=============================================================================
def userName():
	ostype=sys.platform
	if ostype == 'darwin':
		user=os.environ['LOGNAME']
	if ostype == 'win32':
		user=os.environ['USERNAME']
	return (user)


#=============================================================================
# return the machine
#=============================================================================
def machineName():
	ostype=sys.platform
	if ostype == 'darwin':
		user=os.environ['COMPUTERNAME']
	if ostype == 'win32':
		user=os.environ['COMPUTERNAME']
	return (user)

	
#=============================================================================
#	send email
#=============================================================================
def email(filelist):
	user = userName()
	machine = machineName()
	sendto = user + '@extralargetech.com'
	print 'Sending email to ' + sendto + '\n'

	temp = 'Files copied to the server\n'
	for obj in filelist:
		temp = temp + '[' + obj[0] + ']\n'
	temp = temp + 'by ' + user + ' on machine ' + machine
	print temp

#	message = StringIO.StringIO()
	
#	writer = MimeWriter.MimeWriter(message)
#	writer.addheader('Subject','[Copy List Addition] ' + user + ' added ' + "directory") #directory)
#	writer.startmultipartbody('mixed')
	
	# start off with a text/plain part
#	part = writer.nextpart()
#	body = part.startbody('text/plain')
#	body.write('\n\n' + temp)
#	writer.lastpart()
	# send the mail
#	smtp = smtplib.SMTP('64.150.166.191') #'66.235.193.142')
# ---- add attachment! ---- "rendered.csv"
#	smtp.sendmail('apache@extralargetech.com',sendto, message.getvalue())
#	smtp.quit()

#=============================================================================
#	write CSV file
#=============================================================================
def createCSV(filelist):
	user=userName()
	machine=machineName()
	today = datetime.date.today()
	
	print "Creating CSV file"
	file = open("rendered.csv", "w")
	for obj in filelist:
		row = obj[0] + ',' + today.strftime("%Y-%m-%d") + ',' + machine + ',' + user
		#print "row=" + row
		file.write(row)
	file.close()

#Almost Nothing-long-render_CAM01A_jpeg.mov
#    row = obj[0] + ',' + today + ',' + machine + ',' + user
#TypeError: cannot concatenate 'str' and 'datetime.date' objects



#=============================================================================
#	the Actual Program
#=============================================================================

#-----------------------------------------------------------------------------
#	Get the list of files rendered
#-----------------------------------------------------------------------------
files = mach.getValue("CapturePrefs", "renderedFiles")
#scenefile = mach.getValue("CapturePrefs", 

#-----------------------------------------------------------------------------
# Ask the user if they want to copy the files to the server
#-----------------------------------------------------------------------------
#top = Tkinter.Tk()
#
#question = Tkinter.Label(top, test='Copy files to server');
#question.pack()
#yesbutton = Tkinter.Button(top, text='Yes', command=top.quit, bg='red', fg='white')
#yesbutton.pack()
#nobutton = Tkinter.Button(top, text='No', command=top.quit, bg='red', fg='white')
#nobutton.pack()

# TO DO - add a list of all the files about to be copied to the dialog

#Tkinter.mainloop()

# TO DO - check yes or no buttons state?

#-----------------------------------------------------------------------------
# Get the list of rendered files and copy them to the server
#-----------------------------------------------------------------------------
print "-------------------------Post Capture START"

#	loop through all the files (images and movies) just rendered and
#	copy them to the server
#
destfiles = []

for obj in files:
	#	Get one of the files
	srcfile = obj[0]

	#	change "c:\projects" to be on the server
	destfile = 's:' + srcfile[11:]
	destfile = destfile.strip()
	destfiles.append([destfile])

	#	create the directory if not found
	destpath = destfile[0:destfile.rindex('\\') + 1]
	if not os.path.isdir(destpath):
		os.makedirs(destpath)

	#	copy the files
	print '- ' + destfile
	shutil.copy2(srcfile,destfile)

	#	verify the file got copied correctly
	if os.path.isfile(destfile): 
		print "          copy completed"
	else:
		print "          copy failed"

#	create the CSV file
createCSV(destfiles)

#	now send the email
email(destfiles)
print "-------------------------Post Capture FINISH"

