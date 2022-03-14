CC=gcc
CFLAGS=-Wall
DEPS = libbmp.h mandelbrot.h color.h
OBJ = libbmp.o mandelbrot.o color.o main.o
LIBS = -lm

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS) 
 
main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(obj)*.o
