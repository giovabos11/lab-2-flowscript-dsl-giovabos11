compile: 
	clang++ -shared -o ./libjobsystem.so -fPIC ./lib/*.cpp
	clang++ -o a main.cpp -L./ -ljobsystem -Wl,-rpath,./
	# clang++ -g -std=c++14 ./Code/*.cpp -o output