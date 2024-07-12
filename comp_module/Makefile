# CC:=ccache $(CC)
CFLAGS+= -Wall -Werror -std=gnu99 -g 
LDFLAGS=-lpthread -lm

# HW=prgsem
BINARIES=my_comp_module

   
all: $(BINARIES)

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

my_comp_module: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(BINARIES) $(OBJS)