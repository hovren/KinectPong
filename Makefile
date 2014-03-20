CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		src/main.o src/KinectPongGame.o

LIBS =

TARGET =	KinectPong

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)


ESPEAK_OBJS = src/EspeakInterface.o src/espeak_interface_test.o
EspeakInterface: $(ESPEAK_OBJS)  
	 $(CXX) -o espeak_interface_test $(ESPEAK_OBJS) -lportaudio -lespeak

PANTILT_OBJS = src/PanTiltInterface.o src/pan_tilt_interface_test.o
PanTiltInterface: $(PANTILT_OBJS)  
	 $(CXX) -o pan_tilt_interface_test $(PANTILT_OBJS) -lboost_system

ROBOREF_OBJS = src/RoboRef.o src/EspeakInterface.o src/PanTiltInterface.o src/roboref_test.o
RoboRef: $(ROBOREF_OBJS)  
	 $(CXX) -o roboref_test $(ROBOREF_OBJS) -lboost_system -lportaudio -lespeak -lopencv_core -lopencv_highgui

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
