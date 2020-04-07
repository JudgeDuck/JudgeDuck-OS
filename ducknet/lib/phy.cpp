#include <ducknet_phy.h>
#include <ducknet_ether.h>
#include "ducknet_impl.h"

static int (*send)(const void *, int);
static int (*recv)(void *);
static int (*flush)();
static int (*packet_handle)(void *, int);
static int (*send_packet_handle)(const void *, int);

static bool flushed;
static ducknet_time_t last_flush_time;
static ducknet_u64 flush_delay;

int ducknet_phy_init(const DucknetPhyConfig *conf) {
	send = conf->send;
	recv = conf->recv;
	flush = conf->flush;
	flushed = false;
	last_flush_time = ducknet_currenttime;
	flush_delay = ducknet_tsc_freq * 100 / 1000 / 1000;  // 0.1ms
	packet_handle = conf->packet_handle;
	send_packet_handle = conf->send_packet_handle;
	return 0;
}

int ducknet_phy_send(const void *a, int len) {
	int r;
	if (send_packet_handle && (r = send_packet_handle(a, len) < 0)) {
		return r;
	}
	
	flushed = false;
	return send ? send(a, len) : -1;
}

int ducknet_phy_recv(void *a) {
	return recv ? recv(a) : -1;
}

int ducknet_phy_flush() {
	if (flushed) {
		return 0;
	} else {
		int r = flush ? flush() : -1;
		if (r == 0) {
			flushed = true;
		}
		return r;
	}
}

int ducknet_phy_idle() {
	static int flush_cnt = 0;
	if (++flush_cnt >= 128 || ducknet_currenttime - last_flush_time > flush_delay) {
		ducknet_phy_flush();
		flush_cnt = 0;
		last_flush_time = ducknet_currenttime;
	}
	return 0;
}

int ducknet_phy_packet_handle(void *pkt, int len) {
	int r;
	if (packet_handle && (r = packet_handle(pkt, len)) < 0) {
		return r;
	}
	return ducknet_ether_packet_handle(pkt, len);
}
