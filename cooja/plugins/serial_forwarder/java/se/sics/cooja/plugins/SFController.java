/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory,
 * University of Colombo School of Computting.
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
 */ 

package se.sics.cooja.plugins;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Vector;

import org.apache.log4j.Logger;

import se.sics.cooja.interfaces.SerialPort;

public class SFController {

	private static Logger logger = Logger.getLogger(SFController.class);

	private static final int WRITE_BUFFER_SIZE = 20;
	private final static int PORT_START = 25600;

	private PacketBuffer serialWriteBuffer = null;

	private SerialServer serialServer = null;
	private TCPServer tcpServer = null;

	private int tcpPort = PORT_START;

	private SerialPort serialPort = null;

	public SFController(SerialPort serialPort, int tcpPort) {

		this.serialPort = serialPort;
		serialWriteBuffer = new PacketBuffer(WRITE_BUFFER_SIZE);
		// tcpServer = new TCPServer(serialWriteBuffer);
		// serialServer = new SerialServer(this);
		this.tcpPort = tcpPort;
	}

	public boolean startServers() {

		tcpServer = new TCPServer(serialWriteBuffer);
		serialServer = new SerialServer(this);

		if (!tcpServer.startServer(this.tcpPort)) {
			tcpServer = null;
			serialServer = null;
			return false;
		}
		serialServer.startServer();
		return true;
	}

	public void stopServers() {
		tcpServer.stopServer();
		serialServer.stopServer();

		tcpServer = null;
		serialServer = null;
	}

	public void sendPacketToAll(Packet packet) {
		if (tcpServer != null) {
			tcpServer.sendPacketToAll(packet);
		}
	}

	public boolean serversRunning() {
		return (tcpServer != null) && (serialServer != null);
	}

	public PacketBuffer getSerialWriteBuffer() {
		return serialWriteBuffer;
	}

	public SerialPort getSerialPort() {
		return serialPort;
	}

}
