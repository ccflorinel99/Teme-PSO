ifeq ($(OS), Windows)
	RM = del /Q /S /F *.obj;
	CC = cl;
	CFLAGS = /W3 /nologo;
	EXECUTABLE = so_stdio.dll;
else
	RM = rm -f *.o *~;
	CC = gcc;
	CFLAGS = -Wall -Wextra -g;
	EXECUTABLE = libso_stdio.so;
endif

all: build clean

build: $(EXECUTABLE)

$(EXECUTABLE): so-cpp.h
	$(CC) $(CFLAGS) $^ -o $@

.IGNORE:
clean:
	$(RM) $(EXECUTABLE)