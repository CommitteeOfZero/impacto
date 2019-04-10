var sheet = "Data";
var name = "Date";

var yearNumFirstX = 2287.5;
var yearNumFirstY = 94.5;
var yearNumWidth = 37.5;
var yearNum1Width = 12;
var yearNumHeight = 30;

var numFirstX = 2286;
var numFirstY = 49.5;
var numWidth = 60;
var num1Width = 18;
var numHeight = 42;

var weekFirstX = 2677.5;
var weekFirstY = 94.5;
var weekSecondX = 2287.5;
var weekSecondY = 127.5;
var weekWidth = 109.5;
var weekFriWidth = 84;
var weekHeight = 30;

root.DateDisplay = {
	Type: DateDisplayType.RNE,
    YearNumSprites: [],
    MonthNumSprites: [],
    DayNumSprites: [],
    WeekSprites: [],
    BackgroundStartPos: { X: 1632, Y: 109.5 },
    BackgroundEndPos: { X: 1632 - 384, Y: 109.5 },
    DateStartX: 1750.5,
    YearWeekY: 90,
    MonthDayY: 78,
    Spacing: 1.5,
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
        X: 2845.5,
        Y: 49.5,
        Width: 15,
        Height: 42
    }
};
root.DateDisplay.MDSeparatorSprite = name + "MonthDaySeparator";

root.Sprites[name + "DayYearSeparator"] = {
    Sheet: sheet,
    Bounds: {
        X: 2637,
        Y: 94.5,
        Width: 12,
        Height: 30
    }
};
root.DateDisplay.DYSeparatorSprite = name + "DayYearSeparator";

root.Sprites[name + "OpenBracket"] = {
    Sheet: sheet,
    Bounds: {
        X: 2649,
        Y: 94.5,
        Width: 12,
        Height: 30
    }
};
root.DateDisplay.OpenBracketSprite = name + "OpenBracket";

root.Sprites[name + "CloseBracket"] = {
    Sheet: sheet,
    Bounds: {
        X: 2664,
        Y: 94.5,
        Width: 12,
        Height: 30
    }
};
root.DateDisplay.CloseBracketSprite = name + "CloseBracket";

root.Sprites[name + "Background"] = {
    Sheet: sheet,
    Bounds: {
        X: 2287.5,
        Y: 2,
        Width: 675,
        Height: 42
    }
};
root.DateDisplay.BackgroundSprite = name + "Background";