dnl $Id$
dnl config.m4 for extension opencv


PHP_ARG_WITH(opencv, for opencv support,
Make sure that the comment is aligned:
[  --with-opencv             Include opencv support])

if test "$PHP_OPENCV" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-opencv -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/opencv.h"  # you most likely want to change this
  dnl if test -r $PHP_OPENCV/$SEARCH_FOR; then # path given as parameter
  dnl   OPENCV_DIR=$PHP_OPENCV
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for opencv files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       OPENCV_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$OPENCV_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the opencv distribution])
  dnl fi

  dnl # --with-opencv -> add include path
  dnl PHP_ADD_INCLUDE($OPENCV_DIR/include)

  dnl # --with-opencv -> check for lib and symbol presence
  dnl LIBNAME=opencv # you may want to change this
  dnl LIBSYMBOL=opencv # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $OPENCV_DIR/$PHP_LIBDIR, OPENCV_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_OPENCVLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong opencv lib version or lib not found])
  dnl ],[
  dnl   -L$OPENCV_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(OPENCV_SHARED_LIBADD)

  OPENCV_FLAGS="`pkg-config opencv --libs --cflags opencv`"
  opencv_lib_path=""
  opencv_last_path=""
  for i in $OPENCV_FLAGS;do
        if test ${i:0:2} = "-I" ;then
                opencv_last_path=$i
                PHP_ADD_INCLUDE(${i:2})
        elif test ${i:0:2} = "-L" ;then
                opencv_last_path=$i
                opencv_lib_path=${i:2}
        elif test ${i:${#i}-3} = ".so" ;then
                opencv_last_path=$i
                dir_name=`dirname $i`
                file_name=${i/$dir_name/}
                file_name=${file_name/\/lib/}
                file_name=${file_name/.so/}
            PHP_ADD_LIBRARY_WITH_PATH($file_name,$dir_name,OPENCV_SHARED_LIBADD)
        elif test ${i:${#i}-6} = ".dylib" ;then
                opencv_last_path=$i
                dir_name=`dirname $i`
                file_name=${i/$dir_name/}
                file_name=${file_name/\/lib/}
                file_name=${file_name/.dylib/}
            PHP_ADD_LIBRARY_WITH_PATH($file_name,$dir_name,OPENCV_SHARED_LIBADD)
        elif test ${i:0:2} = "-l" ;then 
                opencv_last_path=$i
                file_name=${i:2}
                PHP_ADD_LIBRARY_WITH_PATH($file_name,$opencv_lib_path,OPENCV_SHARED_LIBADD)
        fi
  done


  PHP_ADD_LIBRARY(stdc++,"",OPENCV_SHARED_LIBADD)
  PHP_SUBST(OPENCV_SHARED_LIBADD)
  PHP_REQUIRE_CXX()


  PHP_NEW_EXTENSION(opencv,
    opencv.cpp               \
    lib/demo.cpp             \
    lib/imgproc.cpp,
  $ext_shared)

  PHP_ADD_BUILD_DIR([$ext_builddir/lib])
fi
