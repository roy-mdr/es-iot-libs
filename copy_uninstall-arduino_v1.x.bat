@ECHO OFF

net session >nul 2>&1

if %errorLevel% == 0 (
	GOTO ADMIN
) else (
	GOTO NOADMIN
)


:ADMIN

	FOR %%i IN (%1) DO (
		SET arduino_filedrive=%%~di
		SET arduino_filepath=%%~pi
		SET arduino_filename=%%~ni
		SET arduino_fileextension=%%~xi
	)
	
	FOR /f "usebackq delims=|" %%f in (`dir /a:d /b "%~dp0src\"`) DO (
		rd /S /Q "%arduino_filedrive%%arduino_filepath%libraries\%%f"
	)
	
	PAUSE

	exit /B
	

:NOADMIN

	SET /P arduino=Drop the Arduino IDE .exe here:
	
	FOR %%i IN (%arduino%) DO (
		SET arduino_filedrive=%%~di
		SET arduino_filepath=%%~pi
		SET arduino_filename=%%~ni
		SET arduino_fileextension=%%~xi
	)
	
	SET format_parameter="%arduino_filedrive%%arduino_filepath%"

	ECHO.
	FOR /f "usebackq delims=|" %%f in (`dir /a:d /b "%~dp0src\"`) DO (
		ECHO rd /S /Q "%arduino_filedrive%%arduino_filepath%libraries\%%f"
	)
	ECHO.

	PAUSE
	
	echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
	rem echo UAC.ShellExecute "%~s0", "%format_parameter%", "", "runas", 1 >> "%temp%\getadmin.vbs"
	echo UAC.ShellExecute "cmd", "/c """"%~f0"" "%format_parameter%"""", "", "runas", 1 >> "%temp%\getadmin.vbs"
	"%temp%\getadmin.vbs"
	del "%temp%\getadmin.vbs"
	exit /B