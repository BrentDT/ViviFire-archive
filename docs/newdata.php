<?php
$files = array();
foreach (glob("*.html") as $filename) {
	$files[$filename] = filesize($filename);
}
file_put_contents('filesizes.dat', serialize($files));
?>