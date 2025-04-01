root.OptionsMenu = {
    Type = OptionsMenuType.CCLCC,
    DrawType = DrawComponentType.SystemMenu,

    FadeInDuration = 0.5,
    FadeOutDuration = 0.5,

    HighlightColor = { X = 0.94, Y = 0.49, Z = 0.59 },

    BackgroundSprite = "OptionsBackground",
    BackgroundPosition = { X = 179, Y = 0 },
    BackgroundFadeStartPosition = { X = 179, Y = -375 },

    PointerSprite = "OptionsPointer",
    PointerOffset = { X = 13, Y = 2 },

    HeaderSprite = "OptionsHeader",
    HeaderPosition = { X = 10, Y = 10 },
    PageHeaderSprites = {},
    PageHeaderPosition = { X = 696, Y = 182 },

    PagePanelSprite = "OptionsPagePanel",
    PagePanelPosition = { X = -114, Y = 0 },
    PagePanelFadeStartPosition = { X = -464, Y = 0 },
    PagePanelSprites = {},
    PagePanelIconOffsets = {
        { X = 259, Y = 73 },
        { X = 235, Y = 333 },
        { X = 261, Y = 585 },
        { X = 238, Y = 831 }
    },
    PagePanelHoverBounds = {
        { X = 168, Y = 79, Width = 150, Height = 183 },
        { X = 147, Y = 343, Width = 154, Height = 177 },
        { X = 169, Y = 591, Width = 146, Height = 181 },
        { X = 149, Y = 837, Width = 150, Height = 179 },
    },
    PoleAnimation = "OptionsPoleAnimation",

    SliderTrackSprite = "OptionsSliderTrack",
    SliderTrackOffset = { X = 766, Y = 0 },
    VoiceSliderTrackSprite = "OptionsVoiceSliderTrack",
    VoiceSliderOffset = { X = 114, Y = 57 },
    BinaryBoxSprite = "OptionsBinaryBox",
    BinaryBoxOffset = { X = 914, Y = 0 },
    SliderSpeed = 1.0,

    SkipReadSprite = "OptionsSkipRead",
    SkipAllSprite = "OptionsSkipAll",
    OnSprite = "OptionsOn",
    OffSprite = "OptionsOff",
    YesSprite = "OptionsYes",
    NoSprite = "OptionsNo",

    GuideSprite = "OptionsGuide",
    VoiceGuideSprite = "OptionsVoiceGuide",
    GuidePosition = { X = 0, Y = 986 },
    GuideFadeStartPosition = { X = 0, Y = 1375 },

    EntriesStartPosition = { X = 419, Y = 313 },
    EntriesVerticalOffset = 126,
    SoundEntriesStartPosition = { X = 419, Y = 333 },
    SoundEntriesVerticalOffset = 70,
    VoiceEntriesOffset = { X = 408, Y = 160 },
    EntryDimensions = { X = 1212, Y = 50 },
    VoiceEntryDimensions = { X = 374, Y = 104 },

    LabelSprites = {},
    LabelOffset = { X = 102, Y = -2 },
    NametagSprites = {},
    NametagOffset = { X = 116, Y = 4 },
    PortraitSprites = {},
    PortraitOffset = { X = 3, Y = 2 },
    VoicePosition = { X = 454, Y = 310 },

    MinButtonHoldTime = 0.5,
    ButtonHoldFireInterval = 0.07,

    MenuMask = "MenuMask",
};

root.Sprites["OptionsBackground"] = {
    Sheet = "Config",
    Bounds = { X = 0, Y = 0, Width = 1571, Height = 1089 }
};

root.Sprites["OptionsSliderTrack"] = {
    Sheet = "Config",
    Bounds = { X = 684, Y = 1291, Width = 446, Height = 50 }
};

root.Sprites["OptionsVoiceSliderTrack"] = {
    Sheet = "Config",
    Bounds = { X = 951, Y = 1239, Width = 250, Height = 35 }
};

root.Sprites["OptionsBinaryBox"] = {
    Sheet = "Config",
    Bounds = { X = 684, Y = 1343, Width = 298, Height = 50 }
};

root.Sprites["OptionsPointer"] = {
    Sheet = "Config",
    Bounds = { X = 697, Y = 1241, Width = 71, Height = 45 }
};

root.Sprites["OptionsHeader"] = {
    Sheet = "Config",
    Bounds = { X = 5, Y = 1243, Width = 663, Height = 267 }
};

root.Sprites["OptionsPagePanel"] = {
    Sheet = "Config",
    Bounds = { X = 1571, Y = 0, Width = 477, Height = 1080 }
};

root.Sprites["OptionsSkipRead"] = {
    Sheet = "Config",
    Bounds = { X = 952, Y = 1394, Width = 148, Height = 50 }
};

root.Sprites["OptionsSkipAll"] = {
    Sheet = "Config",
    Bounds = { X = 952, Y = 1448, Width = 148, Height = 50 }
};

root.Sprites["OptionsOn"] = {
    Sheet = "Config",
    Bounds = { X = 952, Y = 1499, Width = 148, Height = 50 }
};

root.Sprites["OptionsOff"] = {
    Sheet = "Config",
    Bounds = { X = 952, Y = 1551, Width = 148, Height = 50 }
};

root.Sprites["OptionsYes"] = {
    Sheet = "Config",
    Bounds = { X = 952, Y = 1603, Width = 148, Height = 50 }
};

root.Sprites["OptionsNo"] = {
    Sheet = "Config",
    Bounds = { X = 952, Y = 1655, Width = 148, Height = 50 }
};

root.Sprites["OptionsGuide"] = {
    Sheet = "Config",
    Bounds = { X = 0, Y = 2476, Width = 1926, Height = 57 }
};

root.Sprites["OptionsVoiceGuide"] = {
    Sheet = "Config",
    Bounds = { X = 0, Y = 2416, Width = 1926, Height = 57 }
};

root.Sprites["MenuMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
};

for i = 0, 3 do
    height = 80;

    root.Sprites["OptionsPageHeader" .. i] = {
        Sheet = "Config",
        Bounds = {
            X = 0,
            Y = 1510 + height * i,
            Width = 942,
            Height = height
        }
    };

    root.OptionsMenu.PageHeaderSprites[#root.OptionsMenu.PageHeaderSprites + 1] = "OptionsPageHeader" .. i;
end

-- Rearange the labels in the array so they're in order of appearance
labelIndices = {5, 6, 7, 1,
                2, 3, 8,
                9, 10, 11, 12, 13, 4};
height = 52;
for i = 0, 12 do
    offset = ((i > 11) and {104} or {0})[1];

    root.Sprites["OptionsLabel" .. i] = {
        Sheet = "Config",
        Bounds = {
            X = 1239,
            Y = 1237 + height * i + offset,
            Width = 809,
            Height = height
        }
    };
    root.OptionsMenu.LabelSprites[labelIndices[i + 1]] = "OptionsLabel" .. i;
end

for i = 0, 12 do
    width = 218;
    height = 53;

    root.Sprites["OptionsNametag" .. i] = {
        Sheet = "Config",
        Bounds = {
            X = width * (i % 6),
            Y = 1863 + height * (i // 6),
            Width = width,
            Height = height
        }
    };
    root.OptionsMenu.NametagSprites[#root.OptionsMenu.NametagSprites + 1] = "OptionsNametag" .. i;
end

for i = 0, 7 do
    -- Interweave highlighted and non-highlighted variants
    width = 224;
    offset = ((i % 2 == 1) and {width * 4} or {0})[1];

    -- Non-highlighted Text and Sound panels are poorly placed in the spritesheet, for some reason
    if (i == 3 or i == 5) then
        offset = offset - 1;
    end

    root.Sprites["OptionsPagePanel" .. i] = {
        Sheet = "Config",
        Bounds = {
            X = offset + width * (i // 2),
            Y = 2205,
            Width = width,
            Height = 195
        }
    };
    root.OptionsMenu.PagePanelSprites[#root.OptionsMenu.PagePanelSprites + 1] = "OptionsPagePanel" .. i;
end

for i = 0, 25 do
    -- Interweave on and muted variants
    width = 100;

    root.Sprites["OptionsPortrait" .. i] = {
        Sheet = "ConfigEx",
        Bounds = {
            X = 768 + (width + 1) * (i // 2),
            Y = ((i % 2 == 0) and {2256} or {2357})[1],
            Width = width,
            Height = width
        }
    };
    root.OptionsMenu.PortraitSprites[#root.OptionsMenu.PortraitSprites + 1] = "OptionsPortrait" .. i;
end

MakeAnimation({
    Name = "OptionsPoleAnimation",
    Sheet = "ConfigEx",

    FirstFrameX = 0,
    FirstFrameY = 0,

    FrameWidth = 539,
    ColWidth = 539,
    FrameHeight = 1080,
    RowHeight = 1096,

    Frames = 15,
    Duration = 0.5,
    Rows = 3,
    Columns = 7,

    PrimaryDirection = AnimationDirections.Right,
    SecondaryDirection = AnimationDirections.Down
});