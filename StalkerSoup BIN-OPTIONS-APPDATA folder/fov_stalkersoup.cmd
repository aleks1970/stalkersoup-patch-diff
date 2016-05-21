@echo off
@echo              STALKERSOUP FOV SWITCHER
@echo          adapted from Kolmogor and Macron
@echo.
@echo This mod reduces the actor collision box to support
@echo the Collector quests. At 67.50 corners will already
@echo suffer from the 'look through surface' effect, higher
@echo FOV means a greater 'look through surface' effect.
@echo.
@echo.
@echo             SELECT FOV (FIELD OF VIEW)
@echo 1 = 67.50 - (SHOC default)
@echo 2 = 75
@echo 3 = 83    - considered best chioce by TB3D
@echo 4 = 85
@echo 5 = 90
@echo.6 = Exit
@echo.
@Set /p var="Enter 0-6: "

if %var%==1 "bin\patch2.exe" "bin\xrgame_ss.dll" $0053C59A $87
if %var%==1 copy /y "bin\xrgame_ss.dll" "bin\xrgame.dll" 
if %var%==2 "bin\patch2.exe" "bin\xrgame_ss.dll" $0053C59A $96
if %var%==2 copy /y "bin\xrgame_ss.dll" "bin\xrgame.dll"
if %var%==3 "bin\patch2.exe" "bin\xrgame_ss.dll" $0053C59A $A6
if %var%==3 copy /y "bin\xrgame_ss.dll" "bin\xrgame.dll"
if %var%==4 "bin\patch2.exe" "bin\xrgame_ss.dll" $0053C59A $AA
if %var%==4 copy /y "bin\xrgame_ss.dll" "bin\xrgame.dll"
if %var%==5 "bin\patch2.exe" "bin\xrgame_ss.dll" $0053C59A $B4
if %var%==5 copy /y "bin\xrgame_ss.dll" "bin\xrgame.dll"
if %var%==6 CLS & exit