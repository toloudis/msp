
	global r_dialogue --stores the sgpuLayerUtil
	global r_export_dialogue
	global tvops --global tvops variable stored in <maxdir>/stdplugs/stdscripts/NET_TreeViewWrapper


--custom attribute that is used to store to which sgpuSgpuLayer does the object belong
	sgpuCustAttr = attributes "sgpuCustAttr" version:1  attribID:#(0x726595dd, 0x1ef7f019) (
		parameters main ( layerData type:#string default:"" ) 
		)

rollout sgpuLib "SgpuLibNamespace"(

	local exportIntentNames = #("Model Export","Vertex Anim Export","Camera Anim Export")
	local animExportChoices = #("geometry", "animation" )



	-----------------------------------------------------------------------------------------
	--utility functions
	
	function safeStream outStream = (
		retStream = outStream
		if outStream == undefined then (
			retStream = stringStream ""
		)
		return retStream
	)
	
	function safeInt oldIntVal txt = (
		retVal = oldIntVal
		try ( retVal = ( txt as integer) ) catch()
		retVal
	)
	
	function safeFloat oldIntVal txt = (
		retVal = oldIntVal
		try ( retVal = ( txt as float) ) catch()
		retVal
	)
	
	function getTheSceneObj = (
		(refs.dependents rootnode) [1]
	)


	function getVersionLabel = (
		local scriptVersion = "1.4.0.19"
		local exporterVersion= SgpuExport.Get_ExporterVersion()
				
		local exportLibVersion=SgpuExport.Get_ExportLibVersion()		
		local lExporterVersionText = "SgpuExport Version:\n"
		lExporterVersionText += "Script Version: "
		lExporterVersionText += scriptVersion + "\n"
		lExporterVersionText += "Exporter Dll Version:"
		lExporterVersionText +=	exporterVersion + "\n"
		lExporterVersionText += "ExportLib Dll Version: " 
		lExporterVersionText += exportLibVersion
		lExporterVersionText
	)
	
	function doesNameContainSpace argName = (
		containsSpace = False
		for i=1 to argName.count do (
			containsSpace = containsSpace or ( isSpace argName[i] )
		)
		containsSpace
	)

	function doesNameContainString argName = (
		
		containsString= False
		for i=1 to argName.count do (
			containsString = containsString or ( argName[i] == "\"" )
		)
		containsString
	
	)
	
	function findItemIgnoreCase &arry item = (
		retVal = 0
		curIndex = 0
		for i in arry do (
			curIndex += 1
			if ( (classof i ) == String ) then (
				if ( (stricmp i item ) == 0) then (
					return curIndex
				)
			)
		)
		return retVal
	)	
	
	
	
	function setStartFrame sgpuOpts  fStartFrame = (
	
		fEndFrame = sgpuOpts.fEndFrame
		sgpuOpts.Set_AnimRange ticksPerFrame fStartFrame fEndFrame
	)
	
	function setEndFrame sgpuOpts fEndFrame = (
	
		fStartFrame = sgpuOpts.fStartFrame
		sgpuOpts.Set_AnimRange ticksPerFrame fStartFrame fEndFrame
	)





	-----------------------------------------------------------------------------------------
	/*
	-- Support functions for accessing sgpuOptions associated with sgpuLayers
	-- in the database	
	--
	--
	*/

	
	function getSeed1 = (
		( getHashValue "studiogpu inc" 0x5678 )
	)

	function getSeed2 = (
		( getHashValue "STUDIOGPU INC" 0x5678 )
	)


	function makeId1 strArg = (
		getHashValue (toLower strArg ) ( getSeed1() )
	)

	function makeId2 strArg = (
		getHashValue (toLower strArg) ( getSeed2() )
	)

	--make a string which consists of the custom attribute defenition
	--which can store the options
	function makeSgpuOptsCustAttrString sgpuLayer = (
		local defaultSgpuOpts
		defaultSgpuOpts = sgpuExport.Get_Options()

		defaultSgpuOptsString = defaultSgpuOpts.LogToString()
		caExe = "attributes " + sgpuLayer + " version:1 "
		caExe +=  "attribID:#( " 
		caExe += 	( ( makeId1 sgpuLayer ) as string )
		caExe += 	","
		caExe += 	( (makeId2 sgpuLayer ) as string )
		caExe += 	")"
		caExe += "( "
		caExe += 	" parameters main "
		caExe += 	"( "
		caExe += 		"sgpuOpts type:#string "--default:\\\"\\\""
		caExe += 	")" 
		caExe += ")"
		return caExe
	)




	--store the given sgpuOpts in the database under the given newName
	function setNewSgpuLayerNameToSgpuOptsInDb_core newName sgpuOpts  =  (
		theSceneObj = getTheSceneObj()
		--make the string representation of thye defenition
		caExe = makeSgpuOptsCustAttrString newName
		--make the defenition
		caDef = execute caExe
		--no need to check this
		--caValAlready = custAttributes.get theSceneObj caDef
		--assert (caValAlready == undefined ) message: "opts name already in the db"
		--get a string representation of the sgpuOpts
		sgpuOptsString = sgpuOpts.LogToString()	
		custAttributes.add theSceneObj caDef
		caVal = custAttributes.get theSceneObj caDef
		caVal.sgpuOpts = sgpuOptsString
	)


	
	--delete the SgpuLayer from the db
	function deleteSgpuLayerFromDb_core sgpuLayer = (
		theSceneObj = getTheSceneObj()

		assert ( sgpuLayer.count > 0 ) message:"optionName should be non-empty\n"		
		assert ( (classof sgpuLayer) == string ) message:"sgpuLayer should be a string\n"


		caExe = makeSgpuOptsCustAttrString sgpuLayer
		caDef = execute caExe
		caVal = custAttributes.get theSceneObj caDef
		assert( caVal != undefined) message:"option should be present in the database\n"
		
		defInstances = custAttributes.getDefInstances caDef
		custAttributes.delete theSceneObj caDef
		caVal = custAttributes.get theSceneObj caDef
		assert( caVal == undefined) message:"option should not be present in the database\n"
	)

	--get the sgpuOption corresponding to the given sgpuLayer
	function getSgpuOptsForSgpuLayerFromDb_core sgpuLayer = (
		theSceneObj = getTheSceneObj()
		local retSgpuOpts
		caExe = makeSgpuOptsCustAttrString sgpuLayer
		caDef = execute caExe
		caVal = custAttributes.get theSceneObj caDef
		if ( caVal != undefined ) then (
			sgpuOptsString = caVal.sgpuOpts	
			retSgpuOpts = sgpuExport.Get_Options()
			retSgpuOpts.InitFromString sgpuOptsString	
		) 	
		retSgpuOpts
	)


	--set the given sgpuLayerName to the given sgpuOPts in the db
	function setSgpuLayerNameToSgpuOptsInDb_core sgpuLayerName  sgpuOpts = (
		theSceneObj = getTheSceneObj()
		local sgpuOptsString
		sgpuOptsString = sgpuOpts.LogToString()
		caExe = makeSgpuOptsCustAttrString sgpuLayerName
		caDef = execute caExe
		caVal = custAttributes.get theSceneObj caDef
		if ( caVal != undefined ) then (
			caVal.sgpuOpts = sgpuOptsString
		) else
		(
			custAttributes.add theSceneObj caDef
			caVal = custAttributes.get theSceneObj caDef
			caVal.sgpuOpts = sgpuOptsString	
		)
	)

	--get all layers which has ben regsitered in the db with an sgpuOpts
	function getSgpuLayerNamesFromDb_core &sgpuLayerNamesInDb = (
		theSceneObj = getTheSceneObj()
		optDefs = custAttributes.getDefs theSceneObj
		if optDefs != undefined then (
			for odef in optDefs do (
				sgpuLayer = (odef.name as string )
				if( matchPattern sgpuLayer pattern:"#*" ) then (
					sgpuLayer = substring sgpuLayer 2 -1
				)
				append sgpuLayerNamesInDb sgpuLayer
			)
		)
	)
	
	--clean the db of all sgpuLayers
	function deleteAllSgpuLayersFromDb_core = (
	
		local theSCeneObj
		theSceneObj = getTheSceneObj()
		sgpuLayersInDb = #()
		getSgpuLayerNamesFromDb_core &sgpuLayersInDb
		numOpts = sgpuLayersInDb.count
		for sgpuLayerInDb in sgpuLayersInDb do (
			deleteSgpuLayerFromDb_core sgpuLayerInDb
		)
	)
	--output the info about all layers regiedstred in the db
	--to the given outStream
	function describeSgpuLayersInDb outStream = (
		toStream = safeStream( outStream )
	
		theSceneObj = getTheSceneObj()
		sgpuLayerNamesInDb = #()
		getSgpuLayerNamesFromDb_core &sgpuLayerNamesInDb
		format "%\n" "sgpu options from the db\n" to:toStream
		for sgpuLayer in sgpuLayerNamesInDb do (
			sgpuOpt = getSgpuOptsForSgpuLayerFromDb_core sgpuLayer
			assert (sgpuOpt != undefined) message:"option missing from db\n"
			sgpuOptString = sgpuOpt.LogToString()
			format "%: %\n" sgpuLayer sgpuOptString to:toStream
		)	
	)
	
	
	
	




	-----------------------------------------------------------------------------------------
	/*
	-- functions for accessing the 'sgpuLayer' custom attribute
	--stored along with objects	
	*/

	function layerData_get_core obj = (
	
		-- get the 'sgpuCustAttr'			
		sgpuca = custattributes.get o sgpuCustAttr
		local retVal 
		if ( sgpuca != undefined )then (
			retVal = sgpuca.layerData
		)
		retVal
	)

	function layerData_isPresent_core layerData argLayer = (
		if( layerData == undefined or layerData.count <= 0 ) then (
			False
		) else if ((stricmp layerData argLayer ) == 0) then (
			True
		) else (
			ss = stringstream layerData
			while (not eof ss ) do (
				curLayer = readDelimitedString ss ";"
				if (stricmp curLayer argLayer) == 0 then (
					return True
				)
			)
			False
		)
	)
	
	function layerData_add_core  layerData newLayer = (
		assert( layerData != undefined ) message: "layer data should not be undefined"
		if (layerData.count <= 0 ) then (
			newLayer
		) else if ( not (  layerData_isPresent_core  layerData newLayer ) ) then (
			layerData + ";" + newLayer
		) else (
			layerData
		)
	)
	
	function layerData_remove_core layerData argLayer = (
		assert( layerData != undefined ) message:"layerData is undefined!"
		if ( ( stricmp layerData argLayer ) == 0 ) then (
			outLayer = ""
			outLayer
		) else (
			outLayer = ""
			ss = stringstream layerData
			local firstLayer = True
			while (not eof ss ) do (
				curLayer = readDelimitedString ss ";"
				if (stricmp curLayer argLayer) != 0 then (					
					if (not firstLayer ) then (
						outLayer += ";"
					) else (
						firstLayer = False
					)
					outLayer += curLayer
				)
			)
			outLayer
		)
	)
	
	function layerData_getalllayers_core layerData &layers = (
		if ( ( layerData == undefined ) or ( layerData.count <= 0 ) ) then (
		
		) else  (		
			ss = stringstream layerData
			while (not eof ss ) do (
				curLayer = readDelimitedString ss ";"
				append layers curLayer
			)		
		) 
	)
		
	--assign the given value to the sgpuCustAttr of the object
	function assignSgpuCustAttrToObject_core o newLayer=
	(
		
		-- get the 'sgpuCustAttr'			
		sgpuca = custattributes.get o sgpuCustAttr
		if ( sgpuca == undefined ) then (
			--if the object doesnt have an 'sgpuCustAttr'
			--create one
			custattributes.add o sgpuCustAttr
			sgpuca = custattributes.get o sgpuCustAttr
		)
		--put the new value in tyhe 'sgpuCustAttr'
		sgpuca.layerData = layerData_add_core (sgpuca.layerData) newLayer

	)
	
	--assign the given value to the sgpuCustAttr of the objects
	function assignSgpuCustAttrToObjects_core &objArray newLayer=
	(
		--for each object in the array
		--assign the object to the layer
		for obj in objArray do (
			assignSgpuCustAttrToObject_core obj newLayer
		)
	)

	--check the object's cust Attr to see if it is asigned to this layer
	function checkObjectIsAssignedToSgpuLayer_core obj sgpuLayer = (
		sgpuca = custattributes.get obj sgpuCustAttr
		
		local retVal = case sgpuca of   ( undefined : False
														default: ( layerData_isPresent_core ( sgpuca.layerData ) sgpuLayer )
												 )
		retVal
	)


	--if the object's sgpuCustAttr has the given value remove it
	function removeSgpuCustAttrFromObject_core o givenVal =
	(
		-- if 'o' has a valid 'sgpuCustAttr' of same value as 'givenVal'
		--delete that sgpuCustAttr
		sgpuca = custattributes.get o sgpuCustAttr
		if ( sgpuca != undefined ) then (
			updatedLayerData = ( layerData_remove_core (sgpuca.layerData) givenVal )
			if (updtedLayerData == "" ) then
			(
				custattributes.delete o sgpuCustAttr
			) else
			(
				sgpuca.layerData = updatedLayerData
			)
		)
	)

	--collect all objects associated with the given layer name to
	--the given array
	function getObjectsWithSgpuLayer_core sgpuLayerName &objectsInSgpuLayer =
	(				
		for o in objects do (
			sgpuca = custattributes.get o sgpuCustAttr
			if ( ( sgpuca != undefined ) and ( layerData_isPresent_core (sgpuca.layerData) sgpuLayerName ) ) then (
				append objectsInSgpuLayer o
			)
		)
	)
		
		



	--get all sgpuLayer-names from all the objects in te max file
	function getExistingSgpuLayersFromObjects_core &existingSgpuLayersFromObjects = 
	(
		assert( existingSgpuLayersFromObjects.count == 0)
		for o in objects do 
		(
			sgpuca = custattributes.get o sgpuCustAttr
			if ( sgpuca != undefined ) then 
			(
				layersAssociatedWithObject = #()
				layerData_getalllayers_core (sgpuca.layerData) &layersAssociatedWithObject
				for layerOfObject in layersAssociatedWithObject  do (
					itemIndex = findItemIgnoreCase existingSgpuLayersFromObjects layerOfObject
					if( itemIndex == 0) then 
					(
						append  existingSgpuLayersFromObjects layerOfObject
					)
				)
			)
		)
	)

	--for each object in the max file
	--if the object is included in the given array
	--set the object to the given layer
	--if the object is not in the array
	--dissociate the obj from the layer
	function defineLayerAsGivenObjectArray_core &objArray sgpuLayerName = 
	(

		for o in objects do (					
			fitem = findItem objArray o
			if ( fitem > 0 ) then (	
				-- if 'o' is in the 'objArray'
				assignSgpuCustAttrToObject_core o sgpuLayerName
			) else (
				-- if 'o' is not in the objArray	
				removeSgpuCustAttrFromObject_core o sgpuLayerName
			)
		)
	)


	--get all the layers from eth objects into the stream
	function describeSgpuLayersInObjects outStream =
	(	
		toStream = safeStream( outStream )
		
		--array of array of object names
		objectNamesInLayers_aofa = #()
		existingSgpuLayersFromObjects = #()
		getExistingSgpuLayersFromObjects_core &existingSgpuLayersFromObjects
		for sgpuLayer in existingSgpuLayersFromObjects do (
			emptyArray = #()
			append objectNamesInLayers_aofa emptyArray
		)
		
		for o in objects do 
		(
			sgpuca = custattributes.get o sgpuCustAttr
			if ( sgpuca != undefined ) then 
			(
				layersAssociatedWithObject = #()
				layerData_getalllayers_core (sgpuca.layerData) &layersAssociatedWithObject
				
				for sgpuLayer in layersAssociatedWithObject do (
					itemIndex = findItemIgnoreCase existingSgpuLayersFromObjects sgpuLayer
					assert( itemIndex > 0 ) message:"=inconsistency in object layering\n"
					assert( (classof o.name) == string ) message:"object name is not a string\n"
					append objectNamesInLayers_aofa[ itemIndex ] o.name 
				)
			)
		)
		
		itemIndex = 1
		for sgpuLayer in existingSgpuLayersFromObjects do (
			objNamesAssociatedWithThisLayer = objectNamesInLayers_aofa[ itemIndex ]
			itemIndex = itemIndex + 1
			format "SgpuLayer: %\n" sgpuLayer to: toStream
			for objName in objNamesAssociatedWithThisLayer  do (
				format "\t%\n" objName	to:toStream
			)
			
		)
	)
	
	

	
	
	--for each sgpuLayer registered in the db
	--get the sgpuOpts and limit the animation range to the current
	--valid animation range
	function clampAnimationRangesInOpts oStream = (
		toStream = safeStream ( oStream )
		theSceneObj = getTheSceneObj()
		existingSgpuLayersFromObjects = #()
		getExistingSgpuLayersFromObjects_core &existingSgpuLayersFromObjects
		for sgpuLayer in existingSgpuLayersFromObjects do (
			opts = getSgpuOptsForSgpuLayerFromDb_core sgpuLayer
			if (opts == undefined ) then (			
				format "SgpuLib::Error: sgpuLayer %: doesnt have an entry in the options database\n" sgpuLayer to:toStream
				continue
			)
			if( opts.fStartFrame  < animationRange.start ) then (
				opts.Set_AnimRange	ticksPerFrame (animationRange.start) (opts.fEndFrame)
				format "SgpuLib::Info, clamping animation range for layer:% to (%, %)\n" sgpuLayer (animationRange.start) (animationRange.end)	to:toStream
			)
			if ( opts.fEndFrame > animationRange.end ) then (			
				opts.Set_AnimRange	ticksPerFrame (opts.fStartFrame) (animationRange.end)
				format "SgpuLib::Info, clamping animation range for layer:% to (%, %)\n" sgpuLayer (animationRange.start) (animationRange.end) to:toStream
			)
			setSgpuLayerNameToSgpuOptsInDb_core sgpuLayer opts
		)

	)

	-----------------------------------------------------------------------------------------
	--misc functions
	

	--refresh db with any animation range changes
	function animationRangeChangeRefresh = (
		ss = stringStream ""
		format "SgpuLib::Info::animationRangeChangeRefresh called\n" to:ss
		clampAnimationRangesInOpts ss
		print (ss as string )
	)




	
	--validation of the layers associated with objects
	--and the layers in the database
	function validateSgpuLayersAndOptsDb outStream= (
		
		toStream = safeStream( outStream )
		--verify that for every layer associated with any object
		--there is an entry for the correspondig option in te options database

		theSceneObj = getTheSceneObj()
		existingSgpuLayersFromObjects = #()
		getExistingSgpuLayersFromObjects_core &existingSgpuLayersFromObjects 
		allSgpuLayersHaveOptsInDatabase = true
		for sgpuLayer in existingSgpuLayersFromObjects do (	
			if ( sgpuLayer.count <= 0 ) then (
				format "SgpuLib::Error: one of the object in the scane has an sgpuLayer with an empty name\n" to:toStream
				continue
			)		
			opts = getSgpuOptsForSgpuLayerFromDb_core sgpuLayer
			if( opts == undefined) then (
				format "SgpuLib::Error: sgpuLayer %: doesnt have an entry in the options database\n" sgpuLayer to:toStream 
				continue
			)
		)

		--verify that for every option in thye options database
		--there is atleast one non-empty layer
		--If there is an option in the options database with no layer
		--delete that option

		sgpuLayerNamesInDb = #()
		getSgpuLayerNamesFromDb_core &sgpuLayerNamesInDb
		for sgpuLayer in sgpuLayerNamesInDb do (
			sgpuLayerIndex = findItemIgnoreCase existingSgpuLayersFromObjects sgpuLayer
			if( sgpuLayerIndex <= 0) then (
				format "SgpuLib::Error: option : % doesnt have a corresponding valid layer with objects\n" sgpuLayer to:toStream
				deleteSgpuLayerFromDb_core sgpuLayer
			)
		)
		
	)
	

	--clean the max file of all the custom attributes set by sgpu
	function cleanMaxFile outStream = (
		
		toStream = safeStream( outStream )
		
		format "SgpuLib::Info, deleting the sgpuCustAttr of all objects\n" to:toStream
		for o in objects do (
			custAttributes.delete o sgpuCustAttr 
		)
		
		--clean the db of all sgpuLayers
		deleteAllSgpuLayersFromDb_core()
		
		
	)

	
)	