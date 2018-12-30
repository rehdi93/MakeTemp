param(
[ValidateSet("Debug", "Release", "RelWithDebInfo")]
[Parameter(Mandatory=$true)]
[string] $Mode,
[string] $Target = 'all',
[string] $generator = "NMake Makefiles"
)

$SEP = [string]::new('-', 35)

$projRoot = Resolve-Path "$PSScriptRoot/.."
$buildDir = Join-Path $projRoot "build"

if (!(Test-Path "$buildDir")) {
    mkdir "$buildDir"
}

Push-Location $buildDir

# run conan
conan install .. -s build_type="$Mode"
echo "conan returned $LASTEXITCODE"
echo $SEP

# run cmake gen
cmake .. -G "$generator" -DCMAKE_BUILD_TYPE="$Mode"
echo "cmake gen returned $LASTEXITCODE"
echo $SEP

if ($generator -eq "NMake Makefiles") {
    & "$PSScriptRoot/fix-nmake-compile-comands.ps1"
}

# run cmake build
cmake --build . --config $Mode --target $Target
echo "cmake build returned $LASTEXITCODE"
echo $SEP

Pop-Location
