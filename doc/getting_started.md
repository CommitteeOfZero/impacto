# Getting started

## Running games
The following directories must be in the same directory as the Impacto executable:

* games - directory that contains game resource files
* profiles - directory that contains profile definition files
* shaders - directory that contains shader files

These folders can be found in the repository root. When building on Windows they will automatically be copied to your specified install location.

In order to launch a game using Impacto, a file named `profile.txt` that contains the name of the profile to launch, must be placed near the Impacto executable.

Following profiles are currently available:

* chlcc - Chaos;Head Love Chu Chu
* cclcc - Chaos;Child Love Chu Chu
* darling - Steins;Gate Darling of Loving Vows
* dash - Robotics;Notes DaSH
* mo6tw - Memories Off 6 ~T-Wave~
* rne - Robotics;Notes Elite (PS Vita)
* characterviewer - A viewer for 2D characters
* modelviewer - A viewer for 3D models from Robotics;Notes Elite
* modelviewer-dash - A viewer for 3D models from Robotics;Notes DaSH

For the list of required game resource files refer to the `vfs.js` file located in desired game profile directory. The resource files should be placed in `/games/<profile_name>/gamedata/` directory.

## Repository map
```
├───doc                               - Documentation
│   └───font-lb                       - LanguageBarrier font documentation
├───docker                            - Docker files for Emscripten build
│   └───impacto-emscripten            - 
├───games                             - Game resources directory
│   └───rne                           - 
│       └───font-lb                   - LanguageBarrier font files
├───modules                           - CMake module lookup files
├───profiles                          - Profile definition files
├───src                               - Main source directory
│   ├───3d                            - 3D system source files
│   ├───audio                         - Audio system source files
│   ├───games                         - Game specific source files
│   ├───hud                           - UI base source files
│   ├───io                            - IO system source files
│   ├───profile                       - Profile system source files
│   │   ├───games                     - 
│   │   └───hud                       - 
│   ├───shaders                       - Shader source files
│   ├───texture                       - Texture loading source files
│   └───vm                            - SC3 Virtual Machine source files
│       └───interface                 - Virtual Machine <-> Engine interface source files
└───vendor                            - Third-party libraries
```

## Making simple changes
The main game loop is located in `/src/game.cpp`. The main scripting execution loop is located in `/src/vm/vm.cpp`.

All of the instructions are split into groups and implemented in their respective files, for example the instructions that alter the control flow are located in `/src/vm/inst_controlflow.cpp`.
In order to make implementing new instructions easier, all instruction implementations use macros. A simple instruction implementation would be defined as:

```
VmInstruction(InstJump) {
  StartInstruction;
  PopLocalLabel(labelAdr);
  thread->Ip = labelAdr;
}
```

After defining an instruction, it must be placed into the instruction table for the desired instruction set. Instruction set definitions are located in `/src/vm/opcodetables_*.cpp`

## Making simple UI changes
Since each game has its own custom interface, UI elements are implemented using class hierarchy. Specific implementations are located in `/src/games/<game_name>/`, while common implementations are located in `/src/hud/`. The type of UI that the game uses and its specifics are defined by the profile configuration files, located in `/profiles/<profile_name>/hud/`.

Each UI element class implements an Update and Render functions. You can draw a simple sprite inside the Render function by using the following function (defined in `/src/renderer2d.h`:

```
Renderer2D::DrawSprite(Sprite, glm::vec2(X, Y));
```

Sprites are definted in profile configuration files. For example, in order to define a simple sprite that uses a texture from the system archive, do the following:

* In apporpriate UI definition file located in `/src/profile/games/<game_name>/` define a `Sprite` (defined in `/src/spritesheet.h`) variable, for example:

```
Sprite BackgroundSprite;
```

* In profile sprites definition file located in `/profiles/<profile_name>/sprites.js` in `root.SpriteSheets` list, define a spritesheet, for example:

```
    "Data": {
        Path: { Mount: "system", Id: 5 },
        DesignWidth: 2048,  //Spritesheet width
        DesignHeight: 1024  //Spritesheet height
    },
```

* In apporpriate profile UI definition file located in `/profiles/<profile_name>/hud/` add a sprite definition to the global `root.Sprites` dictionary, for example:

```
root.Sprites["TitleMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 },
};
```

* In apporpriate profile UI definition file located in `/profiles/<profile_name>/hud/` in the UI element object define the sprite variable, for example:

```
root.TitleMenu = {
    BackgroundSpriteProfile: "TitleMenuBackground",
};
```

* In apporpriate UI definition file located in `/src/profile/games/<game_name>/` in the `Configure` function, get the sprite object defined in the profile UI definition file, for example:

```
BackgroundSprite = EnsureGetMemberSprite("BackgroundSpriteProfile");
```

You can now use the defined sprite in the appropriate UI element.  