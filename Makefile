SOURCES = 3dobject-ase.cpp 3dobject.cpp bitmap.cpp bullet.cpp cmc.cpp \
	construction.cpp enemy_ai.cpp glprintf.cpp main.cpp mainmenu.cpp \
	maps.cpp menu.cpp myglutaux.cpp nether.cpp nethercycle.cpp \
	netherdebug.cpp nethersave.cpp particles.cpp piece3dobject.cpp \
	quaternion.cpp radar.cpp robot_ai.cpp robots.cpp shadow3dobject.cpp \
	vector.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = nether
LIBS = -lSDL -lSDL_mixer -lpthread -lGL -lGLU -lglut -L/usr/X11R6/lib

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LIBS) -I. -I/usr/include

.cpp.o:
	g++ -o $@ -c $<

