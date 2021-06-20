LIB=jngl.a
HEAD=jngl.h

default: $(LIB)

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
definitions.o: jngl.h definitions.cpp
transform.o: jngl.h transform.cpp
loadobj.o: jngl.h loadobj.cpp
context.o: jngl.h context.cpp
object.o: jngl.h object.cpp
camera.o: jngl.h camera.cpp
shader.o: jngl.h shader.cpp
debug.o: jngl.h debug.cpp
scene.o: jngl.h scene.cpp

# Compile library
# Copies 1 directory up
$(LIB): definitions.o transform.o loadobj.o context.o object.o camera.o shader.o debug.o scene.o
	ar -rcs $@ $^
	cp $(LIB) $(HEAD) ..

# c++ compile rule
.cpp.o:
	g++ -c $(FLG) $<

clean:
	rm *.o *.a