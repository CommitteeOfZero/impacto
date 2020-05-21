root.MainMenu = {
    Type: MainMenuType.RNE,
    Background: {
        DurationIn: 0.75,
        DurationOut: 1.0,
        Sprite: "MainMenuBackground",
        Seed: 0,
        Rows: 7,
        Columns: 12,
        CenterY: 0.7,
        VanishingPointX: 0.66,
        Depth: 2,
        MaxAngle: Math.PI / 2
    },
    ButtonBackgroundSprite: "MainMenuButtonBackground",
    SkyBackgroundSprite: "MainMenuSkyBackground",
    SkyArrowSprite: "MainMenuSkyArrow",
    SkyTextSprite: "MainMenuSkyText",
    ButtonPromptsSprite: "MainMenuButtonPrompts",
    SkyBackgroundBeginX: -80,
    SkyBackgroundY: 0,
    SkyTextBeginX: 287,
    SkyTextY: 69,
    ButtonBackgroundStartX: 1257,
    ButtonBackgroundX: 0,
    ButtonBackgroundY: 681,
    ButtonBackgroundTargetWidth: 943,
    ButtonBackgroundSprStartX: 1499,
    MenuEntriesSprites: [],
    MenuEntriesHighlightedSprites: [],
    MenuEntriesNum: 8,
    MenuEntriesHNum: 8,
    MenuEntriesX: 0,
    MenuEntriesXSkew: 20,
    MenuEntriesXOffset: 100,
    MenuEntriesFirstY: 220,
    MenuEntriesYPadding: 50,
    MenuEntriesTargetWidth: 417,
    SkyInStartProgress: 0.285,
    SkyOutStartProgress: 0.715,
    SkyMoveDurationIn: 0.415,
    SkyMoveDurationOut: 0.415,
    EntriesMoveDurationIn: 0.4,
    EntriesMoveDurationOut: 0.4,
    HighlightDurationIn: 0.15,
    HighlightDurationOut: 0.15,
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

root.Sprites["MainMenuSkyBackground"] = {
    Sheet: "Data",
    Bounds: { X: 1383, Y: 534, Width: 418, Height: 90 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["MainMenuSkyArrow"] = {
    Sheet: "Data",
    Bounds: { X: 1802, Y: 534, Width: 70, Height: 90 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["MainMenuSkyText"] = {
    Sheet: "Data",
    Bounds: { X: 1875, Y: 587, Width: 119, Height: 30 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["MainMenuButtonPrompts"] = {
    Sheet: "Menu",
    Bounds: { X: 1231, Y: 528, Width: 400, Height: 21 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};