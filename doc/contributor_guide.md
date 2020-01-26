# Code style

This project includes a clang-format file, which should be used for formatting all of the C++ code files. You can also integrate clang-format directly into your IDE of choice.

CamelCase should be used for everything except local variables. lowerCamelCase should be used for local variables.

All of the code goes into the Impacto namespace. Additionally Systems, UI element bases and game specifc implementations go into their own namespaces.

For example:

```cpp
// Input handling system implementation
namespace Impacto {
namespace Input {
   // ...
}  // namespace Input
}  // namespace Impacto
```

```cpp
//Main menu UI element base implementation
namespace Impacto {
namespace MainMenu {
   // ...
}  // namespace MainMenu
}  // namespace Impacto
```

```cpp
//Robotics;Notes Elite specifc UI elements implementations
namespace Impacto {
namespace RNE {
   // ...
}  // namespace RNE
}  // namespace Impacto
```

