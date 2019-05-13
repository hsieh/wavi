
LD = $(CC)

SOURCE = Source/Wav.c

OBJS = Wav.o

TARGET = Wavi

JUNK = *.h.gch *.o a.out


CFLAGS += -I. -c

LDFLAGS += -o $(TARGET)

LIBS += -lpthread

INSTALL_PATH=~/bin/`echo $(TARGET) | tr '[A-Z]' '[a-z]'`

all: $(TARGET)

install: all
	install -m755 $(TARGET) $(INSTALL_PATH)

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) $(LIBS)

$(OBJS): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE)

$(SOURCE): $(HDRS)

clean:
	$(RM) $(JUNK)

cleanall: clean
	$(RM) $(TARGET)

.PHONY : all clean cleanall

