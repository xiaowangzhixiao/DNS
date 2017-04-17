#ifndef DNS_H_
#define DNS_H_

/*

The header contains the following fields:

                                    1  1  1  1  1  1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

ID              A 16 bit identifier assigned by the program that
                generates any kind of query.  This identifier is copied
                the corresponding reply and can be used by the requester
                to match up replies to outstanding queries.

QR              A one bit field that specifies whether this message is a
                query (0), or a response (1).

OPCODE          A four bit field that specifies kind of query in this
                message.  This value is set by the originator of a query
                and copied into the response.  The values are:

                0               a standard query (QUERY)

                1               an inverse query (IQUERY)

                2               a server status request (STATUS)

                3-15            reserved for future use

AA              Authoritative Answer - this bit is valid in responses,
                and specifies that the responding name server is an
                authority for the domain name in question section.

                Note that the contents of the answer section may have
                multiple owner names because of aliases.  The AA bit
                corresponds to the name which matches the query name, or
                the first owner name in the answer section.

TC              TrunCation - specifies that this message was truncated
                due to length greater than that permitted on the
                transmission channel.

RD              Recursion Desired - this bit may be set in a query and
                is copied into the response.  If RD is set, it directs
                the name server to pursue the query recursively.
                Recursive query support is optional.

RA              Recursion Available - this be is set or cleared in a
                response, and denotes whether recursive query support is
                available in the name server.

Z               Reserved for future use.  Must be zero in all queries
                and responses.

RCODE           Response code - this 4 bit field is set as part of
                responses.  The values have the following
                interpretation:

                0               No error condition

                1               Format error - The name server was
                                unable to interpret the query.

                2               Server failure - The name server was
                                unable to process this query due to a
                                problem with the name server.

                3               Name Error - Meaningful only for
                                responses from an authoritative name
                                server, this code signifies that the
                                domain name referenced in the query does
                                not exist.

                4               Not Implemented - The name server does
                                not support the requested kind of query.

                5               Refused - The name server refuses to
                                perform the specified operation for
                                policy reasons.  For example, a name
                                server may not wish to provide the
                                information to the particular requester,
                                or a name server may not wish to perform
                                a particular operation (e.g., zone
                                transfer) for particular data.

                6-15            Reserved for future use.

QDCOUNT         an unsigned 16 bit integer specifying the number of
                entries in the question section.

ANCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the answer section.

NSCOUNT         an unsigned 16 bit integer specifying the number of name
                server resource records in the authority records
                section.

ARCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the additional records section.



*/

struct DNSHeader
{
	uint_16 id;                     //标志

	uint_16 QR : 1;             //Q or R
	uint_16 OPCODE : 4;         //请求中有效，表明请求的类型
	uint_16 AA : 1;             //authoritative answer，回应中有效，表明该服务器是权威回答
	uint_16 TC : 1;             //可截断的 ( truncated)
	uint_16 RD : 1;             //recursion desired，期望递归be set in a query and is copied into the response.
	uint_16 RA : 1;             //如果名字服务器支持递归，则在响应中将该比特位置1
	uint_16 Z : 3;               //0
	uint_16 RCODE : 4;          //返回码 表明返回包的类型

	uint_16 QDCOUNT;
	uint_16 ANCOUNT;
	uint_16 NSCOUNT;
	uint_16 ARCOUNT;
};



/*4.2.1. UDP usage

Messages sent using UDP user server port 53 (decimal).

Messages carried by UDP are restricted to 512 bytes (not counting the IP
or UDP headers).  Longer messages are truncated and the TC bit is set in
the header.

UDP is not acceptable for zone transfers, but is the recommended method
for standard queries in the Internet.  Queries sent using UDP may be
lost, and hence a retransmission strategy is required.  Queries or their
responses may be reordered by the network, or by processing in name
servers, so resolvers should not depend on them being returned in order.

The optimal UDP retransmission policy will vary with performance of the
Internet and the needs of the client, but the following are recommended:

   - The client should try other servers and server addresses
     before repeating a query to a specific address of a server.

   - The retransmission interval should be based on prior
     statistics if possible.  Too aggressive retransmission can
     easily slow responses for the community at large.  Depending
     on how well connected the client is to its expected servers,
     the minimum retransmission interval should be 2-5 seconds.

More suggestions on server selection and retransmission policy can be
found in the resolver section of this memo.

*/


#endif