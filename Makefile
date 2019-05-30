CROSSCOMPILE := arm-hisiv300-linux-

CFLAGS 	:= -Wall -O2 -c
CFLAGS  += -I$(PWD)/include

LDFLAGS := -lm -lpthread

CC 	:= $(CROSSCOMPILE)gcc
LD 	:= $(CROSSCOMPILE)ld

OBJS := main.o \
			common/logger.o        \
			tcp_server/tcp_server.o


all: $(OBJS)
	$(CC) $(LDFLAGS) -o main $^

clean:
	rm -f main
	rm -f $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $<

