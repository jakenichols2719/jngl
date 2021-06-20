EXE=run
HEAD=jngl.h

default: $(EXE)

# Compilation flags for different platforms
# Msys/MinGW
ifeq "$(OS)" "Windows_NT"
FLG=-O3 -Wall
LIBS=-lglfw3 -lglew32 -lglu32 -lopengl32 -lm
else
# OSX
ifeq "$(shell uname)" "Darwin"
FLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-lglfw3 -lglew -framework Cocoa -framework OpenGL -framework IOKit
# Linux/Unix/Solaris
else
FLG=-O3 -Wall
LIBS=-lglfw -lGLU -lGL -lm
endif
endif

# Object files in library
run.o: run.cpp jngl.a jngl.h

# Compile executeable
$(EXE): run.o jngl.a
	g++ -o $@ $^ $(LIBS)

lib: FORCE
	make -C lib
FORCE:

full: FORCE
	make lib
	make
	cp -r lib/shaders .

# c++ compile rule
.cpp.o:
	g++ -c $(FLG) $<

clean:
	rm *.o *.a