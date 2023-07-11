@ECHO OFF

net session >nul 2>&1

if %errorLevel% == 0 (
	GOTO ADMIN
) else (
	GOTO NOADMIN
)


:ADMIN
	
	FOR /f "usebackq delims=|" %%f in (`dir /a:d /b "%~dp0src\"`) DO (
		::cmd /c mklink /J "%userprofile%\Documents\Arduino\libraries\%%f" "%~dp0src\%%f"
		mklink /J "%userprofile%\Documents\Arduino\libraries\%%f" "%~dp0src\%%f"
	)
	
	PAUSE

	exit /B
	

:NOADMIN

	echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
	echo UAC.ShellExecute "%~s0", "%format_parameter%", "", "runas", 1 >> "%temp%\getadmin.vbs"
	"%temp%\getadmin.vbs"
	del "%temp%\getadmin.vbs"
	exit /B