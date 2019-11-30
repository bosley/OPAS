CC = g++
CFLAGS = -std=c++11
MKDIR_P = mkdir -p
SRC_DIR = ./src
OBJ_DIR = ./obj

BOOST = /usr/local/Cellar/boost/1.66.0

INCLUDES = -I$(BOOST)/include

LIBS = $(BOOST)/lib/libboost_system.dylib\
		$(BOOST)/lib/libboost_filesystem.dylib\
		/usr/local/lib/libhydrogen.a 

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
MAIN = keyman

.PHONY: depend clean dirensure

all: dirensure $(MAIN)
	@echo  Success!

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(OBJ_DIR)/*.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

dirensure: $(OBJ_DIR)

$(OBJ_DIR):
	$(MKDIR_P) $(OBJ_DIR)

# END
