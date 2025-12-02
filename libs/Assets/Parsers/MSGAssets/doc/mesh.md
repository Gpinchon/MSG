# MSGAssets-mesh Schema

```txt
mesh.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                            |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------- |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [mesh.schema.json](json/defs/mesh.schema.json "open original schema") |

## MSGAssets-mesh Type

`object` ([MSGAssets-mesh](mesh.md))

# MSGAssets-mesh Definitions

## Definitions group primitiveDrawingMode

Reference this group by using

```json
{"$ref":"mesh.schema.json#/definitions/primitiveDrawingMode"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group meshLod

Reference this group by using

```json
{"$ref":"mesh.schema.json#/definitions/meshLod"}
```

| Property                          | Type     | Required | Nullable       | Defined by                                                                                                                                |
| :-------------------------------- | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------- |
| [screenCoverage](#screencoverage) | `number` | Required | cannot be null | [MSGAssets-mesh](mesh-definitions-meshlod-properties-screencoverage.md "mesh.schema.json#/definitions/meshLod/properties/screenCoverage") |
| [primitives](#primitives)         | `object` | Required | cannot be null | [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives.md "mesh.schema.json#/definitions/meshLod/properties/primitives")         |

### screenCoverage

the screen coverage of this LOD

`screenCoverage`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshlod-properties-screencoverage.md "mesh.schema.json#/definitions/meshLod/properties/screenCoverage")

#### screenCoverage Type

`number`

### primitives

a list of primitives with their corresponding material index inside the linked material set

`primitives`

* is required

* Type: `object` ([Details](mesh-definitions-meshlod-properties-primitives.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives.md "mesh.schema.json#/definitions/meshLod/properties/primitives")

#### primitives Type

`object` ([Details](mesh-definitions-meshlod-properties-primitives.md))

## Definitions group primitive

Reference this group by using

```json
{"$ref":"mesh.schema.json#/definitions/primitive"}
```

| Property                              | Type      | Required | Nullable       | Defined by                                                                                                                                            |
| :------------------------------------ | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------- |
| [uri](#uri)                           | `string`  | Optional | cannot be null | [MSGAssets-mesh](external-definitions-uri.md "mesh.schema.json#/definitions/primitive/properties/uri")                                                |
| [name](#name)                         | `string`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-name.md "mesh.schema.json#/definitions/primitive/properties/name")                         |
| [drawingMode](#drawingmode)           | `string`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-primitivedrawingmode.md "mesh.schema.json#/definitions/primitive/properties/drawingMode")  |
| [castShadow](#castshadow)             | `boolean` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-castshadow.md "mesh.schema.json#/definitions/primitive/properties/castShadow")             |
| [hasTexCoords](#hastexcoords)         | `array`   | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-hastexcoords.md "mesh.schema.json#/definitions/primitive/properties/hasTexCoords")         |
| [indiceBufferView](#indicebufferview) | `number`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-indicebufferview.md "mesh.schema.json#/definitions/primitive/properties/indiceBufferView") |
| [vertexBufferView](#vertexbufferview) | `number`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-vertexbufferview.md "mesh.schema.json#/definitions/primitive/properties/vertexBufferView") |

### uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-mesh](external-definitions-uri.md "mesh.schema.json#/definitions/primitive/properties/uri")

#### uri Type

`string` ([URI](external-definitions-uri.md))

#### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

#### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```

### name

this object's name, important because it allows for it to be referenced in another asset file

> required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-name.md "mesh.schema.json#/definitions/primitive/properties/name")

#### name Type

`string`

### drawingMode



`drawingMode`

* is optional

* Type: `string` ([PrimitiveDrawingMode](mesh-definitions-meshprimitive-properties-primitivedrawingmode.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-primitivedrawingmode.md "mesh.schema.json#/definitions/primitive/properties/drawingMode")

#### drawingMode Type

`string` ([PrimitiveDrawingMode](mesh-definitions-meshprimitive-properties-primitivedrawingmode.md))

#### drawingMode Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value             | Explanation |
| :---------------- | :---------- |
| `"Points"`        |             |
| `"Lines"`         |             |
| `"LineStrip"`     |             |
| `"LineLoop"`      |             |
| `"Polygon"`       |             |
| `"Triangles"`     |             |
| `"TriangleStrip"` |             |
| `"TriangleFan"`   |             |
| `"Quads"`         |             |
| `"QuadStrip"`     |             |

#### drawingMode Default Value

The default value is:

```json
"Triangles"
```

### castShadow



`castShadow`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-castshadow.md "mesh.schema.json#/definitions/primitive/properties/castShadow")

#### castShadow Type

`boolean`

#### castShadow Default Value

The default value is:

```json
true
```

### hasTexCoords

set to true if this primitive has the corresponding set of texture coordinates

`hasTexCoords`

* is optional

* Type: `boolean[]`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-hastexcoords.md "mesh.schema.json#/definitions/primitive/properties/hasTexCoords")

#### hasTexCoords Type

`boolean[]`

#### hasTexCoords Constraints

**maximum number of items**: the maximum number of items for this array is: `4`

**minimum number of items**: the minimum number of items for this array is: `4`

### indiceBufferView

an index into bufferviews list

`indiceBufferView`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-indicebufferview.md "mesh.schema.json#/definitions/primitive/properties/indiceBufferView")

#### indiceBufferView Type

`number`

#### indiceBufferView Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### vertexBufferView

an index into bufferviews list

`vertexBufferView`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-vertexbufferview.md "mesh.schema.json#/definitions/primitive/properties/vertexBufferView")

#### vertexBufferView Type

`number`

#### vertexBufferView Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## Definitions group mesh

Reference this group by using

```json
{"$ref":"mesh.schema.json#/definitions/mesh"}
```

| Property              | Type     | Required | Nullable       | Defined by                                                                                                      |
| :-------------------- | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom) | `string` | Optional | cannot be null | [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/mesh/properties/copyFrom")   |
| [lods](#lods)         | `array`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-mesh-properties-lods.md "mesh.schema.json#/definitions/mesh/properties/lods") |

### copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/mesh/properties/copyFrom")

#### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

### lods



`lods`

* is optional

* Type: `object[]` ([MeshLod](mesh-definitions-meshlod.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-mesh-properties-lods.md "mesh.schema.json#/definitions/mesh/properties/lods")

#### lods Type

`object[]` ([MeshLod](mesh-definitions-meshlod.md))

#### lods Constraints

**minimum number of items**: the minimum number of items for this array is: `1`

## Definitions group skin

Reference this group by using

```json
{"$ref":"mesh.schema.json#/definitions/skin"}
```

| Property                                    | Type     | Required | Nullable       | Defined by                                                                                                                                        |
| :------------------------------------------ | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------ |
| [copyFrom](#copyfrom-1)                     | `string` | Optional | cannot be null | [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/skin/properties/copyFrom")                                     |
| [jointsRadius](#jointsradius)               | `number` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshskin-properties-jointsradius.md "mesh.schema.json#/definitions/skin/properties/jointsRadius")               |
| [joints](#joints)                           | `array`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshskin-properties-joints.md "mesh.schema.json#/definitions/skin/properties/joints")                           |
| [inverseBindMatrices](#inversebindmatrices) | `number` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshskin-properties-inversebindmatrices.md "mesh.schema.json#/definitions/skin/properties/inverseBindMatrices") |

### copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/skin/properties/copyFrom")

#### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

### jointsRadius

the radius of joints used for BV calculation

`jointsRadius`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshskin-properties-jointsradius.md "mesh.schema.json#/definitions/skin/properties/jointsRadius")

#### jointsRadius Type

`number`

#### jointsRadius Default Value

The default value is:

```json
0.1
```

### joints



`joints`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshskin-properties-joints.md "mesh.schema.json#/definitions/skin/properties/joints")

#### joints Type

`number[]`

### inverseBindMatrices

an index into the bufferview list

`inverseBindMatrices`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshskin-properties-inversebindmatrices.md "mesh.schema.json#/definitions/skin/properties/inverseBindMatrices")

#### inverseBindMatrices Type

`number`

#### inverseBindMatrices Constraints

**minimum**: the value of this number must greater than or equal to: `0`
