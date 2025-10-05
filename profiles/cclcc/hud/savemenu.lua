root.SaveMenu = {
    Type = SaveMenuType.CCLCC,
    DrawType = DrawComponentType.SystemMenu,
    EmptyThumbnailSprite = "EmptyThumbnail",
    EntryStartXL = 143,
    EntryStartYL = 152,
    EntryStartXR = 984,
    EntryStartYR = 52,
    EntryYPadding = 201,
    FadeInDuration = 28 / 60,
    FadeOutDuration = 28 / 60,
    PageSwapDuration = 28 / 60,
    SaveMenuMaskSprite = "SaveMenuMask",
    SaveEntryPrimaryColor = 0xF07390,
    LoadEntryPrimaryColor = 0x00A1E6,
    SaveEntrySecondaryColor = 0x5E357C,
    GuidePosition = {X = 1314, Y = 867},
    MenuTextPosition = {X = 11, Y = 10}, 
    SlotsBackgroundPosition = {X = 135, Y = 0}, 
    PageNumberPosition = {X = 1314, Y = 867},
    SlotLockedSpritePosition = {X = 656, Y = 9},
    NoDataSpritePosition = {X = 232, Y = 9},
};

root.Sprites["EmptyThumbnail"] = {
    Sheet = "SaveMenu",
    Bounds = { X = 0, Y = 0, Width = 0, Height = 0 }
};

root.Sprites["SaveMenuMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
};

local menuTypes = { "QuickLoad", "Save", "Load" }

for i = 1,3 do
    local menuType = menuTypes[i];
    root.SaveMenu[menuType ..  "TextSprite"] = menuType .. "Text";
    root.Sprites[menuType .. "Text"] = {
        Sheet = menuType .. "Menu",
        Bounds = { X = 1, Y = 1083, Width = 1066, Height = 298 }
    };
    root.SaveMenu[menuType .. "EntrySlotsSprite"] = menuType .. "EntrySlots";
    root.Sprites[menuType .. "EntrySlots"] = {
        Sheet = menuType .. "Menu",
        Bounds = { X = 1, Y = 0, Width = 1680, Height = 1080 }
    };
    root.SaveMenu[menuType .. "EntryHighlightedBoxSprite"] = menuType .. "EntryHighlightedBox";
    root.Sprites[menuType .. "EntryHighlightedBox"] = {
        Sheet = menuType .. "Menu",
        Bounds = { X = 1241, Y = 1083, Width = 806, Height = 184 }
    };
    root.SaveMenu[menuType .. "EntryHighlightedTextSprite"] = menuType .. "EntryHighlightedText";
    root.Sprites[menuType .. "EntryHighlightedText"] = {
        Sheet = menuType .. "Menu",
        Bounds = { X = 1241, Y = 1269, Width = 254, Height = 87 }
    };
    root.SaveMenu[menuType .. "ButtonGuideSprite"] = menuType .. "ButtonGuide";
    root.Sprites[menuType .. "ButtonGuide"] = {
        Sheet = menuType .. "Menu",
        Bounds = { X = 0, Y = 1457, Width = 1920, Height = 59 }
    };
    root.SaveMenu[menuType .. "SeparationLineSprite"] = menuType .. "SeparationLine";
    root.Sprites[menuType .. "SeparationLine"] = {
        Sheet = menuType .. "Menu",
        Bounds = { X = 847, Y = 1409, Width = 478, Height = 27 }
    };
    

    for i = 0, 9 do
        root.SaveMenu[menuType .. "NumberDigitSprite" .. i] = menuType .. "NumberDigit" .. i;
        root.Sprites[menuType .. "NumberDigit" .. i] = {
            Sheet = menuType .. "Menu",
            Bounds = {X = i * 40 + 1430 + 1, Y = 1383, Width = 38, Height = 56}
        };
    end

    root.SaveMenu[menuType .. "NoDataEntrySprite"] = menuType .. "NoDataEntry";
    root.Sprites[menuType .. "NoDataEntry"] = {
        Sheet = menuType .. "Menu",
        Bounds = {X = 1683, Y = 1, Width = 364, Height = 182}
    };

    root.SaveMenu[menuType .. "BrokenDataEntrySprite"] = menuType .. "BrokenDataEntry";
    root.Sprites[menuType .. "BrokenDataEntry"] = {
        Sheet = menuType .. "Menu",
        Bounds = {X = 1683, Y = 185, Width = 364, Height = 182}
    };

    root.SaveMenu[menuType .. "SlotLockedSprite"] = menuType .. "SlotLocked";
    root.Sprites[menuType .. "SlotLocked"] = {
        Sheet = menuType .. "Menu",
        Bounds = {X = 0, Y = 1402, Width = 132, Height = 48}
    };


    for i = 0, 5 do
        local col = i//3;
        local row = i % 3;
        root.SaveMenu[menuType .. "PageNumSprite" .. i] = menuType .. "PageNum" .. i;
        root.Sprites[menuType .. "PageNum" .. i] = {
            Sheet = menuType .. "Menu",
            Bounds = {
                X = col * 240, 
                Y = row * 96 + 1534, 
                Width = 192, 
                Height = 64
            }
        };
    end

    root.SaveMenu[menuType .. "SaveTimeSprite"] = menuType .. "SaveTime";
    root.Sprites[menuType .. "SaveTime"] = {
        Sheet = menuType .. "Menu",
        Bounds = {X = 839, Y = 1383, Width = 490, Height = 72}
    };
    
end