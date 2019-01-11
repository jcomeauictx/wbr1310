<h1>SUPPORT MENU</h1>
<table border=0 cellspacing=0 cellpadding=0 width=750>
 <tr>
 <td height=40><p><font size=4><strong>Setup </strong> </font><font size=4><b></b></font><font face=Arial size=4><b></a></b></font></p>
 <p><a name=01><strong>Internet</strong><br>
 There are two ways that you can configure your new D-Link router. You can either use the Setup Wizard or configure the settings manually. </p></td>

 </tr>
 <tr>
 <td height=59><p><b>Setup Wizard</b><br>
 The Setup Wizard section contains two useful wizards to help setup the <?query("/sys/modelname");?> to quickly connect to your ISP (Internet Service Provider), and also to configure its Wireless settings.<br>
<strong>Launch Internet Connection Setup Wizard </strong><br>
 Click this button to have the router walk you through a few simple steps to help you connect your router to the Internet. <br><strong>Launch Wireless Security Setup Wizard </strong><br>
 Click this button to have the router walk you through configuring the settings of your wireless network, including security. </p>

 </td>
 </tr>
 <tr>
 <td height=32>&nbsp;</td>
 </tr>
 <tr>
 <td height=46><strong>Manual Configure </strong></a><br>
 Chose this option if you would like to input the settings needed to connect your router to the Internet manually. </p> <p> <strong>Internet Connection Type </strong><br>

The Internet Connection Settings are used to connect the <?query("/sys/modelname");?> to the Internet. Any information that needs to be entered on this page will be provided to you by your ISP and often times referred to as &quot;public settings&quot;. Please select the appropriate option for your specific ISP. <strong></strong>If you are unsure of which option to select, please contact your ISP. </p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=61><p><strong>Static IP Address</strong><br>

Select this option if your ISP (Internet Service Provider) has provided you with an IP address, Subnet Mask, Default Gateway, and a DNS server address. Enter this information in the appropriate fields. If you are unsure of what to enter in these fields, please contact your ISP. </p> </td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=142>
 <p><strong>Dynamic IP Address</strong><br>

Select this option if your ISP (Internet Service Provider) provides you an IP address automatically. Cable modem providers typically use dynamic assignment of IP Address. </p>
 <p><strong><em>Host Name </em></strong>(optional) - The Host Name field is optional but may be required by some Internet Service Providers. The default host name is the model number of the router. <br><strong><em>MAC Address </em></strong> (optional) - The MAC (Media Access Control) Address field is required by some Internet Service Providers (ISP). The default MAC address is set to the MAC address of the WAN interface on the <?query("/sys/modelname");?>. You can use the &quot;Clone MAC Address&quot; button to automatically copy the MAC address of the Ethernet Card installed in the computer used to configure the device. It is only necessary to fill in the field if required by your ISP.<br> <strong><em>Primary DNS Address </em></strong><em> - </em> Enter the Primary DNS (Domain Name Service) server IP address provided to you by your ISP.<br> <strong><em>Secondary DNS Address </em></strong>(optional) <em> - </em> If you were given a secondary DNS server IP address from your ISP, enter it in this field. <br><strong><em>MTU </em></strong><em> - </em> MTU (Maximum Transmission/Transfer Unit) is the largest packet size that can be sent over a network. Messages larger than the MTU are divided into smaller packets. 1500 is the default value for this option. Changing this number may adversely affect the performance of your router. Only change this number if instructed to by one of our Technical Support Representatives or by your ISP. </p>

 </td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=47>
 <p><strong>PPPoE</strong><br>

Select this option if your ISP requires you to use a PPPoE (Point to Point Protocol over Ethernet) connection. DSL providers typically use this option. Select Dynamic PPPoE to obtain an IP address automatically for your PPPoE connection (used by majority of PPPoE connections). Select Static PPPoE to use a static IP address for your PPPoE connection. </p>
 <p><strong><em>User Name </em></strong> - Enter your PPPoE username. <br>
 <strong><em>Password </em></strong> - Enter your PPPoE password. <br>
 <strong><em>Service Name </em></strong>(optional) - If your ISP uses a service name for the PPPoE connection, enter the service name here.<br> <strong><em>IP Address </em></strong> - This option is only available for Static PPPoE. Enter in the static IP address for the PPPoE connection.<br> <strong><em>MAC Address </em></strong> (optional) - The MAC (Media Access Control) Address field is required by some Internet Service Providers (ISP). The default MAC address is set to the MAC address of the WAN interface on the <?query("/sys/modelname");?>. You can use the &quot;Clone MAC Address&quot; button to automatically copy the MAC address of the Ethernet Card installed in the computer used to configure the device. It is only necessary to fill in the field if required by your ISP. <br><strong><em>Primary DNS Address </em></strong> - Primary DNS (Domain Name System) server IP address, which may be provided by your ISP. You should only need to enter this information if you selected Static PPPoE. If Dynamic PPPoE is chosen, leave this field at its default value as your ISP will provide you this information automatically. <br>

 <strong><em>Secondary DNS Address </em></strong>(optional) - If you were given a secondary DNS server IP address from your ISP, enter it in this field. <strong><em>Maximum Idle time </em></strong>- The amount of time of inactivity before the device will disconnect your PPPoE session. Enter a Maximum Idle Time (in minutes) to define a maximum period of time for which the Internet connection is maintained during inactivity. If the connection is inactive for longer than the defined Maximum Idle Time, then the connection will be dropped. This option only applies to the Connect-on-demand Connect mode.<br> <strong><em>MTU </em></strong><em> - </em> MTU (Maximum Transmission/Transfer Unit) is the largest packet size that can be sent over a network. Messages larger than the MTU are divided into smaller packets. 1492 is the default value for this option. Changing this number may adversely affect the performance of your router. Only change this number if instructed to by one of our Technical Support Representatives or by your ISP.<br><strong><em>Connect mode select - </em></strong>Select Always-on if you would like the router to never disconnect the PPPoE session. Select Manual if you would like to control when the router is connected and disconnected from the Internet. The Connect-on-demand option allows the router to establish a connection to the Internet only when a device on your network tries to access a resource on the Internet. </p>
 </td>
 </tr>

 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>PPTP</strong><br>
Select this option if your ISP uses a PPTP (Point to Point Tunneling Protocol) connection and has assigned you a username and password in order to access the Internet. Select Dynamic PPTP to obtain an IP address automatically for your PPTP connection. Select Static PPTP to use a static IP address for your PPTP connection. </p>
 <p><strong><em>IP Address </em></strong> - Enter the IP address that your ISP has assigned to you. <br>

 <strong><em>Subnet Mask </em></strong> - Enter the Subnet Mask that your ISP has assigned to you.<br><strong><em>Gateway - </em></strong>Enter the Gateway IP address assigned to you by your ISP.<br> <strong><em>DNS - </em></strong>Enter the DNS address assigned to you by your ISP. <br>
 <strong><em>Server IP </em></strong> - Enter the IP address of the server, which will be provided by your ISP that you will be connecting to.<br> <strong><em>Account </em></strong> - Enter your PPTP Username. <br><strong><em>Password </em></strong> - Enter your PPTP Password.<br> <strong><em>Maximum Idle time </em></strong>- The amount of time of inactivity before the device will disconnect your PPTP session. Enter a Maximum Idle Time (in minutes) to define a maximum period of time for which the Internet connection is maintained during inactivity. If the connection is inactive for longer than the defined Maximum Idle Time, then the connection will be dropped. This option only applies to the Connect-on-demand Connect mode.<br><strong><em>MTU </em></strong><em> - </em> MTU (Maximum Transmission/Transfer Unit) is the largest packet size that can be sent over a network. Messages larger than the MTU are divided into smaller packets. 1400 is the default value for this option. Changing this number may adversely affect the performance of your router. Only change this number if instructed to by one of our Technical Support Representatives or by your ISP.<br><strong><em>Connect mode select - </em></strong>Select Always-on if you would like the router to never disconnect the PPTP session. Select Manual if you would like to control when the router is connected and disconnected from the Internet. The Connect-on-demand option allows the router to establish a connection to the Internet only when a device on your network tries to access a resource on the Internet.

</p>
 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>L2TP </strong><br>
Select this option if your ISP uses a L2TP (Layer 2 Tunneling Protocol) connection and has assigned you a username and password in order to access the Internet. Select Dynamic L2TP to obtain an IP address automatically for your L2TP connection. Select Static L2TP to use a static IP address for your L2TP connection. </p>

 <p><strong><em>IP Address </em></strong> - Enter the IP address that your ISP has assigned to you. <br>
 <strong><em>Subnet Mask </em></strong> - Enter the Subnet Mask that your ISP has assigned to you.<br> <strong><em>Gateway - </em></strong>Enter the Gateway IP address assigned to you by your ISP.<br> <strong><em>DNS - </em></strong>Enter the DNS address assigned to you by your ISP. <br><strong><em>Server IP </em></strong> - Enter the IP address of the server, which will be provided by your ISP, that you will be connecting to.<br> <strong><em>L2TP Account </em></strong> - Enter your L2TP Username.<br> <strong><em>L2TP Password </em></strong> - Enter your L2TP Password. <br><strong><em>Maximum Idle time </em></strong>- The amount of time of inactivity before the device will disconnect your L2TP session. Enter a Maximum Idle Time (in minutes) to define a maximum period of time for which the Internet connection is maintained during inactivity. If the connection is inactive for longer than the defined Maximum Idle Time, then the connection will be dropped. This option only applies to the Connect-on-demand Connect mode.<br><strong><em>MTU </em></strong><em> - </em> MTU (Maximum Transmission/Transfer Unit) is the largest packet size that can be sent over a network. Messages larger than the MTU are divided into smaller packets. 1400 is the default value for this option. Changing this number may adversely affect the performance of your router. Only change this number if instructed to by one of our Technical Support Representatives or by your ISP. <strong><em>Connect mode select - </em></strong>Select Always-on if you would like the router to never disconnect the L2TP session. Select Manual if you would like to control when the router is connected and disconnected from the Internet. The Connect-on-demand option allows the router to establish a connection to the Internet only when a device on your network tries to access a resource on the Internet.</p>

 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>BigPond </strong><br>
 Select this option if your ISP is BigPond. </p>

 <p><strong><em>Username - </em></strong>Enter your BigPond Username. <br>
 <strong><em>Password </em></strong><strong><em> - </em></strong>Enter your BigPond Password.<br>
 <strong><em>Auth Server - </em></strong>Select either sm-server or dce-server.<br>
 <strong><em>Login Server IP </em></strong>(optional) - Enter the IP address of Login Server you have been assigned.<br>

 <strong><em>MAC Address </em></strong> (optional) - The MAC (Media Access Control) Address field is required by some Internet Service Providers (ISP). The default MAC address is set to the MAC address of the WAN interface on the <?query("/sys/modelname");?>. You can use the &quot;Clone MAC Address&quot; button to automatically copy the MAC address of the Ethernet Card installed in the computer used to configure the device. It is only necessary to fill in the field if required by your ISP. </p>
 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>

 <tr>
 <td height=20><p><a name=02><strong>Wireless Settings</strong></a><br>
The Wireless Settings page contains the settings for the (Access Point) Portion of the <?query("/sys/modelname");?>. This page allows you to customize your wireless network or configure the <?query("/sys/modelname");?> to fit an existing wireless network that you may already have setup. </p>
 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>

 <tr>
 <td height=20><p> <strong>Wireless Network Name</strong><br>
Also known as the SSID (Service Set Identifier), this is the name of your Wireless Local Area Network (WLAN). By default the SSID of the <?query("/sys/modelname");?> is &quot;dlink&quot;. This can be easily changed to establish a new wireless network or to add the <?query("/sys/modelname");?> to an existing wireless network. </p>
 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>

 </tr>
 <tr>
 <td height=20><p><strong>Wireless Channel </strong><br>
Indicates which channel the <?query("/sys/modelname");?> is operating on. By default the channel is set to 6. This can be changed to fit the channel setting for an existing wireless network or to customize your new wireless network. Click the Enable Auto Scan checkbox to have the <?query("/sys/modelname");?> automatically select the channel that it will operate on. This option is recommended because the router will choose the channel with the least amount of interference.<br>
 </p>
 <p></p></td>
 </tr>
 <tr>

 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>802.11g Only Mode<br></strong>Enable this mode if your network is made up of purely 802.11g devices. If you have both 802.11b and 802.11g wireless clients, disable this mode.
</p>
 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>

 </tr>
 <tr>
 <td height=20><p> <strong>Enable Hidden Wireless </strong><br>
Select Enabled if you would not like the SSID of your wireless network to be broadcasted by the <?query("/sys/modelname");?>.&nbsp;If this option is Enabled, the SSID of the <?query("/sys/modelname");?> will not be seen by Site Survey utilities, so when setting up your wireless clients, you will have to know the SSID of your <?query("/sys/modelname");?> and enter it manually in order to connect to the router. This option is enabled by default. </p>
 <p></p></td>
 </tr>
 <tr>

 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>Wireless Security Mode<br></strong>Securing your wireless network is important as it is used to protect the integrity of the information being transmitted over your wireless network. The <?query("/sys/modelname");?> is capable of 4 types of wireless security; WEP, WPA, WPA2, and WPA2-Auto.
</p>
 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>

 </tr>
 <tr>
 <td height=20><p> <strong>WEP </strong><br>
Wired Equivalent Protocol (WEP) is a wireless security protocol for Wireless Local Area Networks (WLAN). WEP provides security by encrypting the data that is sent over the WLAN. The <?query("/sys/modelname");?> supports 2 levels of WEP Encryption: 64-bit and 128-bit. WEP is disabled by default. The WEP setting can be changed to fit an existing wireless network or to customize your wireless network. </p>
 <strong><em>Authentication - </em></strong>Authentication is a process by which the <?query("/sys/modelname");?> verifies the identity of a network device that is attempting to join the wireless network. There are two types authentication for this device when using WEP. <br> 
 <strong><em>Open System -</em></strong> Select this option to allow all wireless devices to communicate with the <?query("/sys/modelname");?> before they are required to provide the encryption key needed to gain access to the network. <br>

 <strong><em>Shared Key -</em></strong> Select this option to require any wireless device attempting to communicate with the <?query("/sys/modelname");?> to provide the encryption key needed to access the network before they are allowed to communicate with the <?query("/sys/modelname");?>. <strong><em>WEP Encryption -</em></strong>Select the level of WEP Encryption that you would like to use on your network. The two supported levels of WEP encryption are 64-bit and 128-bit. <br>
 <strong><em>Key Type -</em></strong>The Key Types that are supported by the <?query("/sys/modelname");?> are HEX (Hexadecimal) and ASCII (American Standard Code for Information Interchange.) The Key Type can be changed to fit an existing wireless network or to customize your wireless network.<br> <strong><em>Keys -</em></strong>Keys 1-4 allow you to easily change wireless encryption settings to maintain a secure network. Simply select the specific key to be used for encrypting wireless data on the network.
<p></p></td>
 </tr>
 <tr>

 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>WPA </strong><strong>-Personal </strong><br>
Wi-Fi Protected Access authorizes and authenticates users onto the wireless network. WPA uses stronger security than WEP and is based on a key that changes automatically at a regular interval. </p>
 <strong><em>Cipher Type -</em></strong>The <?query("/sys/modelname");?> support two different cipher types when WPA is used as the Security Type. These two options are TKIP (Temporal Key Integrity Protocol) and AES (Advanced Encryption Standard). <br>
 <strong><em>PSK/EAP -</em></strong>When PSK is selected, your wireless clients will need to provide a Passphrase for authentication. When EAP is selected, you will need to have a RADIUS server on your network which will handle the authentication of all your wireless clients. <br>

 <strong><em>Passphrase -</em></strong>This is what your wireless clients will need in order to communicate with your <?query("/sys/modelname");?>, when PSK is selected. Enter 8-63 alphanumeric characters. Be sure to write this Passphrase down as you will need to enter it on any other wireless devices you are trying to add to your network. <br>
 <strong><em>802.1X -</em></strong>This means of WPA authentication is used in conjunction with a RADIUS server that must be present on your network. Enter the IP address, port, and Shared Secret that you RADIUS is configured for. You also have the option to enter information for a second RADIUS server in the even that there are two on your network which you are using to authenticate wireless clients.
</p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>

 <td height=20><p><strong>WPA2 </strong><br>
Wi-Fi Protected Access 2 authorizes and authenticates users onto the wireless network. WPA2 uses stronger security than WEP and is based on a key that changes automatically at a regular interval. </p>
 <strong><em>Cipher Type -</em></strong>The <?query("/sys/modelname");?> support two different cipher types when WPA is used as the Security Type. These two options are TKIP (Temporal Key Integrity Protocol) and AES (Advanced Encryption Standard). <br>
 <strong><em>PSK/EAP -</em></strong>When PSK is selected, your wireless clients will need to provide a Passphrase for authentication. When EAP is selected, you will need to have a RADIUS server on your network which will handle the authentication of all your wireless clients.<br>
 <strong><em>Passphrase -</em></strong>This is what your wireless clients will need in order to communicate with your <?query("/sys/modelname");?>, when PSK is selected. Enter 8-63 alphanumeric characters. Be sure to write this Passphrase down as you will need to enter it on any other wireless devices you are trying to add to your network. <br>

 <strong><em>802.1X -</em></strong>This means of WPA2 authentication is used in conjunction with a RADIUS server that must be present on your network. Enter the IP address, port, and Shared Secret that you RADIUS is configured for. You also have the option to enter information for a second RADIUS server in the even that there are two on your network which you are using to authenticate wireless clients.
</p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p> <strong>WPA2-Auto </strong><br>

This option allows the <?query("/sys/modelname");?> to have both WPA2 and WPA clients connected to it simultaneously. </p>
 <p></p></td>
 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
<td height=20><p><a name=03><strong>Network Settings</strong></a><br>

These are the settings of the LAN (Local Area Network) interface for the device. These settings may be referred to as &quot;private settings&quot;. You may change the LAN IP address if needed. The LAN IP address is private to your internal network and cannot be seen on the Internet. The default IP address is 192.168.0.1 with a subnet mask of 255.255.255.0. <br><br>
<strong><em>IP Address -</em></strong>IP address of the <?query("/sys/modelname");?>, default is 192.168.0.1. <br>
 <strong><em>Subnet Mask -</em></strong>Subnet Mask of <?query("/sys/modelname");?>, default is 255.255.255.0. <br>
 <strong><em>Local Domain Name- </em></strong>(optional) Enter in the local domain name for your network. <strong><em></em></strong>
 <p></p></td>

 </tr>
 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong><em>DNS Relay -</em></strong>When DNS Relay is enabled, DHCP clients of the router will be assigned the router's LAN IP address as their DNS server. All DNS requests that the router receives will be forwarded to your ISPs DNS servers. When DNS relay is disabled, all DHCP clients of the router will be assigned the ISP DNS server. </p>
 <p></p></td>
 </tr>

 <tr>
 <td height=20>&nbsp;</td>
 </tr>
 <tr>
 <td height=20><p><strong>DHCP Server </strong><br>
DHCP stands for Dynamic Host Control Protocol. The DHCP server assigns IP addresses to devices on the network that request them. These devices must be set to &quot;Obtain the IP address automatically&quot;. By default, the DHCP Server is enabled on the <?query("/sys/modelname");?>. The DHCP address pool contains the range of the IP address that will automatically be assigned to the clients on the network. </p>
 <p><strong><em>Starting IP address </em></strong>The starting IP address for the DHCP server's IP assignment. <br>

 <strong><em>Ending IP address </em></strong>The ending IP address for the DHCP server's IP assignment. <br>
 <strong><em>Lease Time </em></strong>The length of time in minutes for the IP lease. </p>
 <p>Dynamic DHCP client computers connected to the unit will have their information displayed in the Dynamic DHCP Client Table. The table will show the Host Name, IP Address, MAC Address, and Expired Time of the DHCP lease for each client computer. </p> <p></p></td>
 </tr>
 </table>
