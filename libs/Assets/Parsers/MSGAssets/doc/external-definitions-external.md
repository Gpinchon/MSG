# External Schema

```txt
external.schema.json#/definitions/external
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [external.schema.json\*](json/defs/external.schema.json "open original schema") |

## external Type

`object` ([External](external-definitions-external.md))

# external Properties

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                          |
| :------------ | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name) | `string` | Required | cannot be null | [MSGAssets-external](external-definitions-external-properties-name.md "external.schema.json#/definitions/external/properties/name") |
| [uri](#uri)   | `string` | Required | cannot be null | [MSGAssets-external](external-definitions-uri.md "external.schema.json#/definitions/external/properties/uri")                       |

## name



`name`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-external](external-definitions-external-properties-name.md "external.schema.json#/definitions/external/properties/name")

### name Type

`string`

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is required

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-external](external-definitions-uri.md "external.schema.json#/definitions/external/properties/uri")

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
