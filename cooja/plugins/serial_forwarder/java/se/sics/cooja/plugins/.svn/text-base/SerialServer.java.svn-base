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

import java.util.Observable;
import java.util.Observer;

import org.apache.log4j.Logger;

import se.sics.cooja.interfaces.SerialPort;

public class SerialServer extends Thread {

	private static final long serialVersionUID = 1L;
	private static Logger logger = Logger.getLogger(SerialServer.class);

	public enum SlipState {
		STATE_OK, STATE_ESC, STATE_RUBBISH
	}

	private final static byte SLIP_END = (byte) 0300;
	private final static byte SLIP_ESC = (byte) 0333;
	private final static byte SLIP_ESC_END = (byte) 0334;
	private final static byte SLIP_ESC_ESC = (byte) 0335;

	private SlipState readSlipState = SlipState.STATE_OK;
	private int readSlipLength = 0;
	private final int READ_SLIP_BUFFER_SIZE = 2048;
	private byte[] readSlipBuffer = new byte[READ_SLIP_BUFFER_SIZE];

	private SerialPort serialPort = null;
	private PacketBuffer serialWriteBuffer = null;
	private SFController sfServer = null;

	public SerialServer(SFController sfServer) {
		this.setName("SerialServer_Thread");
		this.sfServer = sfServer;
		this.serialPort = sfServer.getSerialPort();
		this.serialWriteBuffer = sfServer.getSerialWriteBuffer();

		this.serialPort.addSerialDataObserver(new Observer() {

			@Override
			public void update(Observable arg0, Object arg1) {
				readSlipAccumulated(serialPort.getLastSerialData());
			}
		});

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Thread#run()
	 */
	@Override
	public void run() {
		logger.info("Serial Server started");
		while (true) {
			Packet packet = serialWriteBuffer.removePacket();
			if (packet == null) {
				break;
			} else {
				writePacketAsSlip(packet);
				//logger.info("Packet written");
			}
		}
		logger.info("Serial Server stopped.");
	}

	/**
	 * Start the serial server.
	 */
	public void startServer() {
		this.start();
	}

	/**
	 * Stop the serial server.
	 */
	public void stopServer() {
		if (!this.isInterrupted()) {
			this.interrupt();
		}

		try {
			this.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Write a packet to the serial port through SLIP.
	 * 
	 * @param packet
	 *            the packet to be written.
	 */
	private void writePacketAsSlip(Packet packet) {
		serialPort.writeByte(SLIP_END);

		for (byte b : packet.getData()) {
			if (b == SLIP_END) {
				serialPort.writeByte(SLIP_ESC);
				serialPort.writeByte(SLIP_ESC_END);
			} else if (b == SLIP_ESC) {
				serialPort.writeByte(SLIP_ESC);
				serialPort.writeByte(SLIP_ESC_ESC);
			} else {
				serialPort.writeByte(b);
			}
		}

		serialPort.writeByte(SLIP_END);
	}

	/**
	 * @param b
	 * @return
	 */
	private boolean readSlipAccumulated(byte b) {
		switch (readSlipState) {

		case STATE_RUBBISH:
			readSlipLength = 0;
			if (b == SLIP_END) {
				readSlipState = SlipState.STATE_OK;
			}
			return false;

		case STATE_ESC:
			if (b == SLIP_ESC_END) {
				b = SLIP_END;
			} else if (b == SLIP_ESC_ESC) {
				b = SLIP_ESC;
			} else {
				readSlipState = SlipState.STATE_RUBBISH;
				readSlipLength = 0;
				return false;
			}
			readSlipState = SlipState.STATE_OK;
			break;

		case STATE_OK:
			if (b == SLIP_ESC) {
				readSlipState = SlipState.STATE_ESC;
				return false;
			} else if (b == SLIP_END) {
				/* Handle outgoing packet */
				
				if (sfServer != null) {
					if (readSlipLength > 0) {
						byte[] buffer = null;
						Packet packet = new Packet();
						if(readSlipBuffer[0] == (byte)'\r') {
							buffer = new byte[readSlipLength - 1];
							System.arraycopy(readSlipBuffer, 1, buffer, 0,
									readSlipLength - 1);
							packet.setData(buffer, PacketType.PKT_TYPE_DEBUG);
						} else {
							buffer = new byte[readSlipLength];
							System.arraycopy(readSlipBuffer, 0, buffer, 0,
									readSlipLength);
							packet.setData(buffer, PacketType.PKT_TYPE_DATA);
						}
						sfServer.sendPacketToAll(packet);
					}
				}
				readSlipLength = 0;
				return true;
			}

		}

		/* Receive byte */
		readSlipBuffer[readSlipLength] = b;
		readSlipLength++;
		if (readSlipLength >= READ_SLIP_BUFFER_SIZE) {
			readSlipState = SlipState.STATE_RUBBISH;
			readSlipLength = 0;
		}

		return false;
	}

}
