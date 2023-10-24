compile: 
	clang++ -shared -o ./Code/libjobsystem.so -fPIC ./Code/lib/*.cpp
	clang++ -o a main.cpp -L./Code/ -ljobsystem -Wl,-rpath,./Code/
	# clang++ -g -std=c++14 ./Code/*.cpp -o output