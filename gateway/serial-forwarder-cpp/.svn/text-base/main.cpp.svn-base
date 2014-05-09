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
 *      Main file.
 */

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <string>
#include <cstdlib>


#include "SerialComm.h"
#include "TCPComm.h"
#include "Version.h"

//#define DEBUG_EABLE 1
#define ERROR_EABLE 1

#ifdef DEBUG_EABLE
#include <iostream>
#define DEBUG(message) std::cout << message << std::endl;
#else
#define DEBUG(message)
#endif

#ifdef ERROR_EABLE
#include <iostream>
#define ERROR(message) std::cerr << message << " : " << strerror(errno) << std::endl;
#else
#define ERROR(message)
#endif

// number of command line options
// 0 - serial device
// 1 - baudrate
// 2 - port
#define OPT_NUM     3


using namespace std;

static volatile int exit_flag;  /* Program termination flag	*/


/*---------------------------------------------------------------------------*/
static void
signalHandler(int sig_num)
{
  if(sig_num == SIGCHLD) {

    do {
    }while(waitpid(-1, &sig_num, WNOHANG) > 0);

  }else if(sig_num == SIGTERM ||
           sig_num == SIGINT ||
           sig_num == SIGQUIT) {
    exit_flag = sig_num;
  }
}
/*---------------------------------------------------------------------------*/
static void
showVersion()
{
  cout<<"TikiriDB Serial Forwarder - V "<<VERSION_MAJOR
                                       <<"."
                                       <<VERSION_MINOR
                                       <<endl;
}
/*---------------------------------------------------------------------------*/
static void
printUsage(string str)
{
  showVersion();
  cout<<"Usage:"<<endl;
  cout<<str<<" -s <serial device> -b <baudrate> -p <port>"<<endl;
}
/*---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{

  int i, c;
  char *serialPort;
  int baudrate;
  int port;
  bool argErr = false;
  int wantOpt[OPT_NUM];


  for(i = 0; i < OPT_NUM; i++) {
    wantOpt[i] = 0;
  }

  // processing command line args
  while((c = getopt(argc, argv, "s:b:p:v")) != -1) {
    switch(c) {
      case 's':
        wantOpt[0]++;
        serialPort = optarg;
        break;
      case 'b':
        wantOpt[1]++;
        baudrate = atoi(optarg);
        break;
      case 'p':
        wantOpt[2]++;
        port = atoi(optarg);
        break;
      case 'v':
        showVersion();
        return 0;
      default:
        printUsage(argv[0]);
        argErr = true;
        break;
    }
    if(argErr) {
      return -1;
    }
  }

  for(i = 0; i < OPT_NUM; i++) {
    if(wantOpt[i] == 0) {
      printUsage(argv[0]);
      return -1;
    }
  }

  PacketBuffer readPktBuffer("ReadBuffer", 25);
  PacketBuffer writePktBuffer("WriteBuffer", 25);

  exit_flag = 0;
  (void) signal(SIGCHLD, signalHandler);
  (void) signal(SIGTERM, signalHandler);
  (void) signal(SIGINT, signalHandler);

  // the signal SIGPIPE is posted the process if it tries to write to a
  // closed socket.
  (void) signal(SIGPIPE, signalHandler);
  (void) signal(SIGQUIT, signalHandler);

  TCPComm tcpComm(port, readPktBuffer, writePktBuffer);
  SerialComm serComm(serialPort, baudrate, readPktBuffer, writePktBuffer);

  if(tcpComm.start() < 0) {
    DEBUG("main : can not start TCPComm. Exiting..");
    exit_flag = 1;
  }

  if(serComm.start() < 0) {
    DEBUG("main : can not start SerialComm. Exiting..");
    exit_flag = 1;
  }

  while(exit_flag == 0) {
    sleep(1);
  }

  serComm.cancel();
  tcpComm.cancel();

  return 0;
}
