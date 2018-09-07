#include <inc/lib.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <lwip/err.h>
#include <net/ns.h>

#define MAX_ELF_SIZE (20 * 1024 * 1024)

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

static const char *trapname(int trapno) {
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < ARRAY_SIZE(excnames))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return "Hardware Interrupt";
	return "(unknown trap)";
}

#define MAXARGS 16

static void run_arbiter(const char *judging_elf, const char *args) {
	static char argbuf[PGSIZE];
	strncpy(argbuf, args, PGSIZE - 1);
	argbuf[PGSIZE - 1] = 0;
	
	char *argv[MAXARGS + 1];
	int argc = 1;
	argv[0] = "judging";
	
	char *s = argbuf;
	while (1) {
		while (*s == ' ') ++s;
		char c = *s;
		if (c == 0) break;
		if (c == '>') break;
		argv[argc++] = s;
		while (*s != ' ' && *s != 0) ++s;
		*s = 0;
		++s;
		if (argc >= MAXARGS) break;
	}
	
	int fd = open("arbiter.out", O_RDWR | O_CREAT | O_TRUNC);
	if (fd < 0) {
		panic("arbiter(): open arbiter.out failed\n");
		exit();
	}
	
	envid_t env = spawn_contestant_from_memory(judging_elf, (const char **) argv);
	if (env < 0) {
		panic("arbiter(): spawn contestant failed\n");
		exit();
	}
	
	sys_yield();
	
	struct JudgeResult res;
	int ok = 0;
	
	for (int id = 0; id < 5; id++) {
		int ret = sys_accept_enter_judge(env, &res);
		if (ret == 0) {
			ok = 1;
			break;
		}
		for (int i = 0; i < 150 * 10; i++) sys_yield();
	}
	
	wait(env);
	// Bug: What if the program fall into an infinity loop ...
	
	if (!ok) {
		cprintf("arbiter(): failed since enter_judge() seems not called\n");
		fprintf(fd, "Judge Failed: enter_judge() seems not called\n");
	} else {
		static const char *verdict_str[] = {
			"Run Finished", "Time Limit Exceeded", "Runtime Error", "Illegal Syscall", "System Error", "Memory Limit Exceeded"
		};
		cprintf("ARBITER: verdict = %s\n", verdict_str[(int) res.verdict]);
		fprintf(fd, "verdict = %s\n", verdict_str[(int) res.verdict]);
		if (res.verdict == VERDICT_IS) {
			cprintf("ARBITER: syscall_id = %d\n", res.tf.tf_regs.reg_eax);
			fprintf(fd, "syscall_id = %d\n", res.tf.tf_regs.reg_eax);
		}
		if (res.verdict == VERDICT_RE) {
			cprintf("ARBITER: runtime error %d (%s)\n", res.tf.tf_trapno, trapname(res.tf.tf_trapno));
			fprintf(fd, "runtime error %d (%s)\n", res.tf.tf_trapno, trapname(res.tf.tf_trapno));
		}
		cprintf("ARBITER: time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
		cprintf("ARBITER: time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
		cprintf("ARBITER: mem_kb = %d\n", res.mem_kb);
		fprintf(fd, "time_Mcycles = %d.%06d\n", (int) (res.time_cycles / 1000000), (int) (res.time_cycles % 1000000));
		fprintf(fd, "time_ms = %d.%06d\n", (int) (res.time_ns / 1000000), (int) (res.time_ns % 1000000));
		fprintf(fd, "mem_kb = %d\n", res.mem_kb);
		
		int fd_in = open("arbiter.in", O_RDWR);
		static char buf[256];
		int r = read(fd_in, buf, sizeof(buf));
		if (r > 0) write(fd, buf, r);
		close(fd_in);
	}
	
	close(fd);
	
	cprintf("arbiter(): done\n");
}


typedef struct {
	int size;
	int n_pages;
	int start;
	char md5[50];
	int lru_tag;
	bool to_remove;
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
	
	char *judging_elf = judge_pages + PGSIZE * 2;
	
	void *meta_start = judging_elf + MAX_ELF_SIZE;
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
			
			// arbiter judging ......
			const char *tmp = "arbiter judging ";
			int tmp_len = strlen(tmp);
			char tmp_back = s[1 + tmp_len];
			s[1 + tmp_len] = 0;
			int res = strcmp(s + 1, tmp);
			s[1 + tmp_len] = tmp_back;
			
			if (res == 0) {
				run_arbiter(judging_elf, s + 1 + tmp_len);
				cprintf("run arbiter ok\n");
				strcpy(s, "run arbiter ok");
			} else {
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
			}
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
		} else if (s[0] == 'f' && strcmp(s + 1, "judging") == 0) {
			cprintf("[j][recv judging elf!!!]\n");
			
			// Clear elf header
			memset(judging_elf, 0, 512);
			
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
				
				memcpy(judging_elf + off, s + 8, received - 8);
				
				s[0] = 'a';
				if (sendto(clientsock, s, 8, 0, (struct sockaddr *) &clientStatic, clientlen) < 0) {
					cprintf("sendto error file part\n");
				}
			}
			
			strcpy(s, "sendFile elf ok");
			to_send = strlen(s);
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
				
				int last_pos = file_start - (void *) judge_pages;
				if (n_files > 0) {
					last_pos = filedata[n_files - 1].start + filedata[n_files - 1].n_pages * PGSIZE;
				}
				if (last_pos + size > JUDGE_PAGES_SIZE) {
					// It's full
					int to_remove_size = last_pos + size - JUDGE_PAGES_SIZE;
					while (to_remove_size > 0) {
						int max_tag = 30;
						int max_tag_id = -1;
						int max_size = -1;
						int max_size_id = -1;
						for (int i = 0; i < n_files; i++) {
							FileData *fdata = filedata + i;
							if (fdata->to_remove) continue;
							if (fdata->lru_tag > max_tag) {
								max_tag = fdata->lru_tag;
								max_tag_id = i;
							}
							if (fdata->n_pages > max_size) {
								max_size = fdata->n_pages;
								max_size_id = i;
							}
						}
						int id = max_tag_id != -1 ? max_tag_id : max_size_id;
						filedata[id].to_remove = true;
						to_remove_size -= filedata[id].n_pages * PGSIZE;
					}
					// Defrag
					int j = 0;
					int pos = file_start - (void *) judge_pages;
					for (int i = 0; i < n_files; i++) {
						if (filedata[i].to_remove) continue;
						if (j < i) {
							filedata[j] = filedata[i];
							memmove(judge_pages + pos, judge_pages + filedata[j].start, filedata[j].n_pages * PGSIZE);
						}
						filedata[j].start = pos;
						pos += filedata[j].n_pages * PGSIZE;
						j++;
					}
					n_files = j;
				}
				last_pos = file_start - (void *) judge_pages;
				if (n_files > 0) {
					last_pos = filedata[n_files - 1].start + filedata[n_files - 1].n_pages * PGSIZE;
				}
				FileData *fdata = filedata + n_files;
				fdata->size = size;
				strcpy(fdata->md5, md5);
				fdata->n_pages = ROUNDUP(fdata->size, PGSIZE) / PGSIZE;
				fdata->start = last_pos;
				fdata->lru_tag = 0;
				fdata->to_remove = false;
				
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
				for (int i = 0; i < n_files; i++) {
					filedata[i].lru_tag++;
				}
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
