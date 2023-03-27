#OBJS specifies which files to compile as part of the project
#OBJS = src/bus.cpp src/ppu.cpp src/main.cpp src/win.cpp src/cpu/cpu.cpp src/cpu/op*.cpp src/debug
OBJS = src/*.cpp src/cpu/*.cpp src/ppu/*.cpp src/utils/*.cpp src/platform/linux/*.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
# INCLUDE_PATHS = -IC:\mingw_dev_lib\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
# LIBRARY_PATHS = -LC:\mingw_dev_lib\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
# -g lets you see line numbers in valgrind
COMPILER_FLAGS = -g
# -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lm -lSDL2main -lSDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = amphy

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

