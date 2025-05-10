CC = g++

GLLIBS = -lglut -lGLEW -lGL

all: main.cpp
	$(CC) main.cpp lib/utils.cpp -o main $(GLLIBS)

clean:
	rm -f main