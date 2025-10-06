# Welcome the the EastLang Docs

## Basic "stuff"

### 1. Everything is an expression

### 2. If "statements"
If statements
```el
if check1 {
  print("check1 passed!")
} else_if check2 {
  print("check2 passed!")
} else {
  print("checks didn't pass!")
}
```

### 3. Comments
Currently only block comments are available and are defined with ``` ` ```

```el
print(x) `this line prints x`
```

### 4. Local declarations
By using the local keyword you can limit the definition to the current scope (without using it, it first tries find (and override) the value somewhere in the outer scope), things like: Callables (Functions), If "statements", while loops, have their own scope
```el
x = 10
if true {
  local x = 15
  print(x)
}
print(x)
```
### 5. Constant declarations
Using the `const` keyword the declared variable can't be changed (it also acts as a `local` keyword)
```el
const x = 5
if true {
  const x = 100
  print(x)
}
print(x)
x = 1 ` Errors `
```
### 6. Functions
```el
const add = callable(x, y) {
  x + y
}

print(add(5, 4))
```

### 7. Imports
You can import other files from your script using the `@import("file.el")` syntax, you can also import built-in modules using the `@import("<module_name>")` syntax (more info on [their docs page](./built-in_modules.md))
```el
const array = @import("<array>")
print(array.len([1, 2, 3, 4]))
```
```el
const your_module = @import("utils/main.el")
print(your_module.your_func())
```

### 8. Built-in functions
The reference is [here](./built-in_functions.md)

### 9. String escapes
The language supports the basic escape sequences
```el
print("hello \n World!")
print("hello \t World!")
print("hello \r World!")
print("hello \660 World!")
print("hello \xF8 World!")
```


### 10. empty
Empty is a type representing nothing
```el
ret = print(hi)
print(ret)
```

### 11. logical operations
```el
print(1 and 0)
print(1 or 0)
print(1 xor 0)
print(not 1 or 0)
print(! 1 or 0)
```

### 12. comparison operation
```el
print(1 == 1)
print(1 != 2)
print(1 <= 2)
print("etc.")
```

### 13. argv
When you call the program with bonus arguments like `./EastLangInterpreter.exe file.el hello world` you can access those values in the argv
```
print(argv) `[file.el, hello, world, ]`
```
