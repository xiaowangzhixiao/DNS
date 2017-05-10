//
// Created by wangzhi on 17-5-10.
//
#include "DNS.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>


DNS DNS_getHead(DNS dns)
{
	uint16_t temp;
	dns.dnsHeader.id = ntohs(*(uint16_t *)dns.buff);
	temp = ntohs(*(uint16_t *)((void *)dns.buff+2));
	memcpy(((void *)&(dns.dnsHeader))+2,&temp,sizeof(temp));
	dns.dnsHeader.QDCOUNT = ntohs(*(uint16_t *)((void *)dns.buff + 4));
	dns.dnsHeader.ANCOUNT = ntohs(*(uint16_t *)((void *)dns.buff + 6));
	dns.dnsHeader.NSCOUNT = ntohs(*(uint16_t *)((void *)dns.buff + 8));
	dns.dnsHeader.ARCOUNT = ntohs(*(uint16_t *)((void *)dns.buff + 10));
	return dns;
}

DNS DNS_getHost(DNS dns)
{
	assert(dns.dnsHeader.QDCOUNT>0);
	int i;
	char *host = (void *)dns.buff + 12;
	for (i = 0; host[i] != 0;)
	{
		i = i + host[i] + 1;
	}
	i++;
	if (dns.host != NULL)
	{
	    free(dns.host);
		dns.host = NULL;
	}

	dns.host = malloc((size_t) i);


	return dns;
}


