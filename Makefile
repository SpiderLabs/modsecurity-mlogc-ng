CC=gcc
CFLAGS=-I. -Ipipe_elements/. -Iinput_elements/. -Wno-pointer-sign -Wno-variadic-macros -O0 -g
LIBS=-lyajl -lcurl -lssl -lcrypto

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

OBJS = \
       configuration.o \
       input_elements/read_from_filesystem.o \
       input_elements/input_batch.o \
       input_elements/filesystem-walker.o \
       mlogc-ng.o \
       pipeline.o \
       pipe_elements/dump.o \
       pipe_elements/persistence.o \
       pipe_elements/send_to_server.o

mlogc-ng: $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o pipe_elements/*.o input_elements/*.o mlogc-ng
