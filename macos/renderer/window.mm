#import <Cocoa/Cocoa.h>

namespace Impacto::MacOS {
void HideSystemUI(bool hide) {
  NSApplicationPresentationOptions options =
      hide ? (NSApplicationPresentationHideDock | NSApplicationPresentationHideMenuBar)
           : NSApplicationPresentationDefault;
  [[NSApplication sharedApplication] setPresentationOptions:options];
}
}  // namespace Impacto::MacOS