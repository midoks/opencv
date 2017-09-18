#!/bin/sh


rm -rf php_opencv
wget -O opencv.tar.gz https://github.com/midoks/opencv/archive/master.tar.gz
mv opencv-master php_opencv
cd php_opencv


/usr/local/php71/bin/phpize
./configure --with-php-config=/usr/local/php71/bin/php-config

make && make install