#
# Inspired by Makefiles created for the Artemis-Excicies "T8 - Systemc und C"
# in Course "Grundlagenpraktikum Rechnerarchitektur SS24 - Systemdesign"
# Originally created by: ÜL for Module IN0005
# Last update: 12.06.2024
#

# Set default value for SYSTEMC_HOME
ifndef SYSTEMC_HOME
override SYSTEMC_HOME = ../systemc
endif

#                     #
# Build configuration #
#                     #

# Target name
TARGET := r5mm

# Directory paths
LIB := $(SYSTEMC_HOME)
SRC := ./src
DIST := ./dist

# Source file paths
C_SRC := main.c
CPP_SRC := modules.cpp

#               #
# Env variables #
#               #

# Source file convertion

C_OBJ := $(foreach wrd,$(C_SRC),$(DIST)/$(wrd))
CPP_OBJ := $(foreach wrd,$(CPP_SRC),$(DIST)/$(wrd))

C_SRC := $(foreach wrd,$(C_SRC),$(SRC)/$(wrd))
CPP_SRC := $(foreach wrd,$(CPP_SRC),$(SRC)/$(wrd))

C_OBJ := $(C_OBJ:.c=.o)
CPP_OBJ := $(CPP_OBJ:.cpp=.o)


# Compiler settings

CC := clang
CFLAGS := -std=c17

CXX := clang++
CXXFLAGS := -std=c++14 -I$(LIB)/include
LDFLAGS := -I$(LIB)/include -L$(LIB)/lib -lsystemc


#         #
# Targets #
#         #

.PHONY: all debug release clean

# Default target
all: debug

# Debug target
debug: CCFLAGS += -g
debug: CXXFLAGS += -g
debug: LDFLAGS += -g
debug: $(DIST)/$(TARGET)

# Release target
release: CCFLAGS += -O2
release: CXXFLAGS += -O2
release: LDFLAGS += -O2
release: $(DIST)/$(TARGET)

# Test target
test: debug
	$(shell pytest)

# Clean up
clean:
	rm -f ./$(TARGET)
	rm -rf $(DIST)


#               #
# Build targets #
#               #

# Make dist directory
$(DIST):
	mkdir -p $(DIST)

# Compile c files
$(DIST)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile cpp files
$(DIST)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link files together
$(DIST)/$(TARGET): $(DIST) $(C_OBJ) $(CPP_OBJ)
	$(CXX) $(LDFLAGS) $(C_OBJ) $(CPP_OBJ) -o $(DIST)/$(TARGET)
	cp $(DIST)/$(TARGET) ./$(TARGET)
	