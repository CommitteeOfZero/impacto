NAME = libatrac9

CC = emcc
AR = emar
SFLAGS = -O2
CFLAGS = -Wall -Wextra -std=c99

SRCDIR = src
OBJDIR = obj
BINDIR = bin

STATIC_OBJDIR = $(OBJDIR)_static
SRCS = $(wildcard $(SRCDIR)/*.c)
STATIC_OBJS = $(SRCS:$(SRCDIR)/%.c=$(STATIC_OBJDIR)/%.bc)

STATIC_NAME = $(BINDIR)/$(NAME).a

MKDIR = mkdir -p
RM = rm -f
RMDIR = rm -df

all: static
static: create_static_dir create_bin_dir $(STATIC_NAME)

create_static_dir:
	@$(MKDIR) $(STATIC_OBJDIR)
	
create_bin_dir:
	@$(MKDIR) $(BINDIR)

$(STATIC_NAME): $(STATIC_OBJS)
	$(AR) rcs $@ $^

$(STATIC_OBJS): $(STATIC_OBJDIR)/%.bc : $(SRCDIR)/%.c
	$(CC) $(SFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(STATIC_OBJS) $(STATIC_NAME)
	-@$(RMDIR) $(STATIC_OBJDIR) $(BINDIR) 2>/dev/null || true

.PHONY: all static create_static_dir create_bin_dir clean