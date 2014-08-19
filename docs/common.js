var minimal = (document.getElementsByTagName("H2").length > 0) || (document.body.innerHTML.length > 400);
var warning = document.createElement('P');
warning.className = 'nav';
warning.innerHTML = 'This document '+(minimal?'may be incomplete':'is a stub')+'. You can track changes on the <a href="http://www.b6sw.com/ViviFire/docs/newest.php">list of recent changes</a>. Please also consider submitting anonymous feedback using the poll below.';
document.body.insertBefore(warning, document.body.firstChild);

document.write('<div class="nav" id="poll" style="border:thin solid red; margin:0.5em; padding:0.3em;" role="status" aria-live="polite" aria-relevant="text">' +
'<form>' +
'This article is' +
' <input id="0" type="radio" onclick="vote(0)"><label for="0">good</label>' +
' <input id="1" type="radio" onclick="vote(1)"><label for="1">bad</label>' +
' <input id="2" type="radio" onclick="vote(2)"><label for="2">messy</label>' +
' <input id="3" type="radio" onclick="vote(3)"><label for="3">confusing</label>' +
' <input id="4" type="radio" onclick="vote(4)"><label for="4">incomplete</label>' +
'</form>' +
'</div>');

document.write('<p>Last modified '+document.lastModified+'.</p>');

function getFileName(uri) {
	var fileName = uri.substr(uri.lastIndexOf("/") + 1);
	return fileName.substr(0, fileName.lastIndexOf("."));
}

function vote(value) {
	var xmlhttp;
	if (window.XMLHttpRequest)
		xmlhttp = new XMLHttpRequest();
	else
		xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
	xmlhttp.onreadystatechange = function() {
		//alert('readyState='+xmlhttp.readyState+' && status='+xmlhttp.status+' && text='+xmlhttp.responseText);
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			document.getElementById("poll").innerHTML = xmlhttp.responseText;
		}
	};
	var article = getFileName(document.URL);
	xmlhttp.open('GET', 'http://www.b6sw.com/ViviFire/.poll/vote.php?article='+article+'&vote='+value, true);
	xmlhttp.send();
}
