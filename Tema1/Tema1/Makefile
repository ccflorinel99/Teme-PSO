CC = cl
L = link
CFLAGS = /W3 /nologo

all: build clean

build: so-cpp.exe

so-cpp.exe: so-cpp.cpp
	$(CC) so-cpp.cpp -o $@

.IGNORE:
clean:
	del /Q /S /F *.obj so-cpp.exe