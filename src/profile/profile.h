#pragma once

#include <string>
#include "../io/inputstream.h"

namespace Impacto {
namespace Profile {

void ReadLuaFile(Io::InputStream* stream, std::string const& name, bool needsEnum);
void MakeLuaProfile(std::string const& name);
void ClearProfile();

}  // namespace Profile
}  // namespace Impacto