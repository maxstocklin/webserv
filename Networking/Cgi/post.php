#!/usr/bin/php

<?php

$uploadDir = '../uploads/';
// Check if directory exists
if (!file_exists($uploadDir))
{
	// Attempt to create the directory
	if (!mkdir($uploadDir, 0777, true))
		die("Failed to create directory.");
}

// Check if directory is writable
if (!is_writable($uploadDir))
{
	// Attempt to set the directory permissions
	if (!chmod($uploadDir, 0777))
		die("Upload directory is not writable, and failed to set write permissions.");
}

if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['image']))
{
	$uploadedFile = $_FILES['image'];

	// Check for upload errors
	if ($uploadedFile['error'] === UPLOAD_ERR_OK)
	{
		// Extract file extension
		$fileExtension = pathinfo($uploadedFile['name'], PATHINFO_EXTENSION);
		
		// Create a unique filename using uniqid()
		$uniqueFilename = uniqid() . '.' . $fileExtension;

		// Ensure the filename is unique
		while (file_exists($uploadDir . $uniqueFilename))
			$uniqueFilename = uniqid() . '.' . $fileExtension;

		$destinationPath = $uploadDir . $uniqueFilename;
		$firstname = isset($_POST['firstname']) ? $_POST['firstname'] : '';
		$lastname = isset($_POST['lastname']) ? $_POST['lastname'] : '';
		
		// Move the file to the desired directory
		if (move_uploaded_file($uploadedFile['tmp_name'], $destinationPath))
		{
			echo "SUCCESS:" . $destinationPath;
			echo "\nThanks " . $firstname . " " . $lastname . "! :)";

		}
		else
			echo "Error moving uploaded file.";
	}
	else
		echo "File upload error: " . $uploadedFile['error'];
}
else
	echo "No file uploaded or wrong method.";

?>
