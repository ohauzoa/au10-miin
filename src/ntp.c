#include "cli.h"
#include "ntp.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


typedef struct
{
	uint8_t li_vn_mode; // 8 bits. li, vn, and mode.
	uint8_t stratum; // 8 bits. Stratum level of the local clock.
	uint8_t poll; // 8 bits. Maximum interval between successive messages.

	uint8_t precision; // 8 bits. Precision of the local clock.
	uint32_t rootDelay; // 32 bits. Total round trip delay time.
	uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
	uint32_t refId; // 32 bits. Reference clock identifier.
	uint32_t refTm_s; // 32 bits. Reference time-stamp seconds.
	uint32_t refTm_f; // 32 bits. Reference time-stamp fraction of a second.
	uint32_t origTm_s; // 32 bits. Originate time-stamp seconds.
	uint32_t origTm_f; // 32 bits. Originate time-stamp fraction of a second.
	uint32_t rxTm_s; // 32 bits. Received time-stamp seconds.
	uint32_t rxTm_f; // 32 bits. Received time-stamp fraction of a second.
	uint32_t txTm_s; // 32 bits and the most important field the client cares about. Tran
	uint32_t txTm_f; // 32 bits. Transmit time-stamp fraction of a second.
}ntp_packet;


void delay(long ms)
{
	struct timeval stTimeVal;
	stTimeVal.tv_sec = ms / 1000;
	ms -= (stTimeVal.tv_sec * 1000);
	stTimeVal.tv_usec = ms * 1000;

	select(0, 0, 0, 0, &stTimeVal);
}


time_t query_time(char *addr)
{
	int retry_cnt = 5;
	time_t  nettime = 0;
	ntp_packet  packet = { 0, };
	int  sockfd = 0;
	struct sockaddr_in addr_in;
	struct hostent *host_entry;

	host_entry = gethostbyname(addr);
	if (!host_entry)
	{
		printf("gethostbyname fail\n");
		return 0;
	}

	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd <= 0)
	{
		printf("socket fail\n");
		return 0;
	}

	memset((void *)&addr_in, 0x00, sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(123);
	memcpy((void *)(&addr_in.sin_addr), (void *)(host_entry->h_addr), sizeof(addr_in.sin_addr));

	if (connect(sockfd, (struct sockaddr *)&addr_in, sizeof(addr_in)) == -1)
	{
		printf("connect fail\n");
		close(sockfd);
		return 0;
	}

	memset(&packet, 0, sizeof(ntp_packet));
	packet.li_vn_mode = 0x1b;

	delay(100);

	if (write(sockfd, (char*)&packet, sizeof(ntp_packet)) <= 0)
	{
		printf("write fail\n");
		close(sockfd);
		return 0;
	}

	do
	{
		delay(100);
		if (read(sockfd, (char*)&packet, sizeof(ntp_packet)) > 0)
		{
			unsigned long long NTP_TIMESTAMP_DELTA = 2208988800ull;
			packet.txTm_s = ntohl(packet.txTm_s); // Time-stamp seconds.

			packet.txTm_f = ntohl(packet.txTm_f); // Time-stamp fraction of a second

			nettime = (time_t)(packet.txTm_s - NTP_TIMESTAMP_DELTA);
			printf("[%s] Time: %s", addr, ctime((const time_t*)&nettime));
			break;
		}

		retry_cnt--;

	} while (retry_cnt>0);

	close(sockfd);

	return nettime;

}


void _ntp(char *argv[]) 
{
	time_t m_time;
	struct tm *tm_ptr;
	int offset = 0;
    char buf[32] = {"time.windows.com"};

	if(argv[1]){
		memset(buf, 0, 32);
		snprintf(buf, sizeof(buf), "%s",  argv[1]);
	}
//	if(argv[2] > 0){
//	    offset = atoi(argv[2]);
//	}
//	printf("[%s] offset: %s", buf, offset);
	
//query_time("time.bora.net");
//query_time("time.windows.com");
//query_time("time.nist.gov");
	m_time = query_time(buf);

	tm_ptr = gmtime(&m_time);
	tm_ptr->tm_hour += offset;
	m_time = mktime(tm_ptr);
	stime(&m_time);
}
