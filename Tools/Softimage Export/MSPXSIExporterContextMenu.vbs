'
' MSPSoftimageExporterContextMenu.vbs
'
'
' Author     : Koshy George, February 06, 2010


Function XSILoadPlugin( in_reg )

	' Identify this plugin
	in_reg.Name = "MSPSoftImageExporterContextMenu_ResgisterPlugin"
	in_reg.Author = "Koshy George"
	in_reg.Major = 1
	in_reg.Minor = 0

	'Plugin does nothing on linux platform
	if XSIUtils.IsLinuxOS then
		XSILoadPlugin = true
		exit function
	end if

	
		
	' ------------------------------------------------------------------
	' Register the User Normal Editing plugin - we have to do this here
	' since the plugin DLL is located in a folder which is not included
	' in the "XSI_PLUGINS" path by default ; this allows us to register
	' the "XSI_UserNormalEditing" property handler which will be called
	' if needed when opening the tool using the RMB contextual menu for
	' the first time.
	' ------------------------------------------------------------------

	dim path, pos, i, curSel

	' get the plugin installation path
	path = in_reg.OriginPath
	slash = XSIUtils.Slash
	pos = InStrRev( path, slash)
	if ( pos <  ( Len(path) - 1) )  then
		path = path + slash
	end if
	path = path + "XSIMSPExporter.dll"
	LogMessage path 
	
	' Register the plugin ( location below user installation path )
	Application.LoadPlugin( path )

	in_reg.RegisterCommand "MSPMeshflagsDelete_Open"
	in_reg.RegisterCommand "MSPMeshflagsAdd_Open"


	in_reg.RegisterMenu siMenuSEGeneralContextID, "MSPMeshflagsAdd_Entry", false

	in_reg.RegisterMenu siMenuSELayersContextID, "MSPMeshflagsAdd_Entry", false

	in_reg.RegisterMenu siMenuSEPassesContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEPartitionsContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEObjectContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEGroupContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEAnimContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEClusterContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEOperatorContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEConstraintContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEPreferenceContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEMaterialContextID, "MSPMeshflagsAdd_Entry", false
	
	in_reg.RegisterMenu siMenuSEModelContextID, "MSPMeshflagsAdd_Entry", false



	

	in_reg.RegisterMenu siMenuSEGeneralContextID, "MSPMeshflagsDelete_Entry", false

	in_reg.RegisterMenu siMenuSELayersContextID, "MSPMeshflagsDelete_Entry", false

	in_reg.RegisterMenu siMenuSEPassesContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEPartitionsContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEObjectContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEGroupContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEAnimContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEClusterContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEOperatorContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEConstraintContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEPreferenceContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEMaterialContextID, "MSPMeshflagsDelete_Entry", false
	
	in_reg.RegisterMenu siMenuSEModelContextID, "MSPMeshflagsDelete_Entry", false
	
	
	XSILoadPlugin = true
End Function 



Function MSPMeshflagsAdd_Entry_Init( in_context )

	' Get the menu object from the Context input
	set oMenu = in_context.Source

	' Add the command item using our custom command
	oMenu.AddCommandItem "MSPMeshflagsAdd", "MSPMeshflagsAdd_Open"




	' Finish with success notification
	MSPMeshflagsAdd_Entry_Init = true

End Function 




Function MSPMeshflagsDelete_Entry_Init( in_context )

	' Get the menu object from the Context input
	set oMenu = in_context.Source

	' Add the command item using our custom command
	oMenu.AddCommandItem "MSPMeshflagsDelete", "MSPMeshflagsDelete_Open"




	' Finish with success notification
	MSPMeshflagsDelete_Entry_Init = true

End Function 


Function AddPropIfNotFound( in_obj, in_prop )

	LogMessage "Adding property: " & in_prop & " to " & in_obj 

	set props = in_obj.Properties
	set val = props.Item( in_prop )
	'LogMessage in_obj & "." & in_prop & "= "  & val		
	if ( TypeName( val) = "Nothing" ) then			'
		LogMessage in_obj & "doesnt have " & in_prop & " property"
		set foo = in_obj.AddProperty( "CustomProperty", false, in_prop )
		InspectObj foo
	else
		LogMessage curSelName & "." & in_prop & "=" & val
	end if
End Function



Function DelPropIfFound( in_obj, in_prop )

	LogMessage "Deleting property: " & in_prop & " to " & in_obj 

	set props = in_obj.Properties
	set val = props.Item( in_prop )
	'LogMessage in_obj & "." & in_prop & "= "  & val		
	if ( TypeName( val) <> "Nothing" ) then			'
		LogMessage in_obj & "have " & in_prop & " property"
		DeleteObj val
	else
		LogMessage curSelName & "doesnt have " & in_prop 
	end if
End Function


Function MSPMeshflagsAdd_Open_Execute()
	dim curSel, curSelName
	set sel = Application.Selection
	'Finish with success notification
		LogMessage "Number of object selected = " & sel.Count
	for i = 0 to (sel.Count - 1)
		curSelName = sel(i).Name
		curSel = sel(i)
		Application.LogMessage curSel
		set oGeoms = sel(i).FindChildren( , , siGeometryFamily, true )
		for each geomobj in oGeoms
			
			AddPropIfNotFound geomObj, "MSPMeshflags" 		
		next
	next
	MSPMeshflagsAddOpen_Execute = true

End Function 

Function MSPMeshflagsDelete_Open_Execute()
	dim curSel, curSelName
	set sel = Application.Selection
	'Finish with success notification
		LogMessage "Number of object selected = " & sel.Count
	for i = 0 to (sel.Count - 1)
		curSelName = sel(i).Name
		curSel = sel(i)
		Application.LogMessage curSel
		set oGeoms = sel(i).FindChildren( , , siGeometryFamily, true )
		for each geomobj in oGeoms
			
			DelPropIfFound geomObj, "MSPMeshflags" 		
		next
	next
	MSPMeshflagsDeleteOpen_Execute = true

End Function 
