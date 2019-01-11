<body onload="init();" <?=$G_BODY_ATTR?>>
<form name="frm" id="frm">
<?require("/www/model/__banner.php");?>
<table <?=$G_MAIN_TABLE_ATTR?>>
<tr valign=middle align=center>
	<td>
	<br>
<!-- ________________________________ Main Content Start ______________________________ -->
	<table width=80%>
	<tr>
		<td id="box_header">
			<h1><?=$m_context_title?></h1><br><br>
			<center>
			<?
			if($REQUIRE_FILE!="")
			{
				require($LOCALE_PATH."/".$REQUIRE_FILE);
			}
			else
			{
				echo $m_context;
				echo "<br><br><br>\n";
				if($USE_BUTTON=="1")
				{echo "<input type=button name='bt' value='".$m_button_dsc."' onclick='click_bt();'>\n"; }
			}
			?>
			</center>
			<br>
		</td>
	</tr>
	</table>
<!-- ________________________________  Main Content End _______________________________ -->
	<br>
	</td>
</tr>
</table>
<?require("/www/model/__tailer.php");?>
</form>
</body>
</html>
