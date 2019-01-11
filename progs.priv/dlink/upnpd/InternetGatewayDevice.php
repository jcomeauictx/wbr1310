<?
/* vi: set sw=4 ts=4:

	Device Description for Corega CG-WLBARGS.
	Includes 2 WANIPConnectionDevice for Multiple PPPoE Sessions.
	Also a Corega specific service for easy setting.
 */
$ipaddr =query("/runtime/upnp/host");
$port   =query("/runtime/upnp/port");
$uuid   =query("/runtime/upnp/uuid");
$model  =query("/sys/modelname");
$vendor =query("/sys/vendor");
$url    =query("/sys/url");
$SN     ="000000001";
$UPC    ="123456789";


echo "\<\?xml version=\"1.0\"\?\>";
?>
<root xmlns="urn:schemas-upnp-org:device-1-0">
	<specVersion>
		<major>1</major>
		<minor>0</minor>
	</specVersion>
	<URLBase>http://<?=$ipaddr?>:<?=$port?></URLBase>
	<device>
		<deviceType>urn:schemas-upnp-org:device:InternetGatewayDevice:1</deviceType>
		<friendlyName><?=$model?></friendlyName>
		<manufacturer><?=$vendor?></manufacturer>
		<manufacturerURL><?=$url?></manufacturerURL>
		<modelDescription>Internet Gateway Device</modelDescription>
		<modelName><?=$model?></modelName>
		<modelNumber>1</modelNumber>
		<modelURL><?=$url?></modelURL>
		<serialNumber><?=$SN?></serialNumber>
		<UDN>uuid:<?=$uuid?></UDN>
		<iconList>
			<icon>
				<mimetype>image/gif</mimetype>
				<width>118</width>
				<height>119</height>
				<depth>8</depth>
				<url>/ligd.gif</url>
			</icon>
		</iconList>
		<serviceList>
			<service>
				<serviceType>urn:schemas-microsoft-com:service:OSInfo:1</serviceType>
				<serviceId>urn:microsoft-com:serviceId:OSInfo1</serviceId>
				<controlURL>/OSInfo1</controlURL>
				<eventSubURL>/OSInfo1</eventSubURL>
				<SCPDURL>/OSInfo.xml</SCPDURL>
			</service>
			<service>
				<serviceType>urn:schemas-upnp-org:service:Layer3Forwarding:1</serviceType>
				<serviceId>urn:upnp-org:serviceId:L3Forwarding1</serviceId>
				<controlURL>/L3Forwarding1</controlURL>
				<eventSubURL>/L3Forwarding1</eventSubURL>
				<SCPDURL>/Layer3Forwarding.xml</SCPDURL>
			</service>
		</serviceList>
		<deviceList>
			<device>
				<deviceType>urn:schemas-upnp-org:device:WANDevice:1</deviceType>
				<friendlyName>WANDevice</friendlyName>
				<manufacturer><?=$vendor?></manufacturer>
				<manufacturerURL><?=$url?></manufacturerURL>
				<modelDescription>WANDevice</modelDescription>
				<modelName><?=$model?></modelName>
				<modelNumber>1</modelNumber>
				<modelURL><?=$url?></modelURL>
				<serialNumber><?=$SN?></serialNumber>
				<UDN>uuid:<?=$uuid?></UDN>
				<serviceList>
					<service>
						<serviceType>urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1</serviceType>
						<serviceId>urn:upnp-org:serviceId:WANCommonIFC1</serviceId>
						<controlURL>/WANCommonIFC1</controlURL>
						<eventSubURL>/WANCommonIFC1</eventSubURL>
						<SCPDURL>/WANCommonInterfaceConfig.xml</SCPDURL>
					</service>
				</serviceList>
				<deviceList>
					<device>
						<deviceType>urn:schemas-upnp-org:device:WANConnectionDevice:1</deviceType>
						<friendlyName>WANConnectionDevice</friendlyName>
						<manufacturer><?=$vendor?></manufacturer>
						<manufacturerURL><?=$url?></manufacturerURL>
						<modelDescription>WanConnectionDevice</modelDescription>
						<modelName><?=$model?></modelName>
						<modelNumber>1</modelNumber>
						<modelURL><?=$url?></modelURL>
						<serialNumber><?=$SN?></serialNumber>
						<UDN>uuid:<?=$uuid?></UDN>
						<serviceList>
							<service>
								<serviceType>urn:schemas-upnp-org:service:WANEthernetLinkConfig:1</serviceType>
								<serviceId>urn:upnp-org:serviceId:WANEthLinkC1</serviceId>
								<controlURL>/WANEthLinkC1</controlURL>
								<eventSubURL>/WANEthLinkC1</eventSubURL>
								<SCPDURL>/WANEthernetLinkConfig.xml</SCPDURL>
							</service>
							<service>
								<serviceType>urn:schemas-upnp-org:service:WANIPConnection:1</serviceType>
								<serviceId>urn:upnp-org:serviceId:WANIPConn1</serviceId>
								<controlURL>/WANIPConn1</controlURL>
								<eventSubURL>/WANIPConn1</eventSubURL>
								<SCPDURL>/WANIPConnection.xml</SCPDURL>
							</service>
						</serviceList>
					</device>
				</deviceList>
			</device>
		</deviceList>
		<presentationURL>http://<?=$ipaddr?>/</presentationURL>
	</device>
</root>
