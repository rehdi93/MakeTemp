param(
# Build mode
[ValidateSet("Debug", "Release")]
[string]
$Mode
)

$SEP = [string]::new('-', 35)
$PLATFORM = [System.Environment]::OSVersion.Platform

$projRoot = Resolve-Path "$PSScriptRoot/.."
$buildDir = Join-Path $projRoot "build"

$generate = $false
if (!(Test-Path $buildDir)) { 
    mkdir $buildDir
    $generate = $true
} elseif (!(Test-Path "$buildDir/CMakeCache.txt")) {
    $generate = $true
}

Push-Location $buildDir

# run conan
conan install .. -s build_type=$Mode
Write-Output "conan returned $LASTEXITCODE" $SEP

switch ($PLATFORM) {
    "Win32NT" { 
        if ($generate) {
            cmake .. -G "Visual Studio 15 2017 Win64"
        }
     }
     "Unix" { cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$Mode }
}
Write-Output "cmake returned $LASTEXITCODE" $SEP


# run cmake build
cmake --build . --config $Mode
Write-Output "cmake returned $LASTEXITCODE" $SEP

Pop-Location
