
function expireCounter()
{




if (document.getElementById('PageExpireCountdownS').innerHTML==0 && document.getElementById('PageExpireCountdownM').innerHTML==0) {

	window.location.reload(true);

} else {

	if (document.getElementById('PageExpireCountdownS').innerHTML==0) {
		document.getElementById('PageExpireCountdownM').innerHTML=document.getElementById('PageExpireCountdownM').innerHTML-1;
		document.getElementById('PageExpireCountdownS').innerHTML=59;
	} else {
		document.getElementById('PageExpireCountdownS').innerHTML=document.getElementById('PageExpireCountdownS').innerHTML-1;
	}


	if (document.getElementById('PageExpireCountdownS').innerHTML<=9 && document.getElementById('PageExpireCountdownM').innerHTML>=1) {
		document.getElementById('PageExpireCountdownS').innerHTML="0" + document.getElementById('PageExpireCountdownS').innerHTML;
	}


	if (document.getElementById('PageExpireCountdownM').innerHTML<=0) {
		document.getElementById('PageExpireCountdownM').innerHTML=0
		document.getElementById('PageExpireCountdownM').innerHTML="";
		document.getElementById('PageExpireCountdownColon').innerHTML="";
		document.getElementById('PageExpireCountdownSec').innerHTML="&nbsp;seconds";
		if (document.getElementById('PageExpireCountdownS').innerHTML<=30) {
			document.getElementById('PageExpireWarningS').innerHTML=document.getElementById('PageExpireCountdownS').innerHTML;
			document.getElementById('PageExpireWarning').style.display="block";
			if (document.getElementById('PageExpireCountdownS').innerHTML<=0) {
				document.getElementById('PageExpireCountdownS').innerHTML=0;
			}
		}
	}
	t=setTimeout("expireCounter()",1000);
}
}




function expireCountStart(resetTime)
{
  expireCounter();
}




function expireReset(resetTime)
{
document.getElementById('PageExpireCountdownM').innerHTML=resetTime;
document.getElementById('PageExpireCountdownColon').innerHTML=":";
document.getElementById('PageExpireCountdownS').innerHTML="00";
document.getElementById('PageExpireCountdownSec').innerHTML="";
document.getElementById('PageExpireWarningS').innerHTML="30";
document.getElementById('PageExpireWarning').style.display="none";
}
