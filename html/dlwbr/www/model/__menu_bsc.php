<!-- === BEGIN SIDENAV === -->
<ul>
<?
$link_on="sidenavoff";

if($SUB_CATEGORY=="bsc_internet")
	{echo "<li><div id='".$link_on."'>".		$m_menu_bsc_internet.	"</div></li>\n";	}
else	{echo "<li><div><a href='/bsc_internet.php'>".	$m_menu_bsc_internet.	"</a></div></li>\n";	}

if($MY_NAME=="bsc_wlan")
	{echo "<li><div id='".$link_on."'>".		$m_menu_bsc_wlan.	"</div></li>\n";	}
else	{echo "<li><div><a href='/bsc_wlan.php'>".	$m_menu_bsc_wlan.	"</a></div></li>\n";	}

if($MY_NAME=="bsc_lan")
	{echo "<li><div id='".$link_on."'>".		$m_menu_bsc_lan.	"</div></li>\n";	}
else	{echo "<li><div><a href='/bsc_lan.php'>".	$m_menu_bsc_lan.	"</a></div></li>\n";	}
?>
</ul>
<!-- === END SIDENAV === -->
