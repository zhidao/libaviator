PREFIX=$(HOME)/usr

CC = gcc
CFLAGS = -Wall -fPIC -O3 -funroll-loops -I.
LD = ld
LDFLAGS = -shared

OBJ = aviator.o

DLIB = libaviator.so

TARGET = $(DLIB) sample

all: $(TARGET)
$(DLIB): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIB)
.c.o:
	$(CC) $(CFLAGS) -c $<
sample: sample.c $(DLIB)
	$(CC) $(CFLAGS) -L$(PREFIX)/lib -L. -o $@ $< -laviator
test: test.c $(DLIB)
	$(CC) $(CFLAGS) -L$(PREFIX)/lib -L. -o $@ $<
install:
	install -m 644 *.h $(PREFIX)/include
	install -m 755 $(DLIB) $(PREFIX)/lib
clean:
	-rm -f *.o *~ $(TARGET)
