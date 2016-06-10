# Install Qt
How to dinstall Qt:   
```
wget http://www1.ee.nthu.edu.tw/ee240500/notes/lab12/qt-opensource-linux-x86-5.5.1.run
chmod 755 ./qt-opensource-linux-x86-5.5.1.run
./qt-opensource-linux-x86-5.5.1.run // pop out the installation 
gedit ./Qt5.5.1/Tools/QtCreator/bin/qtcreator.sh
```

modify ```#! /bin/sh``` to ```#! /bin/bash``` and insert the following line to the second line:   
```source /opt/poky/2.0.2/environment-setup-cortexa9hf-vfp-neon-poky-linux-gnueabi```

Start Qt   
```
~/Qt5.5.1/Tools/QtCreator/bin/qtcreator.sh
```
