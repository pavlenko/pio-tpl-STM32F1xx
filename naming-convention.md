# Classes and Class Attributes Names

- class: `ClassName{}`
- method: `void methodName(){}`
- public property: `int propertyName;`
- private property: `int m_propertyName;` or `int mPropertyName;`
- template arg: `template <typename TypeT>` or `template <typename tType>`

# Functions and Function Argument Names

- name: `void functionName(){}` or `int FunctionName(){}`
- arguments: `void f(int firstArg, int secondArg){}`

# Variables

- name: `int totalCost;` or `int total_cost;`
- pointers: `int *pVariable;`
- reference: `int &rVariable;`
- static: `static int sCount;`
- constants: `const int MAX_LENGTH = 1;`

# File names

- should have only lowercase letters, `-` or `_`
- must ends with `.c/.h` for C code and `.cpp/.hpp` for C++ code
- avoid use same names with built in or external libs
- each header must contain header guard via `#ifndef` or `#pragma once`

# Includes

- lib files must be included only via `<>` brackets
- sort order of includes in source files:
  - direct header
  - built in libs
  - external libs
  - other project headers
