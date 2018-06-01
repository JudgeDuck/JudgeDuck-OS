#include <inc/lib.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>

static void
handle_client(int sock)
{
	cprintf("handle sock %d\n", sock);
	int r;
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
		s[received] = 0;
		if(s[0] == 'f')
		{
			// write file
			int fd = open(s + 1, O_RDWR | O_CREAT | O_TRUNC);
			for(;;)
			{
				for(int i = 0; i < 50; i++) sys_yield();
				if(write(sock, "file", 4) != 4)
				{
					cprintf("failed to write\n");
					break;
				}
				if((received = read(sock, s, sizeof(s) - 1)) < 0)
				{
					cprintf("failed to read\n");
					break;
				}
				if(s[0] == 'e') break;
				write(fd, s + 1, received - 1);
			}
			if(write(sock, "ok", 2) != 2)
			{
				cprintf("failed to write\n");
				break;
			}
			close(fd);
			sync();
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
			r = spawnl("sh", "sh", "-w", "tmp", 0);
			if(r < 0)
			{
				cprintf("failed to spawn %e\n", r);
			}
			wait(r);
			cprintf("wait ok\n");
			if(write(sock, "ok", 2) != 2)
			{
				cprintf("failed to write\n");
				break;
			}
		}
		else if(s[0] == 'o')
		{
			// open file
			int fd = open(s + 1, O_RDONLY);
			if((received = read(fd, s, sizeof(s) - 1)) < 0)
			{
				cprintf("failed to read local\n");
				break;
			}
			close(fd);
			sync();
			if(write(sock, s, received) != received)
			{
				cprintf("failed to write\n");
				break;
			}
			break;
		}
		else
		{
			if(write(sock, "inv", 3) != 3)
			{
				cprintf("failed to write\n");
				break;
			}
		}
	}
	close(sock);
	// exit();
}

void
umain(int argc, char **argv)
{
	int serversock, clientsock;
	struct sockaddr_in server, client;

	// Create the TCP socket
	if((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		cprintf("Failed to create socket");

	// Construct the server sockaddr_in structure
	memset(&server, 0, sizeof(server));		// Clear struct
	server.sin_family = AF_INET;			// Internet/IP
	server.sin_addr.s_addr = htonl(INADDR_ANY);	// IP address
	server.sin_port = htons(80);			// server port

	// Bind the server socket
	if(bind(serversock, (struct sockaddr *) &server, sizeof(server)) < 0)
		cprintf("Failed to bind the server socket");

	// Listen on the server socket
	if(listen(serversock, 5) < 0)
		cprintf("Failed to listen on server socket");

	cprintf("Waiting for connections...\n");

	while(1)
	{
		unsigned int clientlen = sizeof(client);
		// Wait for client connection
		if((clientsock = accept(serversock, (struct sockaddr *) &client, &clientlen)) < 0)
			cprintf("Failed to accept client connection");
		handle_client(clientsock);
	}

	close(serversock);
}
