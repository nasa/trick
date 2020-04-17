| [Home](/trick) → [How‐To Guides](How‐To-Guides) → Exclude Code |
|----------------------------------------------------------|

# How do I exclude code from being processed by Trick?

#### Exclude directories via TRICK\_ICG\_EXCLUDE makefile or environment variable:
```
export TRICK_ICG_EXCLUDE = dir1:dir2
```

#### Exclude files via Trick header comment tag:
```c++
// Classic header
/*
PURPOSE: (some code)
ICG: (No)
*/

// Doxygen style header
/*
@trick_parse(dependencies_only)
*/
```

#### Ignore comments throughout the header:
```c++
// Classic header
/*
PURPOSE: (some code)
ICG: (NO_COMMENT)
*/

// Doxygen style header
/*
@trick_parse(attributes)
*/
```

#### Ignore individual classes via the header:
```c++
// Classic header
/*
PURPOSE:
ICG_IGNORE_TYPE: ((class_name)(another_type))
*/

//Doxygen style header
/*
@trick_exclude_typename(class_name)
@trick_exclude_typename(another_type)
*/
```
