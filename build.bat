@echo off
setlocal

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo ERROR: vswhere.exe not found. Visual Studio Build Tools required.
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
if not defined VSINSTALL (
    echo ERROR: Visual Studio C++ tools not found.
    exit /b 1
)

call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1

set "RAYLIB_SRC=vendor\raylib\src"

if not exist build mkdir build

cl /nologo /O2 /W3 /DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_33 /D_CRT_SECURE_NO_WARNINGS ^
   /I"%RAYLIB_SRC%" /I"%RAYLIB_SRC%\external\glfw\include" /Isrc /Isrc\ui /Isrc\calc ^
   /c src\main.c src\ui\ui.c src\win_main.c src\calc\calc.c ^
      "%RAYLIB_SRC%\raudio.c" "%RAYLIB_SRC%\rcore.c" "%RAYLIB_SRC%\rmodels.c" ^
      "%RAYLIB_SRC%\rshapes.c" "%RAYLIB_SRC%\rtext.c" "%RAYLIB_SRC%\rtextures.c" ^
      "%RAYLIB_SRC%\rglfw.c" ^
   /Fo:build\ /Fd:build\
if errorlevel 1 exit /b 1

link /nologo /subsystem:windows ^
     build\main.obj build\ui.obj build\win_main.obj build\calc.obj build\raudio.obj build\rcore.obj build\rmodels.obj ^
     build\rshapes.obj build\rtext.obj build\rtextures.obj build\rglfw.obj ^
     /out:build\calculator.exe ^
     kernel32.lib user32.lib gdi32.lib shell32.lib winmm.lib advapi32.lib ^
     opengl32.lib ole32.lib ws2_32.lib
if errorlevel 1 exit /b 1

echo.
echo Build OK: build\calculator.exe

echo.
echo Building tests...
cl /nologo /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Isrc\calc /Itests ^
   /c src\calc\calc.c tests\test_input.c tests\test_ops.c tests\test_random.c ^
   /Fo:build\ /Fd:build\
if errorlevel 1 exit /b 1

link /nologo /subsystem:console ^
     build\calc.obj build\test_input.obj ^
     /out:build\test_input.exe
if errorlevel 1 exit /b 1

link /nologo /subsystem:console ^
     build\calc.obj build\test_ops.obj ^
     /out:build\test_ops.exe
if errorlevel 1 exit /b 1

link /nologo /subsystem:console ^
     build\calc.obj build\test_random.obj ^
     /out:build\test_random.exe
if errorlevel 1 exit /b 1

echo Build OK: build\test_input.exe build\test_ops.exe build\test_random.exe
endlocal
