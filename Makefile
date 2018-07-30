
########################################
# COMPILER FLAGS
########################################

GC = g++ -std=c++14 -framework opengl \
	-I"." -I"./common/includes/" \
	-L"./common/lib/" \
	-lglfw -lglad -lassimp -lstdc++

GL = $(GC) -c
	
RM = rm -f

########################################
# PROGRAM SPEC
########################################

source = Earth.cpp

program = $(source:.cpp=.exe)

objsrc = \
ShaderProgram.cpp EularCamera.cpp Texture.cpp Mesh.cpp \
Operation.cpp Model.cpp Primitives.cpp Skybox.cpp

object = $(objsrc:.cpp=.o)

########################################
# BUILDING
########################################

all: $(program) $(object)

%.exe: %.cpp $(object)
	$(GC) $< $(object) -o $@ -lm

%.o: %.cpp %.h
	$(GL) $< -o $@ -lm

clean: 
	$(RM) $(program) $(object) *.png

########################################
# Lib link note
########################################

# When you use option -L you specify the directory where linker can find library files. 
# When you use option -l you specify the name of library file. It may be different from "lib*.a".
# If its name is different, just write full name of library file.
 
# For example,
 
# name of the library file is "myownlib.a"
# than you specify option like: -lmyownlib.a
 
# if the name of library is "libmyown.a"
# than option will be: -lmyown
 
# As for your question about difference between types of library, the lib*.a is a static library.
# This type of library is added to executive file.
# The lib*.so is a shared library, which dynamically linked during executing of program.
