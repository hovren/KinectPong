CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -I/usr/include/libfreenect

OBJS =		src/main.o src/KinectPongGame.o src/KinectInput.o

LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lfreenect -lSDL2 -lSDL2_image

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
	 
PLAYERIM_OBJS = src/PlayerImageProcessor.o src/KinectInput.o src/player_image_test.o
PlayerImageProcessor: $(PLAYERIM_OBJS)  
	 $(CXX) -o player_image_test $(PLAYERIM_OBJS) -lopencv_core -lopencv_highgui -lopencv_imgproc -lfreenect -lopencv_objdetect

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
