root.MainMenu = {
    Type: MainMenuType.MO6TW,
    MainMenuBackgroundSprite: "MainMenuBackground",
    MainMenuX: 993,
    MainMenuY: 114,
    MenuEntriesSprites: [],
    MenuEntriesHighlightedSprite: "MenuEntriesHSprite",
    MenuEntriesNum: 7,
    MenuEntriesHNum: 0,
    MenuEntriesX: 1001,
    MenuEntriesXOffset: 100,
    MenuEntriesFirstY: 126,
    MenuEntriesYPadding: 54,
    MenuEntriesTargetWidth: 254
};

for (var i = 0; i < 7; i++) {
    root.Sprites["MainMenuEntry" + i] = {
        Sheet: "Data",
        Bounds: {
            X: 584,
            Y: i * 32,
            Width: root.MainMenu.MenuEntriesTargetWidth,
            Height: 30
        }
    };
    root.MainMenu.MenuEntriesSprites.push("MainMenuEntry" + i);
}

root.Sprites["MenuEntriesHSprite"] = {
    Sheet: "Data",
    Bounds: { X: 65, Y: 1, Width: 94, Height: 30 }
};

root.Sprites["MainMenuBackground"] = {
    Sheet: "Data",
    Bounds: { X: 1765, Y: 1, Width: 282, Height: 380 }
};
