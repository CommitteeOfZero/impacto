#pragma once

#include "stream.h"
#include "../util.h"

namespace Impacto {
namespace Io {

class AssetPath {
 public:
  std::string Mount = "";
  std::string FileName = "";
  uint32_t Id = 0;

  IoError Open(Stream** outStream);
  IoError Slurp(void*& outMemory, int64_t& outSize);
};

// structs below only used for SpriteSheets from System archive
struct AssetPathKey {
  uint32_t MountId;
  uint32_t Id;

  static AssetPath KeyToAssetPath(AssetPathKey const& val) {
    std::string mount = "";
    if (val.MountId == 2) {
      mount = "system";
    }
    return AssetPath{.Mount = mount, .Id = val.Id};
  }
};

struct AssetPathHash {
  using is_transparent = void;

  static uint32_t MountToId(std::string_view mount) {
    if (mount == "system") return 2;
    assert(false);
    return 0;
  }

  static uint64_t Compute(uint32_t mountId, uint32_t id) {
    uint64_t h1 = ankerl::unordered_dense::hash<uint32_t>{}(mountId);
    uint64_t h2 = ankerl::unordered_dense::hash<uint32_t>{}(id);
    std::size_t seed = 0;
    HashCombine(seed, h1, h2);
    return static_cast<uint64_t>(seed);
  }

  uint64_t operator()(const AssetPath& p) const {
    return Compute(MountToId(p.Mount), p.Id);
  }

  uint64_t operator()(const AssetPathKey& k) const {
    return Compute(k.MountId, k.Id);
  }
};

struct AssetPathEqual {
  using is_transparent = void;

  bool operator()(const AssetPath& a, const AssetPath& b) const {
    return a.Mount == b.Mount && a.Id == b.Id;
  }

  bool operator()(const AssetPath& a, const AssetPathKey& b) const {
    return AssetPathHash::MountToId(a.Mount) == b.MountId && a.Id == b.Id;
  }

  bool operator()(const AssetPathKey& a, const AssetPath& b) const {
    return (*this)(b, a);
  }
};

}  // namespace Io
}  // namespace Impacto