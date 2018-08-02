#include <inc/lib.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <lwip/err.h>
#include <net/ns.h>

int errno;
// envid_t idle_env;

static void
handle_client(int sock)
{
	// int r = sys_env_destroy(idle_env);
	// if(r)
		// cprintf("Fail to destroy idle\n");
	cprintf("handle sock %d\n", sock);
	char s[512];
	int received = -1;
	for(;;)
	{
		for(int i = 0; i < 50; i++) sys_yield();
		if((received = read(sock, s, sizeof(s) - 1)) < 0)
		{
			cprintf("failed to read\n");
			break;
		}
	}
	close(sock);
	
	// idle_env = spawnl("idle", "idle", 0);
	// if(idle_env < 0)
		// cprintf("Failed to spawn idle\n");
	
	// exit();
}

typedef struct {
	int size;
	int n_pages;
	int start;
	char md5[50];
} FileData;

void
umain(int argc, char **argv)
{
	int serversock, clientsock;
	struct sockaddr_in server, client, clientStatic;

	// Create the TCP socket
	if((serversock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		cprintf("Failed to create socket");
		return;
	}
	if((clientsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		cprintf("Failed to create socket");
		return;
	}

	// Construct the server sockaddr_in structure
	memset(&server, 0, sizeof(server));		// Clear struct
	server.sin_family = AF_INET;			// Internet/IP
	server.sin_addr.s_addr = htonl(INADDR_ANY);	// IP address
	server.sin_port = htons(8000);			// server port
	
	memset(&clientStatic, 0, sizeof(clientStatic));		// Clear struct
	clientStatic.sin_family = AF_INET;			// Internet/IP
	clientStatic.sin_addr.s_addr = inet_addr(DEFAULT);	// IP address
	clientStatic.sin_port = htons(PIGEON_PORT);		// server port

	// Bind the server socket
	if(bind(serversock, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		cprintf("Failed to bind the server socket");
		return;
	}

	// Listen on the server socket
	/*if(listen(serversock, 5) < 0)
	{
		cprintf("Failed to listen on server socket");
		return;
	}*/

	cprintf("Waiting for connections...\n");
	
	// idle_env = spawnl("idle", "idle", 0);
	// if(idle_env < 0)
		// cprintf("Failed to spawn idle\n");

	char *s = (char *) 0xc0000000;
	const int BUFLEN = PGSIZE;
	sys_page_alloc(0, s, PTE_P | PTE_U | PTE_W);
	
	char *judge_pages = s - JUDGE_PAGES_SIZE;
	
	int ret = sys_map_judge_pages(judge_pages, 0, JUDGE_PAGES_SIZE);
	cprintf("sys_map_judge_pages returns %d, expected %d\n", ret, JUDGE_PAGES_COUNT);
	memset(judge_pages, 0, sizeof(judge_pages));
	
	unsigned *first_page = (unsigned *) judge_pages;
	unsigned *second_page = first_page + PGSIZE / sizeof(unsigned);
	unsigned *input_pos = first_page;
	unsigned *input_len = first_page + 1;
	unsigned *answer_pos = second_page;
	unsigned *answer_len = second_page + 1;
	
	void *meta_start = judge_pages + PGSIZE * 2;
	FileData *filedata = (FileData *) meta_start;
	void *file_start = meta_start + ROUNDUP(JUDGE_PAGES_COUNT * sizeof(FileData), PGSIZE);
	int n_files = 0;
	
	// Use judge pages as LRU cache
	
	while (1) {
		unsigned int clientlen = sizeof(client);
		int received;
		errno = 233;
		clientlen = 233;
		
		memset(s, 0, PGSIZE);
		
		if ((received = recvfrom(serversock, s, BUFLEN - 1, 0, (struct sockaddr *) &client, &clientlen)) < 0) {
			cprintf("recvfrom failed %d\n", errno);
			exit();
		}
		s[received] = 0;  // ??? Is it necessary ?
		
		char md5[50];
		
		int to_send = 0;
		
		if (strcmp(s, "clear") == 0) {
			// Pass
		} else if (s[0] == 'c') {
			cprintf("command:%s:\n", s + 1);
			int fd = open("tmp", O_RDWR | O_CREAT | O_TRUNC);
			write(fd, s + 1, received - 1);
			write(fd, "\n", 1);
			close(fd);
			sync();
			int r = spawnl("sh", "sh", "-w", "tmp", 0);
			if(r < 0) {
				cprintf("failed to spawn %e\n", r);
			}
			wait(r);
			cprintf("wait ok\n");
			strcpy(s, "runCmd ok");
			to_send = strlen(s);
		} else if (s[0] == 'o') {
			// open file
			int fd = open(s + 1, O_RDONLY);
			if (fd < 0) {
				cprintf("Can't open file [%s]\n", s + 1);
			} else {
				for (int i = 0; i < 64; i++) {
					s[0] = 'f';
					int r = read(fd, s + 1, 15);
					if (r > 0) {
						sendto(clientsock, s, r + 1, 0, (struct sockaddr *) &clientStatic, clientlen);
					} else {
						break;
					}
				}
				s[0] = 'F';
				sendto(clientsock, s, 1, 0, (struct sockaddr *) &clientStatic, clientlen);
			}
			close(fd);
			sync();
		} else if (s[0] == 'f') {
			cprintf("[j][recv sendfile!!!]\n");
			// write file
			
			int fd = open(s + 1, O_RDWR | O_CREAT | O_TRUNC);
			
			if (sendto(clientsock, "file", 4, 0, (struct sockaddr *) &clientStatic, clientlen) < 0) {
				cprintf("sendto error file\n");
			}
			
			while (1) {
				if ((received = recvfrom(serversock, s, BUFLEN - 1, 0, (struct sockaddr *) &client, &clientlen)) < 0) {
					cprintf("failed to recvfrom\n");
					break;
				}
				s[received] = 0;
				if (s[0] != 'g') break;
				if (strcmp(s, "clear") == 0) {
					break;
				}
				
				int off;
				memcpy(&off, s + 4, 4);
				
				seek(fd, off);
				write(fd, s + 8, received - 8);
				
				s[0] = 'a';
				if (sendto(clientsock, s, 8, 0, (struct sockaddr *) &clientStatic, clientlen) < 0) {
					cprintf("sendto error file part\n");
				}
			}
			
			close(fd);
			sync();
			strcpy(s, "sendFile ok");
			to_send = strlen(s);
		} else if (strncmp(s, "sendobj ", 8) == 0) {
			int size = *(int *) (s + 8);
			strncpy(md5, s + 12, 49);
			int ok = 0;
			for (int i = 0; i < n_files; i++) {
				if (strcmp(filedata[i].md5, md5) == 0) {
					ok = 1;
					break;
				}
			}
			if (ok) {
				strcpy(s, "gg sendobj");
				sendto(clientsock, s, strlen(s), 0, (struct sockaddr *) &clientStatic, clientlen);
			} else {
				strcpy(s, "sendobj begin");
				sendto(clientsock, s, strlen(s), 0, (struct sockaddr *) &clientStatic, clientlen);
				// TODO
				
				int last_pos = file_start - (void *) judge_pages;
				if (n_files > 0) {
					last_pos = filedata[n_files - 1].start + filedata[n_files - 1].n_pages * PGSIZE;
				}
				FileData *fdata = filedata + n_files;
				fdata->size = size;
				strcpy(fdata->md5, md5);
				fdata->n_pages = ROUNDUP(fdata->size, PGSIZE) / PGSIZE;
				fdata->start = last_pos;
				
				memset(judge_pages + fdata->start, 0, fdata->n_pages * PGSIZE);
				
				while (1) {
					if ((received = recvfrom(serversock, s, BUFLEN - 1, 0, (struct sockaddr *) &client, &clientlen)) < 0) {
						cprintf("failed to recvfrom\n");
						break;
					}
					s[received] = 0;
					if (s[0] != 'g') break;
					if (strcmp(s, "clear") == 0) {
						break;
					}
					int off;
					memcpy(&off, s + 4, 4);
					if (off >= 0 && off <= size && off + received - 8 <= size) {
						memcpy(judge_pages + fdata->start + off, s + 8, received - 8);
					}
					s[0] = 'a';
					if (sendto(clientsock, s, 8, 0, (struct sockaddr *) &clientStatic, clientlen) < 0) {
						cprintf("sendto error file part\n");
					}
				}
				
				// Write Ahead Logging !!!
				++n_files;
				strcpy(s, "sendobj ok");
				sendto(clientsock, s, strlen(s), 0, (struct sockaddr *) &clientStatic, clientlen);
			}
		} else if (strncmp(s, "setobj_I", 8) == 0) {
			strncpy(md5, s + 8, 49);
			
			for (int i = 0; i < n_files; i++) {
				if (strcmp(filedata[i].md5, md5) == 0) {
					*input_pos = filedata[i].start;
					*input_len = filedata[i].size;
					break;
				}
			}
			
			strcpy(s, "setobj_I ok");
			sendto(clientsock, s, strlen(s), 0, (struct sockaddr *) &clientStatic, clientlen);
		} else if (strncmp(s, "setobj_A", 8) == 0) {
			strncpy(md5, s + 8, 49);
			
			for (int i = 0; i < n_files; i++) {
				if (strcmp(filedata[i].md5, md5) == 0) {
					*answer_pos = filedata[i].start;
					*answer_len = filedata[i].size;
					break;
				}
			}
			
			strcpy(s, "setobj_A ok");
			sendto(clientsock, s, strlen(s), 0, (struct sockaddr *) &clientStatic, clientlen);
		} else {
			// Pass
		}
		
		if (to_send > 0) {
			sendto(clientsock, s, to_send, 0, (struct sockaddr *) &clientStatic, clientlen);
		}
	}

	close(serversock);
}
