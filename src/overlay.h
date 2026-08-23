

namespace Impacto::Overlay {
enum class OverlayTab : int8_t {
  Settings,
  Enhancements,
  Achievements,
  About,
};

inline bool OverlayShown = false;
inline std::optional<OverlayTab> RequestedTab = OverlayTab::Settings;

void Init();
void Show();
}  // namespace Impacto::Overlay