CC=gcc
CFLAGS=-I. -Ipipe_elements/. -Iinput_elements/. -Wno-pointer-sign -Wno-variadic-macros
LIBS=-lyajl

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

OBJS = \
       input_elements/read_from_filesystem.o \
       input_elements/input_batch.o \
       input_elements/filesystem-walker.o \
       pipe_elements/dump.o \
       pipe_elements/persistence.o \
       configuration.o \
       mlogc-ng.o \
       pipeline.o 

mlogc-ng: $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o pipe_elements/*.o input_elements/*.o mlogc-ng
