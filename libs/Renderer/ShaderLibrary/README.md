# What is this ?
The shader library containing the renderer's programs, for examples see ../Drivers/OpenGL/Shaders

# Shader library structure
The shader library are comprised of 3 types of files.
 - `header` which contains files that will be made includable like hpp files.
 - `stage` which contains the rendering programs' stages files
 - `program` which contains .json files describing rendering programs

# How to describe rendering programs

Rendering programs are described inside .json files. These are then turned into code by cmake during configuration stage. These .json files may contain the following keys :

`name` : name of this program, used to fetch it inside the shaders library using `ShaderLibrary::GetProgram`

`keywords` : array of `keyword_object` objects defining keywords with their possible values, all possible combinations will be generated via cartesian product

`keyword_object.name` : the name of this keyword

`keyword_object.values` : an array of the possible values of this keyword. "true" and "false" is not a valid value because GLSL preprocessor does not manage it correctly, replace them with "1" and "0"

`defines` : array of values defining defines shared accross all stages, prepended before the stages code

`stages` : array of `stage_object`

`stage_object` : object describing a shader stage

`stage_object.name` : can be `Vertex`, `Fragment`, `Geometry`, `Tessellation`. The stage can also be `Compute` but this stage is forever alone.

`stage_object.version` : the GLSL version for this stage

`stage_object.extensions` : the GLSL version for this stage

`stage_object.defines` : an array of objects defining defines for this stage, prepended before the stage's code

`stage_object.entry_point` : the function used as entry point for the stage

`stage_object.file` : the glsl code file to fetch in the shaders library using `ShaderLibrary::GetStage`

## Example
```json
{
    "name": "Example",
    "keywords" : [
            {
                    "name" : "EXAMPLE_KEYWORD",
                    "values" : [
                            "0",
                            "1"
                    ]
            }
    ],
    "defines" : [
        {
            "name" : "GLOBAL_DEFINE",
            "value" : "SHARED_ACCROSS_ALL_STAGES"
        }
    ],
    "stages" : [
        {
            "name": "Vertex",
            "version": "450",
            "extensions": [
                {
                    "name": "GL_ARB_shader_viewport_layer_array",
                    "behavior": "require"
                }
            ],
            "defines" : [
                {
                    "name": "STAGE_DEFINE",
                    "value" : "ONLY_FOR_THIS_STAGE"
                }
            ],
            "entry_point" : "main",
            "file": "ExampleVertex.glsl"
        },
        {
            "name": "Fragment",
            "version": "450",
            "defines" : [],
            "entry_point" : "main",
            "file": "ExampleFragment.glsl"
        }
    ]
}
```