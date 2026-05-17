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