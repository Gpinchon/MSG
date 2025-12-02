# URI Schema

```txt
animation.schema.json#/definitions/animation/properties/uri
```

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                        |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [animation.schema.json\*](json/defs/animation.schema.json "open original schema") |

## uri Type

`string` ([URI](animation-definitions-animation-properties-uri.md))

## uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

## uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```
