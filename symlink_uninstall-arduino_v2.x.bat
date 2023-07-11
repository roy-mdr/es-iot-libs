@ECHO OFF

net session >nul 2>&1

if %errorLevel% == 0 (
	GOTO ADMIN
) else (
	GOTO NOADMIN
)


:ADMIN
	
	FOR /f "usebackq delims=|" %%f in (`dir /a:d /b "%~dp0src\"`) DO (
		::cmd /c rmdir "%userprofile%\Documents\Arduino\libraries\%%f"
		rmdir "%userprofile%\Documents\Arduino\libraries\%%f"
	)
	
	PAUSE

	exit /B
	

:NOADMIN
	
	echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
	echo UAC.ShellExecute "%~s0", "%format_parameter%", "", "runas", 1 >> "%temp%\getadmin.vbs"
	"%temp%\getadmin.vbs"
	del "%temp%\getadmin.vbs"
	exit /B