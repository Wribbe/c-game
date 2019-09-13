URL_GIT_GLFW=https://github.com/glfw/glfw.git
URL_GIT_GLAD=https://github.com/Dav1dde/glad.git
URL_GIT_CGLTF=https://github.com/jkuhlmann/cgltf
URL_GLTF_SAMPLE_MODELS=https://github.com/KhronosGroup/glTF-Sample-Models
URL_GIT_FREETYPE=https://git.savannah.gnu.org/git/freetype/freetype2.git

DIR_SRC=src
DIR_BIN=bin
DIR_OBJ=obj
DIR_LIB=lib
DIR_SAMPLES=res/samples

DEPS_EXTERNAL=\
	glfw/src/libglfw3.a ${DIR_OBJ}/glad.o $(wildcard ${DIR_LIB}/*.c) \
	cgltf/cgltf.h freetype2/build/libfreetype.a

SRCS=$(wildcard ${DIR_SRC}/*.c)
BINS=$(foreach f,${SRCS},$(patsubst ${DIR_SRC}/%.c,${DIR_BIN}/%,$f))

FLAGS_C= -std=c11 -Werror -Wall --pedantic -g
FLAGS_LD = -L/usr/local/lib64 -Lglfw/src
FLAGS_LIBS= -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp -lGL -lm
INCLUDES=  -I/usr/local/include -Iglfw/include -Iglad/include -I${DIR_LIB} \
					 -Icgltf -Ifreetype2/include

all: ${DIR_SAMPLES} ${BINS}

$(DIR_BIN)/% : $(DIR_SRC)/%.c | ${DIR_BIN}
	gcc ${filter %.c,$^} -o $@ ${filter-out %.c %.h,$^} \
		${FLAGS_LIBS} ${FLAGS_C} ${INCLUDES} ${FLAGS_LD}

${BINS} : ${DEPS_EXTERNAL} $(wildcard ${DIR_LIB}/*)

${DIR_BIN} ${DIR_OBJ}:
	mkdir $@

${DIR_SAMPLES}:
	git clone ${URL_GLTF_SAMPLE_MODELS} --depth 1 $@

glfw:
	git clone ${URL_GIT_GLFW} $@

glad:
	git clone ${URL_GIT_GLAD} $@

cgltf/%:
	git clone ${URL_GIT_CGLTF} $@

freetype2:
	git clone ${URL_GIT_FREETYPE}

%/glad.c : glad
	cd $^ && python -m glad --generator=c --out-path=.

%/glad.o : glad/src/glad.c | ${DIR_OBJ}
	gcc -c $^ -o $@ -Iglad/include

%/libglfw3.a : glfw
	cd $^ && git checkout latest && cmake . && make all

%/libfreetype.a : freetype2
	cd $^ && mkdir build && cd build && cmake .. && make
