#!/bin/sh


rm -rf php_opencv
rm -f opencv.tar.gz

wget -O opencv.tar.gz https://github.com/midoks/opencv/archive/master.tar.gz
tar -zxvf opencv.tar.gz
mv opencv-master php_opencv
cd php_opencv


/usr/local/php71/bin/phpize
./configure --with-php-config=/usr/local/php71/bin/php-config --with-opencv --enable-opencv_d=yes

make && make install