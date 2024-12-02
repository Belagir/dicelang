
# Gabi's Makefile v2.1

# ---------------- Configuration -----------------------------------------------

## Name of the project. This will be the name of the executable placed in the
## executable directory.
PROJECT_NAME = dicelang
## Root source directory. Contains the c implementation files.
SRC_DIR = src
## Root include directory. Contains the c header files. Passed with -I to
## the compiler.
INC_DIR = inc unstandard/inc
## Build diectory. Will contain object and binary files linked in the final
## executable
OBJ_DIR = build
OBJ_LIB_DIR = unstandard/build
## Executable directory. Contains the final binary file.
EXC_DIR = bin

## compiler
CC = gcc-13
## compilation flags
CFLAGS += -Wall -Wextra -Wpedantic -fanalyzer  -Werror
CFLAGS += -Wno-error=unused-function
CFLAGS += -g -std=c2x
## linker flags
LFLAGS += -lm

# additional flags for defines
DFLAGS +=

# --------------- Internal variables -------------------------------------------

## absolute path to executable name
TARGET = $(EXC_DIR)/$(PROJECT_NAME)
## list of all c files without their path
SRC := $(notdir $(shell find $(SRC_DIR) -name *.c))
## list of all duplicate c files to enforce uniqueness of filenames
DUPL_SRC := $(strip $(shell echo $(SRC) | tr ' ' '\n' | sort | uniq -d))
## list of all target object files with their path
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c, %.o, $(SRC))) $(shell find $(OBJ_LIB_DIR)/ -name *.o)

## makefile-managed directories
BUILD_DIRS = $(EXC_DIR) $(OBJ_DIR)

## additional compilation option for includes
ARGS_INCL = $(addprefix -I, $(INC_DIR))

# --------------- Make directories searching -----------------------------------

## where to find c files : all unique directories in SRC_DIR which contain a c
## file
vpath %.c $(sort $(dir $(shell find $(SRC_DIR) -name *.c)))

# --------------- Rules --------------------------------------------------------

.PHONY: all check clean count_lines

# -------- compilation -----------------

all: check $(BUILD_DIRS) $(TARGET) | count_lines

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LFLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $? -o $@ $(ARGS_INCL) $(CFLAGS) $(DFLAGS)

# -------- dir spawning ----------------

$(BUILD_DIRS):
	mkdir -p $@

# -------- check -----------------------

check:
ifdef DUPL_SRC
	$(error duplicate filenames: $(DUPL_SRC))
endif

# -------- cleaning --------------------

clean:
	rm -Rf $(BUILD_DIRS)

# -------- luxury ----------------------

count_lines:
ifneq (, $(shell which cloc))
	@cloc --hide-rate --quiet $(SRC_DIR) $(INC_DIR)
endif

# ----  ----  ----  ----  ----  ----

 # Senteurs d'été doux
 # Claquement brefs des touches
 # Segmentation fault
