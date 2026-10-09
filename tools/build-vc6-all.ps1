# Build selected original VC6 projects in a single offline Windows VM session.
# Run only in an isolated VM with a licensed VC6 installation; no runtime starts.
[CmdletBinding()]
param(
    [ValidateSet("Release", "Debug")]
    [string]$Configuration = "Release",
    [string]$Msdev = "msdev.exe",
    [string]$OutputDir = "",
    [switch]$DryRun
)
$ErrorActionPreference = "Stop"
$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
if (-not $OutputDir) { $OutputDir = Join-Path $root "build\vc6" }
$targets = @(
    @{Name="LuaLib"; Path="Base\LuaLib\LuaLib.dsp" },
    @{Name="Common"; Path="Base\Common\Common.dsp" },
    @{Name="Core_lib"; Path="Client\Core\Core_lib.dsp" },
    @{Name="Engine"; Path="Client\Engine\Engine.dsp" },
    @{Name="Represent2"; Path="Client\Represent\Represent2\Represent2.dsp" },
    @{Name="Faith"; Path="Client\Faith\Faith.dsp" },
    @{Name="lord"; Path="Server\lord\lord.dsp" }
)
if ($DryRun) {
    Write-Host "Dry run — will attempt the following VC6 projects:"
    foreach ($target in $targets) { Write-Host ("  {0} - {1}" -f $target.Name, $Configuration) }
    return
}
$tool = Get-Command $Msdev -ErrorAction SilentlyContinue
if (-not $tool) {
    throw "msdev.exe not found. Install Visual C++ 6.0 in an isolated Windows x86 VM and pass -Msdev 'C:\Path\MSDEV.EXE'."
}
New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
$summary = @()
foreach ($target in $targets) {
    $project = Join-Path $root $target.Path
    $log = Join-Path $OutputDir ("{0}-{1}.log" -f $target.Name, $Configuration)
    $entry = [ordered]@{project=$target.Name; config=$Configuration; exit_code=-1; log=$log; log_found=$false; compiler_errors=$false}
    if (-not (Test-Path $project)) {
        "Missing project file: $project" | Set-Content -Path $log
        $summary += [pscustomobject]$entry
        continue
    }
    Write-Host ("Building {0} / {1} ..." -f $target.Name, $Configuration)
    $cfg = "{0} - Win32 {1}" -f $target.Name, $Configuration
    try {
        # Microsoft Developer Studio 6.0 command line, no game binaries executed.
        & $tool.Source $project /MAKE $cfg /REBUILD /OUT $log
        $entry.exit_code = $LASTEXITCODE
        $entry.log_found = Test-Path $log
        if ($entry.log_found) {
            $contents = Get-Content $log -Raw
            $entry.compiler_errors = [bool]($contents -match '(?im)(fatal error|error C[0-9]{4}|error LNK[0-9]{4}|[1-9][0-9]* error\(s\))')
        }
    } catch {
        $entry.exit_code = -1
        $_.Exception.Message | Add-Content -Path $log
    }
    $summary += [pscustomobject]$entry
}
$report = Join-Path $OutputDir "summary.json"
$summary | ConvertTo-Json -Depth 4 | Set-Content -Path $report -Encoding UTF8
$summary | Format-Table -AutoSize
Write-Host ("Report: {0}" -f $report)
# A process exit code alone is not proof of successful compilation.
if (($summary | Where-Object { $_.exit_code -ne 0 -or -not $_.log_found -or $_.compiler_errors }).Count -gt 0) {
    Write-Warning "One or more targets failed or lacked reliable compiler logs. Inspect each .log."
    exit 1
}
