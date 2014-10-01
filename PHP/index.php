<?php

if (isset($_POST[temp]) && $_POST[temp]>=50 && $_POST[temp]<=80) {
	file_get_contents("http://10.0.0.103:3604?".$_POST[temp]."?");
	sleep(5);
}

$pageName="index";
$pageExpireTime=20;


		include '_head.txt';


?>

<!--<h1>Climate & Such</h1>-->

<?php 


$vars = explode(",",file_get_contents("http://10.0.0.103:3604")); 

?>

<form action="" method="POST">

<table style="width:100%; font-size:2em; line-height:48px;">
	<tr>
		<td style="text-align:right; width:50%;">Current Temperature:&nbsp;</td>
		<td style="width:50%;"><?php echo round(floatval($vars[1]),1); ?> &deg;F</td>
	</tr>
	<tr>
		<td style="text-align:right; width:50%;">Current Humidity:&nbsp;</td>
		<td style="width:50%;"><?php echo round(floatval($vars[3]),1); ?> %</td>
	</tr>
	<tr>
		<td style="text-align:right; width:50%;">Thermostat Setting:&nbsp;</td>
		<td style="width:50%;">
			<div id="show" style="display:block;"><?php echo round(floatval($vars[0]),1); ?> &deg;F <a href="#" onClick="document.getElementById('show').style.display='none'; document.getElementById('hide').style.display='block';">Adjust...</a></div>
			<div id="hide" style="display:none;" >
				<input name="temp" type="number" value="<?php echo round(floatval($vars[0]),0); ?>" max="80" min="50" />
				<input type="submit" value="Apply..." />
			</div>
		</td>
	</tr>
	<tr>
		<td style="text-align:right; width:50%;">Furnace is:&nbsp;</td>
		<td style="width:50%;"><?php if (intval($vars[2])==1) { echo "<span style=\"color:red;\">Running</span>"; } else { echo "Not Running"; } ?> </td>
	</tr>
</table>

</form>

<script type="text/javascript">
setTimeout("window.location.reload();",60000);
</script>

<?php

include '_foot.txt';

?>