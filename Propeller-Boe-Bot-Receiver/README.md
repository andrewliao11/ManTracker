# Install Opencv on Nitrogen6x 
How to install opencv on Nitrogen6x:   

```
mkdir $PRJROOT/opencv
sudo apt-get install -y cmake // install cmake
cd $PRJROOT/opencv
wget http://www1.ee.nthu.edu.tw/ee240500/notes/lab3/opencv-2.4.11.zip
unzip opencv-2.4.11.zip
wget http://www1.ee.nthu.edu.tw/ee240500/notes/lab3/arm-gnueabi.toolchain.cmake // replace the original arm-gnueabi.toolchain.cmake
export PKG_CONFIG_PATH=$PRJROOT/rootfs/usr/lib/pkgconfig:$PRJROOT/rootfs/usr/share/pkgconfig
export PKG_CONFIG_LIBDIR=${PKG_CONFIG_PATH}
export PKG_CONFIG_SYSROOT_DIR=$PRJROOT/rootfs
export PKG_CONFIG_ALLOW_SYSTEM_LIBS=1
mkdir $PRJROOT/opencv/opencv-2.4.11/build
cd $PRJROOT/opencv/opencv-2.4.11/build
cmake -DCMAKE_INSTALL_PREFIX=$PRJROOT/rootfs/usr/local -DCMAKE_TOOLCHAIN_FILE=../platforms/linux/arm-gnueabi.toolchain.cmake -DENABLE_NEON=ON ../
make -j 2
sudo make install
```
the screenshot should be like this:
![](https://github.com/andrewliao11/ManTracker/blob/master/assets/opencv-make-install%20-%20Copy.png?raw=true)
