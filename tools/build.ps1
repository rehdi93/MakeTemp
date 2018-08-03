param(
# Build mode
[ValidateSet("Debug", "Release", "MinSizeRel", "RelWithDebInfo")]
[string]
$Mode
)

$sep = [string]::new('-', 35)
$projRoot = Resolve-Path "$PSScriptRoot/.."
$buildDir = Join-Path $projRoot "build"

if (!(Test-Path $buildDir)) { mkdir $buildDir }

Push-Location $buildDir
conan install .. -s build_type=$Mode
echo $sep
cmake --build . --config $Mode
echo $sep
Pop-Location
