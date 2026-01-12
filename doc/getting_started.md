# Getting started

## Game profiles

To handle the complexity of the full range of MAGES. engine titles (originally developed on different engine branches with many per-game hardcoded customizations) within a single codebase and binary, impacto uses a data-driven, modular design.

On application load, a set of scripts we call *profile* is executed, generating a (declarative) configuration specifying the chosen game's featureset (such as implemented VM instructions or the 3D scene in Robotics;Notes), parameters (like dimensions, names etc. ), asset location/use and "game objects" (menus/UI, database of 3D models in Robotics;Notes, built from primitives). `/profiles/` contains these scripts. `/src/profile/` has the host code to execute them and manually deserialize the resulting configuration into global state.

## Running games
The following directories must be in the same directory as the impacto executable:

* games - directory that contains game resource files
* profiles - directory that contains profile definition files
* shaders - directory that contains shader files

These folders can be found in the repository root. When using the *install* build step they are automatically copied to your specified install location.

In order to launch a game using impacto, until UI is developed for this, you must specify the game that needs to be launched using either of these methods:
- A file named `profile.txt` that contains the name of the profile to launch placed next to the impacto executable.
- A command line argument with the name of the profile as the first argument after the executable name.

The following profiles are currently available:

* **chlcc** - Chaos;Head Love Chu Chu
* **cclcc** - Chaos;Child Love Chu Chu
* **darling** - Steins;Gate My Darling's Embrace
* **dash** - Robotics;Notes DaSH
* **mo6tw** - Memories Off 6 ~T-Wave~
* **rne** - Robotics;Notes Elite (PS Vita)
* **characterviewer** - A viewer for 2D characters
* **modelviewer** - A viewer for 3D models from Robotics;Notes Elite
* **modelviewer-dash** - A viewer for 3D models from Robotics;Notes DaSH

For the list of required game resource files refer to the `vfs.lua` file located in desired game profile directory. The resource files should be placed in `/games/<profile_name>/gamedata/` directory.

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
