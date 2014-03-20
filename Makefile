CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -I/usr/include/libfreenect

OBJS =		src/main.o src/KinectPongGame.o src/KinectInput.o

LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lfreenect -lSDL2 -lSDL2_image

TARGET =	KinectPong

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)


ESPEAK_OBJS = src/EspeakInterface.o src/espeak_interface_test.o
EspeakInterface: $(ESPEAK_OBJS)  
	 $(CXX) -o espeak_interface_test $(ESPEAK_OBJS) -lportaudio -lespeak

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
