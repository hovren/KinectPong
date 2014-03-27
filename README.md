KinectPong
==========

Play Pong using parts of your body as paddle!

This game was created during a short sprint of one working week by Hannes Ovrén and Marcus Wallenberg, at the [Computer Vision Laboratory](http://www.cvl.isy.liu.se/) of Linköping University, Sweden.
The goal was to demonstrate what you can do with fairly simple computer vision and image processing methods.

The only thing you need to play is a Kinect sensor, and some friends!

However, the game was built to make use of *Eddie the embodied*, a research platform used in the [Embodied Visual Object Recognition](http://www.liu.se/forskning/foass/tidigare-foass/per-erik-forssen/EVOR?l=en).
If available, Eddie acts as a referee.

How to play
-----------------------
1. After initialisation, the first view shows what the Kinect sensor "sees".
2. Press <kbd>Return</kbd> to advance to the next screen (which looks similar) where faces are detected.
3. If the game has not started automatically, press <kbd>Return</kbd> again.
4. During play, the player silhouette is shown in green, and the contact surface in white.

The contact surface is calculated as the frontmost (closest to sensor) 20% of your body.

Building
-----------------------
Make sure you install all of the requirements

* OpenCV (Tested on 2.4.7)
* Boost
* libfreenect
* SDL2
* SDL2_image
* SDL2_ttf
* portaudio (for robot referee)
* espeak (for robot referee)

Then compile using `make`.

License
---------------------------
All code available under the GPL version 3.
See LICENSE file for the full licensen.
