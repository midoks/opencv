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


zend_class_entry * opencv_demo_ce;
zend_class_entry * opencv_demo_ce_ns;


/** {{{ proto OpenCV_Demo::__construct()
*/
PHP_METHOD(opencv_demo, __construct) {


}
/* }}} */


/** {{{ opencv_imgproc_methods
*/
zend_function_entry opencv_demo_methods[] = {
	PHP_ME(opencv_demo,  __construct,       NULL,  ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ OPENCV_STARTUP_FUNCTION
*/
OPENCV_STARTUP_FUNCTION(demo) {
	zend_class_entry ce;
  zend_class_entry ce_ns;
	OPENCV_INIT_CLASS_ENTRY(ce, ce_ns, "OpenCV_Demo", "OpenCV\\Demo", opencv_demo_methods);

	opencv_demo_ce = zend_register_internal_class_ex(&ce, NULL);
  opencv_demo_ce_ns = zend_register_internal_class_ex(&ce_ns, NULL);

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
