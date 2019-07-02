CPPFLAGS = -I ./include
SRCDIR = ./src
OBJDIR = ./obj

src = $(wildcard $(SRCDIR)/*.c)
obj = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(src))
target = ./bin/Spider

$(target):$(obj) 
	gcc $(obj) -o $@ $(CPPFLAGS) -g

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	gcc -c $< -o $@ $(CPPFLAGS) -g

.PHONY:clean
clean:
	rm -f $(target) $(obj)
