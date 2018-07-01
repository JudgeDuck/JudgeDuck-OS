#include <inc/lib.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <lwip/err.h>

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
	clientStatic.sin_addr.s_addr = inet_addr("10.0.2.2");	// IP address
	clientStatic.sin_port = htons(8008);			// server port

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

	while(1)
	{
		unsigned int clientlen = sizeof(client);
		char s[512];
		int received;
		errno = 233;
		clientlen = 233;
		if((received = recvfrom(serversock, s, sizeof(s) - 1, 0, (struct sockaddr *) &client, &clientlen)) < 0)
		{
			cprintf("recvfrom failed %d\n", errno);
			exit();
		}
		s[received] = 0;
		if(s[0] == 'f')
		{
			// write file
			int fd = open(s + 1, O_RDWR | O_CREAT | O_TRUNC);
			if(sendto(clientsock, "file", 4, 0, (struct sockaddr *) &clientStatic, clientlen) < 0)
				cprintf("sendto error file\n");
			for(;;)
			{
				if((received = recvfrom(serversock, s, sizeof(s) - 1, 0, (struct sockaddr *) &client, &clientlen)) < 0)
				{
					cprintf("failed to recvfrom\n");
					break;
				}
				if(s[0] != 'g') break;
				int off;
				memcpy(&off, s + 4, 4);
				seek(fd, off);
				write(fd, s + 8, received - 8);
				s[0] = 'a';
				if(sendto(clientsock, s, 8, 0, (struct sockaddr *) &clientStatic, clientlen) < 0)
					cprintf("sendto error file part\n");
			}
			close(fd);
			sync();
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
			if((received = read(fd, s, sizeof(s) - 1)) < 0)
			{
				cprintf("failed to read local\n");
				// break;
			}
			close(fd);
			sync();
		}
		else
		{
			strcpy(s, "inv"); received = 3;
		}
		if(sendto(clientsock, s, received, 0, (struct sockaddr *) &clientStatic, clientlen) < 0)
			cprintf("sendto error\n");
	}

	close(serversock);
}
