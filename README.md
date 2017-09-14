# opencv
OpenCV for PHP Extend


# 安装
```
wget https://github.com/midoks/opencv/archive/master.tar.gz

tar -zxvf master.tar.gz
cd opencv-master

/usr/local/php71/bin/phpize
./configure --with-php-config=/usr/local/php71/bin/php-config --with-opencv
make && make install

extension=opencv.so

```
