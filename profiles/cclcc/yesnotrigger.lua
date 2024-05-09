local function createYesNoData(value1, value2, value3, value4, value5, value6, value7, value8, value9, value10)
    return {
        bgXpos = value1,
        bgYpos = value2,
        index = value3,
        index2 = value4,
        yesChipX = value5,
        yesChipY = value6,
        noChipX = value7,
        noChipY = value8,
        bubbleX = value9,
        bubbleY = value10,
    }
end

local data1 = {
    createYesNoData(698.0, -674.0, 4, 1, 2372.0, 278.0, 2626.0, 440.0, 2646.0, 330.0),
    createYesNoData(698.0, -335.0, 5, 2, 2392.0, 588.0, 2626.0, 764.0, 2646.0, 645.0),
    createYesNoData(698.0, 36.0, 6, 3, 2378.0, 897.0, 2713.0, 1069.0, 2646.0, 975.0),
    createYesNoData(698.0, 234.0, 7, 14, 2410.0, 1224.0, 2702.0, 1347.0, 2661.0, 1287.0),
    createYesNoData(238.0, -469.0, 8, 5, 2042.0, 344.0, 2048.0, 562.0, 2174.0, 504.0),
    createYesNoData(238.0, -179.0, 9, 6, 1975.0, 784.0, 2126.0, 886.0, 2235.0, 765.0),
    createYesNoData(238.0, 188.0, 10, 7, 1974.0, 1162.0, 2036.0, 1187.0, 2187.0, 1098.0),
    createYesNoData(287.0, 412.0, 14, 15, 2330.0, 1440.0, 2040.0, 1460.0, 2229.0, 1356.0),
    createYesNoData(-248.0, -636.0, 11, 9, 1459.0, 232.0, 1727.0, 439.0, 1689.0, 321.0),
    createYesNoData(-283.0, -68.0, 12, 13, 1535.0, 694.0, 1589.0, 936.0, 1647.0, 852.0),
    createYesNoData(-337.0, 396.0, 16, 15, 1424.0, 1370.0, 1540.0, 1448.0, 1632.0, 1341.0),
    createYesNoData(-672.0, -487.0, 18, 12, 1052.0, 335.0, 1156.0, 504.0, 1269.0, 444.0),
    createYesNoData(-708.0, -182.0, 18, 17, 1042.0, 638.0, 1049.0, 819.0, 1230.0, 750.0),
    createYesNoData(-659.0, 131.0, 17, 16, 1064.0, 1029.0, 1184.0, 1181.0, 1302.0, 1068.0),
    createYesNoData(381.0, 730.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 2337.0, 1722.0),
    createYesNoData(-427.0, 730.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 1515.0, 1722.0),
    createYesNoData(-1108.0, 499.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 828.0, 1449.0),
    createYesNoData(-1108.0, -31.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 828.0, 957.0),
    createYesNoData(-1108.0, -529.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 828.0, 453.0),
}

local data2 = {
    createYesNoData(1270.0, 198.0, 1, 2, 3304.0, 996.0, 3304.0, 1364.0, 3364.0, 1200.0),
    createYesNoData(1189.0, -151.0, 3, 4, 2746.0, 767.0, 2883.0, 992.0, 3051.0, 846.0),
    createYesNoData(1189.0, 635.0, 4, 5, 2881.0, 1393.0, 2741.0, 1628.0, 3051.0, 1584.0),
    createYesNoData(592.0, -238.0, 6, 7, 2275.0, 564.0, 2159.0, 740.0, 2469.0, 762.0),
    createYesNoData(592.0, 245.0, 7, 8, 2340.0, 1022.0, 2159.0, 1196.0, 2469.0, 1215.0),
    createYesNoData(592.0, 716.0, 8, 9, 2340.0, 1482.0, 2154.0, 1668.0, 2469.0, 1674.0),
    createYesNoData(-185.0, -598.0, 14, 10, 1396.0, 329.0, 1568.0, 489.0, 1719.0, 351.0),
    createYesNoData(12.0, -214.0, 10, 11, 1588.0, 665.0, 1747.0, 891.0, 1908.0, 762.0),
    createYesNoData(12.0, 278.0, 12, 13, 1588.0, 1124.0, 1747.0, 1349.0, 1908.0, 1215.0),
    createYesNoData(-185.0, 701.0, 13, 18, 1680.0, 1482.0, 1396.0, 1668.0, 1719.0, 1674.0),
    createYesNoData(-629.0, -340.0, 14, 15, 1111.0, 435.0, 998.0, 654.0, 1311.0, 618.0),
    createYesNoData(-629.0, -71, 15, 16, 998.0, 807.0, 998.0, 940.0, 1311.0, 891.0),
    createYesNoData(-629.0, 198.0, 16, 17, 998.0, 1059.0, 998.0, 1196.0, 1311.0, 1150.0),
    createYesNoData(-629.0, 467.0, 17, 18, 998.0, 1364.0, 1111.0, 1578.0, 1311.0, 1422.0),
    createYesNoData(-1245.0, -536.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 615.0, 408.0),
    createYesNoData(-1245.0, -253.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 615.0, 723.0),
    createYesNoData(-1245.0, 73.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 615.0, 1032.0),
    createYesNoData(-1245.0, 396.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 615.0, 1344.0),
    createYesNoData(-1245.0, 693.0, 0, 0, 2042.0, 344.0, 2048.0, 562.0, 615.0, 1668.0),
}

root.YesNoTrigger = {
    YesNoBackground0 = "YesNoBackground0",
    YesNoBackground1 = "YesNoBackground1",
    YesNoBackground2 = "YesNoBackground2",
    YesNoBackground3 = "YesNoBackground3",
    YN1ChipYesL = "YN1ChipYesL",
    YN1ChipNoL = "YN1ChipNoL",
    YN1ChipYesS = "YN1ChipYesS",
    YN1ChipNoS = "YN1ChipNoS",
    YN2ChipYesL = "YN2ChipYesL",
    YN2ChipNoL = "YN2ChipNoL",
    YN2ChipYesS = "YN2ChipYesS",
    YN2ChipNoS = "YN2ChipNoS", 
    ChipStar = "YNChipStar", 
    BGMask = "YNOverlayMask",
    YesNoData1 = data1,
    YesNoData2 = data2,
};

root.Sprites["YesNoBackground0"] = {
    Sheet = "YesNoBG0", 
    Bounds = {X = 0, Y = 0, Width = 3840.0, Height = 2160}
};
root.Sprites["YesNoBackground1"] = {
    Sheet = "YesNoBG1", 
    Bounds = {X = 0, Y = 0, Width = 3840.0, Height = 2160}
};
root.Sprites["YesNoBackground2"] = {
    Sheet = "YesNoBG2", 
    Bounds = {X = 0, Y = 0, Width = 3840.0, Height = 2160}
};
root.Sprites["YesNoBackground3"] = {
    Sheet = "YesNoBG3", 
    Bounds = {X = 0, Y = 0, Width = 3840.0, Height = 2160}
};
root.Sprites["YN1ChipYesL"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1056.0, Y = 0, Width = 96, Height = 64}
};
root.Sprites["YN1ChipNoL"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1056.0, Y = 64, Width = 96, Height = 64}
};
root.Sprites["YN1ChipYesS"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1056.0, Y = 128.0, Width = 96, Height = 64}
};
root.Sprites["YN1ChipNoS"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1056.0, Y = 192.0, Width = 96, Height = 64}
};
root.Sprites["YN2ChipYesL"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1248.0, Y = 0, Width = 148.0, Height = 70}
};
root.Sprites["YN2ChipNoL"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1248.0, Y = 96, Width = 148.0, Height = 70}
};
root.Sprites["YN2ChipYesS"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1248.0, Y = 192.0, Width = 106.0, Height = 46}
};
root.Sprites["YN2ChipNoS"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1248.0, Y = 288.0, Width = 106.0, Height = 46}
};
root.Sprites["YNChipStar"] = {
    Sheet = "YesNoChip", 
    Bounds = {X = 1024.0, Y = 374.0, Width = 400.0, Height = 400}
};
root.Sprites["YNOverlayMask"] = {
    Sheet = "MenuChip", 
    Bounds = {X = 156.0, Y = 144.0, Width = 1896.0, Height = 1056}
};
