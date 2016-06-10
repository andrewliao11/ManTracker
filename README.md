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
  
## Demo 
![](https://github.com/andrewliao11/ManTracker/blob/master/assets/demo.png?raw=true)
  
