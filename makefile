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
TARGET = r5mm

# Directory paths
LIB = $(SYSTEMC_HOME)
SRC = ./src
DIST = ./dist


#               #
# Env variables #
#               #

# Source file settings

C_SRC = main.c
CPP_SRC = modules.cpp

C_OBJ = $(C_SRC:.c=.o)
CPP_OBJ = $(CPP_SRC:.cpp=.o)

# Compiler settings

CX = clang
CXFLAGS = -std=c17

CXX = clang++
CXXFLAGS_OBJ = -std=c++14 -I$(LIB)/include
CXXFLAGS_LINK = -std=c++14 -I$(LIB)/include -L$(LIB)/lib -lsystemc


#         #
# Targets #
#         #

.PHONY: all debug release clean

# Default target
all: debug

# Debug target
debug: CXFLAGS += -g -v
debug: CXXFLAGS_OBJ += -g -v
debug: CXXFLAGS_LINK += -g -v
debug: $(DIST)/$(TARGET)

# Release target
release: CXFLAGS += -O2
release: CXXFLAGS_OBJ += -O2
release: CXXFLAGS_LINK += -O2
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
	$(CX) $(CXFLAGS) -c $< -o $@

# Compile cpp files
$(DIST)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS_OBJ) -c $< -o $@

# Link files together
$(DIST)/$(TARGET): $(DIST) $(DIST)/$(C_OBJ) $(DIST)/$(CPP_OBJ)
	$(CXX) $(CXXFLAGS_LINK) $(DIST)/$(C_OBJ) $(DIST)/$(CPP_OBJ) -o $(DIST)/$(TARGET)
	cp $(DIST)/$(TARGET) ./$(TARGET)
	