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
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import org.apache.log4j.Logger;

enum SFCleintMode {
	CLIENT_MODE_R, CLIENT_MODE_W, CLIENT_MODE_RW
}

public class SFClient {

	private static Logger logger = Logger.getLogger(SFClient.class);

	private static final int VERSION_MAJOR = 1;
	private static final int VERSION_MINOR = 0;

	private static final int ERR_UNKNOWN = -1;
	private static final int ERR_OK = 1;
	private static final int ERR_INVALID_MODE = 2;
	private static final int ERR_UNSUPPORTED_VERSION = 3;
	private static final int ERR_READ_CLIENTS_FULL = 4;
	private static final int ERR_WRITE_CLIENTS_FULL = 5;

	private static final int CLIENT_MODE_R = 1;
	private static final int CLIENT_MODE_W = 2;
	private static final int CLIENT_MODE_RW = 3;

	private static final int DATA_PACKET_TYPE = 1;
	private static final int DEBUG_PACKET_TYPE = 2;

	private OutputStream out = null;
	private InputStream in = null;
	private Socket socket = null;
	private PacketBuffer serialWriteBuffer = null;
	private PacketBuffer clientWriteQueue = null;

	private Thread readThread = null;
	private Thread writeThread = null;

	private Thread disconnectCaller = null;
	private Object mutexObject = null;

	private SFCleintMode mode = SFCleintMode.CLIENT_MODE_R;

	private TCPServer tcpServer = null;

	/**
	 * Create a serial forwarder client.
	 * 
	 * @param socket
	 *            client's socket
	 * @param serialWriteBuffer
	 *            write buffer of the serial port in which all packets read from
	 *            clients are sent to this buffer
	 */
	public SFClient(Socket socket, TCPServer tcpServer) {
		this.tcpServer = tcpServer;
		this.socket = socket;

		this.serialWriteBuffer = this.tcpServer.getSerialWriteBuffer();

		try {
			in = socket.getInputStream();
			out = socket.getOutputStream();
		} catch (IOException e) {
			e.printStackTrace();
		}

		mutexObject = new Object();

	}

	/**
	 * Handshake with the client and return the error code.
	 * 
	 * @return true if the handshaking is successful. Otherwise false.
	 */
	public boolean handShake() {

		byte[] buffer = new byte[4];
		int status = getHandshakeErrorCode();
		boolean handshakeFailed = false;

		if (status == ERR_OK) {
			handshakeFailed = false;
		} else {
			handshakeFailed = true;
		}

		buffer[0] = VERSION_MAJOR;
		buffer[1] = VERSION_MINOR;
		buffer[2] = (byte) status;

		try {
			out.write(buffer, 0, 4);
		} catch (IOException e) {
			// e.printStackTrace();
			handshakeFailed = true;
		}
		return !handshakeFailed;
	}

	/**
	 * Get the handshake error code.
	 * 
	 * @return the handshake error code. If no error ERR_OK will be return.
	 */
	private int getHandshakeErrorCode() {
		byte[] buffer = new byte[4];
		int dataLength = 0;
		try {

			dataLength = in.read(buffer, 0, 4);
			if (dataLength != 4) {
				return ERR_UNKNOWN;
			}

			if (buffer[0] != VERSION_MAJOR || buffer[1] != VERSION_MINOR) {
				return ERR_UNSUPPORTED_VERSION;
			}

			switch (buffer[2]) {
			case CLIENT_MODE_R:
				mode = SFCleintMode.CLIENT_MODE_R;
				break;
			case CLIENT_MODE_W:
				mode = SFCleintMode.CLIENT_MODE_W;
				break;
			case CLIENT_MODE_RW:
				mode = SFCleintMode.CLIENT_MODE_RW;
				break;
			default:
				return ERR_INVALID_MODE;
			}

			if (mode == SFCleintMode.CLIENT_MODE_R
					|| mode == SFCleintMode.CLIENT_MODE_RW) {

				if (tcpServer.getClientInfo().getReadClientCount() >= tcpServer
						.getMaxReadClients()) {
					return ERR_READ_CLIENTS_FULL;
				}

			}

			if (mode == SFCleintMode.CLIENT_MODE_W
					|| mode == SFCleintMode.CLIENT_MODE_RW) {

				if (tcpServer.getClientInfo().getWriteClientCount() >= tcpServer
						.getMaxWriteClients()) {
					return ERR_WRITE_CLIENTS_FULL;
				}
			}

		} catch (IOException e) {
			// e.printStackTrace();
			return ERR_UNKNOWN;
		}

		return ERR_OK;
	}

	/**
	 * Send a packet to the connected client.
	 * 
	 * @param packet
	 *            Packet to be sent
	 * @return true if the packet is sent successfully. Otherwise false
	 */
	public boolean sendPacket(Packet packet) {
		byte[] buffer = new byte[4];
		int dataLength = 0;

		try {
			dataLength = packet.getData().length;
			buffer[0] = (byte) (dataLength & 0xFF);
			buffer[1] = (byte) ((dataLength >> 8) & 0xFF);
			switch (packet.getType()) {
			case PKT_TYPE_DEBUG:
				buffer[2] = DEBUG_PACKET_TYPE;
				break;

			default:
				buffer[2] = DATA_PACKET_TYPE;
				break;
			}

			out.write(buffer);
			out.write(packet.getData());

		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}

		return true;
	}

	/**
	 * Receive a packet form connected client.
	 * 
	 * @return the received packet and returns null in case of error.
	 */
	public Packet receivePacket() {
		byte[] buffer = new byte[4];
		int readCount = 0;
		int dataLength = 0;
		PacketType type = PacketType.PKT_TYPE_DATA;
		Packet packet = null;

		try {

			readCount = in.read(buffer, 0, 4);
			if (readCount != 4) {
				return null;
			}

			dataLength = buffer[0] + (buffer[1] << 8);

			if (buffer[2] == DATA_PACKET_TYPE) {
				type = PacketType.PKT_TYPE_DATA;
			} else if (buffer[2] == DEBUG_PACKET_TYPE) {
				type = PacketType.PKT_TYPE_DEBUG;
			}

			buffer = new byte[dataLength];
			readCount = in.read(buffer, 0, dataLength);

			if (readCount != dataLength) {
				return null;
			}

			packet = new Packet();
			packet.setData(buffer, type);

		} catch (IOException e) {
			// e.printStackTrace();
		}
		return packet;
	}

	/**
	 * Add a packet to the client's packet queue.
	 * 
	 * @param packet
	 *            the packet to be added to the client's queue.
	 * @return true if the packet is added successfully. Otherwise false.
	 */
	public boolean addPacketToQueue(Packet packet) {
		return clientWriteQueue.addPacket(packet);
	}

	/**
	 * Start the threads for packet handling.
	 */
	public void doPacketHandling() {

		clientWriteQueue = new PacketBuffer(20);

		// reader thread to read from clients
		readThread = new Thread(new ReaderThreadRunnable(this));
		readThread.setName("Reader_Thread-"
				+ socket.getRemoteSocketAddress().toString());

		// writer thread to write clients
		writeThread = new Thread(new WriterThreadRunnable(this));
		writeThread.setName("Writer_Thread-"
				+ socket.getRemoteSocketAddress().toString());

		readThread.start();

		writeThread.start();
	}

	/**
	 * Disconnect the connected client.
	 */
	public void disconnect() {

		synchronized (mutexObject) {
			if (disconnectCaller == null) {
				disconnectCaller = Thread.currentThread();
				// close in/out streams and the socket
				try {
					logger.info("Client disconnected : "
							+ socket.getRemoteSocketAddress().toString());
					in.close();
					out.close();
					socket.close();
				} catch (IOException e) {
					// e.printStackTrace();
				}
			}
		}

		// if disconnect() is firstly called from the write thread.
		if (Thread.currentThread() == writeThread) {
			if (disconnectCaller == writeThread) {
				if (!readThread.isInterrupted()) {
					readThread.interrupt();
				}
				try {
					readThread.join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			} else {
				return;
			}
		} else if (Thread.currentThread() == readThread) {
			if (disconnectCaller == readThread) {
				if (!writeThread.isInterrupted()) {
					writeThread.interrupt();
				}
				try {
					writeThread.join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			} else {
				return;
			}
		} else {
			// System.out.println("disconnect by other thread");
			if (writeThread != null && !writeThread.isInterrupted()) {
				writeThread.interrupt();
			}
			if (readThread != null && !readThread.isInterrupted()) {
				readThread.interrupt();
			}
			try {
				// System.out.println("waiting for write thread.");
				if(writeThread != null) {
					writeThread.join();
				}
				// System.out.println("waiting for read thread.");
				if(readThread != null) {
					readThread.join();
				}
			} catch (InterruptedException e) {
				logger.info(e.getMessage() + " NOT CRITICAL");
			}
		}

		// remove the client from the server's clients list
		tcpServer.getClientInfo().removeClient(this);

	}

	/**
	 * Get the write packet buffer of the client.
	 * 
	 * @return write packet buffer
	 */
	public PacketBuffer getSerialWriteBuffer() {
		return serialWriteBuffer;
	}

	/**
	 * Get the read packet buffer of the client
	 * 
	 * @return read packet buffer
	 */
	public PacketBuffer getWriteQueue() {
		return clientWriteQueue;
	}

	/**
	 * Get the connected mode of the client.
	 * 
	 * @return connected mode of the client
	 */
	public SFCleintMode getMode() {
		return mode;
	}

	/**
	 * Set the connected mode of the client.
	 * 
	 * @param mode
	 *            connected mode of the client
	 */
	public void setMode(SFCleintMode mode) {
		this.mode = mode;
	}
}

/**
 * 
 * Runnable for the reader thread which reads packets from the connected
 * clients.
 * 
 * @author kasun
 * 
 */
class ReaderThreadRunnable implements Runnable {

	private static Logger logger = Logger.getLogger(ReaderThreadRunnable.class);

	private SFClient client = null;
	private PacketBuffer serialWriteBuffer = null;

	public ReaderThreadRunnable(SFClient sfClient) {
		this.client = sfClient;
		this.serialWriteBuffer = sfClient.getSerialWriteBuffer();

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Runnable#run()
	 */
	@Override
	public void run() {
		boolean status = false;
		while (true) {
			Packet packet = client.receivePacket();
			// logger.info("Packet recevived : " + (packet != null));
			if (packet == null) {
				break;
			} else {
				status = serialWriteBuffer.addPacket(packet);
				// logger.info("Packet added");
				if (!status) {
					logger.warn("Packet dropped");
				}
			}
		}
		// if we get here, this was an error occurred.
		client.disconnect();

	}
}

/**
 * Runnable for the writer thread which send packets to the connected clients.
 * 
 * @author kasun
 * 
 */
class WriterThreadRunnable implements Runnable {

	private static Logger logger = Logger.getLogger(WriterThreadRunnable.class);
	private SFClient client = null;
	private PacketBuffer writeQueue = null;

	public WriterThreadRunnable(SFClient sfClient) {
		this.client = sfClient;
		this.writeQueue = sfClient.getWriteQueue();

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Runnable#run()
	 */
	@Override
	public void run() {
		boolean status = false;
		while (true) {
			Packet packet = writeQueue.removePacket();
			if (packet == null) {
				break;
			} else {
				status = client.sendPacket(packet);
				if (!status) {
					break;
				}
			}
		}
		// if we get here, this was an error occurred.
		client.disconnect();

	}

}
