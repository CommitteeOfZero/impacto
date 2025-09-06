#pragma once

#include "../backlogentry.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class BacklogEntry : public Widgets::BacklogEntry {
 public:
  using Widgets::BacklogEntry::BacklogEntry;

  void Render() override;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto