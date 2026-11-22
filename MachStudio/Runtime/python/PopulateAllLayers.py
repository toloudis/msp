import mach

""" ================================================================================================
populateRenderLayers can take in a RenderType and a list of options
RenderType will tell it which renderer to populate, see RENDERERS list to see possible types
    Entering "ALL" will populate all render types
Option List must be entered as ["option1", "option2"] - current options are "passes" and "ao"

to populate Default HDR with various permutations of the passes flag use
    populateRenderLayers("Default HDR", ["passes"])

to populate different settings for AO renders use:
    populateRenderLayers("AO Only", ["ao"])

to populate Dirty Matte with passes and AO permutations use:
    populateRenderLayers("Dirty Matte", ["passes", "ao"])
================================================================================================"""


RENDERERS  = ["Default HDR", "AO Only", "Depth Buffer", "Shadow Mask", "Illumination Only",
              "Normals", "Dirty Matte", "Wireframe", "Velocity Map"]
HDR_LAYERS = ["Full Render", "Clamped HDR Buffer", "Scaled HDR Buffer", "Pixel Luminances",
              "DOF Blurriness", "1st Luminance pass", "Bright pass", "Bloom source", "Bloom", "Star"]
PASSES     = ["Render DOF", "Render Glow", "Render Outline", "Render Environments", "Render Lit", "Render Transparent",
              "Enable Diffuse", "Enable Specular", "Enable Shadows", "Enable Invisible Objects Cast Shadows",
              "Enable Invisible Objects Mask Black", "Render Reflections"]

#------------------------------------------------------------------------------
# Rename the new layer based on the original layer and the category changes
#------------------------------------------------------------------------------
def rename_layer( i_OriginalLayer, o_NewLayer, i_Category, i_NewCategoryString ):
    string_list = i_OriginalLayer.split("_")
    i = 0
    new_name_str = ""
    for category in string_list:
        if category.find(i_Category) != -1:
            category = i_NewCategoryString
        new_name_str += category
        if (i+1) < len(string_list):
            new_name_str += "_"
        i += 1
        
    return mach.renameRenderLayer(o_NewLayer, new_name_str)
    
#------------------------------------------------------------------------------
# Set each pass flag to false as default for the initial render layer
#------------------------------------------------------------------------------
def clearPasses(i_RenderLayer):
    layer = mach.duplicateRenderLayer(i_RenderLayer)
    i = 0
    pass_str = ""
    while i < len(PASSES):
        pass_str += "0"
        mach.setRenderLayerPref(layer, PASSES[i], False)
        i += 1
    layer = mach.renameRenderLayer(layer, i_RenderLayer + "_Passes" + pass_str)
    return layer
     
#------------------------------------------------------------------------------
# Set each permutation of possible flags for Passes
#------------------------------------------------------------------------------
def populatePasses(i_RenderLayer, i_StartValue):
    i = i_StartValue
    j = 0
    pass_str = ""
    new_layer = mach.duplicateRenderLayer(i_RenderLayer)
    while j < i_StartValue:
        if mach.getRenderLayerPref(new_layer, PASSES[j]) == True:
            pass_str += "1"
        else:
            pass_str += "0"
        j += 1
    
    bFlagSwitched = False
    while i < len(PASSES):
        if mach.getRenderLayerPref(new_layer, PASSES[i]) == True:
            pass_str += "1"
        elif (mach.getRenderLayerPref(new_layer, PASSES[i]) == False) and not bFlagSwitched:
            mach.setRenderLayerPref(new_layer, PASSES[i], True)
            pass_str += "1"
            bFlagSwitched = True
        else:
            pass_str += "0"
        i += 1
        
    new_pass_str = "Passes" + pass_str
    new_layer = rename_layer(i_RenderLayer, new_layer, "Passes", new_pass_str)
    index = pass_str.rfind("0")
    if (index != -1) and index > i_StartValue:
        populatePasses(new_layer, i_StartValue)


#------------------------------------------------------------------------------
# Set possible combinations of AO options
#------------------------------------------------------------------------------
def populateAO( i_RenderLayer, i_Preset, i_Case ):
    ao_str = ""

    new_layer = mach.duplicateRenderLayer(i_RenderLayer)
    #make sure AO is on
    mach.setRenderLayerPref(new_layer, "Enable AO", True)
    mach.setRenderLayerPref(new_layer, "Sampling Preset", i_Preset)
    ao_str += i_Preset[0]

    if i_Case == 0:
        mach.setRenderLayerPref(new_layer, "Enable Blur", False)
        mach.setRenderLayerPref(new_layer, "Enable Multiple Depths", False)
        ao_str += "00"
    elif i_Case == 1:
        mach.setRenderLayerPref(new_layer, "Enable Blur", True)
        mach.setRenderLayerPref(new_layer, "Enable Multiple Depths", False)
        ao_str += "10"
    elif i_Case == 2:
        mach.setRenderLayerPref(new_layer, "Enable Blur", False)
        mach.setRenderLayerPref(new_layer, "Enable Multiple Depths", True)
        ao_str += "01"
    elif i_Case == 3:
        mach.setRenderLayerPref(new_layer, "Enable Blur", True)
        mach.setRenderLayerPref(new_layer, "Enable Multiple Depths", True)
        ao_str += "11"

    #populate new render layers for each depth layer
    i = 1
    while i <= 4:
        mach.setRenderLayerPref(new_layer, "Num Depth Layers", i)
        new_ao_str = "AmbOcc" + ao_str
        new_ao_str += str(i)
        if i_Case > 0:
           new_layer = rename_layer(i_RenderLayer, new_layer, "AmbOcc", new_ao_str)
        else:
            new_layer = mach.renameRenderLayer(new_layer, i_RenderLayer + "_" + new_ao_str)
        if i < 4:
            new_layer = mach.duplicateRenderLayer(new_layer)
        i += 1

    i_Case += 1
    if i_Case < 4:
        populateAO(new_layer, i_Preset, i_Case)

#------------------------------------------------------------------------------
# The user will pass in a set of additional options to populate
# this function will check those options and run the appropriate code.
#------------------------------------------------------------------------------
def populateOptions( i_CurLayer, i_RenderType, i_OptionList ):
    for option in i_OptionList:
        if option == "passes":
            if i_RenderType == "Default HDR" or i_RenderType == "Dirty Matte":
                passes_layer = clearPasses(i_CurLayer)
                k = 0
                while k < len(PASSES):
                    populatePasses(passes_layer, k)
                    k += 1
        if option == "ao":
            if i_RenderType == "Default HDR" or i_RenderType == "Dirty Matte" or i_RenderType == "AO Only":
                ao_layer = i_CurLayer
                populateAO(ao_layer, "Low", 0)
                populateAO(ao_layer, "Medium", 0)
                populateAO(ao_layer, "High", 0)
    
#------------------------------------------------------------------------------
# Create the initial pairings of render type and hdr layer
#------------------------------------------------------------------------------
def populateRenderLayers( i_RenderType = "ALL", i_OptionList = [] ):
    #first clear all current layers
    for layer in mach.getRenderLayers():
        mach.deleteRenderLayer(layer)
        
    #create initial pairings
    i = 0
    while i < len(RENDERERS):
        if i_RenderType == RENDERERS[i] or i_RenderType == "ALL":
            j = 0
            while j < len(HDR_LAYERS):
                cur_layer = mach.addRenderLayer()
                cur_layer = mach.renameRenderLayer(cur_layer, RENDERERS[i] + "_" + HDR_LAYERS[j])
                mach.setRenderLayerPref(cur_layer, "Renderer", RENDERERS[i])
                mach.setRenderLayerPref(cur_layer, "HDR Layer", HDR_LAYERS[j])
                populateOptions(cur_layer, RENDERERS[i], i_OptionList)
                j += 1
        i += 1
    #report the number of render layers created (minus 1 to account for master layer)
    print str(len(mach.getRenderLayers()) - 1) + " Render Layers populated"
  
