# This a list of every built-in function and constant

### true, false

### empty

### break, continue
technically a constant (everything is a expression so yk)

### pi, e

### argv
an array of arguments passed in to the interpreter

### print( \* )
Print any amount of values
- \*
  - **description:** any amount of arguments you want to print
  - **type:** any
- **returns:** empty

### type(obj)
Check the type of `obj`
- obj
  - **description:** the object you want to check the type of
  - **type:** any
- **returns:** string; returns a type name of an object

### sleep(time)
Sleep for `time` amount of seconds
- time
  - **description:** the amount of time you want to sleep for in seconds
  - **type:** number
- **returns:** empty

### input( \* )
Get user input (1 line)
- \*
  - **description:** any amount of arguments you want to print before asking for input
  - **type:** any; optional
- **returns:** empty

### dir(module)
List every method in a module
- module
  - **description:** a module you want to check
  - **type:** module
- **returns:** array; array of every method found in the module

### ord(str)
Turns a one lenght string into the number representation
- str
  - **description:** a string lenght of one that you want to get the number representation of
  - **type:** string
- **returns:** number

### chr(num)
Turns a number representation into a string
- num
  - **description:** number that you want to get the string representation of
  - **type:** number
- **returns:** string

## Special Expressions

### @name() or @name
returns in what way the script is executed
- **returns:** string; `main` if the script is run from the terminal, `module` when `@import`'ed, `inserted` when `@include`'ed

### @path() or @path
returns the full path in which the current file is
- **returns:** string; `main` if the script is run from the terminal, `module` when `@import`'ed, `inserted` when `@include`'ed

### @import(file, *)
evaluate the file in a seperate environment and give it back as a object
- file
  - **description:** the relative path to the file you want to import
  - **type:** string
- \*
  - **description:** any amount of arguments you want to give to the imported file, (you can access them in argv in the module)
  - **type:** any; optional
- **returns:** module

### @include(file)
evaluate the file in the current environment and give it back as a object
- file
  - **description:** the relative path to the file you want to include
  - **type:** string
- **returns:** module