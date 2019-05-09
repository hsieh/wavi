
LD = $(CC)

SOURCE = Source/Wav.c

OBJS = Wav.o

TARGET = Wavi

JUNK = *.h.gch *.o a.out


CFLAGS += -I. -c

LDFLAGS += -o $(TARGET)

LIBS += -lpthread


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

