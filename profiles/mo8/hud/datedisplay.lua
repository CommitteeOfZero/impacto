local sheet = "Data";
local name = "Date";

local yearNumFirstX = 2287.5;
local yearNumFirstY = 94.5;
local yearNumWidth = 37.5;
local yearNum1Width = 12;
local yearNumHeight = 30;

local numFirstX = 2286;
local numFirstY = 49.5;
local numWidth = 60;
local num1Width = 18;
local numHeight = 42;

local weekFirstX = 2677.5;
local weekFirstY = 94.5;
local weekSecondX = 2287.5;
local weekSecondY = 127.5;
local weekWidth = 109.5;
local weekFriWidth = 84;
local weekHeight = 30;

root.DateDisplay = {
    Type = DateDisplayType.RNE,
    YearNumSprites = {},
    MonthNumSprites = {},
    DayNumSprites = {},
    WeekSprites = {},
    BackgroundStartPos = { X = 1632, Y = 109.5 },
    BackgroundEndPos = { X = 1632 - 384, Y = 109.5 },
    DateStartX = 1750.5,
    YearWeekY = 90,
    MonthDayY = 78,
    Spacing = 1.5,
    FadeInDuration = 0.5,
    FadeOutDuration = 0.5
};

for i = 0, 9 do
    local yearW;
    if i == 1 then
        yearW = yearNum1Width;
    else
        yearW = yearNumWidth;
    end

    root.Sprites[name .. "YearNum" .. i] = {
        Sheet = sheet,
        Bounds = {
            X = yearNumFirstX,
            Y = yearNumFirstY,
            Width = yearW,
            Height = yearNumHeight
        }
    };
    root.DateDisplay.YearNumSprites[#root.DateDisplay.YearNumSprites + 1] = name .. "YearNum" .. i;

    local numW
    if i == 1 then
        numW = num1Width;
    else
        numW = numWidth;
    end

    root.Sprites[name .. "Num" .. i] = {
        Sheet = sheet,
        Bounds = {
            X = numFirstX,
            Y = numFirstY,
            Width = numW,
            Height = numHeight
        }
    };
    root.DateDisplay.MonthNumSprites[#root.DateDisplay.MonthNumSprites + 1] = name .. "Num" .. i;
    root.DateDisplay.DayNumSprites[#root.DateDisplay.DayNumSprites + 1] = name .. "Num" .. i;

    if i == 1 then
        yearNumFirstX = yearNumFirstX + yearNum1Width;
        numFirstX = numFirstX + num1Width;
    else
        yearNumFirstX = yearNumFirstX + yearNumWidth;
        numFirstX = numFirstX + numWidth;
    end
end

for i = 0, 6 do
    if i > 1 then
        local weekW;
        if i == 5 then
            weekW = weekFriWidth;
        else
            weekW = weekWidth;
        end

        root.Sprites[name .. "Week" .. i] = {
            Sheet = sheet,
            Bounds = {
                X = weekSecondX,
                Y = weekSecondY,
                Width = weekW,
                Height = weekHeight
            }
        };
        if i == 5 then
            weekSecondX = weekSecondX + weekFriWidth;
        else
            weekSecondX = weekSecondX + weekWidth;
         end
    else
        root.Sprites[name .. "Week" .. i] = {
            Sheet = sheet,
            Bounds = {
                X = weekFirstX,
                Y = weekFirstY,
                Width = weekWidth,
                Height = weekHeight
            }
        };
        weekFirstX = weekFirstX + weekWidth;
    end
    root.DateDisplay.WeekSprites[#root.DateDisplay.WeekSprites + 1] = name .. "Week" .. i;
end

root.Sprites[name .. "MonthDaySeparator"] = {
    Sheet = sheet,
    Bounds = {
        X = 2845.5,
        Y = 49.5,
        Width = 15,
        Height = 42
    }
};
root.DateDisplay.MDSeparatorSprite = name .. "MonthDaySeparator";

root.Sprites[name .. "DayYearSeparator"] = {
    Sheet = sheet,
    Bounds = {
        X = 2637,
        Y = 94.5,
        Width = 12,
        Height = 30
    }
};
root.DateDisplay.DYSeparatorSprite = name .. "DayYearSeparator";

root.Sprites[name .. "OpenBracket"] = {
    Sheet = sheet,
    Bounds = {
        X = 2649,
        Y = 94.5,
        Width = 12,
        Height = 30
    }
};
root.DateDisplay.OpenBracketSprite = name .. "OpenBracket";

root.Sprites[name .. "CloseBracket"] = {
    Sheet = sheet,
    Bounds = {
        X = 2664,
        Y = 94.5,
        Width = 12,
        Height = 30
    }
};
root.DateDisplay.CloseBracketSprite = name .. "CloseBracket";

root.Sprites[name .. "Background"] = {
    Sheet = sheet,
    Bounds = {
        X = 2287.5,
        Y = 2,
        Width = 675,
        Height = 42
    }
};
root.DateDisplay.BackgroundSprite = name .. "Background";