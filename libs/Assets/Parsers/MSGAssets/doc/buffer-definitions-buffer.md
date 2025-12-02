# Buffer Schema

```txt
buffer.schema.json#/definitions/buffer
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [buffer.schema.json\*](json/defs/buffer.schema.json "open original schema") |

## buffer Type

`object` ([Buffer](buffer-definitions-buffer.md))

# buffer Properties

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                |
| :------------ | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------ |
| [name](#name) | `string` | Optional | cannot be null | [MSGAssets-buffer](buffer-definitions-buffer-properties-name.md "buffer.schema.json#/definitions/buffer/properties/name") |
| [uri](#uri)   | `string` | Required | cannot be null | [MSGAssets-buffer](buffer-definitions-buffer-properties-uri.md "buffer.schema.json#/definitions/buffer/properties/uri")   |

## name

the object's name, important because it can be used to reference it

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-buffer-properties-name.md "buffer.schema.json#/definitions/buffer/properties/name")

### name Type

`string`

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is required

* Type: `string` ([URI](buffer-definitions-buffer-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-buffer-properties-uri.md "buffer.schema.json#/definitions/buffer/properties/uri")

### uri Type

`string` ([URI](buffer-definitions-buffer-properties-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```
