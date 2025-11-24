@echo off

if "%~2"=="" (
    echo Usage: %~0 ^<arch^> ^<config^>
    exit /b 1
)

set "arch=%~1"
set "config=%~2"

set "archs=host x64 x86"
set "configs=Debug Release"

set ret=0

if exist build (
	rmdir /s /q build
)
cmake -S . -B build -G "Visual Studio 17 2022" -DARCH=%arch% -DCMAKE_BUILD_TYPE=%config%
cmake --build build --config %config%

call :test
exit /b %ret%

:cm
	set "proj=%~1"
	set "targets=%~2"

	set "dir=examples\%proj%"
	set "bin=%dir%\bin"
	set "tmp=%dir%\tmp"
	set "build=%dir%\build"

	echo|set /p="%p_arch% %proj% CMake "

	if not exist "%tmp%" (
		mkdir "%tmp%"
	)

	bin\build\%arch%-%config%\build.exe -p %dir% -g C -a "%archs%" -c "%configs%" -t "all cov" -O 0 >%tmp%\out.txt 2>&1
	if errorlevel 1 (
		echo FAIL
		echo build: Failed to generate cmake
		type %tmp%\out.txt
		set ret=1
		exit /b 0
	)

	for %%a in (%archs%) do (
		for %%c in (%configs%) do (
			for %%t in (%targets%) do (
				if not exist "%dir%/bin/%%a-%%c/%%t" (
					echo FAIL
					echo Target not found: %%a-%%c/%%t
					type %tmp%\out.txt
					set ret=1
					exit /b 0
				)
			)
		)
	)

	echo PASS
	rmdir /s /q "%bin%"
	rmdir /s /q "%build%"
	rmdir /s /q "%tmp%"

	exit /b 0

:gen
	call :cm %*
	exit /b 0

:test
	call :gen 00_exe "bin/00_exe.exe"
	call :gen 01_lib "lib/01_lib.lib"
	call :gen 02_multi "bin/a.exe bin/b.exe"
	call :gen 03_depends "bin/exe.exe lib/lib.lib"
	call :gen 04_rdepends "lib/base.lib lib/lib1.lib lib/lib2.lib bin/exe.exe"
	call :gen 05_extern "lib/cbase.lib"
	call :gen 06_lib_test "lib/06_lib_test.lib test/06_lib_test.exe"
	call :gen 07_zip "../../tmp/dl/cbase-main.zip ext/cbase/cbase.lib"

	exit /b 0
