Get-ChildItem -Path . -Include *.cpp,*.h -Recurse |
Where-Object { -not $_.FullName.Contains("\dependencies\") } |
ForEach-Object -Parallel { clang-format-20 -i $_.FullName }