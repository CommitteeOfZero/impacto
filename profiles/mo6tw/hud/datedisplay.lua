local sheet = "Data";
local name = "Date";

local yearNumFirstX = 1525;
local yearNumFirstY = 63;
local yearNumWidth = 25;
local yearNum1Width = 8;
local yearNumHeight = 20;

local numFirstX = 1524;
local numFirstY = 33;
local numWidth = 40;
local num1Width = 12;
local numHeight = 28;

local weekFirstX = 1785;
local weekFirstY = 63;
local weekSecondX = 1525;
local weekSecondY = 85;
local weekWidth = 73;
local weekFriWidth = 56;
local weekHeight = 20;

root.DateDisplay = {
    Type = DateDisplayType.RNE,
    YearNumSprites = {},
    MonthNumSprites = {},
    DayNumSprites = {},
    WeekSprites = {},
    BackgroundStartPos = { X = 1088, Y = 73 },
    BackgroundEndPos = { X = 1088 - 256, Y = 73 },
    DateStartX = 1167,
    YearWeekY = 60,
    MonthDayY = 52,
    Spacing = 1,
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
        X = 1897,
        Y = 33,
        Width = 10,
        Height = 28
    }
};
root.DateDisplay.MDSeparatorSprite = name .. "MonthDaySeparator";

root.Sprites[name .. "DayYearSeparator"] = {
    Sheet = sheet,
    Bounds = {
        X = 1758,
        Y = 63,
        Width = 8,
        Height = 20
    }
};
root.DateDisplay.DYSeparatorSprite = name .. "DayYearSeparator";

root.Sprites[name .. "OpenBracket"] = {
    Sheet = sheet,
    Bounds = {
        X = 1766,
        Y = 63,
        Width = 8,
        Height = 20
    }
};
root.DateDisplay.OpenBracketSprite = name .. "OpenBracket";

root.Sprites[name .. "CloseBracket"] = {
    Sheet = sheet,
    Bounds = {
        X = 1776,
        Y = 63,
        Width = 8,
        Height = 20
    }
};
root.DateDisplay.CloseBracketSprite = name .. "CloseBracket";

root.Sprites[name .. "Background"] = {
    Sheet = sheet,
    Bounds = {
        X = 1525,
        Y = 1,
        Width = 450,
        Height = 28
    }
};
root.DateDisplay.BackgroundSprite = name .. "Background";