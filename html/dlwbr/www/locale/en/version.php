<?
$version_no=query("/runtime/sys/info/firmwareversion");
$build_no=query("/runtime/sys/info/firmwarebuildno");

$m_context_title	="Version";

$m_context = "Version : ".$version_no."<br><br>Build Number : ".$build_no."<br><br>";
$m_context = $m_context."System Uptime : <script>document.write(shortTime());</script>";

$m_days		= "days";
$m_button_dsc	=$m_continue;
?>
