# This a list of every built-in module and it's methods

## \<array\>

### array.len(arr)
  returns the lenght of the array
- arr
  - **description:** the array you get the lenght of 
  - **type:** array
- **returns:** number; the amount of items in array

### array.append(arr, *)
  appends the the items after the array to that array
- arr
  - **description:** the array you append to 
  - **type:** array
- \*
  - **description:** any amount of arguments you want to append
  - **type:** any
- **returns:** array; the instance of the array it assigned to

### array.pop(arr)
  pops the last item of the array
- arr
  - **description:** the array you pop the item off
  - **type:** array
- **returns:** any; the item it popped


## \<regex\>

### regex.compile(pattern)
  compiles the string given into a pattern
- pattern
  - **description:** pattern to be compiled
  - **type:** string
- **returns:** Regex_Pattern; the pattern it compiled

### regex.match(pattern, string)
  matches the pattern to a string
- pattern
  - **description:** the pattern to match against
  - **type:** Regex_Pattern | string
- string
  - **description:** the string to search through
  - **type:** string
- **returns:** Array; the matches (if any), **[regex_match, 1st_group, 2nd_group]**

### regex.replace(pattern, string, replace_string)
  replaces the matched regex with a replacement string
- pattern
  - **description:** the pattern to match against
  - **type:** Regex_Pattern | string
- string
  - **description:** the string to search through
  - **type:** string
- replace_string
  - **description:** the string to replace the match with
  - **type:** string
- **returns:** Array; the matches (if any), **[regex_match, 1st_group, 2nd_group]**