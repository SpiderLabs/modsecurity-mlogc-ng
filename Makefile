CC=gcc
CFLAGS=-I. -Ipipe_elements/.
LIBS=-lyajl

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

OBJS = \
       pipe_elements/dump.o \
       pipe_elements/persistence.o \
       configuration.o \
       filesystem-walker.o \
       input_batch.o \
       mlogc-ng.o \
       pipeline.o 
#       json-conversion.o \
#       persistence.o \

mlogc-ng: $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o pipe_elements/*.o mlogc-ng
