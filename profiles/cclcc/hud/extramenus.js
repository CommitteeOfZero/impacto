root.ExtraMenus = {
    ClearListMenu: {
        DrawType: DrawComponentType.PlayData,
        Type: ClearListMenuType.CCLCC,
        BackgroundSprite: "ClearListBackground",
        FadeInDuration: 0.2,
        FadeOutDuration: 0.2
    }
}

root.Sprites["ClearListBackground"] = {
    Sheet: "ClearList",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 }
};