
filein ((GetDir #Scripts) +"\SgpuScripts\sgpuLib.ms")


global r_dialogue --stores the sgpuLayerUtil
global r_export_dialogue
global tvops --global tvops variable stored in <maxdir>/stdplugs/stdscripts/NET_TreeViewWrapper


global suggestedWidthOfMasterRolloutFloater_SgpuLayer=600
global suggestedHeightOfMasterRolloutFloater_SgpuLayer=400

--This max script implements a layer like  functionality, for sgpu export nprocessing.
--Let us call this by the term 'sgpuLayer'
--Currently, an object can be in only one sgpuLayer


utility sgpuLayerUtil "sgpuLayer" (
    
	local srcobjpos = [5, 70]
	
	local rFooSubRolloutArray = #()
	
	local enterNewLayerNameText
	local enterNewLayerExportIntent
	
	
	dropdownlist existingExportSgpuLayers "existing SgpuLayers"  items:#()across:2 width:200

	editText lExporterVersion caption:"Version" text:( sgpuLib.getVersionLabel() )bold:True  readOnly:true labelOnTop:false  width:200 height:65

	group "Edit Current SgpuLayer" (
	
		button createNewExportSgpuLayer  "Create New" across:5
		
		button renameCurrent  "Rename Current" 

		button delCurrent  "Delete Current"  --across:2
		
		button descDatabase  "Debug DescDb" 

		button cleanMaxFile "Clean Max File" --align:#center
	
	)
	
	group "Export Intent" (
	
		radiobuttons theIntent labels: sgpuLib.exportIntentNames
	
	)
	
	
	rollout renameLayerName "Enter The New Name for the Layer!" width:400 height:60
	(		
		edittext newExportSgpuLayerName ""  fieldWidth:160
		
		button okButton "OK" across:2 
		
		button cancelButton "Cancel" 
		
		function validateNewLayerName newLayerName = (
			retVal = True
			if ( ( newLayerName.count == 0 ) or ( matchPattern newLayerName pattern:"*;*" ) ) then (
				retVal = False
			) else if ( sgpuLib.doesNameContainSpace newLayerName ) or ( sgpuLib.doesNameContainString newLayerName ) then (
				format "SgpuLayerUtil::Error name contains space or quotes\n"
				retVal = False
			)
			retVal
		)
		
		
		
		on okButton pressed do (
			if ( validateNewLayerName newExportSgpuLayerName.text ) then (
				sgpuLayerUtil.enterNewLayerNameText = newExportSgpuLayerName.text--( tolower ( newExportSgpuLayerName.text )	)
				destroyDialog renameLayerName 
			) else (
			
				MessageBox "Please enter a valid name!"
			
			)
			
		 )
		
		on cancelButton pressed do (	
			sgpuLayerUtil.enterNewLayerNameText = ""
			destroyDialog renameLayerName 
		)
		
	)
	rollout enterNewLayerName "Enter The New Layer Name!" width:350 height:120
	(		
		edittext newExportSgpuLayerName ""  fieldWidth:160
		
		group "Export Intent" (
	
			radiobuttons theIntent labels: sgpuLib.exportIntentNames
	
		)
		button okButton "OK" across:2 
		
		button cancelButton "Cancel" 
		
		function validateNewLayerName newLayerName = (
			retVal = True
			if ( ( newLayerName.count == 0 ) or ( matchPattern newLayerName pattern:"*;*" ) ) then (
				retVal = False
			) else if ( sgpuLib.doesNameContainSpace newLayerName ) or ( sgpuLib.doesNameContainString newLayerName ) then (
				format "SgpuLayerUtil::Error name contains space\n"
				retVal = False
			)
			retVal
		)
		
		on enterNewLayerName open do (
			
			opts = SgpuExport.Get_Options()
			enterNewLayerName.theIntent.state = (opts.eExportIntent + 1)
			sgpuLayerUtil.enterNewLayerExportIntent = (opts.eExportIntent + 1)
		)
		
		
		on okButton pressed do (
			if ( validateNewLayerName newExportSgpuLayerName.text ) then (
				sgpuLayerUtil.enterNewLayerNameText = newExportSgpuLayerName.text--( tolower ( newExportSgpuLayerName.text )	)
				sgpuLayerUtil.enterNewLayerExportIntent = enterNewLayerName.theIntent.state
				destroyDialog enterNewLayerName 
			) else (
			
				MessageBox "Please enter a valid name!"
			
			)
			
		 )
		
		on cancelButton pressed do (	
			sgpuLayerUtil.enterNewLayerNameText = ""
			destroyDialog enterNewLayerName
		)
		
	)
	--update the dropdownlist with the short names 
	--of the current selection sets
	function updateExistingExportSgpuLayers_LayerUI = 
	(	
		print "reached here"
		newExportLayers = #()
		sgpuLib.getExistingSgpuLayersFromObjects_core  newExportLayers
		existingExportSgpuLayers.items = 	newExportLayers
	)
	
	function updateExportIntent_LayerGUI state = 
	(
		theIntent.state = state
	
	)



	function updateTreeView_LayerGUI = (
			objectsWithCurrentSgpuLayer = #()
			isel = sgpuLayerUtil.existingExportSgpuLayers.selection
			if ( isel > 0 ) then (
				curSgpuLayer = existingExportSgpuLayers.items[ isel ]
				sgpuLib.getObjectsWithSgpuLayer_core curSgpuLayer &objectsWithCurrentSgpuLayer
			) else
			(
			)		
			sgpuLayerUtil.treeview_rollout.populateTvWithSgpuLayer  &objectsWithCurrentSgpuLayer
	)
	


	function respondToChangeOfCurSgpuLayer_LayerGUI = (

		ival = existingExportSgpuLayers.selection
		if (ival == 0 ) then (	
		
		) else
		(
			curSgpuLayer = existingExportSgpuLayers.items[ ival ]
			ca = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core curSgpuLayer
			assert( ca != undefined ) message:"sgpu Layer shuldbe alreadynin the opts database"


			--update the exportIntent GUI
			updateExportIntent_LayerGUI (ca.eExportIntent + 1)
			
			--update the tree view
			updateTreeView_LayerGUI()
		)
	)


	
 	function closeDownSgpuLayerUtil= (
		
		--format "calling closeDownSgpuLayerUtil\n"
		removeRollout sgpuLayerUtil.treeview_rollout 
		removeRollout sgpuLayerUtil
	)

 	function startSgpuLayerUtil = (	

		rolloutArray = #()
		--format "calling startSgpuLayerUtil\n"	
		addrollout sgpuLayerUtil r_dialogue
		addrollout sgpuLayerUtil.treeview_rollout r_dialogue
	)
	
	function filePreSaveCallback = (
		--format "sgpuLaerUtil::filePreSaveCallBack\n"
		outStream = stringStream ""
		sgpuLib.validateSgpuLayersAndOptsDb outStream
		print (outStream as string)
	)
	
	function filePreOpenCallback= (
		--format "SgpuLayerUtil::filePreOpenCallback\n"
		closeDownSgpuLayerUtil()		
		sgpuLib.deleteAllSgpuLayersFromDb_core()
	)
	
	function filePostOpenCallback = (	
		--format "SgpuLayerUtil::filePostOpenCallback\n"
		startSgpuLayerUtil()
	)
	

	on existingExportSgpuLayers selected i do (

		respondToChangeOfCurSgpuLayer_LayerGUI()
	)




	-- when the intent is changed in the radio buttons
	-- display the correct rollout 
	on theIntent changed state  do 
	( 
		print ("user changed state to: " + sgpuLib.exportIntentNames[state] ) 
			--get the name of the set that need be renamed
		ival = existingExportSgpuLayers.selection
		if (ival != 0 ) then (		
			--get the current layer name
			curSgpuLayerName = existingExportSgpuLayers.items[ ival ]
		
			--get the opts from the database
			opts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core curSgpuLayerName
			assert( opts != undefined ) message:"opts should not be undefined" 
			--change the state
			opts.eExportIntent = (state - 1)
			--save the opts back
			sgpuLib.setSgpuLayerNameToSgpuOptsInDb_core curSgpuLayerName opts
		)
	)




	--when this utility is opened
	on sgpuLayerUtil open do (
		try (
			--get the dedault set of options rom the plugin
			sgpuOpts = sgpuExport.Get_Options()
			-- populate the drop down list with short names of
			-- existing selecion sets
			updateExistingExportSgpuLayers_LayerUI()
			-- respond to chnge of current layer
			respondToChangeOfCurSgpuLayer_LayerGUI()
		
			sgpuLib.animationRangeChangeRefresh()
		
			enterNewLayerNameText = ""
		) catch (
			format "SgpuLayerUtil::Error, error while opening the utility"
		)
	)

	on sgpuLayerUtil close do (
		try (
			enterNewLayerNameText = ""
			if ( enterNewLayerName.open ) then (
		
				destroyDialog enterNewLayerName
			)
		) catch (
		
			format "SgpuLayerUtil::Error, error while closing the utility\n"
		)
	)

	on descDatabase  pressed do (
	
		try (
			ssLayers = stringStream ""
			sgpuLib.describeSgpuLayersInObjects ssLayers
		
			ssOptions = stringStream ""
			sgpuLib.describeSgpuLayersInDb ssOptions
		
			print (ssLayers as string )
			print (ssOptions as string)
		) catch (
			format "SgpuLayerUtil::Error, error while describing the layer database"
		)
	)
	
	on cleanMaxFile pressed do (
		try (
			ss = stringstream ""
			sgpuLib.cleanMaxFile ss
		
			print (ss as string)
			updateExistingExportSgpuLayers_LayerUI()
		) catch (
			format "SgpuLayerUtil::Error, while cleaning max file"
		)
	)

	on delCurrent  pressed do (		

		try (
			ival = existingExportSgpuLayers.selection
		
			if (ival == 0 ) then (			
				messageBox "nothing to delete" 
				throw "empty new export SgpuLayer name!"
			)
		
			curSgpuLayerName = existingExportSgpuLayers.items[ ival ]
			objectsInSgpuLayer = #()
			sgpuLib.getObjectsWithSgpuLayer_core curSgpuLayerName &objectsInSgpuLayer
			for obj in objectsInSgpuLayer do 
			(	
				assert(sgpuLib.checkObjectIsAssignedToSgpuLayer_core  obj curSgpuLayerName ) message: "layer assoiation inconsitency\n"
				sgpuLib.removeSgpuCustAttrFromObject_core obj curSgpuLayerName		
			)

		
			--remove the corresponding option from the options database
			opts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core curSgpuLayerName
			assert( opts != undefined ) message:"opts should not be undefined" 
			sgpuLib.deleteSgpuLayerFromDb_core curSgpuLayerName

			--refresh UI after deletion
			updateExistingExportSgpuLayers_LayerUI()
			--update the tree view after deletion
			updateTreeView_LayerGUI()
		) catch (
			
			format "SgpuLayerUtil::Error, error while deleting layer\n"
		
		)
	)

	on renameCurrent pressed do (
		
		
		createDialog renameLayerName  modal:True escapeEnable:True lockHeight:True lockWidth:True
		
		try
		(
			if( not ( enterNewLayerName.validateNewLayerName enterNewLayerNameText ) ) do (
				messageBox "Please enter a valid name for newLayer" title:"Empty new export SgpuLayer name!!" 
				throw "empty new export SgpuLayer name!"
			)
			
			newSgpuLayerName = enterNewLayerNameText
			
		
			--get the name of the set that need be renamed
			ival = existingExportSgpuLayers.selection
			if (ival == 0 ) then (			
				messageBox "nothing to rename" 
				throw "empty new export SgpuLayer name!"
			)
			curSgpuLayerName = existingExportSgpuLayers.items[ ival ]
			
			
			--check whether the sugested new Name is already there		
			existingSgpuLayers = #()
			sgpuLib.getExistingSgpuLayersFromObjects_core &existingSgpuLayers
			isPresentIndex = sgpuLib.findItemIgnoreCase &existingSgpuLayers newSgpuLayerName
			if( isPresentIndex > 0 ) then (
				if( (  stricmp  curSgpuLayerName newSgpuLayerName ) != 0 ) then (
					throw "set with new suggested name already exists"
				)
			)
		
		

			--get all objects associated with curSgpuLayerName
			objectsInSgpuLayer = #()
			sgpuLib.getObjectsWithSgpuLayer_core curSgpuLayerName &objectsInSgpuLayer

		
			if ( objectsInSgpuLayer.count > 0 ) then (
				--for each object associated with curSgpuLayerName
				for obj in objectsInSgpuLayer do (
					sgpuLib.removeSgpuCustAttrFromObject_core obj curSgpuLayerName
				)
				--for each object associated with curSgpuLayerName
				--change the association to the new name
				sgpuLib.defineLayerAsGivenObjectArray_core &objectsInSgpuLayer newSgpuLayerName			

				opts = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core curSgpuLayerName
				assert( opts != undefined ) message:"opts should not be undefined" 
				sgpuLib.deleteSgpuLayerFromDb_core curSgpuLayerName
				optsAlready = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core newSgpuLayerName
				assert( optsAlready == undefined ) message: "option should not be there in db\n"
				--format "associating % to sgpuOpts % in the db\n" newSgpuLayerName (opts.LogToString() )
				sgpuLib.setNewSgpuLayerNameToSgpuOptsInDb_core newSgpuLayerName  opts
			
			
				--refresh tree view
				--updateExistingExportSgpuLayers
				eitems = existingExportSgpuLayers.items
				eitems[ival] = newSgpuLayerName
				existingExportSgpuLayers.items = eitems
				existingExportSgpuLayers.selection = ival
			
				updateTreeView_LayerGUI()
			
			
			) else
			(

				format "SgpuLayerUtil::Error, Renaming of layer % to % failed, because layer % was empty\n" curSgpuLayerName newSgpuLayerName curSgpuLayerName
			)
		
		) catch (
			format "SgpuLayerUtil::Error, error in renaming\n"
		)
	)



	-- when a new export selection set is created
	on createNewExportSgpuLayer pressed do (
			
			
		
		createDialog enterNewLayerName  modal:True escapeEnable:True lockHeight:True lockWidth:True
		
		try
		(
			if( not ( enterNewLayerName.validateNewLayerName enterNewLayerNameText ) ) do (
				messageBox "Please enter a valid name for newLayer" title:"Empty new export SgpuLayer name!!" 
				throw "empty new export SgpuLayer name!"
			)
			
			newSgpuLayerName = enterNewLayerNameText
			
			
			--check whether the sugested new Name is already there		
			existingSgpuLayers = #()
			sgpuLib.getExistingSgpuLayersFromObjects_core &existingSgpuLayers
			isPresentIndex = sgpuLib.findItemIgnoreCase &existingSgpuLayers newSgpuLayerName
			if( isPresentIndex > 0 ) then (
				throw "set with new suggested name already exists"
			)
			
			
		
			--update dropdown list of selection sets
			updateExistingExportSgpuLayers_LayerUI()
			tempItems = existingExportSgpuLayers.items
			append tempItems newSgpuLayerName
			existingExportSgpuLayers.items = tempItems
			
			
			-- update the current selection in the dropdown list
			itemIndex = sgpuLib.findItemIgnoreCase existingExportSgpuLayers.items newSgpuLayerName
			assert( itemIndex > 0) message:("newSgpuLayer should be already added in the drop down list" + newSgpuLayerName)
			existingExportSgpuLayers.selection = itemIndex
			print (" setting selection to: " + (itemIndex as string ))
			
			sgpuOpts = sgpuExport.Get_Options()
			sgpuOpts.Set_AnimRange ticksPerFrame (animationRange.start) (animationRange.end) 
			sgpuOpts.eExportIntent = (enterNewLayerExportIntent - 1)
			
			--update the intent gui
			updateExportIntent_LayerGUI (sgpuOpts.eExportIntent + 1)
			
			
			--create an option for this new layer i the options database
			optsAlready = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core newSgpuLayerName
			assert ( optsAlready == undefined ) message:"there seems to be already a layer of this name"
			sgpuLib.setNewSgpuLayerNameToSgpuOptsInDb_core newSgpuLayerName sgpuOpts
			optsAlready = sgpuLib.getSgpuOptsForSgpuLayerFromDb_core newSgpuLayerName			
			assert ( optsAlready != undefined ) message:("opst of kind: " + newSgpuLayerName + " should be there")
			
			--asign objects to this layer
			
			objectsThatNeedBeCheckedInTreeView = selection as array
			sgpuLib.defineLayerAsGivenObjectArray_core	&objectsThatNeedBeCheckedInTreeView	newSgpuLayerName
			
			--sync curent selection to the tree view,
			sgpuLayerUtil.treeview_rollout.populateTvWithSgpuLayer  &objectsThatNeedBeCheckedInTreeView
			
		) catch
		(
			
			format "SgpuLayerUtil::Error, error in creating a new layer!"
		)
	)
		
		



	rollout treeview_rollout "TreeView Scene Browser"
	(		
		
		group "Manage Tree View" (

			button syncFrom  "Sync Tree View From Selection"  across:3 
		
			button syncTo  "Sync Tree View To Selection" 

			button uncheckAll "Uncheck All"

		)
		
		dotNetControl tv "TreeView" width:(suggestedWidthOfMasterRolloutFloater_SgpuLayer - 20) height:490 align:#center
		
		fn getIconFromBitmap thePath number &imgListarray =
		(
			tempBmp = dotNetObject "System.Drawing.Bitmap" thePath
			iconBmp = dotNetObject "System.Drawing.Bitmap" 16 16 tempBmp.PixelFormat
			for u = 0 to 15 do (
				for v = 0 to 14 do  ( 
					--iconBmp.setPixel u v (tempBmp.getPixel  u  v )
					pxl = tempBmp.getPixel  ( (number - 1) * 16 +  u) v 
					iconBmp.setPixel u v pxl 
				)
			)
			tempBmp = dotNetObject "System.Drawing.Bitmap" 16 16 tempBmp.PixelFormat
			append imgListArray iconBmp
				
		)
		
		fn InitImageList tv = (
			local hImgList
			local imgListArray = #()
			hImgList = dotNetObject "System.Windows.Forms.ImageList"
			local imgSize 	= dotNetObject "System.Drawing.Size" 16 16
			hImgList.imagesize = imgSIze
			iconDir = (getDir #ui) + "\\icons\\"
			getIconFromBitmap (iconDir + "Standard_16i.bmp") 2 &imgListArray
			getIconFromBitmap (iconDir + "Standard_16i.bmp") 1 &imgListarray
			getIconFromBitmap (iconDir + "Lights_16i.bmp") 3 &imgListArray
			getIconFromBitmap (iconDir + "Cameras_16i.bmp") 2	&imgListArray
			getIconFromBitmap (iconDir + "Helpers_16i.bmp") 1	&imgListArray
			for im in imgListArray do(
				hImgList.Images.add im
			)
			tv.imageList = hImgList				
		)

		function getHitNode arg tv =
		(
			--[brief] Finds the TreeNode that was hit, given a point
			--[param] arg - System.Windows.Forms.MouseEventArgs
			--[param] tv - The TreeView object that was hit
			--[return] Returns an object of the System.Windows.Forms.TreeNode class
			local position = dotNetObject "System.Drawing.Point" arg.x arg.y
			tv.GetNodeAt position
			
		)


		function GetHitCoordinates tv =
		(
			--[brief] Use this method to get the hit coordinates on the treeview.
			--[param] tv - A System.Windows.Forms.TreeNode class object
			--[return] Returns the coordinates of the mouse as a .NET System.Drawing.Point object
			local screenHitPoint = tv.MousePosition
			tv.PointToClient screenHitPoint
		)


		fn addChildren theNode theChildren &selArray =
		(
			for c in theChildren do
			(
				if ( c.ishidden ) then (
					addChildren theNode c.children &selArray

				) else (
					newNode = theNode.Nodes.add c.name
					newNode.tag = dotNetMXSValue c
					newNode.imageIndex = newNode.selectedImageIndex = case superclassof c of
					(
						Default: 1
						Light: 2
						Camera: 3
						Helper: 4

					) 
					newNode.checked = false
					if (finditem selArray c ) > 0 then (
						newNode.checked = true
					)
					newNode.forecolor = (dotNetClass "System.Drawing.Color").fromARGB c.wirecolor.r c.wirecolor.g c.wirecolor.b
					addChildren newNode c.children &selArray
				)
			)
		)



		--fn fillInTreeView tv = (
		--	theRoot = tv.Nodes.add "WORLD ROOT"
		--	rootNodes = for o in objects where o.parent == undefined collect o
		--	addChildren theRoot rootNodes 
		--)


		fn fillInTreeView tv  &selArray = (
			theRoot = tv.Nodes.add "WORLD ROOT"
			rootNodes = for o in objects where o.parent == undefined collect o
			addChildren theRoot rootNodes &selArray
		)


		fn initTreeView tv =
		(
			tvops.InitTreeView tv pLabelEdit:false pFullRowSel:true 
			tv.Indent = 4
			tv.checkBoxes = true
			InitImageList tv

		)

		fn clearTreeView tv =
		(
				
			tvops.ClearTvNodes tv

		)

		fn descTreeViewRec tnode prefix = (			
				
			descTree = tnode.Nodes
			descTreeEnum = descTree.GetEnumerator()
			while( descTreeEnum.MoveNext() ) do
			(
				curnode = descTreeEnum.Current
				if curnode.checked then (
					format "T%%\n" prefix curnode.Text
				) else (
					format "S%%\n" prefix curnode.Text
				)
				descTreeViewRec curnode (prefix + "    " )
			)
		)

		fn checkedNodesRec tnode &sgpuLayer = (			
				
			descTree = tnode.Nodes
			descTreeEnum = descTree.GetEnumerator()
			while( descTreeEnum.MoveNext() ) do
			(
				curnode = descTreeEnum.Current
				if ( curnode.checked ) then (
					append sgpuLayer (curNode.tag.value)
				)
				checkedNodesRec curnode &sgpuLayer
			)			
		)



		fn descTreeView tv =
		(
			print "Tree View"
			descTree = tv.Nodes
			descTreeEnum = descTree.GetEnumerator()
			selLayer = #()
			while( descTreeEnum.MoveNext() ) do
			(
				curnode = descTreeEnum.Current
				descTreeViewRec curnode ""
				checkedNodesRec curNode &selLayer
			)
			for o in selLayer do (
				print o.name
			)							
		)

		fn collectCheckedNodes tv &sgpuLayer =
		(
			descTree = tv.Nodes
			descTreeEnum = descTree.GetEnumerator()
			while( descTreeEnum.MoveNext() ) do
			(
				curnode = descTreeEnum.Current
				checkedNodesRec curNode &sgpuLayer
			)						
		)

			
		

		function  populateTvWithSgpuLayer_Core tv &sgpuLayer = (
				clearTreeView tv 
				initTreeView tv 
   				fillInTreeView tv &sgpuLayer
			)

		function populateTvWithSgpuLayer &objectsThatNeedBeChecked=
		(
			if ( sgpuLayerUtil.treeview_rollout.open) then (
				--format "calling poplulate core\n"
				populateTvWithSgpuLayer_Core  tv &objectsThatNeedBeChecked
			)
		)

		function refreshTv tv = (
			
			sgpuLayerUtil.updateTreeView_LayerGUI()

		)
		
		function tv_set_export_as_subdiv = (		
			ival = existingExportSgpuLayers.selection
		
			if (ival == 0 ) then (			
				messageBox "nothing to delete" 
				throw "empty new export SgpuLayer name!"
			)
		
			curSgpuLayerName = existingExportSgpuLayers.items[ ival ]
			objectsInSgpuLayer = #()
			sgpuLib.getObjectsWithSgpuLayer_core curSgpuLayerName &objectsInSgpuLayer
			for obj in objectsInSgpuLayer do 
			(		
				SgpuExport.Set_ObjectFlagBool obj "export as subdiv" True	
				bVal = SgpuExport.Get_ObjectFlagBool obj "export as subdiv"
				assert( bVal == True ) message:"cant set subdiv flag to True"
			)
		
		)
		
		function tv_unset_export_as_subdiv = (
			ival = existingExportSgpuLayers.selection		
			if (ival == 0 ) then (			
				messageBox "nothing to delete" 
				throw "empty new export SgpuLayer name!"
			)
		
			curSgpuLayerName = existingExportSgpuLayers.items[ ival ]
			objectsInSgpuLayer = #()
			sgpuLib.getObjectsWithSgpuLayer_core curSgpuLayerName &objectsInSgpuLayer
			for obj in objectsInSgpuLayer do 
			(	
				SgpuExport.Set_ObjectFlagBool obj "export as subdiv" False					
				bVal = SgpuExport.Get_ObjectFlagBool obj "export as subdiv"
				assert( bVal == False ) message:"cant set subdiv flag to False"
			)
			
		)
		
		on treeview_rollout open do
		(       
			--format "treeview_rollout open\n"
			
			
			setExportAsSubdivFlag = dotNetObject "System.Windows.Forms.MenuItem"
			setExportAsSubdivFlag.Text = "Set 'Export As Subdiv' Flag"
			
			unsetExportAsSubdivFlag = dotNetObject "System.Windows.Forms.MenuItem"
			unsetExportAsSubdivFlag.Text = "Unset 'Export As Subdiv' Flag"
			
			cmenu = dotNetObject "System.Windows.Forms.ContextMenu"
			cmenu.MenuItems.Add setExportAsSubdivFlag
			cmenu.MenuItems.Add unsetExportAsSubdivFlag
			dotnet.addeventhandler setExportAsSubdivFlag "Click" tv_set_export_as_subdiv
			dotnet.addeventhandler unsetExportAsSubdivFlag "Click" tv_unset_export_as_subdiv

			tv.ContextMenu = cmenu
			sgpuLayerUtil.updateTreeView_LayerGUI()
			
		)


		on treeview_rollout close do
		(
			clearTreeView tv 
		)
		on syncFrom pressed do
		(       
			refreshTv tv
			local sgpuLayer = selection as array
			populateTvWithSgpuLayer_Core  tv &sgpuLayer
			
			--asign objects to this layer	
			local ival = existingExportSgpuLayers.selection
			if (ival > 0 ) then (	
				curSgpuLayerName = 	existingExportSgpuLayers.items[ ival ]
				sgpuLib.defineLayerAsGivenObjectArray_core	&sgpuLayer	curSgpuLayerName
			)			
		)
		

		on syncTo pressed do
		(     
			curSelection = selection as array
			try (
				sgpuLayer = #()
				collectCheckedNodes tv &sgpuLayer
				sgpuLayerCopy = #()
				clearSelection
				for o in objects do (
					itemInSgpuLayer = findItem sgpuLayer o
					if (itemInSgpuLayer > 0) then (
						append sgpuLayerCopy o
					)
				)
				select sgpuLayerCopy
			) catch (
				select curSelection
			)
		)
		


		on uncheckAll pressed do 
		(
			local emptyObjSet = #()
			populateTvWithSgpuLayer_Core  tv &emptyObjSet
			--asign objects to this layer	
			local ival = existingExportSgpuLayers.selection
			if (ival > 0 ) then (	
				curSgpuLayerName = 	existingExportSgpuLayers.items[ ival ]
				sgpuLib.defineLayerAsGivenObjectArray_core	&emptyObjSet	curSgpuLayerName
			)		
		)
		

		on tv Click arg do 
		(
			--hitNode = tv.GetNodeAt (dotNetObject "System.Drawing.Point" arg.x arg.y)
			hitNode = tv.GetNodeAt ( GetHitCoordinates tv )
			if ( hitNode != undefined and  hitNode.tag != undefined ) do (				
				ival = existingExportSgpuLayers.selection
				if (ival != 0 ) then (					
					curSelSet = existingExportSgpuLayers.items[ ival ]
					--get the object
					o = hitNode.tag.value	
					if ( hitNode.checked )then (
						--format "assigning % to: % \n" hitNode.text() curSelSet
						sgpuLib.assignSgpuCustAttrToObject_core o curSelSet
					) else (
						sgpuLib.removeSgpuCustAttrFromObject_core o curSelSet
					)		
				)
				
			)

		) 

	)

	)

macroScript sgpuLayer
            category:"sgpu" 
            internalcategory:"sgpu" 
            tooltip:"Layer Utility for sgpuExport" 
            buttontext:"sgpu layer" 
            Icon:#("standard_16i",5)
(

	global refreshTreeView
	global	closeDownSgpuLayerUtil
	global	startSgpuLayerUtil
 	function refreshTreeView = (

		sgpuLayerUtil.treeview_rollout.refreshTv sgpuLayerUtil.treeview_rollout.tv
 	)



	
	function registerSgpuLayerScripts =
	(
		callbacks.addScript #systemPreReset "sgpuLayerUtil.closeDownSgpuLayerUtil()" id:#sgpuLayer
		callbacks.addScript #systemPostReset "sgpuLayerUtil.startSgpuLayerUtil()" id:#sgpuLayer		
		callbacks.addScript #systemPreNew "sgpuLayerUtil.closeDownSgpuLayerUtil()" id:#sgpuLayer
		callbacks.addScript #systemPostNew "sgpuLayerUtil.startSgpuLayerUtil()" id:#sgpuLayer		
		--callbacks.addScript #filePreNew "sgpuLayerUtil.closeDownSgpuLayerUtil()" id:#sgpuLayer
		--callbacks.addScript #filePostNew "sgpuLayerUtil.startSgpuLayerUtil()" id:#sgpuLayer	
		callbacks.addScript #filePreOpen "sgpuLayerUtil.filePreOpenCallback()" id:#sgpuLayer
		callbacks.addScript #filePostOpen "sgpuLayerUtil.filePostOpenCallback()" id:#sgpuLayer		
		callbacks.addScript #filePreMerge "sgpuLayerUtil.closeDownSgpuLayerUtil()" id:#sgpuLayer		
		callbacks.addScript #filePostMerge "sgpuLayerUtil.startSgpuLayerUtil()" id:#sgpuLayer		
		callbacks.addScript #preImport "sgpuLayerUtil.closeDownSgpuLayerUtil()" id:#sgpuLayer		
		callbacks.addScript #postImport "sgpuLayerUtil.startSgpuLayerUtil()" id:#sgpuLayer
		
		callbacks.addScript #filePreSave "sgpuLayerUtil.filePreSaveCallback()" id:#sgpuLayer
		
		callbacks.addScript #nodeCloned "refreshTreeView()" id:#sgpuLayer		
		callbacks.addScript #nodeCreated "refreshTreeView()" id:#sgpuLayer
		
		callbacks.addScript #sceneNodeAdded "refreshTreeView()" id:#sgpuLayer
		callbacks.addScript #nodeHide "refreshTreeView()" id:#sgpuLayer
		callbacks.addScript #nodeUnhide "refreshTreeView()" id:#sgpuLayer
		callbacks.addScript #nodePostDelete "refreshTreeView()" id:#sgpuLayer
		callbacks.addScript #nodeRenamed "refreshTreeView()" id:#sgpuLayer
		callbacks.addScript #animationRangeChange "sgpuLib.animationRangeChangeRefresh()" id:#sgpuLayer
	)

   	on execute do (
		
		-- close the old rollout floater if it exists

		try( closerolloutfloater r_dialogue )catch ()
		
		try( closerolloutfloater r_export_dialogue )catch ()

		maxVerArray = maxVersion()
		if( maxVerArray[1] >= 12000  ) then ( --max 2010 and above
			-- put up new rollout floater and add rollout to it.
		
		
			r_dialogue = newrolloutfloater "SgpuLayer" suggestedWidthOfMasterRolloutFloater_SgpuLayer suggestedHeightOfMasterRolloutFloater_SgpuLayer

		
		
			callbacks.removeScripts id:#sgpuLayer
			registerSgpuLayerScripts()

			sgpuLayerUtil.startSgpuLayerUtil()
			--callbacks.show id:#sgpuLayer
		) else 
		(
			format "SgpuLayerUtil::Error, sgpu max scripts are supported only  for max 2010 and above\n"
		)
	)


	on isChecked do (
		
		--format "sgpuLayerUtil Macroscript is checked called\n"
		sgpuLayerUtil.open and sgpuLayerUtil.treeview_rollout.open
	)

	on closeDialogs do (
		--format "sgpuLayerUtil close dialog called\n"
		
		--callbacks.show id:#sgpuExport
		callbacks.removeScripts id:#sgpuLayer
		closerolloutfloater r_dialogue

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
	local sgpuLayerUtilAdded = FALSE;
	while(  sgpuMenu.numItems() > 0 ) do 
	(
		sgpuMenu.DeleteItem 1
	)
	for i = 1 to sgpuMenu.numItems() do
	(
		if ((sgpuMenu.GetItem i).GetTitle()) == "sgpu layer"  then sgpuLayerUtilAdded = true		
	)
	if not sgpuLayerUtilAdded then (
		local itemsgpuLayerUtilAdded = menuMan.createActionItem "sgpuLayer" "sgpu" 
		if itemsgpuLayerUtilAdded != undefined then(
			sgpuMenu.addItem itemsgpuLayerUtilAdded -1 
			-- redraw the menu bar with the new item 
			menuMan.updateMenuBar() 
		)
	)

)catch()