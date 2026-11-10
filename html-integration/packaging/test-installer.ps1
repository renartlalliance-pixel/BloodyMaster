param(
  [Parameter(Mandatory=$true)][string]$Setup,
  [Parameter(Mandatory=$true)][string]$ExpectedPlugin
)
$ErrorActionPreference = 'Stop'
$Setup = (Resolve-Path $Setup).Path
$ExpectedPlugin = (Resolve-Path $ExpectedPlugin).Path
$bundle = Join-Path $env:CommonProgramFiles 'VST3\Bloody Master.vst3'
$installed = Join-Path $bundle 'Contents\x86_64-win\Bloody Master.vst3'
$key = 'HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\BRIAudioBloodyMaster'
if ((Test-Path $bundle) -or (Test-Path $key)) { throw 'Test reserve a un runner Windows propre : installation deja presente.' }
$sentinel = Join-Path (Split-Path $bundle) ('BloodyMaster-CI-' + [guid]::NewGuid().ToString() + '.txt')
New-Item -ItemType Directory -Force (Split-Path $bundle) | Out-Null
Set-Content $sentinel 'preserve me'
function Install-Test {
  $process = Start-Process -FilePath $Setup -ArgumentList '/S' -Wait -PassThru
  if ($process.ExitCode -ne 0) { throw "Installation : code $($process.ExitCode)" }
  if (-not (Test-Path $installed)) { throw 'VST3 absent apres installation.' }
  if ((Get-FileHash $installed).Hash -ne (Get-FileHash $ExpectedPlugin).Hash) { throw 'Hash VST3 installe incorrect.' }
  if (-not (Test-Path $key)) { throw 'Entree de desinstallation absente.' }
}
try {
  Install-Test
  Install-Test
  $app = (Get-ItemProperty $key).InstallLocation
  $process = Start-Process -FilePath (Join-Path $app 'Desinstaller.exe') -ArgumentList '/S' -Wait -PassThru
  if ($process.ExitCode -ne 0) { throw "Desinstallation : code $($process.ExitCode)" }
  $deadline = (Get-Date).AddSeconds(15)
  while ((Test-Path $installed) -and ((Get-Date) -lt $deadline)) { Start-Sleep -Milliseconds 100 }
  if (Test-Path $installed) { throw 'VST3 encore present apres desinstallation.' }
  if (Test-Path $key) { throw 'Entree de desinstallation encore presente.' }
  if ((Get-Content $sentinel -Raw).Trim() -ne 'preserve me') { throw 'Fichier tiers modifie.' }
  Install-Test
  Write-Output 'PASS installation, installation repetee, desinstallation, fichier tiers et reinstallation.'
} finally {
  Remove-Item $sentinel -ErrorAction SilentlyContinue
}
