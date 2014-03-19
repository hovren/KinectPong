CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		src/main.o src/KinectPongGame.o

LIBS =

TARGET =	KinectPong

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
