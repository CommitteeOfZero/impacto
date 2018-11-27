var sheet = "Data";
var name = "Date";

var yearNumFirstX = 1525;
var yearNumFirstY = 63;
var yearNumWidth = 25;
var yearNum1Width = 8;
var yearNumHeight = 20;

var numFirstX = 1524;
var numFirstY = 33;
var numWidth = 40;
var num1Width = 12;
var numHeight = 28;

var weekFirstX = 1785;
var weekFirstY = 63;
var weekSecondX = 1525;
var weekSecondY = 85;
var weekWidth = 73;
var weekFriWidth = 56;
var weekHeight = 20;

root.DateDisplay = {
    YearNumSprites: [],
    MonthNumSprites: [],
    DayNumSprites: [],
    WeekSprites: [],
    BackgroundStartPos: { X: 1088, Y: 73 },
    BackgroundEndPos: { X: 1088 - 256, Y: 73 },
    DateStartX: 1167,
    YearWeekY: 60,
    MonthDayY: 52,
    Spacing: 1,
    FadeInDuration: 0.5,
    FadeOutDuration: 0.5
};

for (var i = 0; i < 10; i++) {
    root.Sprites[name + "YearNum" + i] = {
        Sheet: sheet,
        Bounds: {
            X: yearNumFirstX,
            Y: yearNumFirstY,
            Width: i === 1 ? yearNum1Width : yearNumWidth,
            Height: yearNumHeight
        }
    };
    root.DateDisplay.YearNumSprites.push(name + "YearNum" + i);

    root.Sprites[name + "Num" + i] = {
        Sheet: sheet,
        Bounds: {
            X: numFirstX,
            Y: numFirstY,
            Width: i === 1 ? num1Width : numWidth,
            Height: numHeight
        }
    };
    root.DateDisplay.MonthNumSprites.push(name + "Num" + i);
    root.DateDisplay.DayNumSprites.push(name + "Num" + i);

    if (i == 1) {
        yearNumFirstX += yearNum1Width;
        numFirstX += num1Width;
    }
    else {
        yearNumFirstX += yearNumWidth;
        numFirstX += numWidth;
    }
}

for (var i = 0; i < 7; i++) {
    if (i > 1) {
        root.Sprites[name + "Week" + i] = {
            Sheet: sheet,
            Bounds: {
                X: weekSecondX,
                Y: weekSecondY,
                Width: i === 5 ? weekFriWidth : weekWidth,
                Height: weekHeight
            }
        };
        if (i === 5) {
            weekSecondX += weekFriWidth;
        }
        else {
            weekSecondX += weekWidth;
        }
    }
    else {
        root.Sprites[name + "Week" + i] = {
            Sheet: sheet,
            Bounds: {
                X: weekFirstX,
                Y: weekFirstY,
                Width: weekWidth,
                Height: weekHeight
            }
        };
        weekFirstX += weekWidth;
    }
    root.DateDisplay.WeekSprites.push(name + "Week" + i);
}

root.Sprites[name + "MonthDaySeparator"] = {
    Sheet: sheet,
    Bounds: {
        X: 1897,
        Y: 33,
        Width: 10,
        Height: 28
    }
};
root.DateDisplay.MDSeparatorSprite = name + "MonthDaySeparator";

root.Sprites[name + "DayYearSeparator"] = {
    Sheet: sheet,
    Bounds: {
        X: 1758,
        Y: 63,
        Width: 8,
        Height: 20
    }
};
root.DateDisplay.DYSeparatorSprite = name + "DayYearSeparator";

root.Sprites[name + "OpenBracket"] = {
    Sheet: sheet,
    Bounds: {
        X: 1766,
        Y: 63,
        Width: 8,
        Height: 20
    }
};
root.DateDisplay.OpenBracketSprite = name + "OpenBracket";

root.Sprites[name + "CloseBracket"] = {
    Sheet: sheet,
    Bounds: {
        X: 1776,
        Y: 63,
        Width: 8,
        Height: 20
    }
};
root.DateDisplay.CloseBracketSprite = name + "CloseBracket";

root.Sprites[name + "Background"] = {
    Sheet: sheet,
    Bounds: {
        X: 1525,
        Y: 1,
        Width: 450,
        Height: 28
    }
};
root.DateDisplay.BackgroundSprite = name + "Background";