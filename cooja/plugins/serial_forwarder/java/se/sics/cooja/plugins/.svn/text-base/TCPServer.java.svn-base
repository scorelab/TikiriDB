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

public class TCPServer extends Thread {

	private static Logger logger = Logger.getLogger(TCPServer.class);

	private int serverPort = 9999;
	private ServerSocket serverSocket = null;
	private boolean isRunning = false;
	private ClientInfo clientInfo = null;

	private PacketBuffer serialWriteBuffer = null;

	private static final int MAX_READ_CLIENTS = 10;
	private static final int MAX_WRITE_CLIENTS = 1;

	public TCPServer(PacketBuffer serialWriteBuffer) {
		this.setName("TCPServer_Thread");
		this.clientInfo = new ClientInfo();
		this.serialWriteBuffer = serialWriteBuffer;
		this.isRunning = false;

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Thread#run()
	 */
	public void run() {
		logger.info("TCP Server started.");
		while (true) {
			try {
				Socket clientSocket = serverSocket.accept();
				logger.info("Client connected address : "
						+ clientSocket.getRemoteSocketAddress().toString());
				SFClient client = new SFClient(clientSocket, this);
				if (client.handShake()) {
					clientInfo.addClient(client);
					client.doPacketHandling();

				} else {
					client.disconnect();
				}

			} catch (IOException e) {
				//logger.info(e.getMessage());
				if (!isRunning) {
					break;
				} else {
					e.printStackTrace();
				}
			}
		}
		logger.info("TCP Server stopped.");
	}

	public boolean startServer(int port) {

		serverPort = port;
		try {
			serverSocket = new ServerSocket(serverPort);
			serverSocket.setReuseAddress(true);
		} catch (IOException e) {
			logger.warn(e.getMessage());
			e.printStackTrace();
			isRunning = false;
			return false;
		}

		this.start();
		isRunning = true;
		return true;
	}

	@SuppressWarnings("unchecked")
	public void disconnectAllClients() {

		Vector<SFClient> clients = (Vector<SFClient>) clientInfo.getClients()
				.clone();
		for (SFClient client : clients) {
			client.disconnect();
		}
	}

	public void stopServer() {
		disconnectAllClients();
		isRunning = false;
		try {
			this.interrupt();
			serverSocket.close();
			this.join();

		} catch (Exception e) {
			// e.printStackTrace();
		}
	}

	public boolean isRunning() {
		return this.isAlive();
	}

	/**
	 * Send a packet to all clients. This method simply inserts the packet to
	 * the packet queue of each client.
	 * 
	 * @param packet
	 *            the packet to be sent to all clients
	 */
	public void sendPacketToAll(Packet packet) {

		synchronized (clientInfo) {
			for (SFClient client : clientInfo.getClients()) {
				client.addPacketToQueue(packet);
			}
		}

	}

	public ClientInfo getClientInfo() {
		return clientInfo;
	}

	public int getMaxReadClients() {
		return MAX_READ_CLIENTS;
	}

	public static int getMaxWriteClients() {
		return MAX_WRITE_CLIENTS;
	}

	public PacketBuffer getSerialWriteBuffer() {
		return serialWriteBuffer;
	}

}
