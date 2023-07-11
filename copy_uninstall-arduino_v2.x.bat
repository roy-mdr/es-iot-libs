@ECHO OFF

FOR /f "usebackq delims=|" %%f in (`dir /a:d /b "%~dp0src\"`) DO (
	rd /S /Q "%userprofile%\Documents\Arduino\libraries\%%f"
)

PAUSE

exit /B