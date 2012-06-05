
try(

	--------------------------------------------------------------------
	-- This code adds a SGpu sub-menu to MAX's main menu bar. 	
	local sgpuMenu;-- = menuMan.findMenu "SGPU"
	-- Get the main menu bar 
	local mainMenuBar = menuMan.getMainMenuBar();
	for i = 1 to mainMenuBar.numItems() do
	(
		if ((mainMenuBar.GetItem i).GetTitle()) == "sgpu"  then
			sgpuMenu = (mainMenuBar.GetItem i).getSubMenu()
	)
	
	if sgpuMenu == undefined then
	(
		-- Get the main menu bar 
		local mainMenuBar = menuMan.getMainMenuBar() 
	
		-- Create a new menu 
		local sgpuMenu = menuMan.createMenu "sgpu" 
		-- Create a new menu item with the menu as it's sub-menu 
		local sgpuMenuItem = menuMan.createSubMenuItem "sgpu" sgpuMenu		
		-- compute the index of the next-to-last menu item in the main menu bar 
		local sgpuMenuIndex = mainMenuBar.numItems() - 1 
		-- Add the sub-menu just at the second to last slot 
		mainMenuBar.addItem sgpuMenuItem sgpuMenuIndex 

		-- redraw the menu bar with the new item 
		menuMan.updateMenuBar() 
	)

)catch()


--------------------------------------------------------------------
-- load in all the functions we use

filein ((GetDir #Scripts) +"\SgpuScripts\sgpuLayerUtil.ms")

filein ((GetDir #Scripts) +"\SgpuScripts\sgpuExportUtil.ms")