### MAKEFILE CONFIGURATION

# Compiler and flags
CC       := gcc
CFLAGS := -Wall -Wextra -std=gnu99

# Directories
BIN     := bin
SRC     := src
OBJDIR	:= build
HDRDIR	:= src/include
CONF	:= conf

# Files
EXECUTABLE	:= main
UTILS		:=


### MAKEFILE EXECUTION

# utils objects
UTILS_OBJ  = $(addsuffix .o, $(addprefix $(OBJDIR)/,$(UTILS)))

# Build generic executable
$(BINDIR)/%: $(OBJDIR)/%.o $(UTILS_OBJ) $(HDRDIR)/*.h $(CONF)/*.h
	@echo "🚧 Building..."
	$(CC) $(CFLAGS) -o $@ $(filter %.o,$^)

# Build generic .o file from .c file
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HDRDIR)/*.h $(CONF)/*.h
	$(CC) $(CFLAGS) -c $< -o $@


# Builds the executables: default rule
.PHONY: all
all : $(BIN)/$(EXECUTABLE)

# Compile and run
.PHONY: run
run: all
	clear
	@echo "🚀 Executing..."
	./$(BIN)/$(EXECUTABLE)

# Delete executables and objects files
.PHONY: clean
clean:
	@echo "🧹 Clearing..."
	-rm $(BIN)/* $(BUILD)/*
