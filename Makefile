# ls *.cpp
#
#3dobject-ase.cpp  construction.cpp  maps.cpp         netherdebug.cpp    radar.cpp
#3dobject.cpp      enemy_ai.cpp      menu.cpp         nethersave.cpp     robot_ai.cpp
#bitmap.cpp        glprintf.cpp      myglutaux.cpp    particles.cpp      robots.cpp
#bullet.cpp        main.cpp          nether.cpp       piece3dobject.cpp  shadow3dobject.cpp
#cmc.cpp           mainmenu.cpp      nethercycle.cpp  quaternion.cpp     vector.cpp

UNAME_S := $(shell uname -s)

SOURCES = 3dobject-ase.cpp 3dobject.cpp cmc.cpp nether.cpp piece3dobject.cpp vector.cpp bitmap.cpp bullet.cpp glprintf.cpp main.cpp mainmenu.cpp maps.cpp menu.cpp myglutaux.cpp nethercycle.cpp netherdebug.cpp nethersave.cpp particles.cpp construction.cpp quaternion.cpp radar.cpp enemy_ai.cpp robot_ai.cpp robots.cpp shadow3dobject.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = nether_earth

ifeq ($(UNAME_S),Darwin)
  SOURCES += SDLmain.m
  LIBRARIES = -framework Cocoa -framework OpenGL -framework GLUT -framework SDL -framework SDL_mixer -lpthread
else
  LIBRARIES = -lGL -lGLU -lglut -lSDL -lSDL_mixer -lpthread
endif

.cpp.o:
	g++ -c $< -o $@ -I/opt/local/include

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $(OBJECTS) -o $(TARGET) $(LIBRARIES)
