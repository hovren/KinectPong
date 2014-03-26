CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -I/usr/include/libfreenect -I/usr/include/SDL2 -L/usr/local/lib

OBJS =		src/main.o src/KinectPongGame.o src/KinectInput.o src/GameBoard.o src/Player.o src/states/intro.o src/states/kinectview.o src/states/playing.o src/states/WaitForPlayerState.o \
 src/PlayerImageProcessor.o src/RoboRef.o src/EspeakInterface.o src/PanTiltInterface.o src/states/playserve.o src/states/playscore.o src/states/reset.o src/states/finalscore.o

LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lboost_system \
-lfreenect -lSDL2 -lSDL2_image -lSDL2_ttf -lportaudio -lespeak

TARGET =	KinectPong

#This definition is not used anymore
#DEFS = -DBUILD_ROBOREF
DEFS = 

$(TARGET):	$(OBJS)
	$(CXX) $(DEFS) -o $(TARGET) $(OBJS) $(LIBS)


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

KINECTTEST_OBJS = src/kinect_test.o src/KinectInput.o
kinect_test: $(KINECTTEST_OBJS)  
	 $(CXX) -o kinect_test $(KINECTTEST_OBJS) -lopencv_core -lopencv_highgui -lfreenect

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
