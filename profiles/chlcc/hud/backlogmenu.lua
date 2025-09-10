root.BacklogMenu = {
    Type = BacklogMenuType.CHLCC,
    DrawType = DrawComponentType.SystemMenu,
    TransitionDuration = 64 / 60,
    BackgroundColor = 0x94b1ff,
    CircleSprite = "CircleBacklog",
    CircleStartPosition = { X = 20, Y = 20 },
    CircleOffset = 200,
    ErinSprite = "Erin",
    ErinPosition = { X = 301, Y = 1 },
    BackgroundFilter = "BackgroundFilter",
    InitialRedBarPosition = { X = 0, Y = 538 },
    RightRedBarPosition = { X = 1059, Y = 538 },
    RedBarDivision = 1826,
    RedBarBaseX = 1059,
    RedBarSprite = "RedBar",
    RedBarLabelPosition = { X = 1067, Y = 573 },
    RedBarLabel = "RedBarLabel",
    MenuTitleTextRightPosition = { X = 465, Y = 530 },
    MenuTitleTextLeftPosition = { X = 77, Y = -5 },
    MenuTitleTextAngle = 6.02,
    TitleFadeInDuration = 40 / 60,
    TitleFadeOutDuration = 28 / 60,
    MenuTitleText = "BacklogMenuTitleText",
    ButtonPromptPosition = { X = 966, Y = 651 },
    BacklogButtonPromptSprite = "BacklogButtonPrompt",
    BacklogBackgroundSprite = "BacklogBackground",
    EntryHighlightSprite = "EntryHighlight",
    EntryHighlightLocation = EntryHighlightLocationType.AllLinesLeftOfScreen,
    EntryHighlightOffset = { X = 75, Y = 0 },
    EntryHighlightPadding = 2.0,
    VoiceIconSprite = "VoiceIcon",
    VoiceIconOffset = { X = 0, Y = 0 },
    ScrollbarThumbSprite = "ScrollbarThumb",
    ScrollbarTrackSprite = "ScrollbarTrack",
    ScrollbarThumbLength = 59,
    ScrollbarPosition = { X = 1190, Y = 92 },
    EntriesStart = { X = 123, Y = 105 },
    RenderingBounds = { X = 87, Y = 89, Width = 1086, Height = 539 },
    EntryYPadding = 20,
    HoverBounds = { X = 75, Y = 89, Width = 1086, Height = 535 },
    FadeInDuration = 0.5,
    FadeOutDuration = 0.5,
    ScrollingSpeed = 600,
    MinHoldTime = 0.5,
    AdvanceFocusTimeInterval = 0.05,
    PageUpDownHeight = 520
};

root.Sprites["CircleBacklog"] = {
    Sheet = "Backlog",
    Bounds = { X = 1, Y = 917, Width = 106, Height = 106 }
}

root.Sprites["Erin"] = {
    Sheet = "Main",
    Bounds = { X = 641, Y = 1, Width = 978, Height = 798 }
}

root.Sprites["BackgroundFilter"] = {
    Sheet = "Main",
    Bounds = { X = 0, Y = 0, Width = 640, Height = 360 }
}

root.Sprites["RedBar"] = {
    Sheet = "Main",
    Bounds = { X = 767, Y = 913, Width = 1280, Height = 110 }
}

root.Sprites["RedBarLabel"] = {
    Sheet = "Main",
    Bounds = { X = 506, Y = 469, Width = 133, Height = 74 }
}

root.Sprites["BacklogMenuTitleText"] = {
    Sheet = "Backlog",
    Bounds = { X = 2, Y = 640, Width = 759, Height = 116 }
}

root.Sprites["BacklogBackground"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 0, Width = 1208, Height = 634 },
};

root.Sprites["VoiceIcon"] = {
    Sheet = "Backlog",
    Bounds = { X = 1215, Y = 34, Width = 28, Height = 28 },
};

root.Sprites["EntryHighlight"] = {
    Sheet = "Backlog",
    Bounds = { X = 1, Y = 757, Width = 1099, Height = 30 }
};

root.Sprites["BacklogButtonPrompt"] = {
    Sheet = "Backlog",
    Bounds = { X = 1213, Y = 1, Width = 315, Height = 28 }
}

root.Sprites["Erin"] = {
    Sheet = "Main",
    Bounds = { X = 641, Y = 1, Width = 978, Height = 798 }
}

root.Sprites["ScrollbarThumb"] = {
    Sheet = "Backlog",
    Bounds = { X = 1212, Y = 68, Width = 34, Height = 59 },
};

root.Sprites["ScrollbarTrack"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 0, Width = 0, Height = 535 },
};