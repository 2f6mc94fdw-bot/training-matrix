param(
    [Parameter(Mandatory = $true)]
    [string]$QtRoot,

    [string]$SourceDir = (Join-Path $PSScriptRoot "..\.."),
    [string]$OutputDir = (Join-Path $PSScriptRoot "output"),

    [switch]$AllowDirtySource
)

$ErrorActionPreference = "Stop"
$SourceDir = (Resolve-Path $SourceDir).Path
$OutputDir = [System.IO.Path]::GetFullPath($OutputDir)

$cmake = Get-Command cmake -ErrorAction Stop
$windeployqt = Join-Path $QtRoot "bin\windeployqt.exe"
$schemaSource = Join-Path $SourceDir "resources\database\schema.sql"
$migrationSource = Join-Path $PSScriptRoot "PRODUCTION_SCHEMA_MIGRATION.sql"
$setupSource = Join-Path $PSScriptRoot "PRODUCTION_SQL_SETUP.sql"
$validationSource = Join-Path $PSScriptRoot "VALIDATE_DATABASE.sql"
$readmeSource = Join-Path $PSScriptRoot "WINDOWS_DEPLOYMENT_README.md"
$checklistSource = Join-Path $PSScriptRoot "WINDOWS_PILOT_CHECKLIST.md"

if (-not (Test-Path (Join-Path $SourceDir "CMakeLists.txt"))) {
    throw "Aptitude source was not found at: $SourceDir"
}

if (-not (Test-Path $windeployqt)) {
    throw "windeployqt.exe was not found under QtRoot: $QtRoot"
}

foreach ($requiredFile in @(
    $schemaSource,
    $migrationSource,
    $setupSource,
    $validationSource,
    $readmeSource,
    $checklistSource
)) {
    if (-not (Test-Path $requiredFile)) {
        throw "Required deployment file was not found: $requiredFile"
    }
}

$projectDeclaration = Select-String -Path (Join-Path $SourceDir "CMakeLists.txt") `
    -Pattern 'project\(Aptitude VERSION ([0-9]+\.[0-9]+\.[0-9]+)' |
    Select-Object -First 1
if (-not $projectDeclaration) {
    throw "Unable to determine the Aptitude version from CMakeLists.txt"
}
$sourceVersion = $projectDeclaration.Matches[0].Groups[1].Value

$git = Get-Command git -ErrorAction SilentlyContinue
$sourceCommit = "unavailable"
$sourceDirty = $true
if ($git) {
    $sourceCommit = (& $git.Source -C $SourceDir rev-parse HEAD 2>$null)
    if ($LASTEXITCODE -eq 0) {
        $dirtyLines = @(& $git.Source -C $SourceDir status --porcelain -- .)
        $sourceDirty = $dirtyLines.Count -gt 0
    }
}

if ($sourceDirty -and -not $AllowDirtySource) {
    throw "The source tree has uncommitted changes. Commit/tag the release source, or use -AllowDirtySource only for a development build."
}

$buildDir = Join-Path $OutputDir "build"
$packageDir = Join-Path $OutputDir "Aptitude-Windows-x64"

if (Test-Path $buildDir) {
    Remove-Item $buildDir -Recurse -Force
}
if (Test-Path $packageDir) {
    Remove-Item $packageDir -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
New-Item -ItemType Directory -Force -Path $packageDir | Out-Null

Write-Host "Configuring Aptitude..."
& $cmake.Source `
    -S $SourceDir `
    -B $buildDir `
    -G "Visual Studio 17 2022" `
    -A x64 `
    -DCMAKE_PREFIX_PATH=$QtRoot `
    -DBUILD_TESTS=OFF `
    -DAPTITUDE_BUILD_INTEGRATION_TESTS=ON

if ($LASTEXITCODE -ne 0) {
    throw "CMake configuration failed with exit code $LASTEXITCODE"
}

Write-Host "Building Release configuration..."
& $cmake.Source --build $buildDir --config Release --parallel

if ($LASTEXITCODE -ne 0) {
    throw "Release build failed with exit code $LASTEXITCODE"
}

$exe = Join-Path $buildDir "Release\Aptitude.exe"
if (-not (Test-Path $exe)) {
    throw "Build completed without producing Aptitude.exe at: $exe"
}

Copy-Item $exe $packageDir

Write-Host "Deploying Qt runtime..."
& $windeployqt `
    --release `
    --compiler-runtime `
    --no-translations `
    --dir $packageDir `
    (Join-Path $packageDir "Aptitude.exe")

if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}

foreach ($runtimeFile in @(
    (Join-Path $packageDir "sqldrivers\qsqlodbc.dll"),
    (Join-Path $packageDir "platforms\qwindows.dll"),
    (Join-Path $packageDir "Qt6Charts.dll")
)) {
    if (-not (Test-Path $runtimeFile)) {
        throw "Required Windows runtime file is missing after deployment: $runtimeFile"
    }
}

$sqlDir = Join-Path $packageDir "database"
New-Item -ItemType Directory -Force -Path $sqlDir | Out-Null
Copy-Item $schemaSource (Join-Path $sqlDir "schema.sql")
Copy-Item $migrationSource $sqlDir
Copy-Item $setupSource $sqlDir
Copy-Item $validationSource $sqlDir
Copy-Item $readmeSource $packageDir
Copy-Item $checklistSource $packageDir

$buildTimestamp = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
$buildInfo = @(
    "Product=Aptitude",
    "Version=$sourceVersion",
    "BuiltUtc=$buildTimestamp",
    "SourceCommit=$sourceCommit",
    "SourceDirty=$sourceDirty",
    "Architecture=x64",
    "Toolchain=Visual Studio 2022 / MSVC",
    "QtRoot=$QtRoot"
)
Set-Content -Path (Join-Path $packageDir "BUILD_INFO.txt") -Value $buildInfo -Encoding UTF8

$hashLines = Get-ChildItem -Path $packageDir -File -Recurse |
    Where-Object { $_.Name -ne "SHA256SUMS.txt" } |
    Sort-Object FullName |
    ForEach-Object {
        $relativePath = $_.FullName.Substring($packageDir.Length + 1)
        $hash = (Get-FileHash -Algorithm SHA256 -Path $_.FullName).Hash.ToLowerInvariant()
        "$hash  $relativePath"
    }
Set-Content -Path (Join-Path $packageDir "SHA256SUMS.txt") -Value $hashLines -Encoding UTF8

$zipPath = "$packageDir.zip"
if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}
Compress-Archive -Path (Join-Path $packageDir "*") -DestinationPath $zipPath

Write-Host ""
Write-Host "Windows package created successfully:"
Write-Host $zipPath
Write-Host ""
Write-Host "A separate installation of Microsoft ODBC Driver 18 for SQL Server is required on each client PC."
