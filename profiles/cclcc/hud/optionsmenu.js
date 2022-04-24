root.OptionsMenu = {
    Type: OptionsMenuType.CCLCC,
    DrawType: DrawComponentType.SystemMenu,
    BackgroundSprite: "OptionsBackground",
    FadeInDuration: 0.2,
    FadeOutDuration: 0.2,
    SliderTrackSprite: "OptionsBackground",
    SliderFillSprite: "OptionsBackground",
    SliderThumbSprite: "OptionsBackground"
};

root.Sprites["OptionsBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 }
};