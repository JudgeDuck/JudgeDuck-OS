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
	
	static char judge_pages[JUDGE_PAGES_SIZE] __attribute__((aligned(PGSIZE)));
	
	int ret = sys_map_judge_pages(judge_pages, 0, JUDGE_PAGES_SIZE);
	cprintf("sys_map_judge_pages returns %d, expected %d\n", ret, JUDGE_PAGES_COUNT);
	memset(judge_pages, 0, sizeof(judge_pages));
	unsigned *first_page = (unsigned *) judge_pages;
	unsigned *second_page = first_page + PGSIZE / sizeof(unsigned);
	unsigned *input_pos = first_page;
	unsigned *input_len = first_page + 1;
	unsigned *answer_pos = second_page;
	unsigned *answer_len = second_page + 1;
	//unsigned *taskliblog_pos = second_page + 2;
	//unsigned *taskliblog_maxlen = second_page + 3;
	//unsigned *taskliblog_len = second_page + 4;
	
	// Simple implementation of judge pages
	// TODO: Support various size of files
	int INPUT_START = 2 * PGSIZE;
	int INPUT_SIZE = 50 * 1024 * 1024;
	int ANSWER_START = INPUT_START + INPUT_SIZE;
	int ANSWER_SIZE = INPUT_SIZE;
	//int TASKLIBLOG_START = ANSWER_START + ANSWER_SIZE;
	//int TASKLIBLOG_SIZE = 1 * 1024 * 1024;
	
	*input_pos = INPUT_START;
	*answer_pos = ANSWER_START;
	//*taskliblog_pos = TASKLIBLOG_START;
	//*taskliblog_maxlen = TASKLIBLOG_SIZE;
	
	while(1)
	{
		unsigned int clientlen = sizeof(client);
		
		int received;
		errno = 233;
		clientlen = 233;
		if((received = recvfrom(serversock, s, BUFLEN - 1, 0, (struct sockaddr *) &client, &clientlen)) < 0)
		{
			cprintf("recvfrom failed %d\n", errno);
			exit();
		}
		s[received] = 0;
		if(s[0] == 'f')
		{
			cprintf("[j][recv sendfile!!!]\n");
			// write file
			char *file_start = NULL;
			unsigned file_maxsize = 0;
			unsigned file_cursize = 0;
			unsigned *file_len = NULL;
			if (strcmp(s + 1, "input.txt") == 0) {
				file_start = judge_pages + *input_pos;
				file_maxsize = INPUT_SIZE;
				file_len = input_len;
			} else if (strcmp(s + 1, "answer.txt") == 0) {
				file_start = judge_pages + *answer_pos;
				file_maxsize = ANSWER_SIZE;
				file_len = answer_len;
			}
			
			int fd = -1;
			if (!file_start) {
				fd = open(s + 1, O_RDWR | O_CREAT | O_TRUNC);
			}
			
			if(sendto(clientsock, "file", 4, 0, (struct sockaddr *) &clientStatic, clientlen) < 0)
				cprintf("sendto error file\n");
			
			while (1) {
				if ((received = recvfrom(serversock, s, BUFLEN - 1, 0, (struct sockaddr *) &client, &clientlen)) < 0) {
					cprintf("failed to recvfrom\n");
					break;
				}
				if(s[0] != 'g') break;
				int off;
				memcpy(&off, s + 4, 4);
				
				if (file_start) {
					if (off >= 0 && off <= file_maxsize && off + received - 8 <= file_maxsize) {
						int tmp = off + received - 8;
						if (tmp > file_cursize) {
							file_cursize = tmp;
						}
						memcpy(file_start + off, s + 8, received - 8);
					}
				} else {
					seek(fd, off);
					write(fd, s + 8, received - 8);
				}
				
				s[0] = 'a';
				if(sendto(clientsock, s, 8, 0, (struct sockaddr *) &clientStatic, clientlen) < 0)
					cprintf("sendto error file part\n");
			}
			
			if (!file_start) {
				close(fd);
				sync();
			} else {
				*file_len = file_cursize;
				if (file_cursize % PGSIZE != 0) {
					memset(file_start + file_cursize, 0, PGSIZE - file_cursize % PGSIZE);
				}
			}
			
			strcpy(s, "ok"); received = 2;
		}
		else if(s[0] == 'c')
		{
			cprintf("command:%s:\n", s + 1);
			// command
			int fd = open("tmp", O_RDWR | O_CREAT | O_TRUNC);
			write(fd, s + 1, received - 1);
			write(fd, "\n", 1);
			close(fd);
			sync();
			int r = spawnl("sh", "sh", "-w", "tmp", 0);
			if(r < 0)
			{
				cprintf("failed to spawn %e\n", r);
			}
			wait(r);
			cprintf("wait ok\n");
			strcpy(s, "ok"); received = 2;
		}
		else if(s[0] == 'o')
		{
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
			received = 0;
			close(fd);
			sync();
		}
		else
		{
			strcpy(s, "inv"); received = 3;
		}
		if (received)
		if(sendto(clientsock, s, received, 0, (struct sockaddr *) &clientStatic, clientlen) < 0)
			cprintf("sendto error\n");
	}

	close(serversock);
}
