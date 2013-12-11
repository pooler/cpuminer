/*
 * Driver for GC3355 chip to mine Litecoin, power by GridChip & GridSeed
 *
 * Copyright 2013 faster <develop@gridseed.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */

#ifndef WIN32
#include <termios.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#else
#include <windows.h>
#include <io.h>
typedef unsigned int speed_t;
#define  B115200  0010002
#endif
#include <ctype.h>

static const char *gc3355_version = "v0.8.4.20131211";

/* local vars */
static int	gc3355_fd;
static const char **cmd_init = NULL;
static const char **cmd_reset = NULL;

/* commands to set core frequency */
static const char *str_frequency[] = {
	"250",
	"400",
	"450",
	"500",
	"550",
	"600",
	"650",
	"700",
	"750",
	"800",
	"850",
	"900",
	NULL
};
static const char *cmd_frequency[] = {
	"55AAEF0005002001",
	"55AAEF000500E001",
	"55AAEF0005002002",
	"55AAEF0005006082",
	"55AAEF000500A082",
	"55AAEF000500E082",
	"55AAEF0005002083",
	"55AAEF0005006083",
	"55AAEF000500A083",
	"55AAEF000500E083",
	"55AAEF0005002084",
	"55AAEF0005006084",
	NULL
};

/* commands for single LTC mode */
static const char *single_cmd_init[] = {
	"55AAEF020000000000000000000000000000000000000000",
	"55AAEF3040000000",
	"55AA1F2810000000",
	"55AA1F2813000000",
	NULL
};
static const char *single_cmd_reset[] = {
	"55AA1F2810000000",
	"55AA1F2813000000",
	NULL
};
/* commands for dual BTC&LTC mode */
static const char *dual_cmd_init[] = {
	"55AA1F2814000000",
	"55AA1F2817000000",
	NULL
};
static const char *dual_cmd_reset[] = {
	"55AA1F2814000000",
	"55AA1F2817000000",
	NULL
};

/* external functions */
extern void scrypt_1024_1_1_256(const uint32_t *input, uint32_t *output,
    uint32_t *midstate, unsigned char *scratchpad);

/* local functions */
static int gc3355_scanhash(int thr_id, uint32_t *pdata, unsigned char *scratchbuf,
		const uint32_t *ptarget, bool keep_work);

/* print in hex format, for debuging */
static void print_hex(unsigned char *data, int len)
{
    int             i, j, s, blank;
    unsigned char   *p=data;

    for(i=s=0; i<len; i++,p++) {
        if ((i%16)==0) {
            s = i;
            printf("%04x :", i);
        }
        printf(" %02x", *p);
        if (((i%16)==7) && (i!=(len-1)))
            printf(" -");
        else if ((i%16)==15) {
            printf("    ");
            for(j=s; j<=i; j++) {
                if (isprint(data[j]))
                    printf("%c", data[j]);
                else
                    printf(".");
            }
            printf("\n");
        }
    }
    if ((i%16)!=0) {
        blank = ((16-i%16)*3+4) + (((i%16)<=8) ? 2 : 0);
        for(j=0; j<blank; j++)
            printf(" ");
        for(j=s; j<i; j++) {
            if (isprint(data[j]))
                printf("%c", data[j]);
            else
                printf(".");
        }
        printf("\n");
    }
}

/* open UART device */
static int gc3355_open(const char *devname, speed_t baud)
{
#ifdef WIN32
	DWORD	timeout = 5;

	HANDLE hSerial = CreateFile(devname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (unlikely(hSerial == INVALID_HANDLE_VALUE))
	{
		DWORD e = GetLastError();
		switch (e) {
		case ERROR_ACCESS_DENIED:
			applog(LOG_ERR, "Do not have user privileges required to open %s", devname);
			break;
		case ERROR_SHARING_VIOLATION:
			applog(LOG_ERR, "%s is already in use by another process", devname);
			break;
		default:
			applog(LOG_DEBUG, "Open %s failed, GetLastError:%u", devname, e);
			break;
		}
		return -1;
	}

	// thanks to af_newbie for pointers about this
	COMMCONFIG comCfg = {0};
	comCfg.dwSize = sizeof(COMMCONFIG);
	comCfg.wVersion = 1;
	comCfg.dcb.DCBlength = sizeof(DCB);
	comCfg.dcb.BaudRate = baud;
	comCfg.dcb.fBinary = 1;
	comCfg.dcb.fDtrControl = DTR_CONTROL_ENABLE;
	comCfg.dcb.fRtsControl = RTS_CONTROL_ENABLE;
	comCfg.dcb.ByteSize = 8;

	SetCommConfig(hSerial, &comCfg, sizeof(comCfg));

	// Code must specify a valid timeout value (0 means don't timeout)
	const DWORD ctoms = (timeout * 100);
	COMMTIMEOUTS cto = {ctoms, 0, ctoms, 0, ctoms};
	SetCommTimeouts(hSerial, &cto);

	PurgeComm(hSerial, PURGE_RXABORT);
	PurgeComm(hSerial, PURGE_TXABORT);
	PurgeComm(hSerial, PURGE_RXCLEAR);
	PurgeComm(hSerial, PURGE_TXCLEAR);

	gc3355_fd = _open_osfhandle((intptr_t)hSerial, 0);
	return 0;

#else

	struct termios	my_termios;

    gc3355_fd = open(devname, O_RDWR | O_CLOEXEC | O_NOCTTY);
	if (gc3355_fd== -1) {
		if (errno == EACCES)
			applog(LOG_ERR, "Do not have user privileges to open %s\n", devname);
		else
			applog(LOG_ERR, "failed open device %s\n", devname);
		return 1;
	}

	tcgetattr(gc3355_fd, &my_termios);
	cfsetispeed(&my_termios, baud);
	cfsetospeed(&my_termios, baud);
	cfsetspeed(&my_termios,  baud);

	my_termios.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
	my_termios.c_cflag |= CS8;
	my_termios.c_cflag |= CREAD;
	my_termios.c_cflag |= CLOCAL;

	my_termios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK |
			ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	my_termios.c_oflag &= ~OPOST;
	my_termios.c_lflag &= ~(ECHO | ECHOE | ECHONL | ICANON | ISIG | IEXTEN);

	// Code must specify a valid timeout value (0 means don't timeout)
	my_termios.c_cc[VTIME] = (cc_t)10;
	my_termios.c_cc[VMIN] = 0;

	tcsetattr(gc3355_fd, TCSANOW, &my_termios);
	tcflush(gc3355_fd, TCIOFLUSH);

	return 0;

#endif
}

/* close UART device */
static void gc3355_close(void)
{
	if (gc3355_fd > 0)
		close(gc3355_fd);
	return;
}

/* send data to UART */
static int gc3355_write(const void *buf, size_t buflen)
{
	size_t ret;
	int i;
	unsigned char *p;

	if (!opt_quiet) {
		//printf("^[[1;33mSend to UART: %u^[[0m\n", bufLen);
		//print_hex((unsigned char *)buf, bufLen);
		p = (unsigned char *)buf;
		printf("[1;33m>>> LTC :[0m ");
		for(i=0; i<buflen; i++)
			printf("%02x", *(p++));
		printf("\n");
	}

	ret = write(gc3355_fd, buf, buflen);
	if (ret != buflen)
		return 1;

	return 0;
}

/* read data from UART */
static int gc3355_gets(unsigned char *buf, int read_count)
{
	unsigned char	*bufhead, *p;
	fd_set			rdfs;
	struct timeval	tv;
	ssize_t			nread;
	int				read_amount;
	int				n, i;

	bufhead = buf;
	read_amount = read_count;
	memset(buf, 0, read_count);
	tv.tv_sec  = 1;
	tv.tv_usec = 0;
	while(true) {
		FD_ZERO(&rdfs);
		FD_SET(gc3355_fd, &rdfs);
		n = select(gc3355_fd+1, &rdfs, NULL, NULL, &tv);
		if (n < 0) {
			// error occur
			return 1;
		} else if (n == 0) {
			// timeout
			//applog(LOG_DEBUG, "Read timeout");
			return 2;
		}

		usleep(10000);
		nread = read(gc3355_fd, buf, read_amount);
		if (nread < 0)
			return 1;
		else if (nread >= read_amount) {
			if (!opt_quiet) {
				p = (unsigned char *)bufhead;
				printf("[1;31m<<< LTC :[0m ");
				for(i=0; i<read_count; i++)
					printf("%02x", *(p++));
				printf("\n");
			}
			return 0;
		}
		else if (nread > 0) {
			buf += nread;
			read_amount -= nread;
		}
	}
}

static void gc3355_send_cmds(const char *cmds[])
{
	unsigned char	ob[160];
	int				i;

	for(i=0; ; i++) {
		if (cmds[i] == NULL)
			break;
		memset(ob, 0, sizeof(ob));
		hex2bin(ob, cmds[i], sizeof(ob));
		gc3355_write(ob, strlen(cmds[i])/2);
		usleep(10000);
	}
	return;
}

static void gc3355_set_core_freq(const char *freq)
{
	int		i, inx=5;
	char	*p;
	char	*cmds[2];

	if (freq != NULL) {
		for(i=0; ;i++) {
			if (str_frequency[i] == NULL)
				break;
			if (strcmp(freq, str_frequency[i]) == 0)
				inx = i;
		}
	}

	cmds[0] = (char *)cmd_frequency[inx];
	cmds[1] = NULL;
	gc3355_send_cmds((const char **)cmds);
	applog(LOG_INFO, "set GC3355 core frequency to %sMhz", str_frequency[inx]);
	return;
}


/*
 * miner thread
 */
static void *gc3355_thread(void *userdata)
{
	struct thr_info	*mythr = userdata;
	int thr_id = mythr->id;
	struct work work;
	unsigned char *scratchbuf = NULL;
	char s[16];
	int i;

	if (opt_dual) {
		cmd_init = dual_cmd_init;
		cmd_reset = dual_cmd_reset;
	} else {
		cmd_init = single_cmd_init;
		cmd_reset = single_cmd_reset;
	}

	scratchbuf = scrypt_buffer_alloc();

	applog(LOG_INFO, "GC3355 chip mining thread started, in %s mode", opt_dual ? "DUAL" : "SINGLE");

	if (gc3355_open(gc3355_dev, B115200))
		return NULL;
	applog(LOG_INFO, "open UART device %s", gc3355_dev);

	gc3355_send_cmds(cmd_init);
	if (!opt_dual)
		gc3355_set_core_freq(opt_frequency);

	while(1) {
		bool	keep_work;
		int		rc;

		if (have_stratum) {
			while (!*g_work.job_id || time(NULL) >= g_work_time + 120)
				sleep(1);
			pthread_mutex_lock(&g_work_lock);
		} else {
			/* obtain new work from internal workio thread */
			pthread_mutex_lock(&g_work_lock);
			if (!(have_longpoll || have_stratum) ||
					time(NULL) >= g_work_time + LP_SCANTIME*3/4) {
				if (unlikely(!get_work(mythr, &g_work))) {
					applog(LOG_ERR, "work retrieval failed, exiting "
						"mining thread %d", mythr->id);
					pthread_mutex_unlock(&g_work_lock);
					goto out_gc3355;
				}
				time(&g_work_time);
			}
			if (have_stratum) {
				pthread_mutex_unlock(&g_work_lock);
				continue;
			}
		}

		if (memcmp(work.data, g_work.data, 76)) {
			memcpy(&work, &g_work, sizeof(struct work));
			keep_work = false;
		} else
			keep_work = true;
		pthread_mutex_unlock(&g_work_lock);
		work_restart[thr_id].restart = 0;

		rc = gc3355_scanhash(thr_id, work.data, scratchbuf, work.target, keep_work);
		if (rc && !submit_work(mythr, &work))
			break;
	}

out_gc3355:
	tq_freeze(mythr->q);
	gc3355_close();
	return NULL;
}

/* scan hash in GC3355 chips */
static int gc3355_scanhash(int thr_id, uint32_t *pdata, unsigned char *scratchbuf,
		const uint32_t *ptarget, bool keep_work)
{
	static char	*indi = "|/-\\";
	static int	i_indi = 0;

	unsigned char bin[160];
	uint32_t data[20], nonce, hash[8];
	uint32_t midstate[8];
	uint32_t n = pdata[19] - 1;
	const uint32_t Htarg = ptarget[7];
	int ret, i;

	memcpy(data, pdata, 80);
	sha256_init(midstate);
	sha256_transform(midstate, data, 0);
#if 0
	printf("[1;33mTarget[0m\n");
	print_hex((unsigned char *)ptarget, 32);
	printf("[1;33mData[0m\n");
	print_hex((unsigned char*)pdata, 80);
	printf("[1;33mMidstate[0m\n");
	print_hex((unsigned char *)midstate, 32);
#endif
	if (!keep_work) {
		applog(LOG_INFO, "dispatching new work to GC3355 LTC core");
		gc3355_send_cmds(cmd_reset);
		memset(bin, 0, sizeof(bin));
		memcpy(bin, "\x55\xaa\x1f\x00", 4);
		memcpy(bin+4, (unsigned char *)ptarget, 32);
		memcpy(bin+36, (unsigned char *)midstate, 32);
		memcpy(bin+68, (unsigned char*)pdata, 80);
		memcpy(bin+148, "\xff\xff\xff\xff", 4);
		gc3355_write(bin, 152);
		usleep(1000);
	} else {/* keepwork */
		printf(" %c%c%c%c", indi[i_indi], indi[i_indi], indi[i_indi], 13);
		i_indi++;
		if (i_indi >= strlen(indi))
			i_indi = 0;
		fflush(stdout);
	}

	while(!work_restart[thr_id].restart) {
		ret = gc3355_gets((unsigned char *)&nonce,  4/*bytes*/);
		if (ret == 0) {
			unsigned char s[80], *ph;
			memcpy(pdata+19, &nonce, sizeof(nonce));
			data[19] = nonce;
			scrypt_1024_1_1_256(data, hash, midstate, scratchbuf);

			if (!opt_quiet) {
				ph = (unsigned char *)ptarget;
				for(i=0; i<32; i++)
					sprintf(s+i*2, "%02x", *(ph++));
				applog(LOG_DEBUG, "Target: [1;34m%s[0m", s);

				ph = (unsigned char *)hash;
				for(i=0; i<32; i++)
					sprintf(s+i*2, "%02x", *(ph++));
				applog(LOG_DEBUG, "  Hash: [1;34m%s[0m", s);
			}

			ph = (unsigned char *)&nonce;
			for(i=0; i<4; i++)
				sprintf(bin+i*2, "%02x", *(ph++));

			if (hash[7] <= Htarg && fulltest(hash, ptarget)) {
				applog(LOG_INFO, "Got nonce %s, [1;32mHash <= Htarget![0m", bin);
				return 1;
			} else {
				applog(LOG_INFO, "Got nonce %s, [1;31mInvalid nonce![0m", bin);
				break;
			}
		} else
			break;
	}
	return 0;
}
