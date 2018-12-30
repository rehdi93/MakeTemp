$file = "compile_commands.json"

if (!(test-path $file)) {
    "$file not found!"
    return
}

$json = [regex]::Replace((cat $file), "@<<\s\s|\s<<", '', [Text.RegularExpressions.RegexOptions]::Multiline)

# can't use out-file here cause the file need to be UTF8 w/o BOM
[IO.File]::WriteAllText((Convert-Path $file), $json)