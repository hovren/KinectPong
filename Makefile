CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		src/main.o src/KinectPongGame.o

LIBS =

TARGET =	KinectPong

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)


ESPEAK_OBJS = src/EspeakInterface.o src/espeak_interface_test.o
EspeakInterface: $(ESPEAK_OBJS)  
	 $(CXX) -o espeak_interface_test $(ESPEAK_OBJS) -lportaudio -lespeak

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
