sgpuShaveExport.mll is a maya plugin that exports  Shave and Haircut, hair nodes to a text file.


-To build this project,
	-please update MAYA_8_5_LOCATION, MAYA_09_LOCATION, MAYA_10_LOCATION in LaunchDevenv.bat,
	according to the locations of your maya installations.
	-Launch a devenv using LaunchDevenv.bat and  then use the devenv to load up the sgpuShaveExport.sln
	-Choose a configuration/platform and build it. Please let me know of any build errors,
	since I have not tested all configurations and platforms

	-As a PostBuild event, sgpuShaveExport.mll will be written 
	to the corresponding Maya plug-ins directory.(ie: <your maya location>/bin/plug-ins )
 

-usage,
	-Make sure that the sgpuShaveExport.mll which is present at <your maya location>/bin/plug-ins 
	directory is loaded
	-Select one or more 'shaveHair' nodes in the maya scene
	-In the script editor window type in 'sgpuShaveExport -o <filepath.txt>


-sgpuShaveExporter uses the shave & haircut sdk.
-For each selected shave node, its hair info is extracted and exported.
-Based on the switch "General Properties/Interpolate Guides", we get either the full HairInfo 
 or the hair guides
-For each hair, the geometry exported is a sequence of hair vertices which form the CV-s of a Catmul-Rom
 cubic sline. Geometrucally this spline forms the spine of the hair.
-other hair parameters include hair root radius, hair tip radius, hair root color, hair tip color etc.
-all vertex positions are in worldspace.
-See the HairReader, a console program for reading the text file
