REM =======================================================
DIM objApp
DIM actDoc
DIM docRef
DIM imgfolder
DIM fileRef
Dim fsoRef
DIM hpath
Set objApp = CreateObject("Photoshop.Application")

Set WshShell = Wscript.CreateObject("Wscript.Shell")
hpath=WshShell.RegRead("HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders\Personal")


If objApp.Documents.Count > 0 Then
	Set actDoc = objApp.ActiveDocument
End If
Set fsoRef = CreateObject( "Scripting.FileSystemObject" )

Set imgfolder = fsoRef.GetFolder(hpath & "/StudioGPU/MachStudio Pro/Photoshop/")



Set filecollection = imgfolder.Files
extType = 2

For Each fileRef in filecollection
	Set docRef = objApp.Open(fileRef.Path)
	If Err.Number <> 0 Then
		WScript.echo "Unable to open " & fileRef.path
		Err.Clear
	Else
		
		docRef.ArtLayers(1).Duplicate(actDoc)
		
	End If
Next
