@ECHO OFF
set mode=%1
IF "%mode%"=="clean" GOTO CLEAN

IF not exist build mkdir build
cd build

IF "%mode%"=="client" GOTO CLIENT

go build ..\tcp_server\tcp_server.go
IF "%mode%"=="server" GOTO END

:CLIENT
go build ..\tcp_client\tcp_client.go

:END
cd ..
exit 0

:CLEAN
rd /S /Q build
