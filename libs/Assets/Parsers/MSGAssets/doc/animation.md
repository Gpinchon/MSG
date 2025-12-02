# MSGAssets-animation Schema

```txt
animation.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [animation.schema.json](json/defs/animation.schema.json "open original schema") |

## MSGAssets-animation Type

`object` ([MSGAssets-animation](animation.md))

# MSGAssets-animation Definitions

## Definitions group animation

Reference this group by using

```json
{"$ref":"animation.schema.json#/definitions/animation"}
```

| Property    | Type     | Required | Nullable       | Defined by                                                                                                                             |
| :---------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------- |
| [uri](#uri) | `string` | Optional | cannot be null | [MSGAssets-animation](animation-definitions-animation-properties-uri.md "animation.schema.json#/definitions/animation/properties/uri") |

### uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](animation-definitions-animation-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-animation](animation-definitions-animation-properties-uri.md "animation.schema.json#/definitions/animation/properties/uri")

#### uri Type

`string` ([URI](animation-definitions-animation-properties-uri.md))

#### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

#### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```
