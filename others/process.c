/*
 * process.c, a dummy process used in COMP30023 2023 Project 1
 * Authored by Steven Tang
 * Reference: signalfd, waitpid, getopt_long
 * Source: https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
 * Last updated 21/03/23
 */

//#define _POSIX_C_SOURCE 1

#include <err.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "sha256.h"

static long pid = 0;
static int verbose_flag = 0;
typedef enum { STOP = 1, CONTINUE = 2, TERM = 3, START = 0 } Op;

void read_store_dword(Op op, uint8_t hash_content[128], size_t* dest_index);
void store_process_name(const char* process_name, uint8_t hash_content[128], size_t* dest_index);

int main(int argc, char* argv[]) {
	int c;
	char* process_name;
	static struct option long_options[] = {
		{"verbose", no_argument, &verbose_flag, 1  },
		{"help"   , no_argument, 0            , 'h'},
		{0        , 0          , 0            , 0  }};
	int option_index;

	int sfd;
	ssize_t s;
	sigset_t mask;
	struct signalfd_siginfo fdsi;

	size_t dest_index;
	uint8_t sha_content[128];
	char hash[65];

	while (1) {
		option_index = 0;
		c = getopt_long(argc, argv, "hv", long_options, &option_index);
        printf("c = %d\n", c);
		if (c == -1) {
            printf("Fuck\n");
			break;
		}

		switch (c) {
            case 0:
                printf("%s\n", argv[optind-1]);
                break;
            case 'v':
                verbose_flag = 1;
                printf("Case v\n");
                break;
            case 'h':
                printf("Usage: [-v|--verbose] <process-name>\n");
                // exit(EXIT_SUCCESS);
                break;
            default: break;
        }
	}

	pid = getpid();
	if (verbose_flag) {
		fprintf(stderr, "[process.c (%ld)] ppid: %ld\n", pid, (long)getppid());
	}
	if (optind + 1 != argc) {
		fprintf(stderr,
				"[process.c] Error: Less or more arguments than expected\n");
		exit(EXIT_FAILURE);
	}
	process_name = argv[optind++];

	memset(sha_content, 0, 128);
	dest_index = 0;
	store_process_name(process_name, sha_content, &dest_index);

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGTSTP);
	sigaddset(&mask, SIGCONT);
	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
		err(EXIT_FAILURE, "sigprocmask");
	}
	sfd = signalfd(-1, &mask, 0);
	if (sfd == -1) {
		err(EXIT_FAILURE, "signalfd");
	}

	/* Synchronisation at the start */
	/* Must be placed after signal setup to prevent race */
	read_store_dword(START, sha_content, &dest_index);

	for (;;) {
		s = read(sfd, &fdsi, sizeof(fdsi));
		if (s != sizeof(fdsi)) {
			err(EXIT_FAILURE, "read");
		}

		if (fdsi.ssi_signo == SIGINT || fdsi.ssi_signo == SIGTERM) {
			sigemptyset(&mask);
			sigaddset(&mask, SIGINT);
			sigaddset(&mask, SIGTERM);
			if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
				err(EXIT_FAILURE, "sigprocmask");
			}

			if (verbose_flag) {
				fprintf(stderr,
						"[process.c (%ld)] handling SIGINT or SIGTERM\n", pid);
			}
			read_store_dword(TERM, sha_content, &dest_index);

			sha256_hash(hash, sha_content, 128 - 9);
			printf("%s\n", hash);

			exit(EXIT_SUCCESS);
		} else if (fdsi.ssi_signo == SIGTSTP) {
			sigemptyset(&mask);
			sigaddset(&mask, SIGTSTP);
			if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
				err(EXIT_FAILURE, "sigprocmask");
			}

			if (verbose_flag) {
				fprintf(stderr, "[process.c (%ld)] handling SIGTSTP\n", pid);
			}
			read_store_dword(STOP, sha_content, &dest_index);
			raise(SIGSTOP);
		} else if (fdsi.ssi_signo == SIGCONT) {
			if (verbose_flag) {
				fprintf(stderr, "[process.c (%ld)] handling SIGCONT\n", pid);
			}
			/* Must be placed before read to prevent SIGCONT race */
			sigemptyset(&mask);
			sigaddset(&mask, SIGTSTP);
			if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
				err(EXIT_FAILURE, "sigprocmask");
			}
			read_store_dword(CONTINUE, sha_content, &dest_index);
		} else {
			fprintf(stderr, "Read unexpected signal\n");
		}
	}
	return 0;
}

void store(const uint8_t* buf, size_t len, uint8_t* dest, size_t* dest_index) {
	size_t i;
	for (i = 0; i < len; i++) {
		*dest_index = (*dest_index + i) % (128 - 9);
		dest[*dest_index] ^= buf[i];
	}
}

void store_process_name(const char* process_name, uint8_t hash_content[128],
						size_t* dest_index) {
	if (verbose_flag) {
		fprintf(stderr, "[process.c (%ld)] started with process name %s\n",
				pid, process_name);
	}
	store((uint8_t*)process_name, strlen(process_name), hash_content,
		  dest_index);
}

void read_store_dword(Op op, uint8_t hash_content[128], size_t* dest_index) {
	uint8_t buf[5];
	size_t len, n;

	buf[0] = op;
	len = 1;
	if (verbose_flag) {
		fprintf(
			stderr,
			"[process.c (%ld)] reading 4 (Big Endian) bytes from stdin...\n",
			pid);
		fflush(stderr);
	}
	while (len < 5) {
		n = read(STDIN_FILENO, buf + len, 5 - len);
		if (n < 0)
			err(EXIT_FAILURE, "read");
		len += n;
	}

	if (verbose_flag) {
		fprintf(stderr,
				"[process.c (%ld)] op %d, time %d, hex bytes [%02x, %02x, "
				"%02x, %02x]\n",
				pid, op,
				((uint32_t)buf[1]) << 24 | ((uint32_t)buf[2]) << 16 |
					((uint32_t)buf[3]) << 8 | (uint32_t)buf[4],
				buf[1], buf[2], buf[3], buf[4]);
	}
	if (ioctl(STDIN_FILENO, FIONREAD, &n) == 0 && n > 0) {
		fprintf(stderr, "[process.c (%ld)] Error: Leftover bytes in stdin\n",
				pid);
		exit(EXIT_FAILURE);
	}

	// Need to prevent race condition between SIGCONT and SIGTERM
	if (op == CONTINUE || op == START) {
		while (1) {
			n = write(STDOUT_FILENO, &buf[4], 1);
			if (n < 0) {
				err(EXIT_FAILURE, "write");
			}
			if (n == 1) {
				break;
			}
		}
		fsync(STDOUT_FILENO);
		fflush(stdout);
	}
	if (verbose_flag) {
		fprintf(stderr, "[process.c (%ld)] wrote hex byte [%02x] to stdout\n",
				pid, buf[4]);
		fflush(stderr);
	}
	store(buf, 5, hash_content, dest_index);
}
