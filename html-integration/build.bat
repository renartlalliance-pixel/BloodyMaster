@echo off
setlocal
rem ============================================================
rem  Bloody Master - compilation locale (Windows x64, Release)
rem  Prerequis : Visual Studio 2022 (charge "Developpement Desktop en C++"), CMake >= 3.22, Git
rem  JUCE est telecharge dans un dossier COURT (C:\bm_deps) pour eviter la limite de 260 caracteres de Windows.
rem ============================================================
pushd "%~dp0"

where cmake >nul 2>nul || (echo [ERREUR] CMake introuvable. Installez-le puis relancez. & goto :fail)
where git   >nul 2>nul || (echo [ERREUR] Git introuvable. Installez-le puis relancez.   & goto :fail)

rem --- Dossier court pour JUCE : C:\bm_deps, sinon %LOCALAPPDATA%\bm_deps ---
set "DEPS=C:\bm_deps"
if not exist "%DEPS%" mkdir "%DEPS%" 2>nul
echo x> "%DEPS%\.w" 2>nul
if not exist "%DEPS%\.w" (
    set "DEPS=%LOCALAPPDATA%\bm_deps"
    if not exist "%LOCALAPPDATA%\bm_deps" mkdir "%LOCALAPPDATA%\bm_deps"
) else (
    del "%DEPS%\.w" 2>nul
)
set "DEPS_FWD=%DEPS:\=/%"
echo Dependances (JUCE) : %DEPS%

rem --- Avertissement si le projet est dans un chemin long ---
for /f %%L in ('powershell -NoProfile -Command "('%~dp0').Length"') do set PLEN=%%L
if %PLEN% GTR 50 (
    echo [ATTENTION] Chemin du projet long ^(%PLEN% caracteres^). Si la compilation echoue sur un chemin trop long,
    echo             deplacez le dossier vers un chemin court, par exemple C:\BM
)

echo [1/3] Configuration...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBM_DEPS_DIR="%DEPS_FWD%" || goto :fail

echo [2/3] Compilation (VST3 + Standalone + tests)...
cmake --build build --config Release --parallel || goto :fail

echo [3/3] Tests...
ctest --test-dir build -C Release --output-on-failure || goto :fail

echo.
echo === TERMINE ===
echo EXE  : %~dp0build\BloodyMaster_artefacts\Release\Standalone\Bloody Master.exe
echo VST3 : %~dp0build\BloodyMaster_artefacts\Release\VST3\Bloody Master.vst3
echo Copiez le dossier .vst3 dans C:\Program Files\Common Files\VST3\ puis rescannez dans FL Studio.
popd
exit /b 0

:fail
echo.
echo === ECHEC === (voir les messages ci-dessus)
echo Si l'erreur mentionne un chemin trop long : supprimez le dossier "build", deplacez le projet vers C:\BM et relancez.
popd
exit /b 1
