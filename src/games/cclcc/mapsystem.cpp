#include "mapsystem.h"
#include <array>
#include <algorithm>
#include <vector>
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../game.h"
#include "../../profile/scriptvars.h"

#include "../../profile/games/cclcc/mapsystem.h"
#include "../../profile/ui/mapsystem.h"
#include <random>

namespace Impacto {
namespace UI {
namespace CCLCC {
using namespace Impacto::Vm::Interface;
using namespace Impacto::UI::CCLCC;
using namespace Impacto::Profile::CCLCC::MapSystem;
using namespace Impacto::Profile::ScriptVars;

std::array<MapSystemCCLCC::MapPoolMember, 20> MapPool = {};
std::array<MapSystemCCLCC::MapPoolDispStruct, 40> MapPoolDisp;
std::array<MapSystemCCLCC::MapPartsDispStruct, 40> MapPartsDisp;

std::array<int, 20> MapPoolCurCt = {};

int MapPartsMax = 0;

int MapPoolCnt = 0;
int selectedMapPoolIdx = 0xff;
int hoverMapPoolIdx = 0xff;
float mapSizeX;
float mapSizeY;
float mapPosX;
float mapPosY;

constexpr int MapIdMapping[11][4] = {
    // index 0 is map part for photos
    // index 3 is map part for article

    {0, 1, 2, 0x3e8},       {3, 4, 5, 0x3e9},       {6, 7, 8, 0x3ea},
    {9, 10, 11, 0x3eb},     {10, 0xff, 0xff, 0xff}, {12, 0xff, 0xff, 0xff},
    {13, 0xff, 0xff, 0xff}, {14, 0xff, 0xff, 0xff}, {15, 0xff, 0xff, 0xff},
    {16, 0xff, 0xff, 0xff}, {17, 0xff, 0xff, 0xff},
};

constexpr float MapPoolPosOffsets[][2] = {
    {2709.0, 200.0}, {2981.0, 214.0}, {3222.0, 203.0},  {2717.0, 332.0},
    {2984.0, 349.0}, {3236.0, 342.0}, {2713.0, 459.0},  {2974.0, 482.0},
    {3222.0, 485.0}, {2734.0, 596.0}, {2740.0, 784.0},  {3047.0, 787.0},
    {2754.0, 930.0}, {3066.0, 930.0}, {2755.0, 1072.0}, {3064.0, 1073.0},
};

constexpr float MapPoolArticleOffsets[][2] = {
    {2699.0, 200.0}, {3071.0, 214.0}, {2707.0, 482.0}, {3074.0, 499.0}};

constexpr float MapArticleOffsets[][12]{
    {1095.0, 813.0, 1143.0, 745.0, 1169.0, 676.0, 1048.0, 735.0, 898.0, 744.0,
     882.0, 849.0},
    {1095.0, 813.0, 1143.0, 745.0, 302.0, 1259.0, 390.0, 1093.0, 226.0, 982.0,
     218.0, 1094.0},
    {439.0, 1189.0, 481.0, 1113.0, 756.0, 1005.0, 607.0, 869.0, 478.0, 953.0,
     451.0, 843.0},
    {645.0, 975.0, 707.0, 881.0, 756.0, 1005.0, 607.0, 869.0, 478.0, 953.0,
     451.0, 843.0},
    {645.0, 975.0, 707.0, 881.0, 842.0, 621.0, 680.0, 564.0, 698.0, 432.0,
     864.0, 424.0},
    {680.0, 564.0, 779.0, 647.0, 842.0, 621.0, 680.0, 564.0, 698.0, 432.0,
     864.0, 424.0},
    {680.0, 564.0, 779.0, 647.0, 1361.0, 628.0, 1466.0, 484.0, 0.0, 0.0, 0.0,
     0.0},
    {0.0, 0.0, 0.0, 0.0, 155.0, 550.0, 308.0, 440.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 1037.0, 1355.0, 1203.0, 1320.0, 0.0, 0.0, 0.0, 0.0},
    {1113.0, 1419.0, 1298.0, 1324.0, 1188.0, 1043.0, 1294.0, 934.0, 0.0, 0.0,
     0.0, 0.0},
    {1431.0, 980.0, 1390.0, 952.0, 515.0, 1412.0, 645.0, 1499.0, 0.0, 0.0, 0.0,
     0.0},
    {788.0, 1553.0, 750.0, 1511.0, 1132.0, 1139.0, 984.0, 1175.0, 0.0, 0.0, 0.0,
     0.0},
    {832.0, 1183.0, 1082.0, 1194.0, 1277.0, 52.0, 1430.0, 170.0, 0.0, 0.0, 0.0,
     0.0},
    {1589.0, 205.0, 1512.0, 183.0, 1339.0, 1243.0, 1536.0, 1264.0, 0.0, 0.0,
     0.0, 0.0},
    {1644.0, 1359.0, 1638.0, 1284.0, 1508.0, 1411.0, 1357.0, 1490.0, 0.0, 0.0,
     0.0, 0.0},
    {1306.0, 1596.0, 1453.0, 1509.0, 0.0, 0.0, 2658.0, 913.0, 2998.0, 915.0,
     3184.0, 921.0},
    {2803.0, 931.0, 2641.0, 866.0, 1298.0, 1257.0, 1458.0, 1131.0, 0.0, 0.0,
     0.0, 0.0},
    {1627.0, 1126.0, 1309.0, 1284.0, 0.0, 0.0, 205.0, 680.0, 205.0, 680.0,
     205.0, 680.0},
};

constexpr float PartsOffsetData[][3][4] = {{{1169.0, 676.0, 1048.0, 735.0},
                                            {898.0, 744.0, 882.0, 849.0},
                                            {1095.0, 813.0, 1143.0, 745.0}},

                                           {{302.0, 1259.0, 390.0, 1093.0},
                                            {226.0, 982.0, 218.0, 1094.0},
                                            {439.0, 1189.0, 481.0, 1113.0}},

                                           {{756.0, 1005.0, 607.0, 869.0},
                                            {478.0, 953.0, 451.0, 843.0},
                                            {645.0, 975.0, 707.0, 881.0}},

                                           {{842.0, 621.0, 680.0, 564.0},
                                            {698.0, 432.0, 864.0, 424.0},
                                            {620.0, 650.0, 779.0, 647.0}},

                                           {{756.0, 1005.0, 607.0, 869.0},
                                            {478.0, 953.0, 451.0, 843.0},
                                            {645.0, 975.0, 707.0, 881.0}},

                                           {{756.0, 1005.0, 607.0, 869.0},
                                            {478.0, 953.0, 451.0, 843.0},
                                            {645.0, 975.0, 707.0, 881.0}},

                                           {{756.0, 1005.0, 607.0, 869.0},
                                            {478.0, 953.0, 451.0, 843.0},
                                            {645.0, 975.0, 707.0, 881.0}},

                                           {{756.0, 1005.0, 607.0, 869.0},
                                            {478.0, 953.0, 451.0, 843.0},
                                            {645.0, 975.0, 707.0, 881.0}}};

constexpr float PartsIdMap[][4] = {{2, 0xff, 0xff, 0xff}};

constexpr uint32_t Tints[] = {
    0xB43C3C, 0xB43C3C, 0xCA2ECC, 0x387A21, 0xEFA918, 0xEFA918, 0x40D08A,
    0xABABAB, 0xE26E10, 0x1761F5, 0xABABAB, 0xE26E10, 0x1761F5, 0x387A21,
    0xB43C3C, 0xCA2ECC, 0xEFA0DE, 0x387A21, 0xCA2ECC, 0x40D08A, 0xEFA0DE,
    0xABABAB, 0xE26E10, 0xEFA918, 0x40D08A, 0xEFA0DE, 0x1761F5, 0x387A21,
    0x387A21, 0x1761F5, 0xEFA918, 0xEFA918, 0x40D08A, 0x40D08A, 0xEFA0DE,
    0xABABAB, 0x387A21, 0x1761F5, 0xEFA918, 0x40D08A, 0xEFA0DE, 0xABABAB,
    0xE26E10, 0xE26E10, 0xCA2ECC, 0xCA2ECC, 0x387A21, 0x1761F5, 0xEFA918,
    0x40D08A, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC,
    0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC,
    0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC, 0xCA2ECC,
    0xCA2ECC, 0xCA2ECC, 0xE26E10, 0xE26E10, 0xE26E10, 0xE26E10, 0xE26E10,
    0xE26E10, 0xE26E10, 0xE26E10, 0xE26E10, 0x40D08A, 0xEFA0DE, 0xE26E10,
    0x40D08A, 0x40D08A};

static int CALCrnd(int arg1) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, 0x7FFF);
  return distr(gen) * arg1 >> 0xf;
}

void MapSystemCCLCC::MapInit() {
  ScrWork[6369] = 0;
  ScrWork[6370] = 0;
  ScrWork[6371] = 0;

  MapPartsMax = 0;

  MapResetPoolAll(0);
}
void MapSystemCCLCC::MapSetFadein(int arg1, int arg2) {
  mapBgState = MapSystemCCLCC::MapBGState::Showing;
}
void MapSystemCCLCC::MapSetGroup(int arg1, int arg2, int arg3, int arg4) {}
void MapSystemCCLCC::MapSetFadeout(int arg1, int arg2) {}
void MapSystemCCLCC::MapSetDisp(int arg1, int arg2) {
  if (MapPartsMax == 0) {
    for (int i = 0; i < MapPartsMax; ++i) {
      const auto& part = MapPartsDisp[i];
      if (part.partId == arg1 && part.type == arg2) return;
    }
  }
  MapPartsDisp[MapPartsMax].partId = arg1;
  MapPartsDisp[MapPartsMax].type = arg2;
  MapPartsDisp[MapPartsMax].shown = 1;
  MapPartsDisp[MapPartsMax].inOrOut = 1;
  MapPartsDisp[MapPartsMax].progress = 0;
  MapPartsDisp[MapPartsMax].animState = 0;
  MapPartsDisp[MapPartsMax].angle = CALCrnd(3640);

  if (arg2 == 7) {
    float pos1 = PartsOffsetData[arg1][0][0] - PartsOffsetData[arg1][0][2];
    float pos2 = PartsOffsetData[arg1][0][1] - PartsOffsetData[arg1][0][3];
    MapPartsDisp[MapPartsMax].dist =
        sqrt(pos1 * pos1 + pos2 * pos2) / 200.0 * 16.0;

  } else if (arg2 > 11 && arg2 < 15 || arg2 == 6) {
    int index = (arg2 == 6) ? 0 : arg2 - 11;
    int mappedId = (arg1 < sizeof(PartsIdMap) / sizeof(float[4]))
                       ? PartsIdMap[arg1][index]
                       : 0;
    if (mappedId == 0xff) return;
    float pos1 = PartsOffsetData[arg1][0][2] - PartsOffsetData[mappedId][0][2];
    float pos2 = PartsOffsetData[arg1][0][3] - PartsOffsetData[mappedId][0][3];
    MapPartsDisp[MapPartsMax].dist =
        sqrt(pos1 * pos1 + pos2 * pos2) / 200.0 * 16.0;
  }

  if (arg2 == 5) {
    MapPartsDisp[MapPartsMax].angle = CALCrnd(1820) - 910;
  } else {
    MapPartsDisp[MapPartsMax].angle = CALCrnd(3640) - 1820;
  }
  MapPartsMax++;
  if (arg2 > 4) {
    if (arg2 != 7) return;
    int i;
    for (i = 0; i < MapPartsMax; i++) {
      if (MapPartsDisp[i].partId == arg1 && MapPartsDisp[i].type == arg2) {
        if (i != MapPartsMax) {
          return;
        }
        break;
      }
    }
    MapPartsDisp[MapPartsMax].partId = arg1;
    MapPartsDisp[MapPartsMax].type = 0;
    MapPartsDisp[MapPartsMax].shown = 1;
    MapPartsDisp[MapPartsMax].inOrOut = 1;
    MapPartsDisp[MapPartsMax].progress = 0;
    MapPartsDisp[MapPartsMax].animState = 0;
    MapPartsDisp[MapPartsMax].angle = CALCrnd(12);
    MapPartsMax++;
    return;
  }

  if (arg2 > 0 && arg2 < 4 && MapIdMapping[arg1][arg2 - 1] < 999) {
    return;
  }
  MapPartsDisp[MapPartsMax].partId = arg1;
  MapPartsDisp[MapPartsMax].type = arg2 + 7;
  MapPartsDisp[MapPartsMax].shown = 1;
  MapPartsDisp[MapPartsMax].inOrOut = 1;
  MapPartsDisp[MapPartsMax].progress = 0;
  MapPartsDisp[MapPartsMax].animState = 0;
  MapPartsDisp[MapPartsMax].angle = CALCrnd(12);
}
void MapSystemCCLCC::MapSetHide(int arg1, int arg2) {}
bool MapSystemCCLCC::MapPoolFadeEndChk_Wait() {
  for (int i = 0; i < 20; ++i) {
    if (MapPool[i].id == 0xff) continue;
    if (MapPoolDisp[i * 2].info.shown != MapPoolDisp[i * 2].info.inOrOut)
      return false;
    if (MapPoolDisp[i * 2 + 1].info.shown !=
        MapPoolDisp[i * 2 + 1].info.inOrOut)
      return false;
    if (MapPoolDisp[i * 2].info.progress != 0) return false;
    if (MapPoolDisp[i * 2 + 1].info.progress != 0) return false;
    if (MapPoolDisp[i * 2].animState != 0) return false;
    if (MapPoolDisp[i * 2 + 1].animState != 0) return false;
  }
  return true;
}
void MapSystemCCLCC::MapMoveAnimeInit(int arg1, int arg2, int arg3) {}
void MapSystemCCLCC::MapMoveAnimeMain() {}
void MapSystemCCLCC::MapGetPos(int arg1, int arg2) {}
void MapSystemCCLCC::MapSetPool(int arg1, int arg2, int arg3) {
  MapPool[arg1].id = arg2;
  MapPool[arg1].type = arg3;
}
void MapSystemCCLCC::MapResetPoolAll(int arg1) {
  for (int i = 0; i < 20; ++i) {
    MapPool[i].id = 0xff;
    MapPool[i].type = 0xff;
  }
  for (int i = 0; i < 40; ++i) {
    MapPoolDisp[i].info.shown = 0;
    MapPoolDisp[i].info.inOrOut = 0;
    MapPoolDisp[i].info.progress = 0;
    MapPoolDisp[i].angle = 0;
  }
}
bool MapSystemCCLCC::MapFadeEndChk_Wait() { return true; }
void MapSystemCCLCC::MapPoolShuffle(int param_1) {
  auto start = MapPool.begin() + param_1 * 10;
  auto end = start;

  // Find the end of the range to shuffle
  while (end != MapPool.end() && end->id != 255) {
    ++end;
  }

  std::random_device rd;
  std::mt19937 g(rd());

  // Why reinvent the wheel
  std::shuffle(start, end, g);
}

void MapSystemCCLCC::MapPoolSetDisp(int arg1, int arg2) {
  int id = MapPool[arg2].id;

  if (id != 0xff) {
    MapPoolDisp[arg2 * 2].info.shown = 1;
    MapPoolDisp[arg2 * 2].info.inOrOut = 1;
    MapPoolDisp[arg2 * 2].info.progress = 0;
    MapPoolDisp[arg2 * 2].animState = 0;
    int randAngle = CALCrnd(3640);
    MapPoolDisp[arg2 * 2].angle = randAngle - 1820;
    int poolPinIndex = arg2 * 2 + 1;

    // Display map pool pin for non articles
    if (MapIdMapping[MapPool[arg2].id][MapPool[arg2].type] < 1000) {
      MapPoolDisp[poolPinIndex].info.shown = 1;
      MapPoolDisp[poolPinIndex].info.inOrOut = 1;
      MapPoolDisp[poolPinIndex].info.progress = 0;
      MapPoolDisp[poolPinIndex].animState = 0;
      MapPoolDisp[poolPinIndex].pinId = CALCrnd(12);
    } else {
      MapPoolDisp[poolPinIndex].info.inOrOut = 0;
      MapPoolDisp[poolPinIndex].info.shown = 0;
      MapPoolDisp[poolPinIndex].info.progress = 0;
      MapPoolDisp[poolPinIndex].animState = 0;
    }
  }
}
void MapSystemCCLCC::MapPoolSetHide(int arg1, int arg2) {}
void MapSystemCCLCC::MapPoolSetFadein(int arg1, int arg2) {
  // arg1 always 0

  int id = MapPool[arg2].id;
  if (id != 0xff) {
    if (MapPoolDisp[arg2 * 2].info.shown == 0) {
      MapPoolDisp[arg2 * 2].info.shown = 0;
      MapPoolDisp[arg2 * 2].info.inOrOut = 1;
      MapPoolDisp[arg2 * 2].info.progress = 16;
      MapPoolDisp[arg2 * 2].animState = 0;
      int randAngle = CALCrnd(3640);
      MapPoolDisp[arg2 * 2].angle = randAngle - 1820;
    }
    int poolPinIndex = arg2 * 2 + 1;
    if (MapIdMapping[MapPool[arg2].id][MapPool[arg2].type] < 1000) {
      if (MapPoolDisp[poolPinIndex].info.shown == 0) {
        MapPoolDisp[poolPinIndex].info.shown = 0;
        MapPoolDisp[poolPinIndex].info.inOrOut = 1;
        MapPoolDisp[poolPinIndex].info.progress = 16;
        MapPoolDisp[poolPinIndex].animState = 16;
        MapPoolDisp[poolPinIndex].pinId = CALCrnd(12);
      }
    } else {
      MapPoolDisp[poolPinIndex].info.inOrOut = 0;
      MapPoolDisp[poolPinIndex].info.shown = 0;
      MapPoolDisp[poolPinIndex].info.progress = 0;
      MapPoolDisp[poolPinIndex].animState = 0;
    }
  }
}
void MapSystemCCLCC::MapPoolSetFadeout(int arg1, int arg2) {
  if (MapPoolDisp[arg2 * 2].info.shown != 0) {
    MapPoolDisp[arg2 * 2].info.inOrOut = 0;
    MapPoolDisp[arg2 * 2].info.progress = 0x10;
    MapPoolDisp[arg2 * 2].animState = 0x10;
  }
  int poolPinIndex = arg2 * 2 | 1;
  if (MapPoolDisp[poolPinIndex].info.shown != 0) {
    MapPoolDisp[poolPinIndex].info.inOrOut = 0;
    MapPoolDisp[poolPinIndex].info.progress = 0x10;
    MapPoolDisp[poolPinIndex].animState = 0;
  }
  return;
}

void HandlePoolUpDownNav(int maxPoolRow, int poolType, bool isUp) {
  if (hoverMapPoolIdx == 0xff) {
    for (int i = 0; i < 10; ++i) {
      if (MapPool[i].id != 0xff && MapPoolDisp[i * 2].info.shown) {
        hoverMapPoolIdx = i;
        return;
      }
    }
  } else {
    int tmpPoolIdx;
    while (true) {
      if (poolType == 3) {
        hoverMapPoolIdx =
            (hoverMapPoolIdx < 2) ? hoverMapPoolIdx + 2 : hoverMapPoolIdx - 2;
      } else {
        if (isUp)
          hoverMapPoolIdx =
              (hoverMapPoolIdx < 3) ? hoverMapPoolIdx + 6 : hoverMapPoolIdx - 3;
        else
          hoverMapPoolIdx =
              (hoverMapPoolIdx < 6) ? hoverMapPoolIdx + 3 : hoverMapPoolIdx - 6;
        if (MapPoolDisp[18].info.shown && MapPool[9].id != 0xff) {
          hoverMapPoolIdx = 9;
          return;
        }
      }
      if (MapPool[hoverMapPoolIdx].id != 0xff &&
          MapPoolDisp[hoverMapPoolIdx * 2].info.shown) {
        return;
      }
      int colNum = hoverMapPoolIdx % maxPoolRow;
      int rowNum = hoverMapPoolIdx / maxPoolRow;
      tmpPoolIdx = rowNum * maxPoolRow;

      if (colNum >= 0 && colNum <= 2) {
        constexpr int offsets[3][2] = {{1, 2}, {0, 2}, {1, 0}};

        for (int i = 0; i < 2; ++i) {
          if ((MapPool[tmpPoolIdx + offsets[colNum][i]].id != 0xff &&
               MapPoolDisp[(tmpPoolIdx + offsets[colNum][i]) * 2].info.shown)) {
            tmpPoolIdx += offsets[colNum][i];
            break;
          }
          if (colNum < 2 && poolType == 3) break;
        }
      }
    }
    hoverMapPoolIdx = tmpPoolIdx;
  }
}

void HandlePoolLeftRightNav(int maxPoolRow, int poolType, bool isLeft) {
  if (hoverMapPoolIdx == 0xff) {
    for (int i = 0; i < 10; ++i) {
      if (MapPool[i].id != 0xff && MapPoolDisp[i * 2].info.shown) {
        hoverMapPoolIdx = i;
        return;
      }
    }
  } else {
    int mapId = hoverMapPoolIdx;
    while (true) {
      if (poolType == 3) {
        if (hoverMapPoolIdx != 3)
          mapId = (hoverMapPoolIdx == 0 || hoverMapPoolIdx == 2)
                      ? hoverMapPoolIdx + 1
                      : hoverMapPoolIdx - 1;
        else
          mapId = 2;
      } else {
        if (hoverMapPoolIdx == 9) {
          mapId = (isLeft) ? 6 : 8;
        } else {
          if (isLeft) {
            if (hoverMapPoolIdx % 3 == 0) {
              mapId = hoverMapPoolIdx + 2;
            } else {
              mapId = hoverMapPoolIdx - 1;
            }
          } else {
            if (hoverMapPoolIdx % 3 == 2) {
              mapId = hoverMapPoolIdx - 2;
            } else {
              mapId = hoverMapPoolIdx + 1;
            }
          }
        }
      }
      hoverMapPoolIdx = mapId;
      if (MapPool[hoverMapPoolIdx].id != 0xff) {
        if (MapPoolDisp[hoverMapPoolIdx * 2].info.shown == 1) break;
      }

      int rowNum = mapId / maxPoolRow;

      if (rowNum >= 0 && rowNum <= 2) {
        int offsets[3][2] = {{maxPoolRow, 2 * maxPoolRow},
                             {-maxPoolRow, maxPoolRow},
                             {-maxPoolRow, -2 * maxPoolRow}};

        for (int i = 0; i < 2; ++i) {
          mapId = hoverMapPoolIdx + offsets[rowNum][i];

          if (MapPool[mapId].id != 0xff && MapPoolDisp[mapId * 2].info.shown) {
            break;
          }
        }

        if ((rowNum == 2 && hoverMapPoolIdx == maxPoolRow * 2) ||
            (rowNum == 1 && hoverMapPoolIdx == 3) ||
            (rowNum == 0 && hoverMapPoolIdx == 0)) {
          if (MapPoolDisp[18].info.shown == 1 && MapPool[9].id != 0xff) {
            mapId = 9;
            break;
          }
        }
      }
    }
    hoverMapPoolIdx = mapId;
  }
}

bool MapSystemCCLCC::MapPlayerPhotoSelect(int arg1) {
  ScrWork[6500] = 2;

  if (arg1 == 0) {
    int poolType = MapPool[0].type;
    int maxPoolRow =
        (poolType == 3) ? 2 : 3;  // 3 per row for photo, 2 for articles

    if (PADinputButtonWentDown & PAD1UP) {
      HandlePoolUpDownNav(maxPoolRow, poolType, true);
    } else if (PADinputButtonWentDown & PAD1DOWN) {
      HandlePoolUpDownNav(maxPoolRow, poolType, false);
    } else if (PADinputButtonWentDown & PAD1LEFT) {
      HandlePoolLeftRightNav(maxPoolRow, poolType, true);
    } else if (PADinputButtonWentDown & PAD1RIGHT) {
      HandlePoolLeftRightNav(maxPoolRow, poolType, false);
    } else if (PADinputButtonWentDown & PAD1A && (hoverMapPoolIdx != 0xff) &&
               (MapPoolCurCt[hoverMapPoolIdx] == 16)) {
      if (hoverMapPoolIdx != 0xff) {
        selectedMapPoolIdx = hoverMapPoolIdx;
        MapPoolDisp[hoverMapPoolIdx * 2].info.shown = 0;
        ScrWork[6367] = hoverMapPoolIdx;
        ScrWork[6368] = MapPool[selectedMapPoolIdx].id;
        ScrWork[6381] = MapPool[selectedMapPoolIdx].type;
        ScrWork[6500] = 0;
        hoverMapPoolIdx = 0xff;
        return true;
      }
    }
  } else {
    ImpLogSlow(LL_Debug, LC_VMStub, "MapPlayerPhotoSelect: arg1 != 0\n");
  }
  return false;
}

void MapSystemCCLCC::MapResetPool(int arg1) {
  MapPool[arg1].id = 0xff;
  MapPool[arg1].type = 0xff;
}
void MapSystemCCLCC::MapSetGroupEx(int arg1, int arg2, int arg3) {}
void MapSystemCCLCC::MapZoomInit(int arg1, int arg2, int arg3) {}
void MapSystemCCLCC::MapZoomMain() {}
void MapSystemCCLCC::MapZoomInit2(int arg1, int arg2) {}
void MapSystemCCLCC::MapZoomMain3() {}
void MapSystemCCLCC::MapZoomInit3(int arg1, int arg2, int arg3) {}
void MapSystemCCLCC::MapMoveAnimeInit2(int arg1, int arg2, int arg3) {}
void MapSystemCCLCC::MapPlayerPotalSelectInit() {}
bool MapSystemCCLCC::MapPlayerPotalSelect() { return false; }
void MapSystemCCLCC::MapSystem_28() {}

void MapPoolDispPhoto(int poolId) {
  int mappedMapPoolId = MapIdMapping[MapPool[poolId].id][MapPool[poolId].type];
  if (mappedMapPoolId == 0xff) return;

  Sprite displayedSprite;
  float xOffset;
  float yOffset;

  // Display Photo
  if (mappedMapPoolId < 1000) {
    displayedSprite = MapPartsPhotoSprites[mappedMapPoolId];
    xOffset = MapPoolPosOffsets[poolId][0];
    yOffset = MapPoolPosOffsets[poolId][1];
  }
  // Display Article
  else {
    displayedSprite = MapPartsArticleSprites[mappedMapPoolId - 1000];
    xOffset = MapPoolArticleOffsets[poolId][0];
    yOffset = MapPoolArticleOffsets[poolId][1];
  }

  xOffset = xOffset - 16.0f - 82.0;
  yOffset = yOffset - 10.0f;

  int hoverAnimState = 0;
  int alpha = 256;
  float shadowZoom, zoomMulti;
  if (MapPoolDisp[poolId * 2].info.shown) {
    hoverAnimState = MapPoolDisp[poolId * 2].animState;
    shadowZoom = 1;
    zoomMulti = 1;
    if (!(MapPoolDisp[poolId * 2].info.inOrOut ||
          MapPoolDisp[poolId * 2].animState)) {
      zoomMulti = 16.0f - MapPoolDisp[poolId * 2].info.progress;
      shadowZoom = (zoomMulti) / 24.0f + 1.0f;
      zoomMulti = zoomMulti / 16.0f + 1.0f;
      alpha = MapPoolDisp[poolId * 2].info.progress * 16;
    }
  } else {
    zoomMulti = MapPoolDisp[poolId * 2].info.progress - 16.0f;
    shadowZoom = (zoomMulti) / 24.0f + 1.0f;
    zoomMulti = zoomMulti / 16.0f + 1.5f;
    alpha = MapPoolDisp[poolId * 2].info.progress * -16 + 256;
  }
  float scaledFactor = 1080.0f / mapSizeY;
  zoomMulti *= scaledFactor;
  float xMapEdge = mapPosX + mapSizeX;
  float yMapEdge = mapPosY + mapSizeY;

  if (mapPosX <= xOffset + displayedSprite.Bounds.Width &&
      mapPosY <= yOffset + displayedSprite.Bounds.Height &&
      !std::isnan(xOffset) && !std::isnan(xMapEdge) && !std::isnan(yOffset) &&
      !std::isnan(yMapEdge) && xOffset <= xMapEdge && yOffset <= yMapEdge) {
    float angle = MapPoolDisp[poolId * 2].angle * 2.0f * M_PI / 65536.0f;

    xOffset = xOffset - mapPosX;
    yOffset = yOffset - mapPosY;

    float scaledPosOffsetX = (xOffset + 82) * scaledFactor;
    float scaledPosOffsetY = (yOffset + 10) * scaledFactor;

    if (MapPoolCurCt[poolId] != 0) {
      float selectedTagXOffset =
          (MapPoolCurCt[poolId] * 50.0f / 16.0f + 78.0f) * 2;

      Renderer->DrawSpriteCentered(
          SelectedMapPoolTagSprite,
          glm::vec2(scaledPosOffsetX - selectedTagXOffset,
                    scaledPosOffsetY - 18),
          glm::vec2{selectedTagXOffset, 11},
          glm::vec4{1.0f, 1.0f, 1.0f, alpha / 256.0f},
          glm::vec2{zoomMulti / 2.0, zoomMulti / 2.0}, angle);
    }
    // Shadow
    Renderer->DrawSpriteCentered(
        displayedSprite,
        glm::vec2((xOffset + 82 + 3) * scaledFactor - 82,
                  (yOffset + 10 + 3) * scaledFactor - 10),
        glm::vec2{82, 10}, glm::vec4{0.0f, 0.0f, 0.0f, (alpha >> 1) / 256.0f},
        glm::vec2{shadowZoom * scaledFactor, shadowZoom * scaledFactor}, angle);
    // Photo
    Renderer->DrawSpriteCentered(
        displayedSprite,
        glm::vec2(scaledPosOffsetX - 82, scaledPosOffsetY - 10),
        glm::vec2{82, 10}, glm::vec4{1.0f, 1.0f, 1.0f, alpha / 256.0f},
        glm::vec2{zoomMulti, zoomMulti}, angle);
  }
}

void MapPoolDispPin(int poolId) {
  int poolDispIndex = poolId * 2 + 1;
  Sprite displayedSprite = MapPartsPinSprites[MapPoolDisp[poolDispIndex].pinId];

  int alpha = 256;
  float zoomMulti;
  if (MapPoolDisp[poolDispIndex].info.shown) {
    if (!(MapPoolDisp[poolDispIndex].info.inOrOut ||
          MapPoolDisp[poolDispIndex].animState)) {
      zoomMulti = 16.0f - MapPoolDisp[poolDispIndex].info.progress;
      zoomMulti = zoomMulti / 16.0f + 1.0f;
      alpha = MapPoolDisp[poolDispIndex].info.progress * 16;
    } else
      zoomMulti = 1.0f;
  } else {
    zoomMulti = MapPoolDisp[poolDispIndex].info.progress - 16.0f;
    zoomMulti = zoomMulti / 16.0f + 1.5f;
    alpha = MapPoolDisp[poolDispIndex].info.progress * -16 + 256;
  }
  float scaledFactor = 1080.0f / mapSizeY;
  zoomMulti *= scaledFactor;
  float xMapEdge = mapPosX + mapSizeX;
  float yMapEdge = mapPosY + mapSizeY;

  float xOffset = MapPoolPosOffsets[poolId][0] - 16.0f;
  float yOffset = MapPoolPosOffsets[poolId][1] - 29.0f;

  if (mapPosX <= xOffset + displayedSprite.Bounds.Width &&
      mapPosY <= yOffset + displayedSprite.Bounds.Height &&
      xOffset <= xMapEdge && yOffset <= yMapEdge) {
    xOffset = xOffset - mapPosX;
    yOffset = yOffset - mapPosY;

    float scaledPosOffsetX = (xOffset + 16.0f) * scaledFactor;
    float scaledPosOffsetY = (yOffset + 29.0f) * scaledFactor;

    glm::vec4 color = RgbIntToFloat(Tints[poolId]);
    color[3] = alpha / 256.0f;

    Renderer->DrawSpriteCentered(
        displayedSprite,
        glm::vec2(scaledPosOffsetX - 16.0f, scaledPosOffsetY - 29.0f),
        glm::vec2{16.0f, 29.0f}, color, glm::vec2{zoomMulti, zoomMulti}, 0);
  }
}

void MapDispArticle(int id) {
  int partId = MapPartsDisp[id].partId;

  int hoverAnimState = 0;
  int alpha = 256;
  float shadowZoom, zoomMulti;
  if (MapPartsDisp[id].shown) {
    hoverAnimState = MapPartsDisp[id].animState;
    shadowZoom = 1;
    zoomMulti = 1;
    if (!(MapPartsDisp[id].inOrOut || MapPoolDisp[id].animState)) {
      zoomMulti = 16 - MapPartsDisp[id].progress;
      shadowZoom = (zoomMulti) / 24.0 + 1.0;
      zoomMulti = zoomMulti / 16.0 + 1.0;
      alpha = MapPartsDisp[id].progress * 16;
    }
  } else {
    zoomMulti = MapPartsDisp[id].progress - 16;
    shadowZoom = (zoomMulti) / 24.0 + 1.666;
    zoomMulti = zoomMulti / 16.0 + 1.5;
    alpha = MapPartsDisp[id].progress * -16 + 256;
  }

  float xPartOffset = PartsOffsetData[partId][2][0] - 166;
  float yPartOffset = PartsOffsetData[partId][2][1] - 16;

  if (mapPosX <= xPartOffset + 354.0 && mapPosY <= yPartOffset + 247.0) {
    float scaledFactor = 1080.0f / mapSizeY;
    float angle = MapPartsDisp[id].angle * 2 * M_PI / 65536.0;

    xPartOffset = xPartOffset - mapPosX;
    yPartOffset = yPartOffset - mapPosY;

    float displayShadowPhotoX = (xPartOffset + 166 + 3) * scaledFactor - 166;
    float displayShadowPhotoY = (yPartOffset + 16 + 3) * scaledFactor - 16;

    float scaledPosOffsetX = (xPartOffset + 166) * scaledFactor;
    float scaledPosOffsetY = (yPartOffset + 16) * scaledFactor;
    float displayPhotoX = scaledPosOffsetX - 166;
    float displayPhotoY = scaledPosOffsetY - 16;

    // Shadow
    Renderer->DrawSpriteCentered(
        MapPartsPhotoSprites[partId],
        glm::vec2(displayShadowPhotoX, displayShadowPhotoY), glm::vec2{166, 16},
        glm::vec4{0.0f, 0.0f, 0.0f, (alpha >> 1) / 256.0f},
        glm::vec2{shadowZoom * scaledFactor, shadowZoom * scaledFactor * 0.5},
        angle);
    // Photo
    Renderer->DrawSpriteCentered(MapPartsPhotoSprites[partId],
                                 glm::vec2(displayPhotoX, displayPhotoY),
                                 glm::vec2{166, 16}, glm::vec4{1.0f},
                                 glm::vec2{zoomMulti * 0.5 * scaledFactor,
                                           zoomMulti * 0.5 * scaledFactor},
                                 angle);
  }
}

void MapSystemCCLCC::MapSetPos() {
  float fVar6 = ScrWork[6362];
  float fVar7 = 100.0;
  if (fVar6 >= 100.0) fVar7 = fVar6;
  fVar6 = 800.0;
  if (fVar7 <= 800.0) fVar6 = fVar7;
  fVar7 = MapBgSprite.Sheet.DesignWidth;
  float fVar8 = MapBgSprite.Sheet.DesignHeight;

  mapSizeX = fVar7 * 100.0 / fVar6;
  mapSizeY = fVar8 * 100.0 / fVar6;

  mapPosX = ScrWork[0x18db] - mapSizeX / 2;
  mapPosY = ScrWork[0x18dc] - mapSizeY / 2;

  if (!GetFlag(2805)) {
    if (fVar7 < mapPosX + mapSizeX) {
      mapPosX = fVar7 - mapSizeX;
    }
    if (fVar8 < mapPosY + mapSizeY) {
      mapPosY = fVar8 - mapSizeY;
    }
    if (mapPosX < 0.0) {
      mapPosX = 0.0;
    }
    if (mapPosY < 0.0) {
      mapPosY = 0.0;
    }
  }

  // MapScaleMode use??
  MapFadeMain();

  for (int i = 0; i < MapPoolCurCt.size(); ++i) {
    if (i == hoverMapPoolIdx) {
      MapPoolCurCt[i] += (MapPoolCurCt[i] >= 16) ? 0 : 2;
    } else if (MapPoolCurCt[i]) {
      MapPoolCurCt[i] -= 2;
    }
  }
}

void MapSystemCCLCC::MapFadeMain() {
  for (int i = 0; i < MapPoolDisp.size(); ++i) {
    auto& poolDispElem = MapPoolDisp[i];
    if (poolDispElem.info.inOrOut != poolDispElem.info.shown) {
      // MapFadeFl = 1;
      if ((!GetFlag(SF_MESALLSKIP))) {
        if (poolDispElem.animState == 0 || --poolDispElem.animState == 0) {
          if (!poolDispElem.info.shown) {
            if (poolDispElem.info.inOrOut) {
              if (i % 2 == 1 && poolDispElem.info.progress == 16) {
                Io::InputStream* stream;
                int64_t err = Io::VfsOpen("sysse", 7, &stream);
                if (err == IoError_OK) {
                  Audio::Channels[Audio::AC_SSE].Play(
                      Audio::AudioStream::Create(stream), false, 0.0f);
                }
              }
              if (poolDispElem.info.progress == 0 ||
                  --poolDispElem.info.progress == 0) {
                poolDispElem.info.shown = 1;
              }
            }
          } else {
            if (i % 2 == 1 && poolDispElem.info.progress == 16) {
              Io::InputStream* stream;
              int64_t err = Io::VfsOpen("sysse", 9, &stream);
              if (err == IoError_OK) {
                Audio::Channels[Audio::AC_SSE].Play(
                    Audio::AudioStream::Create(stream), false, 0.0f);
              }
            }
            if (poolDispElem.info.progress == 0 ||
                --poolDispElem.info.progress == 0) {
              poolDispElem.info.shown = 0;
            }
          }
        }
      } else {
        poolDispElem.info.shown = poolDispElem.info.inOrOut;
        poolDispElem.info.progress = 0;
        poolDispElem.animState = 0;
      }
    }
  }
}

void MapSystemCCLCC::Update(float dt) {
  if (ScrWork[6360] && GetFlag(2800)) {
    MapSetPos();
  }

  if (ScrWork[0x1964] == 0) {
    if (ScrWork[0x1965] == 0) {
      ScrWork[0x1966] = 0;
    } else {
      ScrWork[0x1965]--;
    }
  } else {
    ScrWork[0x1966] = ScrWork[0x1964];
    if (ScrWork[0x1965] < 0x20) {
      ScrWork[0x1965]++;
    }
  }
}

void MapSystemCCLCC::RenderButtonGuide() {
  if (!GetFlag(1244) && !GetFlag(2487)) {
    if (ScrWork[6501] != 0) {
      float guideXWidth = (ScrWork[6501] * 1920) / 32;
      MapButtonGuideSprite.Bounds.Width = guideXWidth;
      Renderer->DrawSprite(MapButtonGuideSprite, glm::vec2{0.0f, 988.0f});
      if (guideXWidth < 1920) {
        Sprite MapButtonGuideSprite2 = MapButtonGuideSprite;
        std::array<glm::vec4, 4> tints = {
            glm::vec4{1.0f, 1.0f, 1.0f, 0.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
            glm::vec4{1.0f, 1.0f, 1.0f, 0.0f},
        };
        MapButtonGuideSprite2.Bounds.X = guideXWidth;
        MapButtonGuideSprite2.Bounds.Width = 60;

        std::array<glm::vec2, 4> dest = {
            glm::vec2{guideXWidth, 1042.0f},       // bottom left
            glm::vec2{guideXWidth + 60, 1042.0f},  // bottom right
            glm::vec2{guideXWidth, 988.0f},        // top left
            glm::vec2{guideXWidth + 60, 988.0f},   // top right
        };
        Renderer->DrawSprite(MapButtonGuideSprite2, dest, tints, 0, false);
      }
    }
  }
}  // namespace CCLCC

void MapSystemCCLCC::Render() {
  if (GetFlag(2800)) {
    // Render map bg
    MapBgSprite.Bounds = Rect(mapPosX, mapPosY, mapSizeX, mapSizeY);
    Renderer->DrawSprite(MapBgSprite, Rect(0, 0, 1920, 1080), glm::vec4{1.0f},
                         0.0f);

    // Render map parts
    for (int i = 0; i < MapPartsMax; ++i) {
      if (MapPartsDisp[i].shown ||
          MapPartsDisp[i].inOrOut && MapPartsDisp[i].animState == 0) {
        switch (MapPartsDisp[i].type) {
          case 4:
            MapDispArticle(i);
            break;
          default:
            break;
        }
      }
    }

    // Render map pool
    for (int i = 0; i < 20; ++i) {
      if (MapPoolDisp[i * 2].info.shown ||
          MapPoolDisp[i * 2].info.inOrOut &&
              MapPoolDisp[i * 2].animState == 0) {
        MapPoolDispPhoto(i);
      }
      if (MapPoolDisp[i * 2 + 1].info.shown ||
          MapPoolDisp[i * 2 + 1].info.inOrOut &&
              MapPoolDisp[i * 2 + 1].animState == 0) {
        MapPoolDispPin(i);
      }
    }
  }
}
}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto