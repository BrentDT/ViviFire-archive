<?php
$datafile = 'filesizes.dat';

$dir = getcwd();
$dir = substr($dir, strpos($dir, 'html/') + 4);
?><!DOCTYPE HTML>
<html>
<head>
<title>Newest of <?=$dir?></title>
</head>
<body>
<h1>Newest of <?=$dir?></h1>
<table>
<tr><th>Filename</th><th>Date modified</th><th>Size</th><th>Diff*</th></tr>
<?php
// Read in old file info
$filesize = unserialize(file_get_contents($datafile));
// Read in current file info
$files = array();
foreach (glob("*.html") as $filename) {
	$files[$filename] = filemtime($filename);
}
// Reverse-sort puts newest first
arsort($files);
// Loop thru array and display
reset($files);
$now = time();
$maxDays = 14;
$maxAge = $maxDays * 86400;
$changed = false;
for ($i = 0; $i < 25 && list($name, $date) = each($files); $i++) {
$size=filesize($name);
if ($filesize[$name] != $size && $now - $date > $maxAge) {
$filesize[$name] = $size;
$changed = true;
}
if (is_numeric($filesize[$name])) {
$diff = $size - $filesize[$name];
if ($diff > 0) $diff = '+' . $diff;
} else {
$diff = 'New';
}
for ($k = 0; $size >= 1024; $k++, $size /= 1024);
$size = round($size, 1) . substr(' B  KB MB GB', $k * 3, 3);
$modtime=date("d-M-Y H:i", $date);
echo "<tr><td><a href=\"$name\">$name</a></td><td>$modtime</td><td>$size</td><td>$diff</td></tr>";
}
// Continue thru $files updating $filesize
while (list($name) = each($files) && $filesize[$name] != ($size = filesize($name))) {
$filesize[$name] = $size;
$changed = true;
}
// Approximately every 100 runs, remove deleted files from array
if (rand(0, 99) == 0) {
	$changed = true;
	reset($filesize);
	while(list($name) = each($filesize)) {
		if (!file_exists($name)) unset($filesize[$name]);
	}
}
if ($changed) {
file_put_contents($datafile, serialize($filesize));
}
?>
</table>
<p>* Difference in bytes for the last <?=$maxDays?> days.</p>
</body>
</html>
