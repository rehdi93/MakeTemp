param(
# Build mode
[ValidateSet("Debug", "Release")]
[string]
$Mode
)

$SEP = [string]::new('-', 35)
$PLATFORM = [System.Environment]::OSVersion.Platform

$toolsDir = $PSScriptRoot
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
echo "conan returned $LASTEXITCODE" $SEP

# genarate CMakeCache
if ($generate) {
    $gen = '-G "{0}"'

    switch ($PLATFORM) {
        "Win32NT" { $gen = [string]::Format($gen, "Visual Studio 15 2017 Win64") }
        "Unix" { $gen = [string]::Format($gen, "Unix Makefiles") <# + " -DCMAKE_BUILD_TYPE=$Mode" #> }
        Default { $gen = '' }
    }

    $cmd = "cmake .. $gen"
    Invoke-Expression $cmd
    echo "cmake returned $LASTEXITCODE" $SEP
}

# run cmake build
# ? does '--config' make a diference on Unix Makefiles ?
cmake --build . --config $Mode
echo "cmake returned $LASTEXITCODE" $SEP

Pop-Location
