# MSGAssets-external Schema

```txt
external.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [external.schema.json](json/defs/external.schema.json "open original schema") |

## MSGAssets-external Type

`object` ([MSGAssets-external](external.md))

# MSGAssets-external Definitions

## Definitions group uri

Reference this group by using

```json
{"$ref":"external.schema.json#/definitions/uri"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group fromEntity

Reference this group by using

```json
{"$ref":"external.schema.json#/definitions/fromEntity"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group fromURI

Reference this group by using

```json
{"$ref":"external.schema.json#/definitions/fromURI"}
```

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                        |
| :------------ | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name) | `string` | Optional | cannot be null | [MSGAssets-external](external-definitions-fromuri-properties-name.md "external.schema.json#/definitions/fromURI/properties/name") |
| [uri](#uri)   | `string` | Required | cannot be null | [MSGAssets-external](external-definitions-fromuri-properties-uri.md "external.schema.json#/definitions/fromURI/properties/uri")   |

### name

the object's name, important because it can be used to reference it

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-external](external-definitions-fromuri-properties-name.md "external.schema.json#/definitions/fromURI/properties/name")

#### name Type

`string`

### uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is required

* Type: `string` ([URI](external-definitions-fromuri-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-external](external-definitions-fromuri-properties-uri.md "external.schema.json#/definitions/fromURI/properties/uri")

#### uri Type

`string` ([URI](external-definitions-fromuri-properties-uri.md))

#### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

## Definitions group external

Reference this group by using

```json
{"$ref":"external.schema.json#/definitions/external"}
```

| Property        | Type     | Required | Nullable       | Defined by                                                                                                                          |
| :-------------- | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name-1) | `string` | Required | cannot be null | [MSGAssets-external](external-definitions-external-properties-name.md "external.schema.json#/definitions/external/properties/name") |
| [uri](#uri-1)   | `string` | Required | cannot be null | [MSGAssets-external](external-definitions-external-properties-uri.md "external.schema.json#/definitions/external/properties/uri")   |

### name



`name`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-external](external-definitions-external-properties-name.md "external.schema.json#/definitions/external/properties/name")

#### name Type

`string`

### uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is required

* Type: `string` ([URI](external-definitions-external-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-external](external-definitions-external-properties-uri.md "external.schema.json#/definitions/external/properties/uri")

#### uri Type

`string` ([URI](external-definitions-external-properties-uri.md))

#### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")
