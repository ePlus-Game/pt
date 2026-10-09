# Compile original VC6 projects in one batch inside an isolated Windows x86 VM.
# Never execute the game, server, or other untrusted binaries.
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

# Each pattern must correspond to a # Name entry in its original DSP.
# Core_Lib has four distinct client/server configurations.
$targets = @(
    @{Name="LuaLib"; Path="Base\LuaLib\LuaLib.dsp"; Config="LuaLib - Win32 {0}"},
    @{Name="Common"; Path="Base\Common\Common.dsp"; Config="Common - Win32 {0}"},
    @{Name="CoreClient"; Path="Client\Core\Core_lib.dsp"; Config="Core_Lib - Win32 Client {0}"},
    @{Name="CoreServer"; Path="Client\Core\Core_lib.dsp"; Config="Core_Lib - Win32 Server {0}"},
    @{Name="Engine"; Path="Client\Engine\Engine.dsp"; Config="Engine - Win32 {0}"},
    @{Name="Represent2"; Path="Client\Represent\Represent2\Represent2.dsp"; Config="Represent2 - Win32 {0}"},
    @{Name="Faith"; Path="Client\Faith\Faith.dsp"; Config="Faith - Win32 {0}"},
    @{Name="lord"; Path="Server\lord\lord.dsp"; Config="lord - Win32 {0}"}
)

# Verify project paths and exact configuration names before invoking VC6.
# The same preflight runs without VC6 in CI with -DryRun.
$validated = @()
foreach ($target in $targets) {
    $project = Join-Path $root $target.Path
    if (-not (Test-Path -LiteralPath $project -PathType Leaf)) {
        throw ("Missing VC6 project: {0}" -f $project)
    }
    $cfg = [string]::Format($target.Config, $Configuration)
    $projectConfigurations = @(Get-Content -LiteralPath $project | ForEach-Object {
        if ($_ -match '^# Name "([^"]+)"') { $matches[1] }
    })
    if ($projectConfigurations -cnotcontains $cfg) {
        throw ("Config '{0}' does not exist in project '{1}'. Found: {2}" -f $cfg, $target.Path, ($projectConfigurations -join ", "))
    }
    $validated += [pscustomobject]@{Name=$target.Name; Project=$project; Configuration=$cfg}
}

if ($DryRun) {
    Write-Host "Preflight PASS: all VC6 projects and configurations exist."
    $validated | Format-Table Name, Configuration -AutoSize
    return
}

$tool = Get-Command -Name $Msdev -ErrorAction SilentlyContinue
if (-not $tool) {
    throw "msdev.exe not found. Install VC6 in an isolated Windows x86 VM and pass -Msdev 'C:\Path\MSDEV.EXE'."
}
New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
$summary = @()
foreach ($target in $validated) {
    $log = Join-Path $OutputDir ("{0}-{1}.log" -f $target.Name, $Configuration)
    $entry = [ordered]@{
        project=$target.Name
        configuration=$target.Configuration
        exit_code=-1
        log=$log
        log_found=$false
        compiler_errors=$false
    }
    # Prevent an old log from falsely counting as the current result.
    if (Test-Path -LiteralPath $log) { Remove-Item -LiteralPath $log -Force }
    Write-Host ("Building {0} ..." -f $target.Configuration)
    try {
        # Microsoft Developer Studio 6.0 batch interface; does not run binaries.
        & $tool.Source $target.Project /MAKE $target.Configuration /REBUILD /OUT $log
        $entry.exit_code = $LASTEXITCODE
        $entry.log_found = Test-Path -LiteralPath $log
        if ($entry.log_found) {
            $contents = Get-Content -LiteralPath $log -Raw
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
# Successful tool exit alone is not proof that the original game works.
$failures = @($summary | Where-Object { $_.exit_code -ne 0 -or -not $_.log_found -or $_.compiler_errors })
if ($failures.Count -gt 0) {
    Write-Warning ("Build attempts needing investigation: {0}/{1}. Inspect per-project logs." -f $failures.Count, $summary.Count)
    exit 1
}
