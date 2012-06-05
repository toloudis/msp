import os

#------------------------------------------------------------------------------
#read in a cpp file and return a list containing each line of the file
#------------------------------------------------------------------------------
def read_cpp_file(i_CPP_Path):
    stringList = []
    if os.path.exists(i_CPP_Path):
        cppfile = open(i_CPP_Path, "r")
    else:
        return []
    stringList = cppfile.readlines()
    cppfile.close()
    return stringList


#------------------------------------------------------------------------------
# encrypt()
#------------------------------------------------------------------------------
def encrypt( c, numPlaces ) :

    if (c >= 31 and c <= ord('~')) :
        return c - numPlaces
    
    # If the program gets here, it indicates the character isn't
    # a letter (it's a space or punctuation) so return unconverted.
    return c

#------------------------------------------------------------------------------
# write to the compiled file
#------------------------------------------------------------------------------
def write_compiled_file(i_StringList, o_CompiledPath, i_shader_flag, cppFile):
    compiledFile = ""
    if os.path.exists(o_CompiledPath):
        compiledFile = open(o_CompiledPath, "a")
    else:
         compiledFile = open(o_CompiledPath, "w")
         
    for line in i_StringList:

        if i_shader_flag:
            line = line.replace("     ", " ")
            line = line.replace("    ", " ")
            line = line.replace("   ", " ")
            line = line.replace("  ", " ")
            line = line.replace("\t", " ")            

        # Encrypt only if its hlsl code
        if (cppFile.find("Header_Start.hpp") == -1) and (cppFile.find("Footer_Start.hpp") == -1) :
            if (cppFile.find("Header_End.hpp") == -1) and (cppFile.find("Footer_End.hpp") == -1) :
                
                i = 0
                new_line = ""
                for currLetter in line :
                    asciiVal = ord(currLetter)
                    encrypted = encrypt( asciiVal , places )
                    new_line += chr(encrypted)

                line = new_line

        if i_shader_flag:
            line = line.replace("\\", "\\\\")
            line = line.replace("\"", "\\\"") 
            line = line.replace("\n", "\",\\\n")
            line = "\"" + line
       
        compiledFile.write(line)        


def compile_all_files(i_FileList):
    stringList = []
    outPath = ".\\ShaderStrings.hpp"
    if os.path.exists(outPath):
        os.remove(outPath)
    shader_flag = False
    for cppFile in i_FileList:
        if (cppFile.find(".hlsl") != -1) or (cppFile.find(".h") != -1) and cppFile.find(".hpp") == -1 :
            shader_flag = True
        stringList = read_cpp_file(cppFile)
        write_compiled_file(stringList, outPath, shader_flag, cppFile)
        shader_flag = False

places = 1

fileList = [".\\Header_Start.hpp",
            "..\\..\\..\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\Globals.h",
            "..\\..\\..\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\Support.h",
            "..\\..\\..\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\Tessellate.h",
            "..\\..\\..\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\Skinning.h",
            "..\\..\\..\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\Lighting.h",
            ".\\Header.hlsl",
            ".\\Header_End.hpp",
            ".\\Footer_Start.hpp",
            ".\\Footer.hlsl",
            ".\\Footer_End.hpp"]
compile_all_files(fileList)





