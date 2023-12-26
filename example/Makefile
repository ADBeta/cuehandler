#TARGET
TARGET := ./example

SRCS := $(wildcard ./*.cpp) $(wildcard ../*.cpp)

#Compiler
CC := g++

#Flags
CFLAGS   := -std=c++11 -Wall -Wsign-conversion -Wmissing-declarations -Wconversion -Wshadow -Wlogical-op -Wfloat-equal -Wunused -Wuninitialized -Wformat -Wunused-result -Wtype-limits
#LDFLAGS  := -Llib
#LDLIBS   := -lm /usr/lib/ 

.PHONY: all clean

#Make binary
all:
	$(CC) $(SRCS) -I../ $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

#Remove objects and binary
clean:
	@$(RM) -rv $(TARGET)

-include $(OBJ:.o=.d)
