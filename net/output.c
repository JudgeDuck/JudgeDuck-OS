#include "ns.h"

extern union Nsipc nsipcbuf;

static char buf[PGSIZE] __attribute__((aligned(PGSIZE)));
static struct jif_pkt *pkt = (struct jif_pkt *) buf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	for(;;)
	{
		int perm;
		envid_t whom;
		int reqno = ipc_recv(&whom, buf, &perm);
		if(reqno != NSREQ_OUTPUT)
			panic("ns output _ wrong reqno : got %d\n", reqno);
		int r;
		while((r = sys_net_try_transmit(pkt->jp_data, pkt->jp_len)) != 0)
		{
			cprintf("spin %d\n", r);
			sys_yield();
		}
	}
}
