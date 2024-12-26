param(
    [ValidateSet("x64", "x86")][string]$Arch = "x64"
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

function InstallPackages() {
    $vcpkg = "$env:VCPKG_ROOT/vcpkg.exe"
    if (!(Get-Command $vcpkg -ErrorAction SilentlyContinue)) {
        $vcpkg = "vcpkg"
    }

    $local_vcpkg = $false
    if(!(Get-Command $vcpkg -ErrorAction SilentlyContinue)) {
        if (!(Test-Path build/vcpkg)) {
            echo "Cloning vcpkg..."
            mkdir build -Force | Out-Null
            pushd build
            & git clone https://github.com/Microsoft/vcpkg.git --depth 1
            pushd vcpkg
            ./bootstrap-vcpkg -disableMetrics
            popd
            popd
            $vcpkg = "build/vcpkg/vcpkg.exe"
        }
        $local_vcpkg = $true
    }

    & $vcpkg integrate install
    if ($local_vcpkg) {
        Write-Output "Cleaning up..."
        Remove-Item build/vcpkg/downloads -Recurse
        Remove-Item build/vcpkg/buildtrees -Recurse
    }
}

InstallPackages