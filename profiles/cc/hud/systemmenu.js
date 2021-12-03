root.SystemMenu = {
    Type: SystemMenuType.RNE,
    DrawType: DrawComponentType.SystemMenu,
    Background: {
        DurationIn: 0.75,
        DurationOut: 1.0,
        Sprite: "SystemMenuBackground",
        Seed: 0,
        Rows: 7,
        Columns: 12,
        CenterY: 0.7,
        VanishingPointX: 0.66,
        Depth: 2,
        MaxAngle: Math.PI / 2
    },
    ButtonBackgroundSprite: "SystemMenuButtonBackground",
    SkyBackgroundSprite: "SystemMenuSkyBackground",
    SkyArrowSprite: "SystemMenuSkyArrow",
    SkyTextSprite: "SystemMenuSkyText",
    ButtonPromptsSprite: "SystemMenuButtonPrompts",
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
    root.Sprites["SystemMenuEntry" + i] = {
        Sheet: "Data",
        Bounds: {
            X: 964,
            Y: 296 + i * 38,
            Width: root.SystemMenu.MenuEntriesTargetWidth,
            Height: 26
        },
        BaseScale: { X: 1280 / 960, Y: 720 / 544 }
    };
    root.SystemMenu.MenuEntriesSprites.push("SystemMenuEntry" + i);
}

for (var i = 0; i < 8; i++) {
    root.Sprites["SystemMenuEntryHighlighted" + i] = {
        Sheet: "Data",
        Bounds: {
            X: 964,
            Y: 2 + i * 38,
            Width: root.SystemMenu.MenuEntriesTargetWidth,
            Height: 26
        },
        BaseScale: { X: 1280 / 960, Y: 720 / 544 }
    };
    root.SystemMenu.MenuEntriesHighlightedSprites.push("SystemMenuEntryHighlighted" + i);
}

root.Sprites["SystemMenuBackground"] = {
    Sheet: "Menu",
    Bounds: { X: 0, Y: 1088, Width: 1920, Height: 1080 },
};

root.Sprites["SystemMenuButtonBackground"] = {
    Sheet: "Menu",
    Bounds: { X: root.SystemMenu.ButtonBackgroundSprStartX, Y: 975, Width: 0, Height: 30 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["SystemMenuSkyBackground"] = {
    Sheet: "Data",
    Bounds: { X: 1383, Y: 534, Width: 418, Height: 90 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["SystemMenuSkyArrow"] = {
    Sheet: "Data",
    Bounds: { X: 1802, Y: 534, Width: 70, Height: 90 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["SystemMenuSkyText"] = {
    Sheet: "Data",
    Bounds: { X: 1875, Y: 587, Width: 119, Height: 30 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};

root.Sprites["SystemMenuButtonPrompts"] = {
    Sheet: "Menu",
    Bounds: { X: 1231, Y: 528, Width: 400, Height: 21 },
    BaseScale: { X: 1280 / 960, Y: 720 / 544 }
};