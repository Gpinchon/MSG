# BufferView Schema

```txt
buffer.schema.json#/definitions/bufferView
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [buffer.schema.json\*](json/defs/buffer.schema.json "open original schema") |

## bufferView Type

`object` ([BufferView](buffer-definitions-bufferview.md))

# bufferView Properties

| Property          | Type     | Required | Nullable       | Defined by                                                                                                                            |
| :---------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------ |
| [uri](#uri)       | `string` | Optional | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-properties-uri.md "buffer.schema.json#/definitions/bufferView/properties/uri")       |
| [name](#name)     | `string` | Optional | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-properties-name.md "buffer.schema.json#/definitions/bufferView/properties/name")     |
| [size](#size)     | `number` | Optional | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-properties-size.md "buffer.schema.json#/definitions/bufferView/properties/size")     |
| [offset](#offset) | `number` | Optional | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-properties-offset.md "buffer.schema.json#/definitions/bufferView/properties/offset") |
| [buffer](#buffer) | `number` | Optional | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-properties-buffer.md "buffer.schema.json#/definitions/bufferView/properties/buffer") |

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](buffer-definitions-bufferview-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-properties-uri.md "buffer.schema.json#/definitions/bufferView/properties/uri")

### uri Type

`string` ([URI](buffer-definitions-bufferview-properties-uri.md))

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

the object's name, important because it can be used to reference it

> required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-properties-name.md "buffer.schema.json#/definitions/bufferView/properties/name")

### name Type

`string`

## size

the size of this buffer view in bytes, only necessary if this bufferView uses a buffer a its source

> required if uri is not present

`size`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-properties-size.md "buffer.schema.json#/definitions/bufferView/properties/size")

### size Type

`number`

### size Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## offset

the offset of this buffer view inside the buffer in bytes, only necessary if this bufferView uses a buffer a its source

> required if uri is not present

`offset`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-properties-offset.md "buffer.schema.json#/definitions/bufferView/properties/offset")

### offset Type

`number`

### offset Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## buffer

the index of the buffer into the buffers list

> required if uri is not present

`buffer`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-properties-buffer.md "buffer.schema.json#/definitions/bufferView/properties/buffer")

### buffer Type

`number`

### buffer Constraints

**minimum**: the value of this number must greater than or equal to: `0`
