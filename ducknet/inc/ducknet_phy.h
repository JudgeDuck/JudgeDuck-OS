#ifndef DUCKNET_PHY_H
#define DUCKNET_PHY_H

#include <ducknet_types.h>
#include <ducknet_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int (*send)(const void *, int);
	int (*recv)(void *);
	int (*flush)();
	int (*packet_handle)(void *, int);
} DucknetPhyConfig;

int ducknet_phy_init(const DucknetPhyConfig *);

int ducknet_phy_send(const void *, int);
int ducknet_phy_recv(void *);
int ducknet_phy_flush();

int ducknet_phy_idle();
int ducknet_phy_packet_handle(void *pkt, int len);

#ifdef __cplusplus
}
#endif

#endif
