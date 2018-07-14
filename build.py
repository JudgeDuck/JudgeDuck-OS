#encoding=utf-8

import subprocess

def read_file(name, fallback = ""):
	try:
		f = open(name, "r")
		res = f.read()
		f.close()
		return res
	except:
		return fallback

def write_file(name, content):
	try:
		f = open(name, "w")
		f.write(content)
		f.close()
		return True
	except:
		pass

def system(name, args, timeout = 5):
	try:
		cp = subprocess.run([name] + args, stdout=subprocess.PIPE, timeout=timeout)
		return str(cp.stdout, "utf-8")
	except:
		return ""



last_ip_addr = 50

def get_next_ip():
	global last_ip_addr
	last_ip_addr += 1
	return "10.0.2.%s" % last_ip_addr

last_port = 20800

def get_next_port():
	global last_port
	last_port += 1
	return last_port

MACs = read_file("ducks.txt").split("\n")

arp_content = []
dhcp_content = []
ducks_config_content = []

for MAC in MACs:
	MAC_a = MAC.split(":")
	if len(MAC_a) != 6: continue

	duck_mac = []
	for s in MAC_a:
		duck_mac.append("0x" + s)
	duck_mac = ", ".join(duck_mac)
	duck_mac = "{%s}" % duck_mac

	ip_str = get_next_ip()
	ip = '"%s"' % ip_str
	mask = '"255.255.255.0"'
	default = '"10.0.2.2"'
	pigeon_port = get_next_port()

	content = "\n".join([
		"#define IP %s" % ip,
		"#define MASK %s" % mask,
		"#define DEFAULT %s" % default,
		"#define PIGEON_PORT %s" % pigeon_port,
		"#define DUCK_MAC %s" % duck_mac,
	])
	write_file("ip-config.h", content)
	system("make", ["grub"], 100)
	system("mv", ["obj/jos-grub", "obj/jos-grub-%s" % ip_str])
	
	arp_content.append("arp -s %s %s" % (ip_str, MAC))
	dhcp_content.append("\n".join([
		"host Duck%s {" % pigeon_port,
		"hardware ethernet %s;" % MAC,
		"fixed-address %s;" % ip_str,
		"}",
	]))
	ducks_config_content.append("%s %s" % (ip_str, pigeon_port))

write_file("arp-bind.sh", "\n".join(arp_content))
write_file("dhcp-config.txt", "\n".join(dhcp_content))
write_file("ducks-config.txt", "\n".join(ducks_config_content))



# DUCK_MAC {0x52, ...};
# IP "*.*.*.*"
# MASK
# DEFAULT
# PIGEON_PORT

