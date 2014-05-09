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
 *      Implementation of the TCP communication module.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <cstdio>
#include <cstring>


#include "TCPComm.h"
#include "Version.h"

//#define DEBUG_EABLE 1
#define ERROR_EABLE 1

#if DEBUG_EABLE
#include <iostream>
#define DEBUG(message) std::cout << message << std::endl;
#else
#define DEBUG(message)
#endif

#if ERROR_EABLE
#include <iostream>
#define ERROR(message) std::cerr << message << " : " << strerror(errno) << std::endl;
#else
#define ERROR(message)
#endif


#define MAX_PORT_STR_SIZE 5

#ifdef CONF_LISTEN_QUEUE_LENGTH
#define LISTEN_QUEUE_LENGTH CONF_LISTEN_QUEUE_LENGTH
#else
#define LISTEN_QUEUE_LENGTH 10
#endif

using namespace std;

void * writeToClientsThreadFunc(void* ob);
void * readFromClientsThreadFunc(void* ob);

/*----------------------------------------------------------------------------*/
TCPComm::TCPComm(int port, PacketBuffer &readBuffer, PacketBuffer &writeBuffer)
                             : readBuffer(readBuffer), writeBuffer(writeBuffer)
{
  this->serverPort = port;
  this->readBuffer = readBuffer;
  this->writeBuffer = writeBuffer;

  serverThreadRunning = false;
  readerThreadRunning = false;
  writerThreadRunning = false;
}

/*----------------------------------------------------------------------------*/
int
TCPComm::createServer(int port)
{
  int sockfd; // listen on sock_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage *sever_address;
  char port_str[MAX_PORT_STR_SIZE + 1];
  char s[INET6_ADDRSTRLEN];
  int yes=1;
  int rv;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  sprintf(port_str, "%d", port);

    if((rv = getaddrinfo(NULL, port_str, &hints, &servinfo)) != 0) {
    ERROR("getaddrinfo: "<<gai_strerror(rv));
    return -1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      ERROR("Can not create a socket");
      continue;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      ERROR("Can not set socket options");
      return -1;
    }

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      ERROR("Can not bind the socket");
      continue;
    }

    if(listen(sockfd, LISTEN_QUEUE_LENGTH) < 0) {
      ERROR("Can not listen to the socket");
      return -1;
    }
    break;
  }

  if(p == NULL)  {
    ERROR("Could not find a socket to bind.");
    return -1;
  }

  freeaddrinfo(servinfo);
  return sockfd;
}
/*----------------------------------------------------------------------------*/
std::string
TCPComm::getIpAddressString(struct sockaddr_storage *ss)
{
  char s[INET6_ADDRSTRLEN];
  const char *retval;

  std::string addressString;

  switch(ss->ss_family) {
    case AF_INET:
      retval = inet_ntop(ss->ss_family,
               &((struct sockaddr_in *)ss)->sin_addr, s, INET6_ADDRSTRLEN);
      break;

    case AF_INET6:
      retval = inet_ntop(ss->ss_family,
                &((struct sockaddr_in6 *)ss)->sin6_addr, s, INET6_ADDRSTRLEN);
      break;
  }

  if(retval == NULL) {
    return "";
  }

  return s;
}
/*----------------------------------------------------------------------------*/
int
TCPComm::getPort(struct sockaddr_storage *ss)
{
  switch(ss->ss_family) {
    case AF_INET:
      return ((struct sockaddr_in*)ss)->sin_port;
    case AF_INET6:
      return ((struct sockaddr_in6*)ss)->sin6_port;
  }
}
/*----------------------------------------------------------------------------*/
void *
serverThreadFunc(void* ob)
{
  static_cast<TCPComm*>(ob)->waitForClients();
  return NULL;
}
/*----------------------------------------------------------------------------*/
int
TCPComm::start()
{
  int ret;

  pthread_mutex_init(&sharedClientInfo.sharedInfoLock, NULL);
  pthread_mutex_init(&sharedClientInfo.readWakeupLock, NULL);
  pthread_mutex_init(&sharedClientInfo.writeWakeupLock, NULL);

  pthread_cond_init(&sharedClientInfo.readWakeup, NULL);
  pthread_cond_init(&sharedClientInfo.writeWakeup, NULL);

  sharedClientInfo.readClientCount = 0;
  sharedClientInfo.writeClientCount = 0;
  sharedClientInfo.clients.clear();

  this->serverFD = createServer(this->serverPort);
  if(this->serverFD < 1) {
    this->serverFD = -1;
    return -1;
  }

  ret = pthread_create(&serverThread, NULL, serverThreadFunc, this);
  if(ret != 0) {
    ERROR("TCPComm::start : Could not start server thread");
    return -1;
  }
  serverThreadRunning = true;

  ret = pthread_create(&writerThread, NULL, writeToClientsThreadFunc, this);
  if(ret != 0) {
    ERROR("TCPComm::start : Could not start write to clients thread");
    return -1;
  }
  writerThreadRunning = true;

  ret = pthread_create(&readerThread, NULL, readFromClientsThreadFunc, this);
  if(ret != 0) {
    ERROR("TCPComm::start : Could not start read from clients thread");
    return -1;
  }
  readerThreadRunning = true;

  DEBUG("TCPComm::start : TCPComm started.");
  return 0;
}
/*----------------------------------------------------------------------------*/
int
TCPComm::addClient(clientInfo_t &clientInfo)
{
  bool addingFailed = false;
  int errorCode = ERR_OK;

  pthread_testcancel();
  pthread_mutex_lock(&sharedClientInfo.sharedInfoLock);

  if((clientInfo.mode & CLIENT_MODE_R) == CLIENT_MODE_R) {
    if(sharedClientInfo.readClientCount == 0) {
      pthread_cond_broadcast(&sharedClientInfo.readWakeup);
    }
    sharedClientInfo.readClientCount++;
  }

  if((clientInfo.mode & CLIENT_MODE_W) == CLIENT_MODE_W) {
    if(sharedClientInfo.writeClientCount == 0) {
      pthread_cond_broadcast(&sharedClientInfo.writeWakeup);
    }
    sharedClientInfo.writeClientCount++;
  }
  DEBUG("TCPComm::addClient : Client added.");
  sharedClientInfo.clients.insert(clientInfo);

  pthread_mutex_unlock(&sharedClientInfo.sharedInfoLock);

  return errorCode;
}
/*----------------------------------------------------------------------------*/
int
TCPComm::removeClient(clientInfo_t &clientInfo)
{
  bool clientRemoved = false;
  int rCount = 0;

  pthread_testcancel();
  pthread_mutex_lock(&sharedClientInfo.sharedInfoLock);

  DEBUG("TCPComm::removeClient : removing client fd " << clientInfo.clientFD);

  if((clientInfo.mode & CLIENT_MODE_R) == CLIENT_MODE_R &&
     sharedClientInfo.readClientCount > 0) {

    rCount = sharedClientInfo.clients.erase(clientInfo);
    if(close(clientInfo.clientFD) < 0) {
      DEBUG("TCPComm::removeClient : error closing fd " << clientInfo.clientFD);
    }
    if(rCount > 0) {
      sharedClientInfo.readClientCount--;
    }
    clientRemoved = true;
  }

  if((clientInfo.mode & CLIENT_MODE_W) == CLIENT_MODE_W &&
     sharedClientInfo.writeClientCount > 0) {

    if(!clientRemoved) {
      rCount = sharedClientInfo.clients.erase(clientInfo);
      if(close(clientInfo.clientFD) < 0) {
        DEBUG("TCPComm::removeClient : error closing fd " << clientInfo.clientFD);
      }
    }
    if(rCount > 0) {
      sharedClientInfo.writeClientCount--;
    }
  }

  DEBUG("TCPComm::removeClient : removed count " << rCount);
  pthread_mutex_unlock(&sharedClientInfo.sharedInfoLock);
}
/*----------------------------------------------------------------------------*/
int
TCPComm::handshakeClient(clientInfo_t &clientInfo)
{
  char buf[4];
  int k;
  bool invalidMode = false;
  int nRClients, nWClients;

  k = recv(clientInfo.clientFD, buf, 4, 0);
  if(k != 4) {
    DEBUG("TCPComm::handshakeClient : Could not read init byes from the client. fd:"
                                                              <<clientInfo.clientFD);
    return ERR_UNKNOWN;
  }

  /* check version */
  if(buf[0] != VERSION_MAJOR && buf[1] != VERSION_MINOR) {
    return ERR_UNSUPPORTED_VERSION;
  }
  /* check connected mode */
  switch(buf[2]) {
    case CLIENT_MODE_R:
      clientInfo.mode = CLIENT_MODE_R;
      break;
    case CLIENT_MODE_W:
      clientInfo.mode = CLIENT_MODE_W;
      break;
    case  CLIENT_MODE_RW:
      clientInfo.mode = CLIENT_MODE_RW;
      break;
    default:
      invalidMode = true;
  }

  if(invalidMode) {
    DEBUG("TCPComm::handshakeClient : Invalid mode. fd:"<<clientInfo.clientFD);
    return ERR_INVALID_MODE;
  }

  pthread_mutex_lock(&sharedClientInfo.sharedInfoLock);
  nRClients = sharedClientInfo.readClientCount;
  nWClients = sharedClientInfo.writeClientCount;
  pthread_mutex_unlock(&sharedClientInfo.sharedInfoLock);

  if(((clientInfo.mode & CLIENT_MODE_R) == CLIENT_MODE_R) &&
      (nRClients >= TCPComm::maxReadClients)) {
    return ERR_READ_CLIENTS_FULL;
  }

  if(((clientInfo.mode & CLIENT_MODE_W) == CLIENT_MODE_W) &&
      (nWClients >= TCPComm::maxWriteClients)) {
    return ERR_WRITE_CLIENTS_FULL;
  }

  return ERR_OK;
}
/*----------------------------------------------------------------------------*/
int
TCPComm::sendHandshakeError(clientInfo_t &clientInfo, int errorCode)
{
  char buf[4];
  buf[0] = VERSION_MAJOR;
  buf[1] = VERSION_MINOR;
  buf[2] = errorCode;
  return send(clientInfo.clientFD, buf, 4, 0);
}
/*----------------------------------------------------------------------------*/
int
TCPComm::acceptNewClient()
{
  clientInfo_t clientInfo;
  int errorCode = ERR_OK;

  clientInfo.clientAddress.len = sizeof(clientInfo.clientAddress.sa);
  clientInfo.clientFD = accept(this->serverFD,
                               (struct sockaddr *)&clientInfo.clientAddress.sa,
                               &clientInfo.clientAddress.len);
  if(clientInfo.clientFD < 0) {
    ERROR("Could not accept new client");
    return -1;
  }
  DEBUG("TCPComm::acceptNewClient : Client connected IP "
        <<TCPComm::getIpAddressString(&clientInfo.clientAddress.sa)
        << ", PORT "<<TCPComm::getPort(&clientInfo.clientAddress.sa)
        << " fd:"<<clientInfo.clientFD);

  errorCode = handshakeClient(clientInfo);
  sendHandshakeError(clientInfo, errorCode);

  if(errorCode != ERR_OK) {
    DEBUG("TCPComm::acceptNewClient : Handshake failed. fd:"<<clientInfo.clientFD);
    close(clientInfo.clientFD);
    return 0;
  } else {
    addClient(clientInfo);
  }

  return 0;
}
/*----------------------------------------------------------------------------*/
void
TCPComm::waitForClients()
{
  int k;
  while(true) {
    k = acceptNewClient();
    if(k < 0) {
      cancel();
    }
  }
}
/*----------------------------------------------------------------------------*/
void
TCPComm::writeToClients()
{

  Packet packet;
  while(true) {
    std::set<clientInfo_t> clientInfoSet;

    // We do not wait until clients to connect to send packets.
    // Instead, we discard the packets if there is no client connected in
    // write mode.
/*
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock,
                         (void *)&sharedClientInfo.sharedInfoLock);

    pthread_mutex_lock(&sharedClientInfo.sharedInfoLock);
    while(sharedClientInfo.readClientCount == 0)  {

      DEBUG("TCPComm::writeToClients : sleeping writer thread");
      pthread_cond_wait(&sharedClientInfo.readWakeup,
                        &sharedClientInfo.sharedInfoLock);
    }

    DEBUG("TCPComm::writeToClients : writer thread woked ");


    pthread_cleanup_pop(1);
*/
    packet = readBuffer.dequeue();

    pthread_mutex_lock(&sharedClientInfo.sharedInfoLock);
    clientInfoSet = sharedClientInfo.clients;
    pthread_mutex_unlock(&sharedClientInfo.sharedInfoLock);

    DEBUG("TCPComm::writeToClients : number of clients : " << clientInfoSet.size()
          << " packet size : " << packet.getPacketLength());
    if(clientInfoSet.size() == 0) {
      DEBUG("TCPComm::writeToClients : no clients. discarding the packet");
      continue;
    }

    std::set<clientInfo_t>::iterator it;

    for(it = clientInfoSet.begin(); it != clientInfoSet.end(); it++) {

      clientInfo_t clientInfo = *it;

      if((clientInfo.mode & CLIENT_MODE_R) != CLIENT_MODE_R) {
        continue;
      }
      if(writePacket(clientInfo, packet) < 0) {
        DEBUG("TCPComm::writeClients : removeClient");
        removeClient(clientInfo);
      } else {
        // currently do nothing
      }


    }

  }


}
/*----------------------------------------------------------------------------*/
void
TCPComm::readFromClients()
{
  std::set<clientInfo_t> clientInfoSet;

  while(true) {

    // Here, we do wait until at lease a client is connected in write mode.
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock,
                         (void *)&sharedClientInfo.sharedInfoLock);

    pthread_mutex_lock(&sharedClientInfo.sharedInfoLock);
    while(sharedClientInfo.writeClientCount == 0)  {

      DEBUG("TCPComm::readFromClients : sleeping reader thread");
      pthread_cond_wait(&sharedClientInfo.writeWakeup,
                        &sharedClientInfo.sharedInfoLock);
    }

    clientInfoSet = sharedClientInfo.clients;

    pthread_cleanup_pop(1);

    fd_set rfds;
    std::set<clientInfo_t>::iterator it;
    int maxFD;

    FD_ZERO(&rfds);

    if(clientInfoSet.size() != 0) {
      maxFD = ((clientInfo_t)*(clientInfoSet.begin())).clientFD;
    }
    // scan all of the connected clients who have connected for writting
    for(it = clientInfoSet.begin(); it != clientInfoSet.end(); it++) {
      clientInfo_t clientInfo = *it;

      if((clientInfo.mode & CLIENT_MODE_W) == CLIENT_MODE_W) {
        if(clientInfo.clientFD > maxFD) {
          maxFD = clientInfo.clientFD;
        }
        FD_SET(clientInfo.clientFD, &rfds);
      }
    }


    if(select(maxFD + 1, &rfds, NULL, NULL, NULL) < 0 ) {
      ERROR("TCPComm::readFromClients : select(maxFD+1, &rfds, NULL, NULL, NULL)");

    } else {

      for(it = clientInfoSet.begin(); it != clientInfoSet.end(); it++) {
        clientInfo_t clientInfo = *it;
        if((clientInfo.mode & CLIENT_MODE_W) == CLIENT_MODE_W) {
          if(FD_ISSET(clientInfo.clientFD, &rfds)) {
            Packet packet;
            if(readPacket(clientInfo, packet) < 0) {
              // error on reading from client, we remove him
              DEBUG("TCPComm::readFromClients : error on reading from client");
              removeClient(clientInfo);

            } else {
              if(!writeBuffer.isFull()) {
                writeBuffer.enqueueBack(packet);
                DEBUG("TCPComm::readFromClients : packet qued");
              } else {
                DEBUG("TCPComm::readFromClients : read buffer is full. packet is dropped");
              }
            }

          } // end of if(FD_ISSET(clientInfo.clientFD, &rfds))
        } // end of if((clientInfo.mode & CLIENT_MODE_W) == CLIENT_MODE_W)
      }
    }



  }
}
/*----------------------------------------------------------------------------*/

void *
writeToClientsThreadFunc(void* ob)
{
  static_cast<TCPComm*>(ob)->writeToClients();
  return NULL;
}
/*----------------------------------------------------------------------------*/
void *
readFromClientsThreadFunc(void* ob)
{
  static_cast<TCPComm*>(ob)->readFromClients();
  return NULL;
}
/*----------------------------------------------------------------------------*/
int
TCPComm::writePacket(const clientInfo_t &clientInfo, Packet &packet)
{
  char buf[4]; // for initial meta data of the packet
  int k;
  int pktLen = packet.getPacketLength();

  buf[0] = pktLen & 0xFF;
  buf[1] = (pktLen >> 8) & 0xFF;
  buf[2] = packet.getPacketType();
  buf[3] = 0; // this field is not used

  // initial meta data of the packet
  k = send(clientInfo.clientFD, buf, 4, NULL);
  if(k != 4) {
    DEBUG("TCPComm::writePacket : Can not send initial meta data fd:"<<clientInfo.clientFD);
    return -1;
  }

  k = writeAll(clientInfo.clientFD, packet.getPayload(), pktLen);
  if(k != pktLen) {
    DEBUG("TCPComm::writePacket : Can not send packet payload fd:"<<clientInfo.clientFD);
    return -1;
  }

  return 0;
}
/*----------------------------------------------------------------------------*/
int
TCPComm::readPacket(const clientInfo_t &clientInfo, Packet &packet)
{
  char pktBuf[packet.getMaxPacketSize()]; // for the packet buffer
  char buf[4]; // for initial meta data of the packet
  int k;
  int pktLen;
  int pktType;

  k = recv(clientInfo.clientFD, buf, 4, NULL);
  if(k != 4) {
    DEBUG("TCPComm::readPacket : Can not receive initial meta data fd : "
                                                         <<clientInfo.clientFD);
    return -1;
  }

  pktLen = (unsigned char)buf[0] + (unsigned char)(buf[1] << 8);
  pktType = buf[2];
  //DEBUG("TCPComm::readPacket : receive initial meta data len:" << pktLen);

  if(pktLen > packet.getMaxPacketSize()) {
    DEBUG("TCPComm::readPacket : Can not accept more than the maximum packet size");
    return -1;
  }

  k = readAll(clientInfo.clientFD, pktBuf, pktLen);
  if(k != pktLen) {
    DEBUG("TCPComm::readPacket : Can not receive packet payload");
    return -1;
  }
  //DEBUG("TCPComm::readPacket : receive payload");
  packet.setPayload(pktBuf, pktLen, pktType);

  return 0;
}
/*----------------------------------------------------------------------------*/
void
TCPComm::cancel()
{
  pthread_t callingThread = pthread_self();

  if(pthread_equal(callingThread, readerThread)) {
    DEBUG("TCPComm::cancel : by readerThread")
    pthread_detach(readerThread);
    if(writerThreadRunning) {
      pthread_cancel(writerThread);
      DEBUG("TCPComm::cancel : writerThread canceled, joining")
      pthread_join(writerThread, NULL);
      writerThreadRunning = false;
    }
    if(serverThreadRunning) {
      pthread_cancel(serverThread);
       DEBUG("TCPComm::cancel : serverThread canceled, joining")
       pthread_join(serverThread, NULL);
       serverThreadRunning = false;
    }
    readerThreadRunning = false;
    pthread_exit(NULL);

  } else if(pthread_equal(callingThread, writerThread)) {
    DEBUG("TCPComm::cancel : by writerThread")
    pthread_detach(writerThread);
    if(readerThreadRunning) {
      pthread_cancel(readerThread);
      DEBUG("TCPComm::cancel : readerThread canceled, joining")
      pthread_join(readerThread, NULL);
      readerThreadRunning = false;
    }
    if(serverThreadRunning) {
      pthread_cancel(serverThread);
      DEBUG("TCPComm::cancel : serverThread canceled, joining")
      pthread_join(serverThread, NULL);
      serverThreadRunning = false;
    }
    writerThreadRunning = false;
    pthread_exit(NULL);

  } else if(pthread_equal(callingThread, serverThread)) {
    DEBUG("TCPComm::cancel : by serverThread")
    pthread_detach(serverThread);
    if(readerThreadRunning) {
      pthread_cancel(readerThread);
      DEBUG("TCPComm::cancel : readerThread canceled, joining")
	    pthread_join(readerThread, NULL);
      readerThreadRunning = false;
    }
    if(writerThreadRunning) {
      pthread_cancel(writerThread);
      DEBUG("TCPComm::cancel : writerThread canceled, joining")
      pthread_join(writerThread, NULL);
      writerThreadRunning = false;
    }
    serverThreadRunning = false;
    pthread_exit(NULL);

  } else {
    DEBUG("TCPComm::cancel : by other thread")
	  if(serverThreadRunning) {
      pthread_cancel(serverThread);
      DEBUG("TCPComm::cancel : serverThread canceled, joining")
      pthread_join(serverThread, NULL);
      serverThreadRunning = false;
    }
 	  if(writerThreadRunning) {
      pthread_cancel(writerThread);
      DEBUG("TCPComm::cancel : writerThread canceled, joining")
      pthread_join(writerThread, NULL);
      writerThreadRunning = false;
    }
    if(readerThreadRunning) {
      pthread_cancel(readerThread);
      DEBUG("TCPComm::cancel : readerThread canceled, joining")
      pthread_join(readerThread, NULL);
      readerThreadRunning = false;
    }
  }

}

