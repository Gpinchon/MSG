# FromProperties Schema

```txt
mesh.schema.json#/definitions/primitive/oneOf/0
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## 0 Type

unknown ([FromProperties](mesh-definitions-meshprimitive-oneof-fromproperties.md))

# 0 Properties

| Property                              | Type      | Required | Nullable       | Defined by                                                                                                                                                                         |
| :------------------------------------ | :-------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name)                         | `string`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-name.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/name")                         |
| [drawingMode](#drawingmode)           | `string`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-primitivedrawingmode.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/drawingMode")  |
| [castShadow](#castshadow)             | `boolean` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-castshadow.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/castShadow")             |
| [hasTexCoords](#hastexcoords)         | `array`   | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-hastexcoords.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/hasTexCoords")         |
| [indiceBufferView](#indicebufferview) | `number`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-indicebufferview.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/indiceBufferView") |
| [vertexBufferView](#vertexbufferview) | `number`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-vertexbufferview.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/vertexBufferView") |

## name

this object's name, important because it allows for it to be referenced in another asset file

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-name.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/name")

### name Type

`string`

## drawingMode



`drawingMode`

* is optional

* Type: `string` ([PrimitiveDrawingMode](mesh-definitions-meshprimitive-oneof-fromproperties-properties-primitivedrawingmode.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-primitivedrawingmode.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/drawingMode")

### drawingMode Type

`string` ([PrimitiveDrawingMode](mesh-definitions-meshprimitive-oneof-fromproperties-properties-primitivedrawingmode.md))

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

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-castshadow.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/castShadow")

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

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-hastexcoords.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/hasTexCoords")

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

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-indicebufferview.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/indiceBufferView")

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

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromproperties-properties-vertexbufferview.md "mesh.schema.json#/definitions/primitive/oneOf/0/properties/vertexBufferView")

### vertexBufferView Type

`number`

### vertexBufferView Constraints

**minimum**: the value of this number must greater than or equal to: `0`
