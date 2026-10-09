# Run from the extracted archive directory after reviewing source and rights.
$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot
$remote = "https://github.com/hoangsvit/pt.git"
if (-not (Get-Command git -ErrorAction SilentlyContinue)) { throw "Install Git for Windows first." }
if (-not (Test-Path ".git")) {
    git init -b main
    if ($LASTEXITCODE -ne 0) { throw "git init failed" }
}
git remote get-url origin *> $null
if ($LASTEXITCODE -ne 0) { git remote add origin $remote }
else {
    $actual = (git remote get-url origin).Trim()
    if ($actual -ne $remote) { throw "Unexpected origin remote: $actual" }
}
$heads = @(git ls-remote --heads origin)
if ($LASTEXITCODE -ne 0) { throw "Cannot contact GitHub. Authenticate/verify network first." }
if ($heads.Count -gt 0) { throw "Remote already contains branches; stopping to avoid overwriting content. Clone and merge manually." }
git add -A
if ($LASTEXITCODE -ne 0) { throw "git add failed" }
git commit -m "chore: import legacy Phong Than 2 source snapshot"
if ($LASTEXITCODE -ne 0) { throw "git commit failed. Configure git user.name and user.email." }
git push -u origin main
if ($LASTEXITCODE -ne 0) { throw "Git push failed; check GitHub authentication and permissions." }
Write-Host "Push complete: $remote" -ForegroundColor Green
