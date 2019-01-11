<!-- === BEGIN SIDENAV === -->
<ul>
<?
$link_on="sidenavoff";

if(query("/sys/modelname")=="WBR-2310")
{
	if($MY_NAME=="adv_vrtsrv")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_vrtsrv.	"</div></li>\n";	}
	else	{echo "<li><div><a href='/adv_vrtsrv.php'>".	$m_menu_adv_vrtsrv.	"</a></div></li>\n";	}
}
if($MY_NAME=="adv_port")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_port.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_port.php'>".	$m_menu_adv_port.	"</a></div></li>\n";	}

if($MY_NAME=="adv_app")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_app.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_app.php'>".	$m_menu_adv_app.	"</a></div></li>\n";	}

if($MY_NAME=="adv_mac_filter")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_mac_filter.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_mac_filter.php'>".$m_menu_adv_mac_filter.	"</a></div></li>\n";	}
/*
if($SUB_CATEGORY=="adv_acl")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_acl.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_acl.php'>".	$m_menu_adv_acl.	"</a></div></li>\n";	}
*/
if($MY_NAME=="adv_url_filter")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_url_filter.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_url_filter.php'>".$m_menu_adv_url_filter.	"</a></div></li>\n";	}

if($MY_NAME=="adv_dmz")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_dmz.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_dmz.php'>".	$m_menu_adv_dmz.	"</a></div></li>\n";	}

if($MY_NAME=="adv_wlan")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_wlan.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_wlan.php'>".	$m_menu_adv_wlan.	"</a></div></li>\n";	}

if($MY_NAME=="adv_network")
	{echo "<li><div id='".$link_on."'>".		$m_menu_adv_network.	"</div></li>\n";	}
else	{echo "<li><div><a href='/adv_network.php'>".	$m_menu_adv_network."	</a></div></li>\n";	}
?></ul>
<!-- === END SIDENAV === -->
