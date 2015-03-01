#--Source code--
OBJ = flappyBird.cpp

#--Compiler used--
CC = g++

#--Libraries we're linking against.--
LIBRARY_LINKS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -std=c++11

#--Name of our exectuable--
OBJ_NAME = "Flappy Bird"

#--This is the target that compiles our executable--
all : $(OBJS)  
	$(CC) $(OBJ) $(LIBRARY_LINKS) -o $(OBJ_NAME)