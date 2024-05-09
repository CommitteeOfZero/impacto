
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
  createYesNoData(698.0, -674., 4, 1, 2372.0 , 278.0  , 2626.0 , 440.0  , 2646.0 , 330.0),
  createYesNoData(698.0, -335., 5, 2, 2392.0 , 588.0  , 2626.0 , 764.0  , 2646.0 , 645.0),
  createYesNoData(698.0, 36.0, 6, 3, 2378.0 , 897.0  , 2713.0 , 1069.0 , 2646.0 , 975.0),
  createYesNoData(698.0, 234.0, 7, 14, 2410.0 , 1224.0 , 2702.0 , 1347.0 , 2661.0 , 1287.0),     
  createYesNoData(238.0, -469., 8, 5, 2042.0 , 344.0  , 2048.0 , 562.0  , 2174.0 , 504.0),
  createYesNoData(238.0, -179., 9, 6, 1975.0 , 784.0  , 2126.0 , 886.0  , 2235.0 , 765.0),
  createYesNoData(238.0, 188.0, 10, 7, 1974.0 , 1162.0 , 2036.0 , 1187.0 , 2187.0 , 1098.0),     
  createYesNoData(287.0, 412.0, 14, 15, 2330.0 , 1440.0 , 2040.0 , 1460.0 , 2229.0 , 1356.0),     
  createYesNoData(-248., -636., 11, 9, 1459.0 , 232.0  , 1727.0 , 439.0  , 1689.0 , 321.0),
  createYesNoData(-283., -68.0, 12, 13, 1535.0 , 694.0  , 1589.0 , 936.0  , 1647.0 , 852.0),
  createYesNoData(-337., 396.0, 16, 15, 1424.0 , 1370.0 , 1540.0 , 1448.0 , 1632.0 , 1341.0),     
  createYesNoData(-672., -487., 18, 12, 1052.0 , 335.0  , 1156.0 , 504.0  , 1269.0 , 444.0),
  createYesNoData(-708., -182., 18, 17, 1042.0 , 638.0  , 1049.0 , 819.0  , 1230.0 , 750.0),
  createYesNoData(-659., 131.0, 17, 16, 1064.0 , 1029.0 , 1184.0 , 1181.0 , 1302.0 , 1068.0),
  createYesNoData(381.0, 730.0, 0, 0, 2042.0 , 344.0  , 2048.0 , 562.0  , 2337.0 , 1722.0),
  createYesNoData(-427., 730.0, 0, 0, 2042.0 , 344.0  , 2048.0 , 562.0  , 1515.0 , 1722.0),
  createYesNoData(-1108., 499.0, 0, 0, 2042.0 , 344.0  , 2048.0 , 562.0  , 828.0  , 1449.0),
  createYesNoData(-1108., -31.0, 0, 0, 2042.0 , 344.0  , 2048.0 , 562.0  , 828.0  , 957.0),
  createYesNoData(-1108., -529., 0, 0, 2042.0 , 344.0  , 2048.0 , 562.0  , 828.0  , 453.0),
}

local data2 = {
  createYesNoData(1270.0, 198, 1, 2, 3304, 996, 3304, 1364, 3364, 1200.0),
  createYesNoData(1189, -151, 3, 4, 2746, 767, 2883, 992, 3051, 846.0 ),
  createYesNoData(1189, 635, 4, 5, 2881, 1393, 2741, 1628, 3051, 1584.0),
  createYesNoData(592, -238, 6, 7, 2275, 564, 2159, 740, 2469, 762.0 ),
  createYesNoData(592, 245, 7, 8, 2340, 1022, 2159, 1196, 2469, 1215.0),
  createYesNoData(592, 716, 8, 9, 2340, 1482, 2154, 1668, 2469, 1674.0),
  createYesNoData(-185, -598, 14, 10, 1396, 329, 1568, 489, 1719, 351.0 ),
  createYesNoData(12.0, -214, 10, 11, 1588, 665, 1747, 891, 1908, 762.0 ),
  createYesNoData(12.0, 278, 12, 13, 1588, 1124, 1747, 1349, 1908, 1215.0),
  createYesNoData(-185, 701, 13, 18, 1680, 1482, 1396, 1668, 1719, 1674.0),
  createYesNoData(-629, -340, 14, 15, 1111, 435, 998, 654, 1311, 618.0 ),
  createYesNoData(-629, -71, 15, 16, 998, 807, 998, 940, 1311, 891.0 ),
  createYesNoData(-629, 198, 16, 17, 998, 1059, 998, 1196, 1311, 1150.0),
  createYesNoData(-629, 467, 17, 18, 998, 1364, 1111, 1578, 1311, 1422.0),
  createYesNoData(-1245.0, -536, 0, 0, 2042, 344, 2048, 562, 615, 408.0 ),
  createYesNoData(-1245.0, -253, 0, 0, 2042, 344, 2048, 562, 615, 723.0 ),
  createYesNoData(-1245.0, 73.0, 0, 0, 2042, 344, 2048, 562, 615, 1032.0),
  createYesNoData(-1245.0, 396, 0, 0, 2042, 344, 2048, 562, 615, 1344.0),
  createYesNoData(-1245.0, 693, 0, 0, 2042, 344, 2048, 562, 615, 1668.0),
}

root.YesNoTrigger = {
  YesNoBackground0 = "YesNoBackground0",
  YesNoBackground1 = "YesNoBackground1",
  YesNoBackground2 = "YesNoBackground2",
  YesNoBackground3 = "YesNoBackground3",
  YN1_ChipYesL = "YNTRG1_Chip_Yes_L",
  YN1_ChipNoL = "YNTRG1_Chip_No_L",
  YN1_ChipYesS = "YNTRG1_Chip_Yes_S",
  YN1_ChipNoS = "YNTRG1_Chip_No_S",
  YN2_ChipYesL = "YNTRG2_Chip_Yes_L",
  YN2_ChipNoL = "YNTRG2_Chip_No_L",
  YN2_ChipYesS = "YNTRG2_Chip_Yes_S",
  YN2_ChipNoS = "YNTRG2_Chip_No_S",
  ChipStar = "YNTRG_Chip_Star",
  BGMask = "YNTRG_BGMask",
  YesNoData1 = data1,
  YesNoData2 = data2,
};

root.Sprites["YesNoBackground0"] = {
  Sheet = "YNTRG_BG0",
  Bounds = { X = 0, Y = 0, Width = 3840, Height = 2160 }
};
root.Sprites["YesNoBackground1"] = {
  Sheet = "YNTRG_BG1",
  Bounds = { X = 0, Y = 0, Width = 3840, Height = 2160 }
};
root.Sprites["YesNoBackground2"] = {
  Sheet = "YNTRG_BG2",
  Bounds = { X = 0, Y = 0, Width = 3840, Height = 2160 }
};
root.Sprites["YesNoBackground3"] = {
  Sheet = "YNTRG_BG3",
  Bounds = { X = 0, Y = 0, Width = 3840, Height = 2160 }
};
root.Sprites["YNTRG1_Chip_Yes_L"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1056, Y = 0, Width = 96, Height = 64 }
};
root.Sprites["YNTRG1_Chip_No_L"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1056, Y = 64, Width = 96, Height = 64 }
};
root.Sprites["YNTRG1_Chip_Yes_S"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1056, Y = 128, Width = 96, Height = 64 }
};
root.Sprites["YNTRG1_Chip_No_S"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1056, Y = 192, Width = 96, Height = 64 }
};
root.Sprites["YNTRG2_Chip_Yes_L"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1248, Y = 0, Width = 148, Height = 70 }
};
root.Sprites["YNTRG2_Chip_No_L"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1248, Y = 96, Width = 148, Height = 70 }
};
root.Sprites["YNTRG2_Chip_Yes_S"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1248, Y = 192, Width = 106, Height = 46 }
};
root.Sprites["YNTRG2_Chip_No_S"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1248, Y = 288, Width = 106, Height = 46 }
};
root.Sprites["YNTRG_Chip_Star"] = {
  Sheet = "YNTRG_CHIP",
  Bounds = { X = 1024, Y = 374, Width = 400, Height = 400 }
};
root.Sprites["YNTRG_BGMask"] = {
  Sheet = "MenuChip",
  Bounds = { X = 156, Y = 144, Width = 1896, Height = 1056 },
};
