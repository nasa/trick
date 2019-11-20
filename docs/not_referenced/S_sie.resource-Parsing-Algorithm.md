# Data Structures

## Classes
- **MemberInfo**  
  a class that encapsulates the data contained in a `<top_level_object>` or `<member>` xml element
- **EnumInfo**  
  a class that encapsulates the data contained in an `<enumeration>` xml element

## Lists
- **allInstances**  
  a list of `MemberInfo`s
- **rootInstances**  
  another list of `MemberInfo`s

## Hash Maps
- **typeHashMap**  
  a hash map whose keys are strings and whose values are lists of `MemberInfo`s
- **enumHashMap**  
  a hash map whose keys are strings and whose values are `EnumInfo`s

# Logic Pseudo-Code

```
for each topLevelObject in (xml elements of type <top_level_object>):
    info = new MemberInfo(topLevelObject)
    allInstances.add(info)
    rootInstances.add(info)

for each clazz in (xml elements of type <class>):
    memberList = new List()
    typeHashMap[clazz.name] = memberList
    for each member in (xml element of type <member>):
        info = new MemberInfo(member)
        allInstances.add(info)
        memberList.add(info)

for each enum in (xml elements of type <enumeration>):
    enumHashMap[enum.name] = new EnumInfo(enum)

for each instance in allInstances:
    instance.children = typeHashMap[instance.type]       // one of these
    instance.enumeration = enumHashMap[instance.type]    // will be NULL
```

# Walking the Tree
The entire variable hierarchy can be obtained via a pre-order depth-first traversal of `rootInstances`. The other data structures are now redundant and can be discarded.