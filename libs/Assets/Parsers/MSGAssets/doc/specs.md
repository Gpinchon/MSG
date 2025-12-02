# MSGAssets-specs Schema

```txt
specs.schema.json
```

this describes the structure of a MSGAssets file

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                         |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :----------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [specs.schema.json](json/specs.schema.json "open original schema") |

## MSGAssets-specs Type

`object` ([MSGAssets-specs](specs.md))

# MSGAssets-specs Properties

| Property                    | Type    | Required | Nullable       | Defined by                                                                                     |
| :-------------------------- | :------ | :------- | :------------- | :--------------------------------------------------------------------------------------------- |
| [externals](#externals)     | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-externals.md "specs.schema.json#/properties/externals")     |
| [buffers](#buffers)         | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-buffers.md "specs.schema.json#/properties/buffers")         |
| [bufferViews](#bufferviews) | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-bufferviews.md "specs.schema.json#/properties/bufferViews") |
| [images](#images)           | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-images.md "specs.schema.json#/properties/images")           |
| [textures](#textures)       | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-textures.md "specs.schema.json#/properties/textures")       |
| [samplers](#samplers)       | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-samplers.md "specs.schema.json#/properties/samplers")       |
| [materials](#materials)     | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-materials.md "specs.schema.json#/properties/materials")     |
| [primitive](#primitive)     | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-primitive.md "specs.schema.json#/properties/primitive")     |
| [entities](#entities)       | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-entities.md "specs.schema.json#/properties/entities")       |
| [scenes](#scenes)           | `array` | Optional | cannot be null | [MSGAssets-specs](specs-properties-scenes.md "specs.schema.json#/properties/scenes")           |

## externals

the path to external resources to load prior to loading this file

`externals`

* is optional

* Type: `object[]` ([External](external-definitions-external.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-externals.md "specs.schema.json#/properties/externals")

### externals Type

`object[]` ([External](external-definitions-external.md))

### externals Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.

## buffers

a list of buffers

`buffers`

* is optional

* Type: `object[]` ([Buffer](buffer-definitions-buffer.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-buffers.md "specs.schema.json#/properties/buffers")

### buffers Type

`object[]` ([Buffer](buffer-definitions-buffer.md))

### buffers Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.

## bufferViews

a list of bufferViews

`bufferViews`

* is optional

* Type: `object[]` ([BufferView](buffer-definitions-bufferview.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-bufferviews.md "specs.schema.json#/properties/bufferViews")

### bufferViews Type

`object[]` ([BufferView](buffer-definitions-bufferview.md))

### bufferViews Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.

## images

a list of images

`images`

* is optional

* Type: `object[]` ([Details](image-definitions-image.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-images.md "specs.schema.json#/properties/images")

### images Type

`object[]` ([Details](image-definitions-image.md))

### images Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.

## textures

a list of textures

`textures`

* is optional

* Type: `object[]` ([Details](texture-definitions-texture.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-textures.md "specs.schema.json#/properties/textures")

### textures Type

`object[]` ([Details](texture-definitions-texture.md))

### textures Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.

## samplers

a list of samplers

`samplers`

* is optional

* Type: `object[]` ([Details](sampler-definitions-sampler.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-samplers.md "specs.schema.json#/properties/samplers")

### samplers Type

`object[]` ([Details](sampler-definitions-sampler.md))

## materials

a list of materials

`materials`

* is optional

* Type: `object[]` ([Material](material-definitions-material.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-materials.md "specs.schema.json#/properties/materials")

### materials Type

`object[]` ([Material](material-definitions-material.md))

### materials Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.

## primitive

a list of primitives

`primitive`

* is optional

* Type: `object[]` ([MeshPrimitive](mesh-definitions-meshprimitive.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-primitive.md "specs.schema.json#/properties/primitive")

### primitive Type

`object[]` ([MeshPrimitive](mesh-definitions-meshprimitive.md))

## entities

a list of entities with their components

`entities`

* is optional

* Type: `object[]` ([Entity](entity-definitions-entity.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-entities.md "specs.schema.json#/properties/entities")

### entities Type

`object[]` ([Entity](entity-definitions-entity.md))

### entities Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.

## scenes

a list of scenes

`scenes`

* is optional

* Type: `object[]` ([Details](scene-definitions-scene.md))

* cannot be null

* defined in: [MSGAssets-specs](specs-properties-scenes.md "specs.schema.json#/properties/scenes")

### scenes Type

`object[]` ([Details](scene-definitions-scene.md))

### scenes Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.
