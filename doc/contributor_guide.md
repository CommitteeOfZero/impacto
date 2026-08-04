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

## General engine information
To get an overall understanding of how the original Mages. engine functions please refer to https://committeeofzero.gitbooks.io/mages-engine-compendium/content/

## Making simple changes
The main game loop is located in `/src/game.cpp`. The main scripting execution loop is located in `/src/vm/vm.cpp`.

All of Mages. engine games use bytecode script files to define game behaviour.
The scripting engine instructions are split into groups and implemented in their respective files, for example the instructions that alter the control flow are located in `/src/vm/inst_controlflow.cpp`.
In order to make implementing new instructions easier, all instruction implementations use macros. A simple instruction implementation would be defined as:

```cpp
VmInstruction(InstJump) {
  StartInstruction;
  PopLocalLabel(labelAdr);
  thread->Ip = labelAdr;
}
```

After defining an instruction, it must be placed into the instruction table for the desired instruction set. Instruction set definitions are located in `/src/vm/opcodetables_*.cpp`

## Making simple UI changes
Since each game has its own custom interface, UI elements are implemented using class hierarchy. Base class implementations are located in `/src/hud/`, while specific class implementations are located in `/src/games/<game_name>/`. The UI element implementations used along with their display specifications (co-ordinates, animation parameters etc) for each game are defined by the profile configuration files, located in `/profiles/<profile_name>/hud/`.

Each UI element class implements an Update functions, which updates dynamic values, and a Render function, which draws sprites on the screen. For example, you can draw a simple sprite inside the Render function by using the following function (defined in `/src/renderer2d.h`):

```cpp
Renderer->DrawSprite(Sprite, glm::vec2(X, Y));
```

Sprites are defined in profile configuration files. For example, in order to define a simple sprite that uses a texture from the system archive, do the following:

1. In the apporpriate UI definition file located in `/src/profile/games/<game_name>/` define a `Sprite` (defined in `/src/spritesheet.h`) variable, for example:

```cpp
Sprite BackgroundSprite;
```

2. In profile sprites definition file located in `/profiles/<profile_name>/sprites.lua` in `root.SpriteSheets` list, define a spritesheet, for example:

```lua
["Data"] = {
    Path: { Mount = "system", Id = 5 },
    DesignWidth = 2048,  --Spritesheet width
    DesignHeight = 1024  --Spritesheet height
},
```

3. In the appropriate profile UI definition file located in `/profiles/<profile_name>/hud/` add a sprite definition to the global `root.Sprites` dictionary, for example:

```lua
root.Sprites["TitleMenuBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};
```

4. In the appropriate profile UI definition file located in `/profiles/<profile_name>/hud/` in the UI element object define the sprite variable, for example:

```lua
root.TitleMenu = {
    BackgroundSpriteProfile = "TitleMenuBackground"
};
```

5. In the appropriate UI definition file located in `/src/profile/games/<game_name>/` in the `Configure` function, get the sprite object defined in the profile UI definition file, for example:

```cpp
BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSpriteProfile");
```

You can now use the defined sprite in the appropriate UI element.  
