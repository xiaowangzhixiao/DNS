//
// Created by wangzhi on 17-5-10.
//
#include "DNS.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

static void updataHead(DNSHeader dnsHeader, char *buff)
{
	uint16_t temp;

	temp = htons(dnsHeader.id);
	memcpy(buff,&temp,sizeof(temp));

	memcpy(&temp,(void *)&dnsHeader + 2,sizeof(temp));
	temp = htons(temp);
	memcpy(buff+2,&temp,sizeof(temp));

	temp = htons(dnsHeader.QDCOUNT);
	memcpy(buff+4,&temp,sizeof(temp));
	temp = htons(dnsHeader.ANCOUNT);
	memcpy(buff+6,&temp,sizeof(temp));
	temp = htons(dnsHeader.NSCOUNT);
	memcpy(buff+8,&temp,sizeof(temp));
	temp = htons(dnsHeader.ARCOUNT);
	memcpy(buff+10,&temp,sizeof(temp));
}

DNS DNS_getHead(DNS dns)
{
	uint16_t temp;
	dns.dnsHeader.id = ntohs(*(uint16_t *)dns.buff);
	temp = ntohs(*(uint16_t *)(dns.buff+2));
	memcpy(((void *)&(dns.dnsHeader))+2,&temp,sizeof(temp));
	dns.dnsHeader.QDCOUNT = ntohs(*(uint16_t *)(dns.buff + 4));
	dns.dnsHeader.ANCOUNT = ntohs(*(uint16_t *)(dns.buff + 6));
	dns.dnsHeader.NSCOUNT = ntohs(*(uint16_t *)(dns.buff + 8));
	dns.dnsHeader.ARCOUNT = ntohs(*(uint16_t *)(dns.buff + 10));
	return dns;
}

DNS DNS_getHost(DNS dns)
{
	assert(dns.dnsHeader.QDCOUNT>0);
	int i;
	char *host = dns.buff + 12;

	//i指向的是问题域的最后一个字符0
	//问题转化为.分割的长度为i-1，因为开头的数字多占一位
	for (i = 0; host[i] != 0;)
	{
		i = i + host[i] + 1;
	}

	if (dns.host != NULL)
	{
	    free(dns.host);
		dns.host = NULL;
	}

	dns.host = malloc((size_t) i);//给'/0'留个位置

	for (int j = 0; j < i; )
	{
		for (int k = 0; k < host[j]; ++k)
		{
			dns.host[j+k] = host[j+k+1];
		}
		j=j+host[j];
		dns.host[j] = '.';
		j++;
	}
	dns.host[i-1] = '\0';

	//12+i指向0，下一位是QTYPE的开始
	dns.QTYPE = ntohs(*(uint16_t *)(dns.buff + 12 + i + 1));
	dns.QCLASS = ntohs(*(uint16_t *)(dns.buff + 12 + i + 1 + 2));

	dns.questionLength = i + 1 + 4;

	dns.answerOffset = 12 + i + 1 + 4;//回答域的开始，在回答包中有用

	return dns;
}

DNS DNS_addAnswer(DNS dns, uint32_t ip)
{
	uint16_t uint16;
	uint32_t uint32;
	dns.dnsHeader.ARCOUNT = 1;
	updataHead(dns.dnsHeader,dns.buff);
	dns.ip = ip;
	memcpy(dns.buff+dns.answerOffset,dns.buff+12, (size_t) dns.questionLength);//资源中的域名和类型和类
	uint32 = htonl(172800);//两天的生存期
	memcpy(dns.buff+dns.answerOffset+dns.questionLength,&uint32,sizeof(uint32));
	uint16 = htons(4);
	memcpy(dns.buff+dns.answerOffset+dns.questionLength+4,&uint16,sizeof(uint16));
	uint32 = htonl(ip);
	memcpy(dns.buff+dns.answerOffset+dns.questionLength+6,&uint32,sizeof(uint32));

	dns.size_n = (size_t) (12 + dns.questionLength + dns.questionLength + 6 + 4);

	return dns;
}

DNS DNS_changeId(DNS dns, uint16_t id)
{
	dns.dnsHeader.id = id;
	updataHead(dns.dnsHeader,dns.buff);
	return dns;
}

DNS DNS_errorAnswer(DNS dns)
{
	dns.dnsHeader.RCODE = 3;
	dns.dnsHeader.AA = 1;
	updataHead(dns.dnsHeader,dns.buff);
	return dns;
}

void DNS_clear(DNS *dns)
{
	if (dns->host!=NULL)
	{
		free(dns->host);
	}
	memset(dns,0,sizeof(DNS));
}


