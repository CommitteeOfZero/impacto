root.MainMenu = {
    Type: MainMenuType.None,
    ButtonBackgroundSprite: "MainMenuButtonBackground",
    ButtonPromptsSprite: "MainMenuButtonPrompts",
    ButtonBackgroundStartX: 1257,
    ButtonBackgroundX: 0,
    ButtonBackgroundY: 681,
    ButtonBackgroundTargetWidth: 943,
    ButtonBackgroundSprStartX: 1499,
    MenuEntriesSprites: [],
    MenuEntriesHighlightedSprites: [],
    MenuEntriesX: 0,
    MenuEntriesXOffset: 100,
    MenuEntriesFirstY: 220,
    MenuEntriesYPadding: 50,
};

for (var i = 0; i < 8; i++) {
    root.Sprites["MainMenuEntry" + i] = {
        Sheet: "Data",
        Bounds: {
            X: 964,
            Y: 296 + i * 38,
            Width: root.MainMenu.MenuEntriesTargetWidth,
            Height: 26
        },
        BaseScale: { X: 1280 / 960, Y: 720 / 544 }
    };
    root.MainMenu.MenuEntriesSprites.push("MainMenuEntry" + i);
}

for (var i = 0; i < 8; i++) {
    root.Sprites["MainMenuEntryHighlighted" + i] = {
        Sheet: "Data",
        Bounds: {
            X: 964,
            Y: 2 + i * 38,
            Width: root.MainMenu.MenuEntriesTargetWidth,
            Height: 26
        },
        BaseScale: { X: 1280 / 960, Y: 720 / 544 }
    };
    root.MainMenu.MenuEntriesHighlightedSprites.push("MainMenuEntryHighlighted" + i);
}

root.Sprites["MainMenuBackground"] = {
    Sheet: "Menu",
    Bounds: { X: 0, Y: 0, Width: 960, Height: 544 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["MainMenuButtonBackground"] = {
    Sheet: "Menu",
    Bounds: { X: root.MainMenu.ButtonBackgroundSprStartX, Y: 975, Width: 0, Height: 30 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["MainMenuButtonPrompts"] = {
    Sheet: "Menu",
    Bounds: { X: 1231, Y: 528, Width: 400, Height: 21 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};