root.SaveMenu = {
    Type = SaveMenuType.CHLCC,
    DrawType = DrawComponentType.SystemMenu,
    SaveBackgroundColor = 0xff8e99,
    LoadBackgroundColor = 0x50b5ff,
    QuickLoadBackgroundColor = 0x01b06c,
    SaveCircle = "PinkCircle",
    LoadCircle = "LightBlueCircle",
    QuickLoadCircle = "GreenCircle",
    CircleStartPosition = {X = 20, Y = 20},
    CircleOffset = 200,
    ErinSprite = "Erin",
    ErinPosition = {X = 301, Y = 1},
    BackgroundFilter = "BackgroundFilter",
    InitialRedBarPosition = {X = 0, Y = 538},
    RightRedBarPosition = {X = 1059, Y = 538},
    RedBarDivision = 1826,
    RedBarBaseX = 1059,
    RedBarSprite = "RedBar",
    RedBarLabelPosition = {X = 1067, Y = 573},
    RedBarLabel = "RedBarLabel",
    QuickLoadTextSprite = "QuickLoadText",
    SaveTextSprite = "SaveText",
    LoadTextSprite = "LoadText",
    MenuTitleTextAngle = 4.45,
    MenuTitleTextLeftPos = {X = 1, Y = 1},
    MenuTitleTextRightPos = {X = 782, Y = 584},
    TitleFadeInDuration = 40 / 60,
    TitleFadeOutDuration = 28 / 60,
    SaveListPosition = {X = 0, Y = 0},
    SaveListSprite = "SaveList",
    EntryPositions = {
        {X = 91, Y = 127}, {X = 91, Y = 254},
        {X = 91, Y = 381}, {X = 91, Y = 508},
        {X = 661, Y = 127}, {X = 661, Y = 508}
    },
    EmptyThumbnailSprite = "EmptyThumbnail",
    EntryStartX = 153,
    EntryXPadding = 512,
    EntryStartY = 102,
    EntryYPadding = 141,
    QuickLoadEntrySprite = "QuickLoadEntry",
    SaveEntrySprite = "SaveEntry",
    LoadEntrySprite = "LoadEntry",
    SelectionMarkerSprite = "SelectionMarker",
    SelectionMarkerOffset = {X = 5, Y = 49},
    EntryHighlightedSprite = "EntryHighlighted",
    LockedSymbolSprite = "LockedSymbol",
    ThumbnailRelativePos = {X = 21, Y = 12},
    FadeInDuration = 64 / 60,
    FadeOutDuration = 64 / 60,
    PageNumBackgroundPos = {X = 1090, Y = 61},
    PageNumBackground = "PageNumBackground",
    CurrentPageNumPos = {X = 1133, Y = 65},
    BigDigits = {},
    PageNumSeparatorSlashPos = {X = 1161, Y = 90},
    PageNumSeparatorSlash = "PageNumSeparatorSlash",
    MaxPageNumPos = {X = 1179, Y = 90},
    MaxPageNum = "MaxPageNum",
    ButtonPromptPosition = {X = 853, Y = 651},
    ButtonPrompt = "SaveButtonPrompt",
    SelectDataFadeDuration = 110 / 60,
    SelectDataTextPositions = {
        {X = 94, Y = 51}, {X = 109, Y = 51},
        {X = 122, Y = 51}, {X = 134, Y = 51},
        {X = 147, Y = 51}, {X = 161, Y = 51},
        {X = 180, Y = 51}, {X = 199, Y = 51},
        {X = 213, Y = 51}, {X = 228, Y = 51}
    },
    SelectDataText = {},
    EntryNumberHintTextRelativePos = {X = 209, Y = 10},
    EntryNumberTextRelativePos = {X = 249, Y = 10},
    SceneTitleTextRelativePos = {X = 209, Y = 35},
    NoDataTextRelativePos = {X = 299, Y = 45},
    PlayTimeHintTextRelativePos = {X = 259, Y = 67},
    PlayTimeTextRelativePos = {X = 419, Y = 67},
    SaveDateHintTextRelativePos = {X = 259, Y = 84},
    SaveDateTextRelativePos = {X = 339, Y = 84}
};

root.Sprites["PinkCircle"] = {
    Sheet = "Save",
    Bounds = {X = 1, Y = 917, Width = 106, Height = 106}
};

root.Sprites["LightBlueCircle"] = {
    Sheet = "Save",
    Bounds = {X = 109, Y = 917, Width = 106, Height = 106}
};

root.Sprites["GreenCircle"] = {
    Sheet = "Save",
    Bounds = {X = 217, Y = 917, Width = 106, Height = 106}
};

root.Sprites["Erin"] = {
    Sheet = "ClearList",
    Bounds = {X = 641, Y = 1, Width = 978, Height = 798}
};

root.Sprites["RedBar"] = {
    Sheet = "ClearList",
    Bounds = {X = 767, Y = 913, Width = 1280, Height = 110}
};

root.Sprites["BackgroundFilter"] = {
    Sheet = "Main",
    Bounds = {X = 0, Y = 0, Width = 640, Height = 360}
};

root.Sprites["RedBarLabel"] = {
    Sheet = "ClearList",
    Bounds = {X = 506, Y = 469, Width = 133, Height = 74}
};

root.Sprites["SaveMenuBackground"] = {
    Sheet = "Save",
    Bounds = {X = 0, Y = 0, Width = 1280, Height = 720}
};

root.Sprites["EmptyThumbnail"] = {
    Sheet = "Save",
    Bounds = {X = 1, Y = 727, Width = 160, Height = 90}
};

root.Sprites["QuickLoadText"] = {
    Sheet = "Save",
    Bounds = {X = 945, Y = 1, Width = 130, Height = 974}
};

root.Sprites["SaveText"] = {
    Sheet = "Save",
    Bounds = {X = 681, Y = 1, Width = 130, Height = 420}
};

root.Sprites["LoadText"] = {
    Sheet = "Save",
    Bounds = {X = 813, Y = 1, Width = 130, Height = 420}
};

root.Sprites["SaveList"] = {
    Sheet = "Save",
    Bounds = {X = 1, Y = 1, Width = 678, Height = 578}
}

root.Sprites["QuickLoadEntry"] = {
    Sheet = "Save",
    Bounds = {X = 1077, Y = 233, Width = 532, Height = 116}
};
root.Sprites["SaveEntry"] = {
    Sheet = "Save",
    Bounds = {X = 1077, Y = 1, Width = 532, Height = 116}
};
root.Sprites["LoadEntry"] = {
    Sheet = "Save",
    Bounds = {X = 1077, Y = 117, Width = 532, Height = 116}
};

root.Sprites["SelectionMarker"] = {
    Sheet = "Save",
    Bounds = {X = 440, Y = 589, Width = 14, Height = 14}
};

root.Sprites["EntryHighlighted"] = {
    Sheet = "Save",
    Bounds = {X = 1077, Y = 349, Width = 532, Height = 116}
};

root.Sprites["LockedSymbol"] = {
    Sheet = "Save",
    Bounds = {X = 463, Y = 581, Width = 28, Height = 28}
};

root.Sprites["PageNumBackground"] = {
    Sheet = "Save",
    Bounds = {X = 1, Y = 611, Width = 204, Height = 57}
};

for i = 0, 9 do
    root.Sprites["BigDigit" .. i] = {
        Sheet = "Data",
        Bounds = {X = i * 36 + 371, Y = 1, Width = 34, Height = 48}
    };
    root.SaveMenu.BigDigits[#root.SaveMenu.BigDigits + 1] = "BigDigit" .. i;
end

root.Sprites["PageNumSeparatorSlash"] = {
    Sheet = "Data",
    Bounds = {X = 173, Y = 67, Width = 16, Height = 22}
};

root.Sprites["MaxPageNum"] = {
    Sheet = "Data",
    Bounds = {X = 335, Y = 67, Width = 16, Height = 22}
};

root.Sprites["SaveButtonPrompt"] = {
    Sheet = "Save",
    Bounds = {X = 1, Y = 581, Width = 430, Height = 28}
};

-- This is a mess

root.Sprites["SaveMenuFadeTextS"] = {
    Sheet = "Save",
    Bounds = {X = 1, Y = 670, Width = 18, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextS";

root.Sprites["SaveMenuFadeTextE"] = {
    Sheet = "Save",
    Bounds = {X = 21, Y = 670, Width = 16, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextE";

root.Sprites["SaveMenuFadeTextL"] = {
    Sheet = "Save",
    Bounds = {X = 39, Y = 670, Width = 15, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextL";

root.Sprites["SaveMenuFadeTextE2"] = {
    Sheet = "Save",
    Bounds = {X = 56, Y = 670, Width = 16, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextE2";

root.Sprites["SaveMenuFadeTextC"] = {
    Sheet = "Save",
    Bounds = {X = 74, Y = 670, Width = 17, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextC";

root.Sprites["SaveMenuFadeTextT"] = {
    Sheet = "Save",
    Bounds = {X = 93, Y = 670, Width = 22, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextT";

root.Sprites["SaveMenuFadeTextD"] = {
    Sheet = "Save",
    Bounds = {X = 117, Y = 670, Width = 22, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextD";

root.Sprites["SaveMenuFadeTextA"] = {
    Sheet = "Save",
    Bounds = {X = 141, Y = 670, Width = 17, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextA";

root.Sprites["SaveMenuFadeTextT2"] = {
    Sheet = "Save",
    Bounds = {X = 159, Y = 670, Width = 18, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextT2";

root.Sprites["SaveMenuFadeTextA2"] = {
    Sheet = "Save",
    Bounds = {X = 179, Y = 670, Width = 20, Height = 55}
};
root.SaveMenu.SelectDataText[#root.SaveMenu.SelectDataText + 1] = "SaveMenuFadeTextA2";