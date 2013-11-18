CFLAGS = -c `pkg-config --cflags opencv` -g -Wall
LDFLAGS = `pkg-config --libs opencv` -g

CC=clang++


SOURCE_DIR=src
OBJ_DIR=obj
EXEC_DIR=target

EXECUTABLE_EXTENSION=Test

ALL_CPP_FILES := $(wildcard ${SOURCE_DIR}/*.cpp)
EXECUTABLE_FILES := $(wildcard ${SOURCE_DIR}/*${EXECUTABLE_EXTENSION}.cpp)
USE_FILES := $(filter-out $(EXECUTABLE_FILES),$(ALL_CPP_FILES))
OBJ_FILES = $(patsubst ${SOURCE_DIR}/%.cpp,${OBJ_DIR}/%.o,$(USE_FILES))


all: init imglib ObjectDetectionTest

.SECONDARY:

imglib: init ${OBJ_FILES}
	ar r ${EXEC_DIR}/imglib ${OBJ_FILES}
	
${OBJ_DIR}/%.o: ${SOURCE_DIR}/%.cpp 
	$(CC) $(CFLAGS) -o $@ $<
	
%${EXECUTABLE_EXTENSION}: ${OBJ_DIR}/%${EXECUTABLE_EXTENSION}.o 
	$(CC) -o ${EXEC_DIR}/$@ ${OBJ_DIR}/$@.o ${EXEC_DIR}/imglib  $(LDFLAGS)
	
init:
	mkdir -p ${OBJ_DIR} ${EXEC_DIR}

clean:
	/bin/rm -rf ${OBJ_DIR}/*.o ${EXEC_DIR}/*



