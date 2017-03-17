$scriptpath = $MyInvocation.MyCommand.Path
$dir = Split-Path $scriptpath
cd $dir

.\putty -ssh -l pi -pw raspberry -m .\mountRW 169.254.238.96
Start-Sleep -s 1

.\pscp.exe -pw raspberry ..\CAN.cpp ..\storage.cpp ..\storage.h ..\frameHolder.h ..\makefile pi@169.254.238.96:sunstangCAN  
.\pscp.exe -pw raspberry -r C:\Users\Peter\Documents\SunStang\Arduino\libraries\SunstangHelpers pi@169.254.238.96:sunstangCAN

.\putty -ssh -l pi -pw raspberry -m .\remoteCommands 169.254.238.96
Start-Sleep -s 30
.\putty -ssh -l pi -pw raspberry -m .\mountRO 169.254.238.96