del /Q "../src/engine/network/packets/"
for %%f in (../src/engine/network/proto/*.proto) do protoc.exe --proto_path=../src/engine/network/proto/ --cpp_out=../src/engine/network/packets/ %%f
pause
