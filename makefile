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
C_SRC := main.c file_parser.c
CPP_SRC := modules.cpp

# Test source file paths
TEST_C_SRC :=
TEST_CPP_SRC :=

#               #
# Env variables #
#               #

# Source file conversion

TEST_C_SRC := $(foreach wrd,$(TEST_C_SRC),$(SRC)/$(wrd))
TEST_CPP_SRC := $(foreach wrd,$(TEST_CPP_SRC),$(SRC)/$(wrd))

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
LDFLAGS := -L$(LIB)/lib -I$(LIB)/include -lsystemc

ifneq ($(shell uname -s), Darwin)
  LDFLAGS += -Wl,-rpath=$(LIB)/lib
endif

# Test build flags

TIDYFLAGS := -checks=bugprone,cppcoreguidelines,clang-analyzer,performance,portability,readability,misc
CHECKFLAGS := -fsyntax-only -Wall -Wextra -Wpedantic
CHECKFLAGS += -Wno-unused-but-set-variable -Wno-unused-parameter   # Benötigt, da sonst systemc immer Fehler wirft

ifdef ERROR_ON_LINT
  TIDYFLAGS += --warnings-as-errors=*
  CHECKFLAGS += -Werror
endif


#         #
# Targets #
#         #

.PHONY: all debug release tidy clean

# Default target
all: release

# Debug target
debug: CFLAGS += -g -Wall -Wextra -Wno-unused-but-set-variable -Wno-unused-parameter -D DEBUG_BUILD
debug: CXXFLAGS += -g -Wall -Wextra -Wno-unused-but-set-variable -Wno-unused-parameter -D DEBUG_BUILD
debug: LDFLAGS += -g -fsanitize=undefined,address
debug: $(DIST)/$(TARGET)

# Release target
release: CFLAGS += -O2
release: CXXFLAGS += -O2
release: LDFLAGS += -O2
release: $(DIST)/$(TARGET)

# Test targets
test: test_c test_cpp

test_c: CFLAGS += -g -Wall -Wextra
test_c: $(DIST)/test_c

test_cpp: CXXFLAGS += -g -Wall -Wextra
test_cpp: LDFLAGS += -g
test_cpp: $(DIST)/test_cpp

# Tidy targets
tidy:: 
	clang $(CHECKFLAGS) $(C_SRC) $(CFLAGS)
tidy:: 
	clang++ $(CHECKFLAGS) $(CPP_SRC) $(CXXFLAGS)
tidy::
	clang-tidy $(C_SRC) $(TIDYFLAGS) -- $(CFLAGS)
tidy::
	clang-tidy $(CPP_SRC) $(TIDYFLAGS) -- $(CXXFLAGS)

# Clean up
clean:
	rm -f ./$(TARGET) ./test_c ./test_cpp
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


#              #
# Test targets #
#	             #

# Compile c test files
$(DIST)/test_c: $(TEST_C_SRC)
ifdef TEST_C_SRC
	mkdir -p $(DIST)
	$(CC) $(CFLAGS) $(TEST_C_SRC) -o $@
	cp $(DIST)/test_c ./test_c
endif

# Compile cpp test files
$(DIST)/test_cpp: $(TEST_CPP_SRC)
ifdef TEST_CPP_SRC
	mkdir -p $(DIST)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(TEST_CPP_SRC) -o $@
	cp $(DIST)/test_cpp ./test_cpp
endif