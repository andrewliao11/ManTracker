# ManTracker
Use Nitrogen6x and Boe-Bot Car to implement ManTracking task
## Concept
Build a car that can track person. We can set the car in the site that shouldn't be intruded. Once some people intrude that area, the car will track the person and beep.   
Use the techniques of skin detection by opencv, and transmit signal to the Boe-Bot car via Xbee. Beep once the car is near the guy! 
## Hardware Requirement 
- 1 Boe-Bot Car   
![](https://github.com/andrewliao11/ManTracker/blob/master/assets/boe-bot%20car.jpg?raw=true)
- 1 Nitrogen6x board with opencv
- 2 Xbee
- 1 Webcam  
   

## Software requirement
- [Opencv](http://opencv.org) on Nitrogen6x
      - [installatino guide](https://github.com/andrewliao11/ManTracker/blob/master/Propeller-Boe-Bot-Receiver/README.md) 
- [qt](https://www.qt.io) for compileing code for Boe-Bot car
      - [installatino guide](https://github.com/andrewliao11/ManTracker/blob/master/skin_detection/README.md) 


## Usage
For Nitrogen6x
  1. ~/Qt5.5.1/Tools/QtCreator/bin/qtcreator.sh // open qt  
  2. open the file in skin_detection/qt_opencv_2015.pro
  3. connect XBee and webcam to the Boe-Bot car board 
  4. compile in ARM mode  

For Boe-Bot car 
  1. sudo simpleide // open simpleide 
  2. open the file in Propeller-Boe-Bot-Receiver/Propeller-Boe-Bot-Receiver.side  
  3. make electric circuits like the following picture (for Ping, Beep, Xbee)  
  ![](http://learn.parallax.com/sites/default/files/content/propeller-c-tutorials/simple-devices/PING/simple-ping-wiring.png)
  ![](http://learn.parallax.com/sites/default/files/content/propeller-c-tutorials/simple-circuits/piezo-beep/circuit-PiezoBeep.jpg)
  ![](https://github.com/andrewliao11/ManTracker/blob/master/assets/XBee_BOE.png?raw=true)
  4. run the code!

## Algorithm behind ManTracker
The ManTracker uses skin detection, detect human skin color through webcam. Once the response of skin is higher than the threshold, the Xbee will transmit the corresponding signal to the Boe-Bot car. Once the car is near the intruder, the car will stop and beep.

skin detection algorithm:
1. access the pixel from the webcam and turn from the RGB color space to HSV space.
2. check if the color lies in the certain range
```
//pseudocode of skin detection
for(int i = 0; i < src.rows; i++) {
    for(int j = 0; j < src.cols; j++) {
        bool c = R3(src_H,src_S,src_V);
        if(!(a&&b&&c)) dst.ptr<Vec3b>(i)[j] = cblack;
		// the camera range in Nitrogen6x is 250~1280
        else{
            count ++;
            if (j<500&&j>250) count_left ++;
            else if (j<1280&&j>1030) count_right ++;
            else if(j<1030&&j>500) count_middle ++;
        }
    }
}
```
   
## Demo picture   
![](https://github.com/andrewliao11/ManTracker/blob/master/assets/demo.png?raw=true)   
   
## Reference 
- [使用OpenCV做膚色偵測 ( Skin Color Detection using OpenCV )](http://ccw1986.blogspot.tw/2012/11/opencvycbcr.html)
