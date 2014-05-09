/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory -
 * University of Colombo School of Computing.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the tikiridb system.
 *
 * \author
 *      Kasun Hewage <kch@ucsc.cmb.ac.lk>
 */

/* \file
 *      Header file of the TCP communication module.
 */


#ifndef TCPCOMM_H
#define TCPCOMM_H

#include <pthread.h>
#include <sys/socket.h>
#include <stdint.h>

#include <set>
#include <string>
#include <sstream>

#include "BaseComm.h"
#include "PacketBuffer.h"

/* hanshaking error codes  */
#define ERR_UNKNOWN -1
#define ERR_OK 1
#define ERR_INVALID_MODE 2
#define ERR_UNSUPPORTED_VERSION 3
#define ERR_READ_CLIENTS_FULL 4
#define ERR_WRITE_CLIENTS_FULL 5

/* client modes */
#define CLIENT_MODE_R  1 // read
#define CLIENT_MODE_W  2 // write
#define CLIENT_MODE_RW 3 // read + write


#ifdef CONF_MAX_READ_CLIENTS
#define MAX_READ_CLIENTS CONF_MAX_READ_CLIENTS
#else
#define MAX_READ_CLIENTS 10
#endif

#ifdef CONF_MAX_WRITE_CLIENTS
#define MAX_WRITE_CLIENTS CONF_MAX_WRITE_CLIENTS
#else
#define MAX_WRITE_CLIENTS 1
#endif

class TCPComm : public BaseComm
{
  protected:

    const static int maxReadClients = MAX_READ_CLIENTS;

    const static int maxWriteClients = MAX_WRITE_CLIENTS;

    /* socket server pthread */
    pthread_t serverThread;

    bool serverThreadRunning;

    /* pthread for tcp client reading */
    pthread_t readerThread;

    bool readerThreadRunning;

    /* pthread for tcp client writing */
    pthread_t writerThread;

    bool writerThreadRunning;

    /* strcuture to keep information of connected clients. */
    typedef struct clientInfo {
      /* file descriptor of the connected client. */
      int clientFD;
      /* Unified socket address. */
      struct usa {
        socklen_t len;
        struct sockaddr_storage sa;
      } clientAddress;
      /* remote port of the connected client. */
      int clientPort;
      /* client connected mode */
      int mode;
      /* overriding the default less than operator */
      friend bool operator<(clientInfo const& a, clientInfo const& b)
      {
        return a.clientFD < b.clientFD;
      }

    } clientInfo_t;

    //typedef std::set<clientInfo_t> clientInfoSet_t;

    typedef struct sharedClientInfo {

      /* mutex to proect client count and client set */
      pthread_mutex_t sharedInfoLock;
      /* mutex to protect read wakeup condition */
      pthread_mutex_t readWakeupLock;
      /* wakeup condition which is siganled when clients are connected for reading*/
      pthread_cond_t readWakeup;
       /* mutex to protect write wakeup condition */
      pthread_mutex_t writeWakeupLock;
      /* wakeup condition which is siganled when clients are connected for writing*/
      pthread_cond_t writeWakeup;
      /* connected clients count for reading */
      int readClientCount;
      /* connected clients count for writting */
      int writeClientCount;
      /* a set to store informsation of connected clients */
      std::set<clientInfo_t> clients;

    } sharedClientInfo_t;

    /* shared client information */
    sharedClientInfo_t sharedClientInfo;

    /* packet buffer to store packets read from clients. */
    PacketBuffer &readBuffer;

    /* packet buffer to store packsts which are to be sent to connected clients. */
    PacketBuffer &writeBuffer;

    /* file descriptor of the socket server */
    int serverFD;

    /* port of the server */
    int serverPort;

    /* create a socket server on specified port */
    int createServer(int port);

    /* return the port of a socket address */
    static int getPort(struct sockaddr_storage *ss);

    /* return the ip address string of a socket address */
    static std::string getIpAddressString(struct sockaddr_storage *ss);

    /* waiting to accept new clients */
    void waitForClients();

    int addClient(clientInfo_t &clientInfo);

    int removeClient(clientInfo_t &clientInfo);

    /* accept a new client */
    int acceptNewClient();

    /* handshae with the newly connected client. */
    int handshakeClient(clientInfo_t &clientInfo);

    /* send handshake error codes */
    int sendHandshakeError(clientInfo_t &clientInfo, int errorCode);

    /* read from connected clients */
    void readFromClients();

    /* write to connected clients. */
    void writeToClients();

    /* write a packet to client */
    int writePacket(const clientInfo_t &clientInfo, Packet &packet);

    /* read a packet from client */
    int readPacket(const clientInfo_t &clientInfo, Packet &packet);

    /* friend functions to operate threads  */
    friend void * serverThreadFunc(void* ob);
    friend void * readFromClientsThreadFunc(void* ob);
    friend void * writeToClientsThreadFunc(void* ob);

  private:
    /* disable standard constructor */
    TCPComm();

  public:
    /* constructor */
    TCPComm(int port, PacketBuffer &readBuffer, PacketBuffer &writeBuffer);

    /* start TCP Communication server */
    int start();

    void cancel();

};

#endif /* TCPCOMM_H */
