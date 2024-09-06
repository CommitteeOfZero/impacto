param(
    [ValidateSet("x64", "x86")][string]$Arch = "x64",
    [string]$VcpkgDir = $env:VCPKG_INSTALLATION_ROOT
)

echo "Hello World!"