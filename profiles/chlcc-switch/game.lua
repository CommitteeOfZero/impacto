include(root.BasePaths.RootProfilesDir .. '/chlcc/game.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/scriptvars.lua');

root.WindowName = "CHAOS;HEAD Love Chu☆Chu! (Switch)";
root.DesignWidth = 1920;
root.DesignHeight = 1080;
root.CharaIsMvl = true;
root.LayFileBigEndian = false;
root.LayFileTexXMultiplier = 1;
root.LayFileTexYMultiplier = 1;
root.PlatformId = 0x100000;

root.Vm.StartScript = 1;
root.Vm.GameInstructionSet = InstructionSet.LCCSwitch;
root.Vm.UseMsbStrings = true;
root.Vm.UseSeparateMsbArchive = true;
root.Vm.UseReturnIds = true;
root.Vm.StringEncodingType = StringUnitEncoding.Uint32;
root.Vm.StringIdSize = 4;
root.Vm.ScrWorkChaStructSize = 24;
root.Vm.ScrWorkChaOffsetStructSize = 10;
root.Vm.ScrWorkBgStructSize = 24;
root.Vm.ScrWorkBgOffsetStructSize = 10;
root.Vm.ScrWorkCaptureStructSize = 20;
root.Vm.ScrWorkCaptureOffsetStructSize = 10;
root.Vm.ScrWorkCaptureEffectInfoStructSize = 3;
root.Vm.ScrWorkBgEffStructSize = 30;
root.Vm.ScrWorkBgEffOffsetStructSize = 18;
root.Vm.ScrWorkMesStructSize = 7;
root.Vm.MaxLinkedBgBuffers = 2;

include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/font.lua');
include(root.BasePaths.RootProfilesDir .. '/chlcc-switch/hud/saveicon.lua');
