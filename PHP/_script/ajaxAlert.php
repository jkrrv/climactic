<script type="text/javascript">

function newAJAX() {
	if (window.XMLHttpRequest) {
		return new XMLHttpRequest();
	}
	if (window.ActiveXObject) {
		return new ActiveXObject("Microsoft.XMLHTTP");
	}
return null;  // else implied by prev returns
}


if (newAJAX() == null) {
  document.write("<?php errorBox("red","inline","AJAX Can't Function","AJAX, a system used to get small bits of information from our servers to this page, cannot function because XMLHTTP is disabled on your computer.  To use this page fully, you must enable XMLHTTP."); ?>");
}

</script>