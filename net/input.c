#include "ns.h"

extern union Nsipc nsipcbuf;

static char buf[PGSIZE * 2] __attribute__((aligned(PGSIZE)));
static struct jif_pkt *pkt = (struct jif_pkt *) buf;
static struct jif_pkt *pkt2 = (struct jif_pkt *) (buf + PGSIZE);

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.
	memset(buf, 0, sizeof(buf));
	for(;;)
	{
		for(int i = 0; i < 10; i++) sys_yield();
		if(sys_net_try_receive(pkt) < 0)
			continue;
		ipc_send(ns_envid, NSREQ_INPUT, pkt, PTE_U | PTE_P);
		struct jif_pkt *t = pkt; pkt = pkt2; pkt2 = t;
	}
}
