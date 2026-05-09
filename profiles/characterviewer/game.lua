include(root.BaseConfig.RootProfilesDir .. '/chlcc/game.lua');

root.GameFeatures = GameFeature.Renderer2D | GameFeature.CharacterViewer | GameFeature.Audio | GameFeature.Input;

root.WindowName = "Character Viewer";