#pragma once

#include "subtitle.h"

namespace Impacto::Profile::Patch {
enum class DateFormatType : uint8_t {
  DMY,
  MDY,
  YMD,
};

struct DateFormatDef {
  DateFormatDef(DateFormatType sel) : Sel(sel) {}
  std::string_view FormattedString() const {
    switch (Sel) {
      case DateFormatType::DMY:
        return "{:%d/%m/%y}";
      case DateFormatType::MDY:
        return "{:%m/%d/%y}";
      case DateFormatType::YMD:
      default:
        return "{:%y/%m/%d}";
    }
  }
  DateFormatType Sel;
};

inline char const* Language;
inline DateFormatDef DateFormat = DateFormatType::YMD;
inline bool HasScriptedExitLogic = false;
inline bool HasDelusionMouseSupport = false;

void Configure();

}  // namespace Impacto::Profile::Patch