root.SystemMenu = {
    Type: SystemMenuType.MO6TW,
    DrawType: DrawComponentType.SystemMenu,
    SystemMenuBackgroundSprite: "SystemMenuBackground",
    SystemMenuX: 993,
    SystemMenuY: 114,
    MenuEntriesSprites: [],
    MenuEntriesHighlightedSprite: "MenuEntriesHSprite",
    MenuEntriesNum: 7,
    MenuEntriesHNum: 0,
    MenuEntriesX: 1001,
    MenuEntriesXOffset: 100,
    MenuEntriesFirstY: 126,
    MenuEntriesYPadding: 54,
    MenuEntriesTargetWidth: 254,
    FadeInDuration: 0.2,
    FadeOutDuration: 0.2
};

for (var i = 0; i < 7; i++) {
    root.Sprites["SystemMenuEntry" + i] = {
        Sheet: "Data",
        Bounds: {
            X: 584,
            Y: i * 32,
            Width: root.SystemMenu.MenuEntriesTargetWidth,
            Height: 30
        }
    };
    root.SystemMenu.MenuEntriesSprites.push("SystemMenuEntry" + i);
}

root.Sprites["MenuEntriesHSprite"] = {
    Sheet: "Data",
    Bounds: { X: 65, Y: 1, Width: 94, Height: 30 }
};

root.Sprites["SystemMenuBackground"] = {
    Sheet: "Data",
    Bounds: { X: 1765, Y: 1, Width: 282, Height: 380 }
};
