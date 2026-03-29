CC = gcc
CFLAGS = -g -Wall -std=c11
TARGET = app
SRCS = vm_opcodes.c vm_const_table.c vm_stack.c vm_state.c vm.c table.c vm_globals.c vm_frame.c
OBJDIR = obj
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
RM = rm -f

.PHONY: all clean

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET)
	$(RM) -r $(OBJDIR)