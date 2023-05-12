TARGET=gravitysim

OBJ=gravitySim.o 2DPhysEnv.o object.o 2DVec.o bbox.o circle.o rectangle.o control.o simParams.o
OBJS=$(addprefix $(BIN), $(OBJ))

LINK=clang++
LFLAGS=-L/usr/lib/mb-libs -lmbgfx -lSDL2 -lGL -lglfw -lfreetype -ltinyxml2

DFLAGS=-g -O0 

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
	$(CPP) -std=c++17 $(DFLAGS) -I/usr/include/freetype2 -c $< -o $@
