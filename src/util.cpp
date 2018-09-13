#include "util.h"

namespace Impacto {
char* DumpMat4(glm::mat4* matrix, const char* columnSeparator,
               const char* rowSeparator) {
  size_t reqSz =
      snprintf(
          NULL, 0,
          "%s%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f%s%s%"
          ".03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f",
          columnSeparator, (*matrix)[0][0], columnSeparator, (*matrix)[1][0],
          columnSeparator, (*matrix)[2][0], columnSeparator, (*matrix)[3][0],
          rowSeparator, columnSeparator, (*matrix)[0][1], columnSeparator,
          (*matrix)[1][1], columnSeparator, (*matrix)[2][1], columnSeparator,
          (*matrix)[3][1], rowSeparator, columnSeparator, (*matrix)[0][2],
          columnSeparator, (*matrix)[1][2], columnSeparator, (*matrix)[2][2],
          columnSeparator, (*matrix)[3][2], rowSeparator, columnSeparator,
          (*matrix)[0][3], columnSeparator, (*matrix)[1][3], columnSeparator,
          (*matrix)[2][3], columnSeparator, (*matrix)[3][3]) +
      1;
  char* result = (char*)malloc(reqSz);
  sprintf(result,
          "%s%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f%s%s%"
          ".03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f",
          columnSeparator, (*matrix)[0][0], columnSeparator, (*matrix)[1][0],
          columnSeparator, (*matrix)[2][0], columnSeparator, (*matrix)[3][0],
          rowSeparator, columnSeparator, (*matrix)[0][1], columnSeparator,
          (*matrix)[1][1], columnSeparator, (*matrix)[2][1], columnSeparator,
          (*matrix)[3][1], rowSeparator, columnSeparator, (*matrix)[0][2],
          columnSeparator, (*matrix)[1][2], columnSeparator, (*matrix)[2][2],
          columnSeparator, (*matrix)[3][2], rowSeparator, columnSeparator,
          (*matrix)[0][3], columnSeparator, (*matrix)[1][3], columnSeparator,
          (*matrix)[2][3], columnSeparator, (*matrix)[3][3]);
  return result;
}
}  // namespace Impacto