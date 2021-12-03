root.TitleMenu = {
    Type: TitleMenuType.MO6TW,
    DrawType: DrawComponentType.TitleMenu,
    PressToStartX: 497,
    PressToStartY: 402,
    PressToStartAnimDurationIn: 0.5,
    PressToStartAnimDurationOut: 0.5,
    PressToStartSprite: "TitleMenuPressToStart",
    BackgroundSprite: "TitleMenuBackground",
    CopyrightSprite: "TitleMenuCopyright",
    LogoSprite: "TitleMenuLogo",
    CopyrightX: 465,
    CopyrightY: 662,
    LogoX: 321,
    LogoY: 24,
    MenuBackgroundSprite: "MenuBackground",
    MenuItemLockedSprite: "MenuEntryLocked",
    MenuItemLockedSpriteH: "MenuEntryLockedH",
    MenuEntriesNum: 23,
    MenuEntriesSprites: [],
    MenuEntriesHighlightedSprites: [],
    MenuEntriesX: 513,
    MenuEntriesFirstY: 269,
    MenuEntriesYPadding: 32,
    MenuEntriesTargetWidth: 254,
    PrimaryFadeAnimDuration: 0.3,
    SecondaryMenuAnimTarget: { X: 110, Y: 0 },
    SecondaryMenuPadding: 20,
    SecondaryMenuAnimDuration: 0.512,
    SecondaryMenuAnimUnderX: 0,
    ExtraStoryItemCount: 3,
    ContinueItemCount: 2,
    MemoriesItemCount: 5,
    SystemItemCount: 2,
};

for (var i = 0; i < 23; i++) {
    root.Sprites["MenuEntry" + i] = {
        Sheet: "Title",
        Bounds: {
            X: 641,
            Y: i * 32,
            Width: root.TitleMenu.MenuEntriesTargetWidth,
            Height: 30
        }
    };
    root.TitleMenu.MenuEntriesSprites.push("MenuEntry" + i);
}

for (var i = 0; i < 23; i++) {
    root.Sprites["MenuEntryHighlighted" + i] = {
        Sheet: "Title",
        Bounds: {
            X: 897,
            Y: i * 32,
            Width: root.TitleMenu.MenuEntriesTargetWidth,
            Height: 30
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites.push("MenuEntryHighlighted" + i);
}

root.Sprites["MenuEntryLocked"] = {
    Sheet: "Title",
    Bounds: { X: 641, Y: 672, Width: 254, Height: 30 }
};

root.Sprites["MenuEntryLockedH"] = {
    Sheet: "Title",
    Bounds: { X: 897, Y: 672, Width: 254, Height: 30 }
};

root.Sprites["MenuBackground"] = {
    Sheet: "MainBg",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};


root.Sprites["TitleMenuBackground"] = {
    Sheet: "Titlebg",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 },

};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet: "Title",
    Bounds: { X: 257, Y: 193, Width: 302, Height: 30 },
    
};

root.Sprites["TitleMenuCopyright"] = {
    Sheet: "Title",
    Bounds: { X: 1, Y: 225, Width: 384, Height: 30 },
    
};

root.Sprites["TitleMenuLogo"] = {
    Sheet: "Title",
    Bounds: { X: 1, Y: 1, Width: 638, Height: 190 },
   
};