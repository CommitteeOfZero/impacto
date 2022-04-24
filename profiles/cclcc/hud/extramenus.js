root.ExtraMenus = {
    ClearListMenu: {
        DrawType: DrawComponentType.SystemMenu,
        Type: ClearListMenuType.CCLCC,
        BackgroundSprite: "ClearListBackground",
        FadeInDuration: 0.2,
        FadeOutDuration: 0.2,
        ClearListBookLayerSprite: "ClearListBookLayer",
        ClearListGuideSprite: "ClearListGuide",
        ClearListGuideX: 0,
        ClearListGuideY: 990,
        ClearListMaskSprite: "ClearListMask"
    }
}

// ClearList
root.Sprites["ClearListBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 1080, Width: 1920, Height: 1080 }
};

root.Sprites["ClearListBookLayer"] = {
    Sheet: "ClearList",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 }
};

root.Sprites["ClearListGuide"] = {
    Sheet: "ClearList",
    Bounds: { X: 0, Y: 2265, Width: 1925, Height: 55 }
};

root.Sprites["ClearListMask"] = {
    Sheet: "MenuChip",
    Bounds: { X: 154, Y: 140, Width: 1900, Height: 1061 },
};