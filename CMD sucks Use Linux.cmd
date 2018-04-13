@echo off
echo.
call :draw "*" 40
echo.
echo davidhcefx
whoami
date /T
echo "     ___                 __      "    
echo "    /       |\     /|   |  \     "
echo "   |        |  \_/  |   |   |    "
echo "   |        |       |   |   |    "
echo "    \___    |       |   |__/     "
echo "                                 "
echo         Cmd Sucks, Use Linux
call :draw "*" 40
echo.
echo.
pause
goto :eof

:draw
 setlocal
 set i=0
 :loop
  <nul set /p=%1
  set /A i=i+1
  if %i%==%2 ( goto :end
  ) else ( goto :loop )
 :end
endlocal
