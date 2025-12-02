# Untitled array in MSGAssets-specs Schema

```txt
specs.schema.json#/properties/externals
```

the path to external resources to load prior to loading this file

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                           |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [specs.schema.json\*](json/specs.schema.json "open original schema") |

## externals Type

`object[]` ([External](external-definitions-external.md))

## externals Constraints

**unique items**: all items in this array must be unique. Duplicates are not allowed.
