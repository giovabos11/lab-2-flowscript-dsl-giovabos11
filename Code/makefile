.SILENT:
project1:
	-g++ -fdiagnostics-format=json -g -std=c++14 ./compilecode/Project1/*.cpp -o auto_out1
project2:
	-g++ -fdiagnostics-format=json -g -std=c++14 ./compilecode/Project2/*.cpp -o auto_out2
project3:
	-g++ -fdiagnostics-format=json -g -std=c++14 ./compilecode/Project3/*.cpp -o auto_out3
project4:
	-g++ -fdiagnostics-format=json -g -std=c++14 ./compilecode/Project4/*.cpp -o auto_out4

libLinux:
	clang++ -shared -o ./lib/libjobsystem.so -fPIC ./lib/*.cpp

libWindows:
	g++ -shared -o ./libjobsystem.dll ./lib/*.cpp -Wl,--out-implib,./libjobsystem.a

buildLinux:
	clang++ -o a main.cpp -L./lib -ljobsystem -Wl,-rpath,./lib

buildWindows:
	g++ -o a main.cpp -L./ -ljobsystem
	./a

runWindows:
	g++ -shared -o ./libjobsystem.dll ./lib/*.cpp -Wl,--out-implib,./libjobsystem.a
	g++ -o a main.cpp -L./ -ljobsystem
	./a

# ALL LIB FILES WILL BE NAMED: lib[name].dll
# THE BUILD COMMAND ARGUMENT HAS TO BE: -l[name]