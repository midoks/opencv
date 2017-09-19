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






zend_class_entry * opencv_imgproc_ce;
zend_class_entry * opencv_imgproc_ce_ns;


ZEND_BEGIN_ARG_INFO_EX(opencv_imgproc_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(opencv_imgproc_construct_arginfo, 0, 0, 0)
  ZEND_ARG_INFO(0, src)
ZEND_END_ARG_INFO()



/** {{{ proto OpenCV_ImgProc::__construct(string $source)
*/
PHP_METHOD(opencv_imgproc, __construct) {
  
  char *src = NULL;
  zval *self = NULL;
  size_t src_len;

  if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|s", &src, &src_len) == FAILURE) {
    return;
  }

  if (src){
    self = getThis();
    zend_update_property_string(opencv_imgproc_ce, self, "src", sizeof("src")-1, src);
  }
}
/* }}} */


/** {{{ proto public OpenCV_ImgProc::__desctruct(void)
*/
PHP_METHOD(opencv_imgproc, __destruct) {
}
/* }}} */

/** {{{ proto private OpenCV_ImgProc::__sleep(void)
*/
PHP_METHOD(opencv_imgproc, __sleep) {
}
/* }}} */

/** {{{ proto private OpenCV_ImgProc::__wakeup(void)
*/
PHP_METHOD(opencv_imgproc, __wakeup) {
}
/* }}} */

/** {{{ proto private OpenCV_ImgProc::__clone(void)
*/
PHP_METHOD(opencv_imgproc, __clone) {
}
/* }}} */


/** {{{ opencv_imgproc_methods
*/
zend_function_entry opencv_imgproc_methods[] = {
	PHP_ME(opencv_imgproc,  __construct, opencv_imgproc_construct_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
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
  zend_class_entry ce_ns;
	OPENCV_INIT_CLASS_ENTRY(ce, ce_ns, "OpenCV_ImgProc", "OpenCV\\ImgProc", opencv_imgproc_methods);

	opencv_imgproc_ce = zend_register_internal_class_ex(&ce, NULL);
  opencv_imgproc_ce_ns = zend_register_internal_class_ex(&ce_ns, NULL);

  opencv_imgproc_ce->ce_flags |= ZEND_ACC_FINAL;
  opencv_imgproc_ce_ns->ce_flags |= ZEND_ACC_FINAL;

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
