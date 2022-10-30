TARGET=gravitySim

OBJ=gravitySim.o window.o 2DPhysEnv.o object.o 2DVec.o imageTools.o bbox.o circle.o rectangle.o control.o
OBJS=$(addprefix $(BIN), $(OBJ))

LINK=g++
LFLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf

CPP=g++
SRC=src/
BIN=bin/

all: $(TARGET)

clean:
	rm $(BIN)*.o

$(TARGET): $(OBJS)
	$(LINK) -std=c++11 -o $(TARGET) $(OBJS) $(LFLAGS)
	
$(BIN)%.o: $(SRC)%.cpp
	$(CPP) -c $< -o $@
