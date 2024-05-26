param(
    [ValidateSet("x64", "x86")][string]$Arch = "x64",
    [string]$VcpkgDir = $env:VCPKG_INSTALLATION_ROOT
)

function SetEnv() {
    $vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
    $ip = & $vswhere -prerelease -latest -property InstallationPath
    if ($ip -and (test-path "$ip\Common7\Tools\vsdevcmd.bat")) {
        & "${env:COMSPEC}" /s /c "`"$ip\Common7\Tools\vsdevcmd.bat`" -arch=x64 -no_logo && set" | foreach-object {
            $name, $value = $_ -split '=', 2
            set-content env:\"$name" $value
        }
    }
}

function BuildLibatrac9() {
    if (!(Get-Command msbuild -ErrorAction SilentlyContinue)) {
        SetEnv
    }
    
    pushd "vendor"
    $repo_root = "LibAtrac9"
    if (!(Test-Path $repo_root)) {
        & git clone https://github.com/Thealexbarney/LibAtrac9.git --depth 1
    }
    
    pushd $repo_root
    pushd "C"
    & devenv /Upgrade libatrac9.sln
    $args = @(
        "/m",
        "/p:Configuration=Release",
        "/p:Platform=$Arch",
        "/p:WindowsTargetPlatformVersion=10.0",
        "/p:PlatformToolset=v142"
    )
    & msbuild $args
    popd
    
    $includedir = "include/libatrac9"
    mkdir $includedir -Force | Out-Null
    Get-ChildItem -Path "./C/src/*" -Include *.h | Copy-Item -Destination $includedir
    mkdir "bin/x86" -Force | Out-Null
    mkdir "bin/x64" -Force | Out-Null
    if(Test-Path "./C/Release") {
        Get-ChildItem -Path "./C/Release/*" -Include *.dll,*.lib | Copy-Item -Destination "bin/x86"
    }
    if(Test-Path "./C/x64/Release") {
        Get-ChildItem -Path "./C/x64/Release/*" -Include *.dll,*.lib | Copy-Item -Destination "bin/x64"
    }
    popd
    popd
}

function InstallPackages() {
    $vcpkg = "$VcpkgDir/vcpkg.exe"
    if (!(Get-Command $vcpkg -ErrorAction SilentlyContinue)) {
        $vcpkg = "vcpkg"
    }
    # Visual studio's vcpkg doesn't support classic mode
    if(("${env:VSINSTALLDIR}VC\vcpkg" -eq "${env:VCPKG_ROOT}") -or !(Get-Command $vcpkg -ErrorAction SilentlyContinue)) {
        if (!(Test-Path build/vcpkg)) {
            mkdir build -Force | Out-Null
            pushd build
            & git clone https://github.com/Microsoft/vcpkg.git --depth 1
            pushd vcpkg
            ./bootstrap-vcpkg -disableMetrics
            popd
            popd
        }
        $vcpkg = "build/vcpkg/vcpkg.exe"
        $local_vcpkg = $true
    }
    

    & $vcpkg install sdl2 sdl2[vulkan] vulkan openal-soft libogg libvorbis zlib glm ffmpeg libwebp --triplet $Arch-windows
    if ($local_vcpkg) {
        & $vcpkg integrate install
        Write-Output "Cleaning up..."
        Remove-Item build/vcpkg/downloads -Recurse
        Remove-Item build/vcpkg/buildtrees -Recurse
    }
}

BuildLibatrac9
InstallPackages
