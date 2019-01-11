<h1>SUPPORT MENU</h1>
	<table border=0 cellspacing=0 cellpadding=0 width=750 height=1103>
 <tr>
 <td width="750" height=40><font size=4><b>Advanced</b></font></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>

 </tr>
 <tr>
 <td height=228><p><strong>Port Forwarding </strong><font size=4><b><a name=04></a></b></font><br>
 Port Forwarding can be used to open a port or range of ports to a device on your network . To use them, click the checkbox to Enable the entry, select a pre-defined service from the Application Name drop down menu, select a computer from the Computer Name drop down menu, and click Save Settings. The Application, Computer, and Ports can also be filled in manually if your Application or Computer is not listed in the drop down menus. <br>
 <strong><em>Name </em></strong> - The name for the service being provided by the device on your LAN that uses the ports being opened. <br>
 <strong><em>IP Address </em></strong> - The server computer on the LAN network that the specified ports will be opened to. <br>

 <strong><em>Application Name </em></strong> - This contains a list of pre-defined services. </p>
 <p><strong><em>Computer Name </em></strong> - This contains a list of the devices on your network which have obtained an IP Address from the router. </p>
 <p><strong><em>Public Port </em></strong> - The port number that users on the Internet will use to access the defined service. <br>
 <strong><em>Private Port </em></strong> - The port number of the service being hosted by the server computer on the LAN. <br>

 <em><strong>Traffic Type </strong></em> - The protocol used by the service the device on your LAN is providing. </p> </td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>

 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=64> <strong>Applications<font size=4><b><a name=05></a></b></font> </strong><br>
Some applications require multiple connections, such as Internet gaming, video conferencing, Internet telephony and others. These applications have difficulties working through NAT (Network Address Translation). If you need to run applications that require multiple connections, specify the port normally associated with an application in the &quot;Trigger Port&quot; field, select the protocol type as TCP (Transmission Control Protocol) or UDP (User Datagram Protocol), then enter the public ports associated with the trigger port in the Firewall Port field to open them for inbound traffic. There are already defined well-known applications in the Application Name drop down menu. <br>

<strong><em>Name </em></strong> - This is the name referencing the application. <br>
<strong><em>Trigger Port </em></strong>- This is the port used to trigger the application. It can be either a single port or a range of ports. <br>
<strong><em>Traffic Type </em></strong> - This is the protocol used to trigger the application. <br>
<strong><em>Firewall Port </em></strong> - This is the port number on the WAN side that will be used to access the application. You may define a single port or a range of ports. You can use a comma to add multiple ports or port ranges. <br>
<strong><em>Traffic Type </em></strong> - This is the protocol used for the application. </td>

 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=99><p><strong>Network Filter </strong><a name=06></a><br>

 Filters are used to deny or allow LAN computers from accessing the Internet and also your network. Within your network, the unit can be setup to allow or deny Internet access to computers using their MAC addresses. </p>
 <p><strong>MAC Filters </strong><br>
 Use MAC Filters to deny computers within the local area network from accessing the Internet. You can either manually add a MAC address or select the MAC address from the list of clients that are currently connected to the unit. <br>
 Select &quot;Turn MAC Filtering ON and ALLOW computers with MAC address listed below to access the network&quot; if you only want selected computers to have network access and all other computers not to have network access. <br>
 Select &quot;Turn MAC Filtering ON and DENY computers with MAC address listed below to access the network&quot; if you want all computers to have network access except those computers in the list. <br>

 <strong><em>MAC Address - </em></strong>The MAC address of the network device to be added to the MAC Filter List. <br>
 <strong><em>DHCP Client List - </em></strong> DHCP clients will have their hostname in the Computer Name drop down menu. You can select the client computer you want to add to the MAC Filter List and click arrow button. This will automatically add that computer's MAC address to the appropriate field. <strong><em></em></strong></p>
 <strong><em>CLEAR - </em></strong>This will remove the MAC Address on the corresponding line from the MAC Filtering table. </td>
 </tr>
 <tr>

 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=44><p><strong>Website Filter <a name=07></a></strong></p>
 <p>Along with the ability to filter devices from accessing your network and the Internet, the <?query("/sys/modelname");?> can also be configured to allow or deny access, for the computers on your network, to inappropriate websites either by domain name or by keywords. </p>

 <p><strong>Website Filtering </strong><br>
 Website Filtering is used to allow or deny computers on your network from accessing specific web sites by keywords or specific Domain Names. Select &quot;Turn Website Filtering ON and ALLOW computers access to ONLY these sites&quot; in order only allow computers on your network to access the specified URLs and Domain Names. &quot;Turn Website Filtering ON and DENY computers access to ONLY these sites&quot; in order deny computers on your network to access the specified URLs and Domain Names. </p>
 <p><span class="style1">Example 1: </span><br>
 If you wanted to block LAN users from any website containing a URL pertaining to shopping, you would need to select &quot;Turn Website Filtering ON and DENY computers access to ONLY these sites&quot; and then enter &quot;shopping&quot; into the Website Filtering Rules list. Sites like these will be denied access to LAN users because they contain the keyword in the URL. <br>
 <a href="http://www.yahoo.com/shopping/stores.html">http://www.yahoo.com/shopping/stores.html </a><br>

 <a href="http://www.msn.com/search/shopping-spree.html">http://www.msn.com/search/shopping-spree.html </a></p>
 <p><span class="style1">Example 2: </span> If you want your children to only access particular sites, you would then choose &quot;Turn Website Filtering ON and ALLOW computers access to ONLY these sites&quot; and then enter in the domains you want your children to have access to. </p>
 <ul>
 <li>Disney.com </li>
 <li>Cartoons.com </li>

 <li>DiscoveryChannel.com </li>
 </ul></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>

 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=248>
 <div align=left>
 <p><strong>Firewall Settings </strong><a name=08></a><br>

 The Firewall Settings section contains the option to configure a DMZ Host as well as PPTP, L2TP, and IPSec passthrough. </p>
 <p><strong>DMZ </strong></a><br>
 If you have a computer that cannot run Internet applications properly from behind the <?query("/sys/modelname");?>, then you can allow the computer to have unrestricted Internet access. Enter the IP address of that computer as a DMZ (Demilitarized Zone) host with unrestricted Internet access. Adding a client to the DMZ may expose that computer to a variety of security risks; so only use this option as a last resort. </p>
 <p><strong>VPN Passthrough </strong><br>The device supports VPN (Virtual Private Network) passthrough for PPTP (Point-to-Point Tunneling Protocol), L2TP (Layer 2 Tunneling Protocol), and IPSec (IP Security). Once VPN passthrough is enabled, there is no need to create any Virtual Server or Port Forwarding entries in order for outbound VPN sessions to establish properly. Multiple VPN connections can be made through the device. This is useful when you have many VPN clients on the Local Area Network. </p>
 </div></td>
 </tr>

 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>Advanced Wireless <a name=09></a></strong></p>

 <p>The options on this page should be changed by advanced users or if you are instructed to by one of our support personnel, as they can negatively affect the performance of your router if configured improperly. </p>
 <p><strong><em>Transmission (TX) Rates - </em></strong> Select the basic transfer rates based on the speed of wireless adapters on the WLAN (wireless local area network). </p>
 <p><strong><em>Transmit Power - </em></strong>You can lower the output power of the <?query("/sys/modelname");?> by selecting lower percentage Transmit Power values from the drop down. Your choices are: 100%, 50%, 25%, and 12.5%. </p>
 <p><strong><em>Beacon Interval - </em></strong> Beacons are packets sent by an Access Point to synchronize a wireless network. Specify a Beacon interval value between 20 and 1000. The default value is set to 100 milliseconds. </p>

 <p><strong><em>RTS Threshold - </em></strong> This value should remain at its default setting of 2346. If you encounter inconsistent data flow, only minor modifications to the value range between 1 and 2346 are recommended. The default value for RTS Threshold is set to 2346. </p>
 <p><strong><em>Fragmentation - </em></strong> This value should remain at its default setting of 2346. If you experience a high packet error rate, you may slightly increase your &quot;Fragmentation&quot; value within the value range of 256 to 2346. Setting the Fragmentation value too low may result in poor performance. </p>
 <p><strong><em>DTIM Interval - </em></strong> Enter a value between 1 and 255 for the Delivery Traffic Indication Message (DTIM). A DTIM is a countdown informing clients of the next window for listening to broadcast and multicast messages. When the Access Point has buffered broadcast or multicast messages for associated clients, it sends the next DTIM with a DTIM Interval value. AP clients hear the beacons and awaken to receive the broadcast and multicast messages. The default value for DTIM interval is set to 1. </p>

 <p><strong><em>Preamble Type - </em></strong> The Preamble Type defines the length of the CRC (Cyclic Redundancy Check) block for communication between the Access Point and roaming wireless adapters. Make sure to select the appropriate preamble type and click the Apply button. </p>
 <p><span class="style2">Note: </span>High network traffic areas should use the shorter preamble type. CRC is a common technique for detecting data transmission errors. </p>
 <p><strong><em>CTS Mode - </em></strong>Select None to disable this feature. Select Auto to force the router to require each wireless device on the network to perform and RTS/CTS handshake before they are allowed to transmit data. Select Auto to allow the router to decide when RTS/CTS handshakes are necessary. </p>
 <p><strong><em>WMM - </em></strong>Select Enable to turn on QoS for the wireless interface of the <?query("/sys/modelname");?>. </p> </td>

 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>Advanced Network <a name=10></a></strong></p>

 <p>This section contains settings which can change the way the router handles certain types of traffic. We recommend that you not change any of these settings unless you are already familiar with them or have been instructed to change them by one of our support personnel. </p>
 <p><strong>UPnP </strong><br>UPnP is short for Universal Plug and Play which is a networking architecture that provides compatibility among networking equipment, software, and peripherals. The <?query("/sys/modelname");?> is a UPnP enabled router, meaning it will work with other UPnP devices/software. If you do not want to use the UPnP functionality, it can be disabled by selecting &quot;Disabled&quot;. </p>
 <p><strong>WAN Ping </strong><br>When you Enable WAN Ping respond, you are causing the public WAN (Wide Area Network) IP address on the device to respond to ping commands sent by Internet users. Pinging public WAN IP addresses is a common method used by hackers to test whether your WAN IP address is valid. </p>
 <p><strong>WAN Port Speed </strong><br>This a llows you to select the speed of the WAN interface of the <?query("/sys/modelname");?>: Choose 100Mbps, 10Mbps, or 10/100Mbps Auto. </p>

 <p><strong>Gaming Mode </strong><br>If you are experiencing difficulties when playing online games or even certain applications that use voice data, you may need to enable Gaming Mode for these applications to work correctly. When not playing games or using these voice applications, it is recommended that Gaming Mode is disabled. </p>
 <p><strong>Multicast Streams</strong><br>Enable this option to allow Multicast traffic to pass from the Internet to your network more efficiently. </p> </td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>

 </table>
