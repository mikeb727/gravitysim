TARGET=gravitysim

OBJ=gravitySim.o 2DPhysEnv.o object.o 2DVec.o bbox.o circle.o rectangle.o control.o simParams.o cursor.o
OBJS=$(addprefix $(BIN), $(OBJ))

LINK=clang++
LFLAGS=-lSDL2 -lGL -lglfw -lfreetype -ltinyxml2 -L/usr/lib/mb-libs -lmbgfx
LFLAGS_STATIC=-lSDL2 -lGL -lglfw -lfreetype -ltinyxml2 -L/usr/lib/mb-libs /usr/lib/mb-libs/libmbgfx.a

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
	
$(TARGET)_static: $(OBJS)
	$(LINK) -o $(TARGET)_static $(OBJS) $(LFLAGS_STATIC)

$(BIN)%.o: $(SRC)%.cpp
	mkdir -p $(BIN)
	$(CPP) -std=c++17 $(DFLAGS) -I/usr/include/freetype2 -c $< -o $@
