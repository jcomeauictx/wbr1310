<script>
// vi: set sw=4 ts=4: ************************ ip.js start *************************************

// convert to MAC address string to array.
// myMAC[0] contains the orginal ip string. (dot spereated format).
// myMAC[1~6] contain the 6 parts of the MAC address.
function get_mac(m)
{
	var myMAC=new Array();
	if (m.search(":") != -1)    var tmp=m.split(":");
	else                        var tmp=m.split("-");

	for (var i=0;i <= 6;i++) myMAC[i]="";
	if (m != "")
	{
		for (var i=1;i <= tmp.length;i++) myMAC[i]=tmp[i-1];
		myMAC[0]=m;
	}
	return myMAC;
}

// convert to ip address string to array.
// myIP[0] contains the orginal ip string. (dot spereated format).
// myIP[1~4] contain the 4 parts of the ip address.
function get_ip(str_ip)
{
	var myIP=new Array();

	myIP[0] = myIP[1] = myIP[2] = myIP[3] = myIP[4] = "";
	if (str_ip != "")
	{
		var tmp=str_ip.split(".");
		for (var i=1;i <= tmp.length;i++) myIP[i]=tmp[i-1];
		myIP[0]=str_ip;
	}
	else
	{
		for (var i=0; i <= 4;i++) myIP[i]="";
	}
	return myIP;
}

// return netmask array according to the class of the ip address.
function generate_mask(str)
{
	var mask = new Array();
	var IP1 = decstr2int(str);

	mask[0] = "0.0.0.0";
	mask[1] = mask[2] = mask[3] = mask[4] = "0";

	if		(IP1 > 0 && IP1 < 128)
	{
		mask[0] = "255.0.0.0";
		mask[1] = "255";
	}
	else if	(IP1 > 127 && IP1 < 191)
	{
		mask[0] = "255.255.0.0";
		mask[1] = "255";
		mask[2] = "255";
	}
	else
	{
		mask[0] = "255.255.255.0";
		mask[1] = "255";
		mask[2] = "255";
		mask[3] = "255";
	}
	return mask;
}

// construct a IP array
function generate_ip(str1, str2, str3, str4)
{
	var ip = new Array();

	ip[1] = (str1=="") ? "0" : decstr2int(str1.value);
	ip[2] = (str2=="") ? "0" : decstr2int(str2.value);
	ip[3] = (str3=="") ? "0" : decstr2int(str3.value);
	ip[4] = (str4=="") ? "0" : decstr2int(str4.value);
	ip[0] = ip[1]+"."+ip[2]+"."+ip[3]+"."+ip[4];
	return ip;
}

// return IP array of network ID
function get_network_id(ip, mask)
{
	var id = new Array();
	var ipaddr = get_ip(ip);
	var subnet = get_ip(mask);

	id[1] = ipaddr[1] & subnet[1];
	id[2] = ipaddr[2] & subnet[2];
	id[3] = ipaddr[3] & subnet[3];
	id[4] = ipaddr[4] & subnet[4];
	id[0] = id[1]+"."+id[2]+"."+id[3]+"."+id[4];
	return id;
}

// return IP array of host ID
function get_host_id(ip, mask)
{
	var id = new Array();
	var ipaddr = get_ip(ip);
	var subnet = get_ip(mask);

	id[1] = ipaddr[1] & (subnet[1] ^ 255);
	id[2] = ipaddr[2] & (subnet[2] ^ 255);
	id[3] = ipaddr[3] & (subnet[3] ^ 255);
	id[4] = ipaddr[4] & (subnet[4] ^ 255);
	id[0] = id[1]+"."+id[2]+"."+id[3]+"."+id[4];
	return id;
}

// return IP array of Broadcast IP address
function get_broadcast_ip(ip, mask)
{
	var id = new Array();
	var ipaddr = get_ip(ip);
	var subnet = get_ip(mask);

	id[1] = ipaddr[1] | (subnet[1] ^ 255);
	id[2] = ipaddr[2] | (subnet[2] ^ 255);
	id[3] = ipaddr[3] | (subnet[3] ^ 255);
	id[4] = ipaddr[4] | (subnet[4] ^ 255);
	id[0] = id[1]+"."+id[2]+"."+id[3]+"."+id[4];
	return id;
}

function is_valid_port_str(port)
{
	return is_in_range(port, 1, 65535);
}

// return true if the port is valid.
function is_valid_port(obj)
{
	if (is_valid_port_str(obj.value)==false)
	{
		field_focus(obj, '**');
		return false;
	}
	return true;
}

function is_valid_port_range_str(port1, port2)
{
	if (is_blank(port1)) return false;
	if (!is_valid_port_str(port1)) return false;
	if (is_blank(port2)) return true;
	if (!is_valid_port_str(port2)) return false;
	var i = parseInt(port1, [10]);
	var j = parseInt(port2, [10]);
	if (i > j) return false;
	return true;
}

// return true if the port range is valid.
function is_valid_port_range(obj1, obj2)
{
	return is_valid_port_range_str(obj1.value, obj2.value);
}

// return true if the IP address is valid.
function is_valid_ip(ipaddr, optional)
{
	var ip = get_ip(ipaddr);

	if (optional != 0)
	{
		if (ip[1]=="" && ip[2]=="" && ip[3]=="" && ip[4]=="") return true;
	}

	if (is_in_range(ip[1], 1, 223)==false) return false;
	if (decstr2int(ip[1]) == 127) return false;
	if (is_in_range(ip[2], 0, 255)==false) return false;
	if (is_in_range(ip[3], 0, 255)==false) return false;
	if (is_in_range(ip[4], 1, 254)==false) return false;

	ip[0] = parseInt(ip[1],[10])+"."+parseInt(ip[2],[10])+"."+parseInt(ip[3],[10])+"."+parseInt(ip[4],[10]);
	if (ip[0] != ipaddr) return false;

	return true;
}

// return false if the value is not a valid netmask value.
function __is_mask(str)
{
	d = decstr2int(str);
	if (d==0 || d==128 || d==192 || d==224 || d==240 || d==248 || d==252 || d==254 || d==255) return true;
	return false;
}

// return true if the netmask is valid.
function is_valid_mask(mask)
{
	var IP = get_ip(mask);

	if (__is_mask(IP[1])==false) return false;
	if (IP[1] != "255")
	{
		if (IP[2]=="0" && IP[3]=="0" && IP[4]=="0") return true;
		return false;
	}

	if (__is_mask(IP[2])==false) return false;
	if (IP[2] != "255")
	{
		if (IP[3]=="0" && IP[4]=="0") return true;
		return false;
	}

	if (__is_mask(IP[3])==false) return false;
	if (IP[3] != "255")
	{
		if (IP[4]=="0") return true;
		return false;
	}

	if (__is_mask(IP[4])==false) return false;
	return true;
}

function is_valid_mac(mac)
{
	return is_hexdigit(mac);
}

// *********************************** ip.js stop *************************************
</script>
