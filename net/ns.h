#include <inc/ns.h>
#include <inc/lib.h>

#include <ip-config.h>

#ifndef IP
#define IP "10.0.2.15"
#endif

#ifndef MASK
#define MASK "255.255.255.0"
#endif

#ifndef DEFAULT
#define DEFAULT "10.0.2.2"
#endif

#ifndef PIGEON_PORT
#define PIGEON_PORT 8008
#endif

#ifndef DUCK_MAC
#define DUCK_MAC {0x52, 0x54, 0x00, 0x12, 0x34, 0x56}
#endif

#define TIMER_INTERVAL 250

// Virtual address at which to receive page mappings containing client requests.
#define QUEUE_SIZE	20
#define REQVA		(0xdffff000 - QUEUE_SIZE * PGSIZE)

/* timer.c */
void timer(envid_t ns_envid, uint32_t initial_to);

/* input.c */
void input(envid_t ns_envid);

/* output.c */
void output(envid_t ns_envid);

