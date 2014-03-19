CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -I/usr/include/libfreenect

OBJS =		src/main.o src/KinectPongGame.o src/KinectDevice.o src/KinectSource.o

LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lfreenect

TARGET =	KinectPong

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
