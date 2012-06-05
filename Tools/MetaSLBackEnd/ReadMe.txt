MetaSLBackend Solution

gen_sgpu project compiles a plugin DLL for our shader generation code.
compile_sgpu project uses the plugin DLL to compile a shader for a simple node graph.

sgpu_sample.fx is created, containing the generated shader.

By having both projects in one solution, it is possible to debug within the plugin.

I have created a custom MetaSL node "illumination_sgpu.msl" to make the shader syntax tree simple 
and this node is used in compile_sgpu.cpp. The custom node was added to the library 
used for this app, shader_lib.xmsl.

There are some other files in the Support Files folder that are just used for reference.

TODO:
mslVisitorHLSL.cpp contains syntax based on the MSL shader generation sample, 
it needs to be converted to export HLSL syntax.

mslCodeGenerator.cpp handles the high level structure of the shader and the traversal of the 
abstract syntax tree. This is where most of the work on the back end is needed.

Look for the "case ISyntax_tree::CLASS_DECLARATION:" in mslCodeGenerator.cpp function mslCodeGenerator::visit_declaration()
The code there wants to handle classes of types "shader" "brdf" and "technique". 
Our sample case generates a "shader", I am not sure what we should do with the other types.

When handling the shader class declaration, it looks at the class blocks beneath it. There seem to
be three types of class blocks "member", "output", and "input".
The "input" block should turn into the parameter annotations that declare the shader constants.
The "member" block becomes the functions in HLSL, but we need to arrange the order so that the
first "root" function is written last (maybe have to reverse the order completely?)
I think we can ignore the "output" block.

Please look at mslCodeGenerator::visit_type_definition(), these types need to be mapped from MetaSL to HLSL.
Unfortunately, it doesn't seem to affect types declarations in functions.

- Types need to be converted from MetaSL to HLSL (Color -> float4)
- Shading state global values need to be mapped to a HLSL state structure
- MetaSL standard library functions need to be supported

Some variables need to be prefixed with somethign like "msl_" in order to avoid conflicts with HLSL keywords.

All inputs should be converted to properties in the FX file, but it seems to be that there is 
some relationship to annoations that aren't converting all shader inputs. 

