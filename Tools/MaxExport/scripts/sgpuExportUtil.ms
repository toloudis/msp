
filein ((GetDir #Scripts) +"\SgpuScripts\sgpuLib.ms")

global r_dialogue --stores the sgpuLayerUtil
global r_export_dialogue --stores the sgpuExportUtil

global suggestedWidthOfMasterRolloutFloater_SgpuExport=450
global suggestedHeightOfMasterRolloutFloater_SgpuExport=400

global g_export_dir

--This max script implements a selection set like functionality, for sgpu export nprocessing.
--Let us call this by the term 'sgpuSgpuLayer'
--Currently, an object can be in only one sgpuSgpuLayer




utility sgpuExportUtil "sgpuExport" (


	local rolloutArray
	
	rollout rDoExport "Do Export"  height:100 
	(

    
		local srcpos = [5, 70]
		editText rExportDir "ExportDir:" fieldWidth:300 align:#left across:2
		button rExportDirBrowser "" fieldWidth:40 align:#right 

		button rDo "Export!" width:100 height:100 across:2
		editText lExporterVersion caption:"Version" text:( sgpuLib.getVersionLabel() )bold:True  readOnly:true labelOnTop:false  width:200 height:65

		function updateRExportDir_RExportGUI  exportFiledir = (
			
			rExportDir.text = exportFiledir
		)

		on rDoExport open do 
		(
			rExportDir.enabled = False
			if ( g_export_dir != undefined ) then (
				updateRExportDir_RExportGUI g_export_dir
			)
		)

		on rDoExport close do 
		(
		)
		
		on rExportDir entered theText do 
		( 
		)
		
		
		on rExportDirBrowser pressed do 
		( 
			local exportFiledir=""
			if( ( g_export_dir != undefined ) and ( g_export_dir.count > 0 ) ) then ( 
				exportFiledir = getSavePath caption:"Sgpu Export Dir" initialDir:g_export_dir
			) else (
				exportFiledir = getSavePath caption:"Sgpu Export Dir"
			)
			if ( ( exportFiledir == undefined ) or ( exportFileDir.count <= 0 ) ) then (
				exportFileDir = ""
			) else if ( not ( isDirectoryWriteable exportFiledir ) ) then (
				exportFiledir = ""
			) else (
				 g_export_dir = exportFiledir
			)
			
			
			format "SgpuExportUtil::Info exportFiledir %\n" g_export_dir
			updateRExportDir_RExportGUI( exportFiledir )
			
			
		)
		
		on rDo pressed do 
		( 
			if( ( g_export_dir != undefined ) and ( g_export_dir.count > 0 ) ) then (
				
				assert( isDirectoryWriteable g_export_dir )
				if ( maxFilename.Count == 0 ) then (
					format "SgpuExportUtil::Error, No Max file loaded!!\n" 
				) else (
					fullpath = maxFilePath + maxFilename
					sgpuExportUtil.foo fullpath g_export_dir
					
					
				)
			)
		)
	)



	function makeLayerName lname opts = (
		exportIntentDecsription = case (opts.eExportIntent ) of (
			1: "vertex animation export options"
			2: "camera animation export options"
			default: "model export options"
		)
		("layer '" + lname + "' " + exportIntentDecsription  )
	)


	function makeModelExportRollout rName = (
		local rolloutDlg		
		local rNameLocal = "sgpu_" + rName 
		local rOpts =  sgpuLib.getSgpuOptsForSgpuLayerFromDb_core rName
		rcFoo = rolloutCreator  rNameLocal ( makeLayerName rName  rOpts )  height:100
		rcFoo.begin()
		rcFoo.addText ("local sgpuLayer = (\"" + rName + "\" as string)" ) filter:off
		rcFoo.addText "local curOpts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core sgpuLayer"
		
		rcFoo.addControl #checkBox "exportChildIfParentExports"  "exportChildIfParentExports" paramStr:(" across:2 align:#left checked:" + (rOpts.bExportChildIfParentExports as string) )
		rcFoo.addControl #checkBox "exportThis"  "export this" paramStr:(" align:#right checked:" + (rOpts.bExportSelected as string) )
		
		rcFoo.addControl #checkBox "mergeTriMeshesBasedOnMaterials"  "mergeTriMeshes" paramStr:(" across:2 align:#left checked:" + (rOpts.bMergeBasedOnMtls as string) )
		rcFoo.addControl #editText "maxNumTrianglesInMergedMesh" "MaxNumTris:"  paramStr:(" width: 170 align:#right text:" + "(" + (rOpts.nMaxNumTrianglesInMergedMesh as string) + " as string)" )
		
		rcFoo.addHandler #exportThis #changed paramStr:"state" codeStr:"curOpts.bExportSelected = state ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #exportChildIfParentExports #changed paramStr:"state" codeStr:"curOpts.bExportChildIfParentExports = state ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #mergeTriMeshesBasedOnMaterials #changed paramStr:"state" codeStr:"curOpts.bMergeBasedOnMtls = state ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #maxNumTrianglesInMergedMesh #entered paramStr:"txt" codeStr:"curOpts.nMaxNumTrianglesInMergedMesh = ( sgpuLib.safeInt  curOpts.nMaxNumTrianglesInMergedMesh  txt) ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		
		
		rolloutDlg = rcFoo.end()
		--print rcFoo.str
		rolloutDlg
		
	)
	
	
	function makeVertexAnimExportRollout rName = (
		
		local rolloutDlg		
		local rNameLocal = "sgpu_" + rName
		local rOpts =  sgpuLib.getSgpuOptsForSgpuLayerFromDb_core rName
		local eGeomExport = case (rOpts.bExportGeom) of (
						True: 1
						False: 2
						)
		rcFoo = rolloutCreator  rNameLocal ( makeLayerName rName  rOpts )  height:100
		rcFoo.begin()
		rcFoo.addText ("local sgpuLayer = (\"" + rName + "\" as string)" ) filter:off
		rcFoo.addText "local curOpts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core sgpuLayer"
		
		--radiobuttons theIntent labels: sgpuLib.exportIntentNames
		rcFoo.addControl #radiobuttons "exportGeomOrAnim" "What to export?" paramstr: (" across:2 align:#left labels: (sgpuLib.animExportChoices) default: " + ( eGeomExport as string ) )
		rcFoo.addControl #checkBox "exportThis"  "export this" paramStr:(" align:#right checked:" + (rOpts.bExportSelected as string) )
		
		rcFoo.addControl #checkBox "useCompressionForVertexAnimation"  "UseCompression"  paramStr: (" across:2 align:#left checked:" + ( rOpts.bCompressVertexAnim as string ) )
		rcFoo.addControl #editText "toleranceForVertexAnimation" "Tol:" paramStr:(" width:100  align:#right text: " + "( " + ( rOpts.fToleranceVertexAnim as string ) + " as string)" )
		rcFoo.addControl #editText "startFrame" "StartFrame:" paramStr:(" width:130 across: 2 align:#left text:" + "( " + ( rOpts.fStartFrame as string ) + " as string)" )
		rcFoo.addControl #editText "endFrame" "EndFrame:" paramStr:(" width:130 align:#right text:" + "( " + ( rOpts.fEndFrame as string ) + " as string)" )
		
		
		rcFoo.addHandler #exportThis #changed paramStr:"state" codeStr:"curOpts.bExportSelected = state ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #exportGeomOrAnim #changed paramStr:"state" codeStr:"local bVal = case state of (\n\t 2: (False)\n\t default: (True)\n\t ); curOpts.bExportGeom = bVal; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #useCompressionForVertexAnimation #changed paramStr:"state" codeStr:"curOpts.bCompressVertexAnim = state ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #toleranceForVertexAnimation #entered paramStr:"txt" codeStr:"curOpts.fToleranceVertexAnim = ( sgpuLib.safeFloat  curOpts.fToleranceVertexAnim  txt) ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #startFrame #entered paramStr:"txt" codeStr:" sgpuLib.setStartFrame curOpts ( sgpuLib.safeFloat  curOpts.fStartFrame  txt) ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #endFrame #entered paramStr:"txt" codeStr:"sgpuLib.setEndFrame curOpts ( sgpuLib.safeFloat  curOpts.fEndFrame  txt) ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "

		rolloutDlg = rcFoo.end()
		--print rcFoo.str
		rolloutDlg
		
	)

	
	
	function makeCameraAnimExportRollout rName  = (
	
		
		local rolloutDlg		
		local rNameLocal = "sgpu_" + rName 
		local rOpts =  sgpuLib.getSgpuOptsForSgpuLayerFromDb_core rName
		rcFoo = rolloutCreator  rNameLocal ( makeLayerName rName  rOpts )  height:100
		rcFoo.begin()
		rcFoo.addText ("local sgpuLayer = (\"" + rName + "\" as string)" ) filter:off
		rcFoo.addText "local curOpts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core sgpuLayer"
	
		rcFoo.addControl #checkBox "exportThis"  "export this" paramStr:(" align:#right checked:" + (rOpts.bExportSelected as string) )
	
		rcFoo.addControl #editText "startFrame" "StartFrame:" paramStr:(" width:130 across:2 align:#left text:" + "( " + (rOpts.fStartFrame as string ) + " as string)" )
		rcFoo.addControl #editText "endFrame" "EndFrame:" paramStr:(" width:130 align:#right text:" + "( " + (rOpts.fEndFrame as string ) + " as string)" )
	
		rcFoo.addHandler #exportThis #changed paramStr:"state" codeStr:"curOpts.bExportSelected = state ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "

		rcFoo.addHandler #startFrame #entered paramStr:"txt" codeStr:" sgpuLib.setStartFrame curOpts ( sgpuLib.safeFloat  curOpts.fStartFrame  txt) ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "
		rcFoo.addHandler #endFrame #entered paramStr:"txt" codeStr:"sgpuLib.setEndFrame curOpts ( sgpuLib.safeFloat  curOpts.fEndFrame  txt) ; sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer curOpts "

	
	
	
		rolloutDlg = rcFoo.end()
		rolloutDlg
		
	)

	function foo fMaxFileName fExportDir = (
			
		existingLayersFromObjects = #()
		sgpuLib.getExistingSgpuLayersFromObjects_core &existingLayersFromObjects
		for sgpuLayer in existingLayersFromObjects do (
			local fExportPath = fExportDir + "\\" + sgpuLayer 
			format "SgpuExportUtil::Info, exporting file to %\n" fExportPath
			local objectsInThisLayer = #()
			try (
				sgpuOpts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core sgpuLayer
				if ( sgpuOpts == undefined ) then (
					format "SgpuExportUtil::Error: layer % is not in the database\n" sgpuLayer
					continue
				)
				if ( not sgpuOpts.bExportSelected ) then  (
			
					continue
				)
				sgpuOpts.Set_CurMaxFilepath fMaxFileName
				sgpuLib.getObjectsWithSgpuLayer_core sgpuLayer &objectsInThisLayer			
				exportResult = SgpuExport.Do_2 sgpuOpts fExportPath objectsInThisLayer
				if ( exportResult > 0 ) then (
					format "SgpuExportUtil::Info, export succeeded\n"
					format "\t max file name: % \n" fMaxFilename
					format "\t export Path: % \n" fExportPath
					format "\t options : %\n" ( sgpuOpts.LogToString() )
				)
			) catch ()
		)
		
		
	)

 	function refreshSgpuExportUtil= (

	)
 	function closeDownSgpuExportUtil= (

		--format "calling closeDownSgpuExportUtil\n"
		for rItem in rolloutArray do (
			removeRollout rItem
		)

		removeRollout sgpuExportUtil.rDoExport
	)

 	function startSgpuExportUtil = (		
	
		--format "calling startSgpuExportUtil\n"



		addrollout sgpuExportUtil.rDoExport r_export_dialogue


		rolloutArray = #()		
		existingLayersFromObjects = #()
		sgpuLib.getExistingSgpuLayersFromObjects_core &existingLayersFromObjects
		for sgpuLayer in existingLayersFromObjects do (
			opts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core sgpuLayer
			if ( opts == undefined ) then (
				format "SgpuExportUtil::Error, layer % is not in db\n" sgpuLayer
				continue
			)
			try (
				rolloutDlg = 	case (opts.eExportIntent) of (
							1: ( makeVertexAnimExportRollout sgpuLayer )
							2:  ( makeCameraAnimExportRollout sgpuLayer )
							default:  ( makeModelExportRollout sgpuLayer )
						)
				append rolloutArray rolloutDlg
			) catch ( 
				format "SgpuExportUtil:Error, not able to create an export subrollout for %, pl. verify the layer name is proper\n" sgpuLayer			
			)
			
		)
		
		for rItem in rolloutArray do (
			addRollout rItem  r_export_dialogue
		)
		
	)



)


	
	

macroScript sgpuExportMacro
            category:"sgpu" 
            internalcategory:"sgpu" 
            tooltip:"Export To Sgpu" 
            buttontext:"sgpu export" 
            Icon:#("standard_16i",4)
(


	
		
	function registerSgpuExportScripts = 
	(
		callbacks.addScript #systemPreReset "sgpuExportUtil.closeDownSgpuExportUtil()" id:#sgpuExport
		callbacks.addScript #systemPostReset "sgpuExportUtil.startSgpuExportUtil()" id:#sgpuExport		
		callbacks.addScript #systemPreNew "sgpuExportUtil.closeDownSgpuExportUtil()" id:#sgpuExport
		callbacks.addScript #systemPostNew "sgpuExportUtil.startSgpuExportUtil()" id:#sgpuExport		
		--callbacks.addScript #filePreNew "sgpuExportUtil.closeDownSgpuExportUtil()" id:#sgpuExport
		--callbacks.addScript #filePostNew "sgpuExportUtil.startSgpuExportUtil()" id:#sgpuExport	
		callbacks.addScript #filePreOpen "sgpuExportUtil.closeDownSgpuExportUtil()" id:#sgpuExport
		callbacks.addScript #filePostOpen "sgpuExportUtil.startSgpuExportUtil()" id:#sgpuExport		
		callbacks.addScript #filePreMerge "sgpuExportUtil.closeDownSgpuExportUtil()" id:#sgpuExport		
		callbacks.addScript #filePostMerge "sgpuExportUtil.startSgpuExportUtil()" id:#sgpuExport		
		callbacks.addScript #preImport "sgpuExportUtil.closeDownSgpuExportUtil()" id:#sgpuExport		
		callbacks.addScript #postImport "sgpuExportUtil.startSgpuExportUtil()" id:#sgpuExport
		
		callbacks.addScript #nodeCloned "sgpuExportUtil.refreshSgpuExportUtil()" id:#sgpuExport		
		callbacks.addScript #nodeCreated "sgpuExportUtil.refreshSgpuExportUtil()" id:#sgpuExport
		
		callbacks.addScript #sceneNodeAdded "sgpuExportUtil.refreshSgpuExportUtil()" id:#sgpuExport
		callbacks.addScript #nodeHide "sgpuExportUtil.refreshSgpuExportUtil()" id:#sgpuExport
		callbacks.addScript #nodeUnhide "sgpuExportUtil.refreshSgpuExportUtil()" id:#sgpuExport
		callbacks.addScript #nodePostDelete "sgpuExportUtil.refreshSgpuExportUtil()" id:#sgpuExport
		callbacks.addScript #nodeRenamed "sgpuExportUtil.refreshSgpuExportUtil()" id:#sgpuExport

		callbacks.addScript #animationRangeChange "sgpuLib.animationRangeChangeRefresh()" id:#sgpuExport
	)

   	on execute do (
		
		-- close the old rollout floater if it exists

		try( closerolloutfloater r_dialogue )catch ()
		
		try( closerolloutfloater r_export_dialogue )catch ()

		maxVerArray = maxVersion()
		if( maxVerArray[1] >= 12000  ) then ( --max 2010 and above

			-- put up new rollout floater and add rollout to it.

			r_export_dialogue = newrolloutfloater "SgpuExport" suggestedWidthOfMasterRolloutFloater_SgpuExport suggestedHeightOfMasterRolloutFloater_SgpuExport

		
			callbacks.removeScripts id:#sgpuExport
			registerSgpuExportScripts()
			sgpuExportUtil.startSgpuExportUtil()
	
			--callbacks.show id:#sgpuExport
		) else (
		
			format "SgpuExportUtil::Error: sgpu max scripts are supported only  for max 2010 and above\n"
		)
	)


	on isChecked do (
		
		sgpuExportUtil.rDoExport.open
	)


	on closeDialogs do (
		
		--callbacks.show id:#sgpuExport
		callbacks.removeScripts id:#sgpuExport
		closerolloutfloater r_export_dialogue
		

	)
)


try
(
	local sgpuMenu;-- = menuMan.findMenu "sgpu"
	-- Get the main menu bar 
	local mainMenuBar = menuMan.getMainMenuBar();
	for i = 1 to mainMenuBar.numItems() do
	(
		if ((mainMenuBar.GetItem i).GetTitle()) == "sgpu"  then
			sgpuMenu = (mainMenuBar.GetItem i).getSubMenu()
	)
	-- create a menu item that calls the sample macroScript 
	local sgpuExportUtilAdded = FALSE;
	for i = 1 to sgpuMenu.numItems() do
	(
		if ((sgpuMenu.GetItem i).GetTitle()) == "sgpu export"  then sgpuExportUtilAdded = true		
	)
	if not sgpuExportUtilAdded then (
		local itemsgpuExportUtilAdded = menuMan.createActionItem "sgpuExportMacro" "sgpu" 
		if itemsgpuExportUtilAdded != undefined then(
			sgpuMenu.addItem itemsgpuExportUtilAdded -1 
			-- redraw the menu bar with the new item 
			menuMan.updateMenuBar() 
		)
	)

)catch()