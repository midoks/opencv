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
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"

using namespace cv;
using namespace std;


/** {{{ proto php_opencv_imgproc
*/
typedef struct _php_opencv_imgproc
{
  char *src_path;
  int width;
  int height;
} php_opencv_imgproc_t;
/* }}} */

php_opencv_imgproc_t * opencv_imgproc_var;
Mat opencv_imgproc_src_im;
Mat opencv_imgproc_dst_im;

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

  CascadeClassifier face_cascade;
  string config_path = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
  
  if (!face_cascade.load( config_path ) ){
    php_printf("load:true\r\n");
    return -1;
    //php_error_docref(NULL TSRMLS_CC, E_WARNING, "can not load classifier file！%s", config_path.c_str());
  }

  php_printf("config_path:%s\r\n",config_path.c_str());
  php_printf("load:%d\r\n",!face_cascade.empty());


  std::vector<Rect> faces;
  Mat img_gray;
  int face_size;
  int Y;
  cvtColor( img, img_gray, CV_BGR2GRAY );
  equalizeHist( img_gray, img_gray );

  face_cascade.detectMultiScale( img_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
  face_size = faces.size();
  php_printf("face_size:%d\r\n",face_size);

  // if ( face_size > 0)
  // {
  //   Y = faces[face_size -1].y - faces[face_size -1].height / 2;
  //   if ( Y > img.size().height / 2 ) //fix
  //   {
  //     return -1;
  //   } else {
  //     return Y < 0 ? 0 : Y;
  //   }
  // } else {
  //   return -1;
  // }

  return -1;
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
    // if (!php_opencv_imgproc->src_im.data) {
    //     php_error_docref(NULL TSRMLS_CC, E_WARNING, "fail to load image from %s", php_opencv_imgproc->src_path);
    // }
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
  long height, width;
  if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "ll", &height, &width) == FAILURE) {
    return;
  }

  //check,import
  if (!opencv_imgproc_var->src_path){
    RETURN_FALSE;
  }

  opencv_imgproc_var->width = width;
  opencv_imgproc_var->height = height;


  Size tmp_size;
  float ratio = 0;
  int clip_top = 0,clip_bottom = 0,clip_left = 0, clip_right = 0;

  if (opencv_imgproc_src_im.size().width * 3 <= opencv_imgproc_src_im.size().height) {
    ratio = (float)width / opencv_imgproc_src_im.size().width;
    tmp_size = Size((int)(opencv_imgproc_src_im.size().width * ratio), (int)(opencv_imgproc_src_im.size().height * ratio));
    resize(opencv_imgproc_src_im, opencv_imgproc_dst_im, tmp_size);
    clip_top = 0;
    clip_bottom = width - opencv_imgproc_dst_im.size().height;
    clip_left = 0;
    clip_right = 0;
    opencv_imgproc_dst_im.adjustROI(clip_top, clip_bottom, clip_left, clip_right);
    RETURN_TRUE;
  }

  tmp_size = Size(width, height);
  opencv_imgproc_dst_im = Mat(tmp_size, CV_32S);
  resize(opencv_imgproc_src_im, opencv_imgproc_dst_im, tmp_size);


  int result = opencv_imgproc_detect_face(opencv_imgproc_dst_im TSRMLS_CC);
  if (result){
    php_printf("ok\r\n");
  } else {
    php_printf("fail\r\n");
  }

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
