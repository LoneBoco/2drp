pushd "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\"
call VsDevCmd.bat
popd
pushd ..\dependencies\luajit-2.0\src
call msvcbuild.bat
popd
pause
