#!/usr/bin/php
<?php

phpinfo();

if (($_FILES['my_file']['name']!="")){
// Where the file is going to be stored
	$target_dir = "upload/";
	$file = $_FILES['my_file']['name'];
	$path = pathinfo($file);
	$filename = $path['filename'];
	$ext = $path['extension'];
	$temp_name = $_FILES['my_file']['tmp_name'];
	$path_filename_ext = $target_dir.$filename.".".$ext;
	echo $path_filename_ext;
 
// Check if file already exists
if (file_exists($path_filename_ext)) {
 echo "Sorry, file already exists.";
 }else{
 move_uploaded_file($temp_name,$path_filename_ext);
 echo "Congratulations! File Uploaded Successfully.";
 }
}


// $value = getenv('QUERY_STRING');

// echo "<!DOCTYPE html>\r\n"
//     . "<html>\r\n"
//     . "  <head><title>My Page </title></head>\r\n"
//     . "<body>\r\n"
//     . "  <h1>Hello, my name is $value !</h1>\r\n"
//     . "<p>I'm a web developer with a passion for learning new things.</p>\r\n"
//     . "<h2>Some of my hobbies:</h2>\r\n"
//     . "<ul>\r\n"
//     . "      <li>Coding</li>\r\n"
//     . "    <li>Photography</li>\r\n"
//     . "    <li>Traveling</li>\r\n"
//     . "</ul>\r\n"
//     . "</body>\r\n"
//     . "</html>\r\n";
 ?>