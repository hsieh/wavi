
SOURCE = Source/Wav.c

OBJS = Wav.o

TARGET = Wavi

JUNK = *.h.gch *.o a.out


CFLAGS += --machine-64 -I. -c

LDFLAGS += -melf_x86_64 \
	   -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
	   -L/usr/lib/gcc/x86_64-linux-gnu/6/ \
	   -L/usr/lib/gcc/x86_64-linux-gnu/ \
	   -L/lib/ -L/usr/lib/ \
	   -o $(TARGET)

LIBS += /usr/lib/x86_64-linux-gnu/crt1.o \
	/usr/lib/x86_64-linux-gnu/crti.o \
	/usr/lib/x86_64-linux-gnu/crtn.o \
	-lpthread -lc


all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) $(LIBS)

$(OBJS): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE)

$(SOURCE): $(HDRS)

clean:
	$(RM) $(JUNK)

clean_all: clean
	$(RM) $(TARGET)

.PHONY : all clean clean_all

