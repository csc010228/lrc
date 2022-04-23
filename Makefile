SRC=./src
INC=./include
OBJ=./obj

SOURCE=$(wildcard ${SRC}/*.cpp)
OBJECT=$(patsubst %.cpp,${OBJ}/%.o,$(notdir ${SOURCE}))

TARGET=lrc

CC=clang++
CFLAGS=-I${INC}

${TARGET}:${OBJECT}
	$(CC) -std=c++17 -O2 -lm -o $@ ${OBJECT}

${OBJ}/%.o:${SRC}/%.cpp
	$(CC) -std=c++17 -O2 -lm $(CFLAGS) -o $@ -c $<

.PHONY:clean
clean:
	find $(OBJ) -name *.o -exec rm -rf {} \;
	rm -rf $(TARGET)