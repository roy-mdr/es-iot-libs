@ECHO OFF

FOR /f "usebackq delims=|" %%f in (`dir /a:d /b "%~dp0src\"`) DO (
	xcopy /I /F /E "%~dp0src\%%f" "%userprofile%\Documents\Arduino\libraries\%%f"
)

PAUSE

exit /B