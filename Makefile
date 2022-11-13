TARGET=gravitysim

OBJ=gravitySim.o 2DPhysEnv.o object.o 2DVec.o bbox.o circle.o rectangle.o control.o
OBJS=$(addprefix $(BIN), $(OBJ))

LINK=clang++
LFLAGS=-lmbgfx -lSDL2 -lSDL2_image -lSDL2_ttf

CPP=clang++
SRC=src/
BIN=bin/

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(OBJS)
	rmdir -p $(BIN)

$(TARGET): $(OBJS)
	$(LINK) -o $(TARGET) $(OBJS) $(LFLAGS)
	
$(BIN)%.o: $(SRC)%.cpp
	mkdir -p $(BIN)
	$(CPP) -std=c++17 -c $< -o $@
