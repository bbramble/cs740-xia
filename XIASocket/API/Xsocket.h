/*
** Copyright 2011 Carnegie Mellon University
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
/*!
  @file Xsocket.h
  @brief Xsocket API header file
*/

#ifndef XSOCKET_H
#define XSOCKET_H

#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAXBUFLEN 65000 // Note that this limits the size of chunk we can receive TODO: What should this be?
#define XIA_MAXBUF MAXBUFLEN
#define XIA_MAXCHUNK	MAXBUFLEN

#define XSOCK_INVALID -1 // invalid socket type	
#define XSOCK_STREAM 1	// Reliable transport (SID)
#define XSOCK_DGRAM 2	// Unreliable transport (SID)
#define XSOCK_RAW	3	// Raw XIA socket
#define XSOCK_CHUNK 4	// Content Chunk transport (CID)

#define WAITING_FOR_CHUNK 0
#define READY_TO_READ 1
#define REQUEST_FAILED -1
#define INVALID_HASH -2

/* Cache policy */
#define POLICY_LRU				0x00000001
#define POLICY_FIFO				0x00000002
#define POLICY_REMOVE_ON_EXIT	0x00001000
#define POLICY_RETAIN_ON_EXIT	0x00002000
#define POLICY_DEFAULT			(POLICY_LRU | POLICY_RETAIN_ON_EXIT)

#define CID_HASH_SIZE 40

#define DEFAULT_CHUNK_SIZE	2000

/* CID cache context */
typedef struct {
    int sockfd;
    int contextID;
	unsigned cachePolicy;
    unsigned cacheSize;
	unsigned ttl;
} ChunkContext;

typedef struct {
	int size;
	char cid[CID_HASH_SIZE];
	int32_t ttl;
	struct timeval timestamp;
} ChunkInfo;

typedef struct {
	char* cid;
	size_t cidLen; 
	int status; // 1: ready to be read, 0: waiting for chunk response, -1: failed
} ChunkStatus;

// Xsetsockopt options
#define XOPT_HLIM		1	// Hop Limit TTL
#define XOPT_NEXT_PROTO	2	// change the next proto field of the XIA header

// XIA protocol types
#define XPROTO_XIA_TRANSPORT	0x0e
#define XPROTO_XCMP		0x3d

// error codes
#define ECLICKCONTROL 9999	// error code for general click communication errors

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

//Function list
extern int Xsendto(int sockfd,const void *buf, size_t len, int flags,char * dDAG, size_t dlen);
extern int Xrecvfrom(int sockfd,void *rbuf, size_t len, int flags, char * sDAG, size_t *dlen);
extern int Xsocket(int transport_type);
extern int Xconnect(int sockfd, const char* dDAG);
extern int Xbind(int sockfd, char* SID);
extern int Xclose(int sock);
extern int Xrecv(int sockfd, void *rbuf, size_t len, int flags);
extern int Xsend(int sockfd, const void *buf, size_t len, int flags);

extern int XrequestChunk(int sockfd, char* dag, size_t dagLen);
extern int XrequestChunks(int sockfd, const ChunkStatus *chunks, int numChunks);
extern int XgetChunkStatus(int sockfd, char* dag, size_t dagLen);
extern int XgetChunkStatuses(int sockfd, ChunkStatus *statusList, int numCids);
extern int XreadChunk(int sockfd, void *rbuf, size_t len, int flags, char *cid, size_t cidLen);

extern ChunkContext *XallocCacheSlice(unsigned policy, unsigned ttl, unsigned size);
extern int XfreeCacheSlice(ChunkContext *ctx);
extern int XputChunk(const ChunkContext *ctx, const char *data, unsigned length, ChunkInfo *info);
extern int XputFile(ChunkContext *ctx, const char *fname, unsigned chunkSize, ChunkInfo **infoList);
extern int XputBuffer(ChunkContext *ctx, const char *, unsigned size, unsigned chunkSize, ChunkInfo **infoList);
extern int XremoveChunk(ChunkContext *ctx, const char *cid);
extern void XfreeChunkInfo(ChunkInfo *infoList);

extern int Xaccept(int sockfd);
extern void error(const char *msg);
extern void set_conf(const char *filename, const char *sectioname);
extern void print_conf();

extern int Xsetsockopt(int sockfd, int optname, const void *optval, socklen_t optlen);
extern int Xgetsockopt(int sockfd, int optname, void *optval, socklen_t *optlen);

extern char *XgetDAGbyName(const char *name);
extern int XregisterName(const char *name, const char *DAG);

extern int XreadLocalHostAddr(int sockfd, char *localhostAD, unsigned lenAD, char *localhostHID, unsigned lenHID);

/* internal only functions */
extern int XupdateAD(int sockfd, char *newad);
extern int XupdateNameServerDAG(int sockfd, char *nsDAG);
extern int XreadNameServerDAG(int sockfd, char *nsDAG);

#ifdef __cplusplus
}
#endif

#endif

