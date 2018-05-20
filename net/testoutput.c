#include "ns.h"

#ifndef TESTOUTPUT_COUNT
#define TESTOUTPUT_COUNT 10
#endif

static envid_t output_envid;

static struct jif_pkt *pkt = (struct jif_pkt*)REQVA;


void
umain(int argc, char **argv)
{
	envid_t ns_envid = sys_getenvid();
	int i, r;

	binaryname = "testoutput";

	output_envid = fork();
	if (output_envid < 0)
		panic("error forking");
	else if (output_envid == 0) {
		output(ns_envid);
		return;
	}

	for (i = 0; i < TESTOUTPUT_COUNT; i++) {
		if ((r = sys_page_alloc(0, pkt, PTE_P|PTE_U|PTE_W)) < 0)
			panic("sys_page_alloc: %e", r);
		pkt->jp_len = snprintf(pkt->jp_data,
				       PGSIZE - sizeof(pkt->jp_len),
				       // "Packet %02d", i);
				       "\x98\x83\x89\x2a\x3b\x64\x52\x54\x00\x12\x34\x56\x08\x00\x45\x00\x00\x3c\x6d\xe4\x00\x00\x80\x01\xb4\xcc\x0a\x00\x02\x0f\x0a\x00\x02\x02\x08\x00\x47\x06\x00\x01\x06\x55""aaaaaaaabbbbbbbbcccccccc%08d", i);
		cprintf("Transmitting packet %d\n", i);
		ipc_send(output_envid, NSREQ_OUTPUT, pkt, PTE_P|PTE_W|PTE_U);
		sys_page_unmap(0, pkt);
	}

	// Spin for a while, just in case IPC's or packets need to be flushed
	for (i = 0; i < TESTOUTPUT_COUNT*2; i++)
		sys_yield();
}
