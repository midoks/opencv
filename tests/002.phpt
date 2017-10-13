--TEST--
Check for opencv \OpenCV\ImgProc tclip
--SKIPIF--
<?php if (!extension_loaded("opencv")) print "skip"; ?>
--FILE--
<?php 

$dir = dirname(__FILE__);
$im = new \OpenCV\ImgProc();

$im->setImage($dir."/demo.jpg");
$ret = $im->tclip(100,100);
var_dump($ret);
$im->writeImage($dir.'/demo_test.jpg');

?>
--EXPECT--
bool(true)
