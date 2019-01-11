<h1>SUPPORT MENU</h1>
<table border=0 cellspacing=0 cellpadding=0 width=750>
<tr>
<td height=2><b><font size=4>Tools</font></b></td>
</tr>
<tr>
<td height=16><p><br>
</td>
</tr>
<tr>
<td height=20><p><strong>Admin </strong><a name=11></a><br>
There are two accounts that can access the router's management interface. These accounts are <strong>admin </strong> and <strong>user </strong>. <strong>Admin </strong> has read/write access while <strong>user </strong> has read-only access. <strong>User </strong> can only view the settings but cannot make any changes. Only the <strong>admin </strong> account has the ability to change both <strong>admin </strong> and <strong>user </strong> account passwords. </p>
<p><strong><em>Administrator Login Name - </em></strong>Enter the name that will be used to login to the router with Admin access. </p>
<p><strong><em>Administrator password - </em></strong> Enter and confirm the password that the <strong>admin </strong> account will use to access the router's management interface. </p>
<p><strong><em>User Login Name - </em></strong>Enter the name that will be used to login to the router with User access. <strong><em></em></strong></p>
<p><strong><em>User Password - </em></strong> Enter and confirm the password that the <strong>user </strong> account will use to view the router's settings through its management interface. </p>
<p><strong>Remote Management </strong><br>
Remote Management allows the device to be configured through the WAN (Wide Area Network) port from the Internet using a web browser. A username and password is still required to access the router's management interface. <br>
<strong><em>IP Address </em></strong> - The IP address of the computer on the Internet that has access to the <?query("/sys/modelname");?>. Enter an asterisk if you would like any computer on the Internet to access the management interface remotely. <br>
<strong><em>Port </em></strong> - Select the port which will be used to access the <?query("/sys/modelname");?>. </p>
<p><strong><em>Example: </em></strong><br>
<a href="http://192.168.0.1/help_tools.html">http://x.x.x.x:8080 </a> whereas x.x.x.x is the WAN IP address of the <?query("/sys/modelname");?> and 8080 is the port used for the Web-Management interface. </p></td>
</tr>
<tr>
<td height=18>&nbsp;</td>
</tr>
<tr>
<td height=10>&nbsp;</td>
</tr>
<tr>
<td height=10>&nbsp;</td>
</tr>
<tr>
<td height=33><b>Time<font size=4> <a name=12></a></font></b> 
<p>The Time Configuration settings are used by the router for synchronizing scheduled services and system logging activities. You will need to set the time zone corresponding to your location. The time can be set manually or the device can connect to a NTP (Network Time Protocol) server to retrieve the time. You may also set Daylight Saving dates and the system time will automatically adjust on those dates. </p>
<p><strong><em>Time Zone - </em></strong>Select the Time Zone for the region you are in. </p>
<p><strong><em>Daylight Saving - </em></strong>If the region you are in observes Daylight Savings Time, enable this option and specify the Starting and Ending Month, Week, Day, and Time for this time of the year. </p>
<p><strong><em>Automatic Time Configuration - </em></strong>Enter the NTP server which you would like the <?query("/sys/modelname");?> to synchronize its time with. Also, select the interval at which the <?query("/sys/modelname");?> will communicate with the specified NTP server. </p>
<strong><em>Set the Date and Time Manually - </em></strong>Select this option if you would like to specify the time manually. You must specify the Year, Month, Day, Hour, Minute, and Second, or you can click the Copy Your Computer's Time Settings button to copy the system time from the computer being used to access the management interface. </td>
</tr>
<tr>
<td height=2>&nbsp;</td>
</tr>
<tr>
<td height=2>&nbsp;</td>
</tr>
<tr>
<td height=40><b>System</b> <a name=13></a><br> 
<br>
<font size="2">The current system settings can be saved as a file onto the local hard drive. The saved file or any other saved setting file created by device can be uploaded into the unit. To reload a system settings file, click on <strong>Browse</strong> to search the local hard drive for the file to be used. The device can also be reset back to factory default settings by clicking on <strong>Restore</strong>. Use the restore feature only if necessary. This will erase previously save settings for the unit. Make sure to save your system settings before doing a factory restore.</font> <br>
<font size="2"><strong><em>Save &ndash; </em></strong>Click this button to save the configuration file from the router.</font> <br>
<font size="2"><strong><em>Browse &ndash; </em></strong>Click Browse to locate a saved configuration file and then click to Load to apply these saved settings to the router.</font> <br> 
<font size="2"><strong><em>Restore &ndash; </em></strong>Click this button to restore the router to its factory default settings.</font></td>
</tr>
<tr>
<td height=20>&nbsp;</td>
</tr>
<tr>
<td height=20>&nbsp;</td>
</tr>
<tr>
<td height=51><b>Firmware Upgrade</b> <a name=14></a><br> 
You can upgrade the firmware of the device using this tool. Make sure that the firmware you want to use is saved on the local hard drive of the computer. Click on <strong>Browse </strong> to search the local hard drive for the firmware to be used for the update. Upgrading the firmware will not change any of your system settings but it is recommended that you save your system settings before doing a firmware upgrade. Please check the D-Link support site for firmware updates at <a href="http://support.dlink.com/">http://support.dlink.com/ </a>. <br>
</td>
</tr>
<tr>
<td height=20>&nbsp;</td>
</tr>
<tr>
<td height="197"><p><strong>System Check </strong><a name=15></a><br>
These tools can be used to verify physical connectivity on both the LAN and WAN interfaces. </p>
<p><strong>Fast Ethernet Virtual Cable Tester (VCT) </strong><br>
Cable Test is an advanced feature that integrates a LAN cable tester on every Ethernet port on the router. Through the graphical user interface (GUI), Cable Test can be used to remotely diagnose and report cable faults such as opens, shorts, swaps, and impedance mismatch. This feature significantly reduces service calls and returns by allowing users to easily troubleshoot their cable connections.</p>
<p><strong>Ping Test</strong><br>
This useful diagnostic utility can be used to check if a computer is on the Internet. It sends ping packets and listens for replies from the specific host. Enter in a host name or the IP address that you want to ping (Packet Internet Groper) and click <strong>Ping </strong>. The status of your Ping attempt will be displayed in the Ping Result box. </p></td>
</tr>
</table>
						   
