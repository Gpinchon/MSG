# MeshPrimitive Schema

```txt
mesh.schema.json#/definitions/primitive
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## primitive Type

`object` ([MeshPrimitive](mesh-definitions-meshprimitive.md))

# primitive Properties

| Property                              | Type      | Required | Nullable       | Defined by                                                                                                                                            |
| :------------------------------------ | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------- |
| [uri](#uri)                           | `string`  | Optional | cannot be null | [MSGAssets-mesh](external-definitions-uri.md "mesh.schema.json#/definitions/primitive/properties/uri")                                                |
| [name](#name)                         | `string`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-name.md "mesh.schema.json#/definitions/primitive/properties/name")                         |
| [drawingMode](#drawingmode)           | `string`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-primitivedrawingmode.md "mesh.schema.json#/definitions/primitive/properties/drawingMode")                           |
| [castShadow](#castshadow)             | `boolean` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-castshadow.md "mesh.schema.json#/definitions/primitive/properties/castShadow")             |
| [hasTexCoords](#hastexcoords)         | `array`   | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-hastexcoords.md "mesh.schema.json#/definitions/primitive/properties/hasTexCoords")         |
| [indiceBufferView](#indicebufferview) | `number`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-indicebufferview.md "mesh.schema.json#/definitions/primitive/properties/indiceBufferView") |
| [vertexBufferView](#vertexbufferview) | `number`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-vertexbufferview.md "mesh.schema.json#/definitions/primitive/properties/vertexBufferView") |

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-mesh](external-definitions-uri.md "mesh.schema.json#/definitions/primitive/properties/uri")

### uri Type

`string` ([URI](external-definitions-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```

## name

this object's name, important because it allows for it to be referenced in another asset file

> required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-name.md "mesh.schema.json#/definitions/primitive/properties/name")

### name Type

`string`

## drawingMode



`drawingMode`

* is optional

* Type: `string` ([PrimitiveDrawingMode](mesh-definitions-primitivedrawingmode.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-primitivedrawingmode.md "mesh.schema.json#/definitions/primitive/properties/drawingMode")

### drawingMode Type

`string` ([PrimitiveDrawingMode](mesh-definitions-primitivedrawingmode.md))

### drawingMode Constraints

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

### drawingMode Default Value

The default value is:

```json
"Triangles"
```

## castShadow



`castShadow`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-castshadow.md "mesh.schema.json#/definitions/primitive/properties/castShadow")

### castShadow Type

`boolean`

### castShadow Default Value

The default value is:

```json
true
```

## hasTexCoords

set to true if this primitive has the corresponding set of texture coordinates

`hasTexCoords`

* is optional

* Type: `boolean[]`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-hastexcoords.md "mesh.schema.json#/definitions/primitive/properties/hasTexCoords")

### hasTexCoords Type

`boolean[]`

### hasTexCoords Constraints

**maximum number of items**: the maximum number of items for this array is: `4`

**minimum number of items**: the minimum number of items for this array is: `4`

## indiceBufferView

an index into bufferviews list

`indiceBufferView`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-indicebufferview.md "mesh.schema.json#/definitions/primitive/properties/indiceBufferView")

### indiceBufferView Type

`number`

### indiceBufferView Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## vertexBufferView

an index into bufferviews list

`vertexBufferView`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-properties-vertexbufferview.md "mesh.schema.json#/definitions/primitive/properties/vertexBufferView")

### vertexBufferView Type

`number`

### vertexBufferView Constraints

**minimum**: the value of this number must greater than or equal to: `0`
