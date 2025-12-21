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

	bin\%arch%-%config%\bin\build.exe -p %dir% -g C -a "%archs%" -c "%configs%" -t "all test" -O 0 >%tmp%\out.txt 2>&1
	if errorlevel 1 (
		echo FAIL
		echo build: Failed to build cmake
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
	call :gen 07_exe_driver "bin/07_exe_driver.exe"
	call :gen 08_lib_driver "bin/exe.exe test/lib.exe"
	call :gen 09_zip "../../tmp/dl/pkgs/cbase/cbase-main.zip ../../tmp/dl/pkgs/cutils/cutils-main.zip ext/cbase/cbase.lib ext/cutils/cutils.lib bin/exe.exe"
	call :gen 10_tar_gz "../../tmp/dl/pkgs/cbase/cbase-main.tar.gz ../../tmp/dl/pkgs/cutils/cutils-main.tar.gz ext/cbase/cbase.lib ext/cutils/cutils.lib bin/exe.exe"

	exit /b 0
