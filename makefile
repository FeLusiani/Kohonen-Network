### MAKEFILE CONFIGURATION

# Compiler and flags
CC		= gcc
CFLAGS	= -Wall -Wextra -std=gnu99
ALLEG_FLAGS = -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_image

# Directories
BINDIR	= bin
SRCDIR	= src
OBJDIR	= build
HDRDIR	= src/include
CONFDIR	= conf

# Files
EXECUTABLE	= main
UTILS		=


### MAKEFILE EXECUTION

# utils objects
UTILS_OBJ = $(addsuffix .o, $(addprefix $(OBJDIR)/,$(UTILS)))

.SECONDARY: $(OBJS) # altrimenti vengono considerati files intermedi e cancellati da make


# Build generic .o file from .c file
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HDRDIR)/*.h $(CONFDIR)/*.h
	$(CC) $(CFLAGS) -c $< -o $@ -I$(CONFDIR)

# Build generic executable
$(BINDIR)/%: $(OBJDIR)/%.o $(UTILS_OBJ) $(HDRDIR)/*.h $(CONFDIR)/*.h
	@echo "🚧 Building..."
	$(CC) -o $@ $(filter %.o,$^) $(ALLEG_FLAGS)



# Builds the executables: default rule
.PHONY: all
all: $(BINDIR)/$(EXECUTABLE)

# Compile and run
.PHONY: run
run: all
	clear
	@echo "🚀 Executing..."
	./$(BINDIR)/$(EXECUTABLE)

# Delete executables and objects files
.PHONY: clean
clean:
	@echo "🧹 Clearing..."
	-rm $(BINDIR)/* $(OBJDIR)/*
