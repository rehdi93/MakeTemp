param(
# Build mode
[ValidateSet("Debug", "Release", "MinSizeRel", "RelWithDebInfo")]
[string]
$Mode
)

$sep = [string]::new('-', 35)
$projRoot = Resolve-Path "$PSScriptRoot/.."
$buildDir = Resolve-Path (Join-Path $projRoot "build")

Push-Location $buildDir
conan install .. -s build_type=$Mode
echo $sep
cmake --build . --config $Mode
echo $sep
Pop-Location
