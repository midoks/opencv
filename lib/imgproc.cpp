/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: midoks                                                       |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

extern "C"{
  #ifdef HAVE_CONFIG_H
  #include "config.h"
  #endif

  #include "php.h"
  #include "php_ini.h"
  #include "ext/standard/info.h"
  #include "ext/standard/php_var.h"
  #include "php_opencv.h"
}


#include "opencv/cv.h"
#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "opencv2/features2d.hpp"
//#include "opencv2/xfeatures2d.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <math.h>
#include <string>

using namespace cv;
using namespace std;




/** {{{ proto php_opencv_imgproc
*/
typedef struct _php_opencv_imgproc
{
  char *src_path;
  long width;
  long height;
} php_opencv_imgproc_t;
/* }}} */


php_opencv_imgproc_t * opencv_imgproc_var;
Mat opencv_imgproc_src_im;
Mat opencv_imgproc_dst_im;
static CascadeClassifier face_cascade;

zend_class_entry * opencv_imgproc_ce;


ZEND_BEGIN_ARG_INFO_EX(opencv_imgproc_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(opencv_imgproc_construct_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, src)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(opencv_imgproc_setimage_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, src)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(opencv_imgproc_writeimage_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(opencv_imgproc_tclip_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, width)
  ZEND_ARG_INFO(0, height)
ZEND_END_ARG_INFO()


/* {{{ private methods
*/

int opencv_imgproc_detect_face(Mat &img TSRMLS_DC){

  //macosx always wrong
  int cmp = strcmp(PHP_OS, "Darwin");
  if ( !cmp ){
    return -1;
  }


  vector<Rect> faces;
  Mat img_gray;
  int face_size;
  int Y;

  cvtColor( img, img_gray, CV_BGR2GRAY );
  equalizeHist( img_gray, img_gray );

  face_cascade.detectMultiScale( img_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
  face_size = faces.size();

  if ( face_size > 0 )
  {
    Y = faces[face_size -1].y - faces[face_size -1].height / 2;
    if ( Y > img.size().height / 2 ) //fix
    {
      return -1;
    } else {
      return Y < 0 ? 0 : Y;
    }
  } else {
    return -1;
  }
}


void print_zstr(zend_string *key){
  php_printf("<!--[zend_string](");
  PHPWRITE(ZSTR_VAL(key), ZSTR_LEN(key));
  php_printf(")-->\n");
}


int opencv_imgproc_detect_character(Mat &img TSRMLS_DC){

  //zend_string *s = php_get_uname('s');
  //print_zstr(s);
  //mac have some question!
  int cmp = strcmp(PHP_OS, "Darwin");
  if ( !cmp ){
    return -1;
  }

  int start_x = 0;  //特征点X坐标开始位置 
  int end_x = 0;    //特征点X坐标结束位置
  int section_index = 0;    //Y坐标段数字索引
  map<int,int> section_num; //每个Y坐标段中特征点的数量
  int total = 0;    //总共特征点数量
  int avg = 0;      //每个Y坐标段的平均特征点数量
  int con_num = 4;  //需要连续的阀值 
  int flag = 0;
  int counter = 0;
  int Y = 0;

  vector<KeyPoint> keypoints;
  //cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create();
  cv::Ptr<cv::FastFeatureDetector> detector = cv::FastFeatureDetector::create();

  if( detector.empty())
  {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can not create detector or descriptor exstractor or descriptor matcher of given types！");
    return -1;
  }

  start_x = 0;
  end_x = img.size().width;

  detector->detect( img, keypoints );

  for (vector<KeyPoint>::iterator i = keypoints.begin(); i != keypoints.end(); i++)
  {
    if (i->pt.x > start_x && i->pt.x < end_x)
    {
      section_index = (int)ceil(i->pt.y / 10);
      section_num[section_index] = section_num[section_index] + 1;
      total = total + 1;
    }
  }

  if (section_num.size() == 0)
  {
    return -1;
  }
  avg = total / section_num.size();

  //检测特征点分布是否均匀
  int slice_total = 10 ; 
  int slice_num = section_num.size() / slice_total;
  int slice_counter = 0;
  for (int m = 0; m < slice_total; m++)
  {
    for (int n = m * slice_num; n < (m+1) * slice_num; n++)
    {
      if ( section_num[n] >= avg )
      {
        slice_counter++;
        break;
      }
    }
  }
  if (slice_counter >= slice_total)
  {
    return -1;
  }

  //检测特征点主要分布区域[找最开始连续大于avg的Y]
  for (map<int,int>::iterator i = section_num.begin(); i != section_num.end(); i++)
  {
    if (i->second >= avg && flag == 0)
    {
      counter++;
    } else {
      counter = 0;
    }
    if (counter >= con_num && flag == 0)
    {
      Y = i->first;
      flag = 1;
    }
  }
  if (Y > con_num && Y  < img.size().height / 4)
  {
    return (Y - con_num - 11) * slice_total < 0 ? 0 : (Y - con_num - 11) * slice_total ;//fix
  } else if (Y > con_num){
    return (Y - con_num) * slice_total;
  }
  return Y * slice_total;
}
/*}}} */


/** {{{ proto \OpenCV\ImgProc::__construct(string $source)
*/
PHP_METHOD(opencv_imgproc, __construct) {
  
  char *src = NULL;
  zval *self = NULL;
  size_t src_len;

  php_opencv_imgproc_t t = {0};
  opencv_imgproc_var = &t;
  opencv_imgproc_var->src_path = NULL;

  if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|s", &src, &src_len) == FAILURE) {
    return;
  }

  if (src){
    self = getThis();
    
    opencv_imgproc_var->src_path = src;

    opencv_imgproc_src_im = imread(src);
    opencv_imgproc_dst_im = imread(src);

    if (!opencv_imgproc_src_im.data) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "fail to load image from %s", src);
    }
  }
}
/* }}} */

/** {{{ proto \OpenCV\ImgProc::setImage(string $imgPath)
*/
PHP_METHOD(opencv_imgproc, setImage) {
  char *src = NULL;
  size_t src_len;
  if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &src, &src_len) == FAILURE) {
    return;
  }

  if (src){
    opencv_imgproc_var->src_path = src;
    opencv_imgproc_src_im = imread(src);
    opencv_imgproc_dst_im = imread(src);
  }

  RETURN_TRUE;
}
/* }}} */


/** {{{ proto \OpenCV\ImgProc::tclip(int $width, int $height)
*/
PHP_METHOD(opencv_imgproc, tclip) {
  long dst_width, dst_height;
  if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "ll", &dst_width, &dst_height) == FAILURE) {
    return;
  }

  //check,import
  if (!opencv_imgproc_var->src_path){
    RETURN_FALSE;
  }

  opencv_imgproc_var->width = dst_width;
  opencv_imgproc_var->height = dst_height;

  Size tmp_size;
  float ratio = 0.0, ratio_w = 0.0, ratio_h = 0.0;
  int clip_top = 0,clip_bottom = 0,clip_left = 0, clip_right = 0;

  if (opencv_imgproc_src_im.size().width * 3 <= opencv_imgproc_src_im.size().height) {
    ratio = (float)dst_width / opencv_imgproc_src_im.size().width;
    tmp_size = Size((int)(opencv_imgproc_src_im.size().width * ratio), (int)(opencv_imgproc_src_im.size().height * ratio));
    resize(opencv_imgproc_src_im, opencv_imgproc_dst_im, tmp_size);
    clip_top = 0;
    clip_bottom = dst_width - opencv_imgproc_dst_im.size().height;
    clip_left = 0;
    clip_right = 0;
    opencv_imgproc_dst_im.adjustROI(clip_top, clip_bottom, clip_left, clip_right);
    RETURN_TRUE;
  }

  ratio = 200.0 / (float)opencv_imgproc_src_im.size().width;
  tmp_size = Size((int)(opencv_imgproc_src_im.size().width * ratio), (int)(opencv_imgproc_src_im.size().height * ratio));
  opencv_imgproc_dst_im = Mat(tmp_size, CV_32S );
  resize(opencv_imgproc_src_im, opencv_imgproc_dst_im, tmp_size);


  int result = opencv_imgproc_detect_face( opencv_imgproc_src_im TSRMLS_CC);
  opencv_show("opencv_imgproc_detect_face:%d\r\n", result);
  if (result == -1) {
      result = opencv_imgproc_detect_character( opencv_imgproc_src_im TSRMLS_CC);
  }
  opencv_show("opencv_imgproc_detect_character:%d\r\n", result);

  ratio_w = (float)dst_width / opencv_imgproc_src_im.size().width;
  ratio_h = (float)dst_height / opencv_imgproc_src_im.size().height;
  
  if (ratio_w > ratio_h) {
    ratio = ratio_w;
  } else {
    ratio = ratio_h;
  }

  opencv_show("ratio:%f\r\n", ratio);
  opencv_show("width_src:%d,height_src:%d\r\n", opencv_imgproc_src_im.size().width, opencv_imgproc_src_im.size().height);
  opencv_show("width_r:%F,height_r:%F\r\n", ratio_w, ratio_h);
  opencv_show("width_dst:%d,height_dst:%d\r\n", dst_width, dst_height);
  opencv_show("width_make_dst:%d,height_make_dst:%d\r\n", (int)(opencv_imgproc_src_im.size().width * ratio), 
            (int)(opencv_imgproc_src_im.size().height * ratio));

  result = result == -1 ? -1 : (int)((float)result * ratio);
  
  tmp_size = Size((int)(opencv_imgproc_src_im.size().width * ratio), (int)(opencv_imgproc_src_im.size().height * ratio));
  opencv_imgproc_dst_im = Mat(tmp_size, CV_32S);
  resize(opencv_imgproc_src_im, opencv_imgproc_dst_im, tmp_size);
  
  //原图片 宽度小于高度
  if (ratio_w > ratio_h) {
    if (result == -1) {
      opencv_show("c1\r\n");
      clip_top = -((opencv_imgproc_dst_im.size().height - dst_height) / 2);
      clip_bottom = clip_top;
    } else {
      if (opencv_imgproc_dst_im.size().height - result >= dst_height) {
        opencv_show("c2\r\n");
        clip_top = -result;
        clip_bottom = -(opencv_imgproc_dst_im.size().height - result - dst_height);
      } else {
        opencv_show("c3\r\n");
        clip_top = -(opencv_imgproc_dst_im.size().height - dst_height);
      }
    }
  } else {
    opencv_show("c4\r\n");
    clip_left = -((opencv_imgproc_dst_im.size().width - dst_width) / 2);
    clip_right = clip_left;
  }

  opencv_show("clip_top:%d,clip_bottom:%d,clip_left:%d,clip_right:%d\r\n", clip_top, clip_bottom, clip_left, clip_right);
  opencv_imgproc_dst_im.adjustROI(clip_top, clip_bottom, clip_left, clip_right);

  RETURN_TRUE;
}
/* }}} */


/** {{{ proto \OpenCV\ImgProc::writeImage(string $filename)
*/
PHP_METHOD(opencv_imgproc, writeImage) {

  char *filename = NULL;
  size_t filename_len;
  if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &filename, &filename_len) == FAILURE) {
    return;
  }

  imwrite(filename, opencv_imgproc_dst_im);
  RETURN_TRUE;
}
/* }}} */


/** {{{ proto public \OpenCV\ImgProc::__desctruct(void)
*/
PHP_METHOD(opencv_imgproc, __destruct) {

  // if (php_opencv_imgproc->src_path) {
  //   efree(php_opencv_imgproc->src_path);
  //   php_opencv_imgproc->src_path = NULL;
  // }

  // php_opencv_imgproc = NULL;
}
/* }}} */

/** {{{ proto private \OpenCV\ImgProc::__sleep(void)
*/
PHP_METHOD(opencv_imgproc, __sleep) {
}
/* }}} */

/** {{{ proto private \OpenCV\ImgProc::__wakeup(void)
*/
PHP_METHOD(opencv_imgproc, __wakeup) {
}
/* }}} */

/** {{{ proto private \OpenCV\ImgProc::__clone(void)
*/
PHP_METHOD(opencv_imgproc, __clone) {
}
/* }}} */


/** {{{ opencv_imgproc_methods
*/
zend_function_entry opencv_imgproc_methods[] = {
	PHP_ME(opencv_imgproc, __construct, opencv_imgproc_construct_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(opencv_imgproc, setImage,    opencv_imgproc_setimage_arginfo,   ZEND_ACC_PUBLIC)
  PHP_ME(opencv_imgproc, tclip,       opencv_imgproc_tclip_arginfo,      ZEND_ACC_PUBLIC)
  PHP_ME(opencv_imgproc, writeImage,  opencv_imgproc_writeimage_arginfo, ZEND_ACC_PUBLIC)
  PHP_ME(opencv_imgproc, __destruct,          NULL,                      ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
  PHP_ME(opencv_imgproc, __clone,             NULL,                      ZEND_ACC_PRIVATE)
  PHP_ME(opencv_imgproc, __sleep,             NULL,                      ZEND_ACC_PRIVATE)
  PHP_ME(opencv_imgproc, __wakeup,            NULL,                      ZEND_ACC_PRIVATE)
	{NULL, NULL, NULL}
};
/* }}} */


/** {{{ OPENCV_STARTUP_FUNCTION
*/
OPENCV_STARTUP_FUNCTION(imgproc) {

  char *face_xml_path;
  spprintf(&face_xml_path, 0, "%s/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml", OPENCV_G(root_path));
  
  opencv_show("imgproc:%s\r\n", opencv_globals.root_path);

  string config_path = face_xml_path;
  if (!face_cascade.load( config_path ) ){
    opencv_show("load face_xml_path fail!\r\n");
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "can not load classifier file！%s", config_path.c_str());
  }

  opencv_show("load face_xml_path ok!\r\n");

  if (face_xml_path) {
      efree(face_xml_path);
  }

	zend_class_entry ce;
  INIT_CLASS_ENTRY(ce, "OpenCV\\ImgProc", opencv_imgproc_methods);

	opencv_imgproc_ce = zend_register_internal_class_ex(&ce, NULL);
  opencv_imgproc_ce->ce_flags |= ZEND_ACC_FINAL;

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
