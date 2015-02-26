#--Source code--
OBJ = flappyBird.cpp

#--Compiler used--
CC = g++

#--Libraries we're linking against.--
LIBRARY_LINKS =-lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -std=c++11

#--Name of our exectuable--
OBJ_NAME = flappyBird

#--This is the target that compiles our executable--
all : $(OBJS)  
	$(CC) $(C++11) $(OBJ) $(LIBRARY_LINKS) -o $(OBJ_NAME)