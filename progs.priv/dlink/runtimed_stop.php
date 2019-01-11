#!/bin/sh
echo [$0] ...> /dev/console
<?
/* vi: set sw=4 ts=4: */
$template_root=query("/runtime/template_root");
if ($template_root=="") { $template_root="/etc/templates";  }

$generate_start=0;
require($template_root."/misc/runtimed_run.php");
?>
