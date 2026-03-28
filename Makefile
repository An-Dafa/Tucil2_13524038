CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -std=c++14 -O3 -pthread -static -static-libgcc -static-libstdc++
SRC = src/main.cpp src/obj_parser.cpp src/octree.cpp
OUT = bin/voxelizer.exe

VIEWER_SRC = src/viewer.cpp
VIEWER_OUT = bin/viewer.exe

RAYLIB_FLAGS = -I./include -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++
all: build viewer

build:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

viewer:
	mkdir -p bin
	$(CXX) -std=c++14 $(VIEWER_SRC) -o $(VIEWER_OUT) $(RAYLIB_FLAGS)

clean:
	rm -rf bin/*