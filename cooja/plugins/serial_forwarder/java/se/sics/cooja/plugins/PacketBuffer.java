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

import java.util.LinkedList;

public class PacketBuffer {
	
	private int maxPackets;
	private LinkedList<Packet> packets;

	/**
	 * @param maxPackets
	 *            maximum number of packets in the buffer
	 */
	public PacketBuffer(int maxPackets) {
		this.maxPackets = maxPackets;
		packets = new LinkedList<Packet>();
	}

	/**
	 * Adds a packet to the packet buffer. The method returns false if the
	 * adding is failed.(i.e: maximum capacity of packet buffer is reached.)
	 * 
	 * @param packet
	 *            The packet to be added to the buffer
	 * @return true if the adding is successful. otherwise false.
	 */
	public boolean addPacket(Packet packet) {
		synchronized (packets) {
			if (packets.size() >= maxPackets) {
				return false;
			} else {
				packets.add(packet);
				packets.notifyAll();
			}
		}

		return true;
	}

	/**
	 * Removes a packet from the packet buffer. If no packets are in the buffer,
	 * this waits until a packet arrives.
	 * 
	 * @return The packet removed form the packet buffer. returns NULL if error
	 *         occurred.
	 */
	public Packet removePacket() {
		synchronized (packets) {
			if (packets.size() == 0) {
				try {
					packets.wait();
				} catch (InterruptedException e) {
					// e.printStackTrace();
					return null;
				}
			}
			return packets.removeLast();
		}
	}

	/**
	 * Returns the number of the packets in the packet buffer.
	 * 
	 * @return the number of the packets in the buffer
	 */
	public synchronized int getPacketCount() {
		return this.packets.size();
	}

}
