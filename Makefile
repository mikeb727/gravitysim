TARGET=gravitysim-3d

OBJ=sim3d.o env3d.o ball.o vec3d.o quaternion.o bbox.o control.o simParams.o cursor.o utility.o
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
