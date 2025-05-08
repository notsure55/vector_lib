# if build directory doesnt exist
if (-not (Test-Path Build))
{
    New-Item -ItemType Directory -Path Build | Out-Null
}

Set-Location Build

if (-not (Test-Path CMakeCache.txt))
{
    # build cmake files
    cmake -G "Visual Studio 17 2022" -A x64 ..
}

# build project
cmake --build . --config Release

# execute binary, can comment this out if you want 
.\Release\vector_test.exe
