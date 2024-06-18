# What is this ?
The shader programs listing shader's stages and defines

# Keys
`name` : name of this program, used to fetch it inside the shaders library using `ShaderLibrary::GetProgram`
`defines` : array of values defining defines shared accross all stages, prepended before the stages code
`stages` : array of `stage_object`
`stage_object` : object describing a shader stage
`stage_object.name` : can be `Vertex`, `Fragment`, `Geometry`, `Tessellation`. The stage can also be `Compute` but this stage is forever alone.
`stage_object.version` : the GLSL version for this stage
`stage_object.defines` : an array of objects defining defines for this stage, prepended before the stage's code
`stage_object.entry_point` : the function used as entry point for the stage
`stage_object.file` : the glsl code file to fetch in the shaders library using `ShaderLibrary::GetStage`

# Example
```json
{
    "name": "Example",
    "defines" : [
        {
            "name" : "GLOBAL_DEFINE",
            "value" : "\"Shared accross all stages\""
        }
    ],
    "stages" : [
        {
            "name": "Vertex",
            "version": "450"
            "defines" : [
                {
                    "name": "STAGE_DEFINE",
                    "value" : "\"Only for this stage\""
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