#pragma once

#include "../backlogentry.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CC {

class BacklogEntry : public Widgets::BacklogEntry {
 public:
  using Widgets::BacklogEntry::BacklogEntry;

  void Render() override;
};

}  // namespace CC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto