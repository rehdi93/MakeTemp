param(
# Build mode
[ValidateSet("Debug", "Release")]
[Parameter(Mandatory=$true)]
[string]
$Mode
)

$SEP = [string]::new('-', 35)
$PLATFORM = [System.Environment]::OSVersion.Platform

$projRoot = Resolve-Path "$PSScriptRoot/.."
$buildDir = Join-Path $projRoot "build"
$generator = "Visual Studio 15 2017 Win64"
#$generator = "NMake Makefiles"

if (!(Test-Path "$buildDir")) {
    mkdir "$buildDir"
}

Push-Location $buildDir

# run conan
conan install .. -s build_type="$Mode"
Write-Output "conan returned $LASTEXITCODE" $SEP

# run cmake gen
cmake .. -G "$generator" -DCMAKE_BUILD_TYPE="$Mode"
Write-Output "cmake returned $LASTEXITCODE" $SEP

if ($generator -eq "NMake Makefiles") {
    # fix nmake's compile_commands
    $file = "compile_commands.json"
    $json = gc $file
    $json = [regex]::Replace($json, "@<<\s\s|\s<<", '', [Text.RegularExpressions.RegexOptions]::Multiline)
    
    # can't use out-file here cause the file need to be UTF8 w/o BOM
    [IO.File]::WriteAllLines((Convert-Path $file), $json)
}

# run cmake build
cmake --build . --config $Mode
Write-Output "cmake returned $LASTEXITCODE" $SEP

Pop-Location
