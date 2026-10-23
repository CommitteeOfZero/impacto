-- STEINS;GATE 0 system message (gsysmes.dds): full-screen backdrop, centred
-- message lines and the script's choice strings in one row below them.
root.SysMesBoxDisplay = {
    Type = SysMesBoxType.SG0,
    DrawType = DrawComponentType.SystemMessage,
    BackgroundSprite = "SysMesBoxBackground",
    BackgroundOpacity = 1.0,
    MessageColor = 0xFFFFFF,
    ChoiceColor = 0x9A9A9A,
    ChoiceHighlightColor = 0xFFB040,
    OutlineColor = 0x000000,
    ChoiceSpacing = 160,
    ChoicePadding = { X = 48, Y = 12 },
    TextFontSize = 40,
    TextMiddleY = 470,
    TextX = 960,
    TextLineHeight = 56,
    TextMarginY = 56,
    AnimationSpeed = 25,
    FadeInDuration = 0.25,
    FadeOutDuration = 0.25
};

root.Sprites["SysMesBoxBackground"] = {
    Sheet = "SysMesBox",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};
