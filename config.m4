PHP_ARG_WITH(opencv, for opencv support,
Make sure that the comment is aligned:
[  --with-opencv           Include opencv support])

PHP_ARG_ENABLE(opencv_d, whether to enable opencv debug,
[  --enable-opencv_d       opencv debug support default=no], no, no)

if test "$PHP_OPENCV" != "no"; then
  
  # DEBUG
  if test "$PHP_OPENCV_D" != "no";then
    AC_DEFINE(OPENCV_DEBUG, 1, [OPENCV_DEBUG Setting])
  fi
  

  SEARCH_PATH="/usr/lib/pkgconfig /usr/local/lib/pkgconfig"     # you might want to change this
  SEARCH_FOR="opencv.pc"  # you most likely want to change this
  if test -r $PHP_TCLIP/$SEARCH_FOR; then # path given as parameter
     TCLIP_DIR=$PHP_TCLIP
  else # search default path list
     AC_MSG_CHECKING([for opencv.pc file in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         TCLIP_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
  fi

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


  # have some question
  #AC_DEFINE(PHP_OPENCV_OPENCV_PATH, \"$opencv_lib_path\", [Include OPENCV_SHARE XML Path])
  
  opencv_root_path=`dirname $opencv_lib_path`
  if test -r php_opencv.bak.h;then
    echo "php_opencv.bak.h ok"
  else
    cp php_opencv.h php_opencv.bak.h
  fi

  echo "" > php_opencv.h
  echo "#define PHP_OPENCV_OPENCV_PATH \"$opencv_root_path\"" >> php_opencv.h
  cat php_opencv.bak.h >> php_opencv.h


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
