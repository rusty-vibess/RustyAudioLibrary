# Try to find an available C++ compiler, g++ being the first choice
CC = $(shell command -v g++ || command -v clang++ || command -v c++)

# Throw error if no C++ compiler
ifeq ($(CC),)
  $(error No C++ compiler found. Please install g++, clang++, or c++)
endif

# Set the compiler flags for debugging, warnings, and C++17 standard
CFLAGS = -g -Wall -std=c++17

# Set the linker flags for the libraries
LDFLAGS = -Llib -lportaudio

# Define the source, include, build, and binary directories
LIB_DIR = RustyAudioLib
LIB_SRC_DIR = $(LIB_DIR)/src
LIB_INC_DIR = $(LIB_DIR)/include
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
DIST_DIR = dist

# Find all library source files in the LIB_SRC_DIR and its subdirectories
LIB_SOURCES = $(shell find $(LIB_SRC_DIR) -name "*.cpp")

# Find all header file directories in the INC_DIR, LIB_INC_DIR, and their subdirectories
INC_DIRS = $(shell find $(INC_DIR) $(LIB_INC_DIR) -type d)

# Create a list of include flags for the compiler from the INC_DIRS variable
INC_FLAGS = $(addprefix -I, $(INC_DIRS))

# Create a list of object files for library source files by replacing the source file extension with .o and
# moving the path from LIB_SRC_DIR to BUILD_DIR
LIB_OBJECTS = $(patsubst $(LIB_SRC_DIR)/%.cpp, $(BUILD_DIR)/lib/%.o, $(LIB_SOURCES))

# Define the final static library name
LIB_NAME = libRustyAudioLib.a
LIB_PATH = $(BIN_DIR)/$(LIB_NAME)

# Default target
all: check_library clean $(LIB_PATH) dist

# Build the static library
$(LIB_PATH): $(LIB_OBJECTS) | $(BIN_DIR)
	ar rcs $@ $^

# Compile the library source files into object files
$(BUILD_DIR)/lib/%.o: $(LIB_SRC_DIR)/%.cpp
	@mkdir -p $(@D) # Create the build subdirectory if it doesn't exist
	$(CC) $(CFLAGS) $(INC_FLAGS) -c -o $@ $<

# Create the binary directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Create the distribution directory and copy the static library and header files
dist: $(LIB_PATH)
	mkdir -p $(DIST_DIR)
	cp $(LIB_PATH) $(DIST_DIR)
	cp -r $(LIB_INC_DIR) $(DIST_DIR)/include

# Remove the build, binary and distribution directories
clean:
	-rm -rf $(BUILD_DIR) $(BIN_DIR) $(DIST_DIR)

# Check if the library is available before building
check_library:
ifeq (,$(wildcard lib/libportaudio.*))
	$(error "PortAudio not built... Ensure to run build script before attempting to run RustyAudioLib makefile.")
	make clean
endif
