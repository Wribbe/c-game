SRC=$(wildcard *.c)
BINS=$(foreach f,$(SRC),bin/${f:%.c=%})

DIR_BIN=bin

all: ${BINS}

${DIR_BIN}/% : %.c | ${DIR_BIN}
	gcc $^ -o $@

${DIR_BIN} :
	mkdir $@
