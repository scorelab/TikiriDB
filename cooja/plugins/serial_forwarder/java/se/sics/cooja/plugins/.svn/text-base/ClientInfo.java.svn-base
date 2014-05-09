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

import java.util.Vector;

public class ClientInfo {
	private Vector<SFClient> clients = null;
	private int readClientCount;
	private int writeClientCount;

	public ClientInfo() {
		clients = new Vector<SFClient>();
		readClientCount = 0;
		writeClientCount = 0;
	}

	/**
	 * Return the number of connected clients for reading
	 * @return
	 */
	public synchronized int getReadClientCount() {
		return readClientCount;
	}

	/**
	 * Return the number of connected clients for writting
	 * @return
	 */
	public synchronized int getWriteClientCount() {
		return writeClientCount;
	}

	public synchronized Vector<SFClient> getClients() {
		return clients;
	}

	/**
	 * Remove a client from the client list.
	 * 
	 * @param client
	 *            the client to be removed.
	 * @return true if the removal is successful. Otherwise returns false
	 */
	public synchronized boolean removeClient(SFClient client) {
		SFCleintMode mode = client.getMode();
		boolean isRemoved = clients.remove(client);
		if (isRemoved) {
			if (mode == SFCleintMode.CLIENT_MODE_R
					|| mode == SFCleintMode.CLIENT_MODE_RW) {
				readClientCount--;
			}
			if (mode == SFCleintMode.CLIENT_MODE_W
					|| mode == SFCleintMode.CLIENT_MODE_RW) {
				writeClientCount--;
			}
			return true;
		}
		return false;
	}

	/**
	 * Add a client to the clients list.
	 * 
	 * @param client
	 *            the client to be added.
	 */
	public synchronized boolean addClient(SFClient client) {
		SFCleintMode mode = client.getMode();
		boolean isAdded = clients.add(client);

		if (isAdded) {
			if (mode == SFCleintMode.CLIENT_MODE_R
					|| mode == SFCleintMode.CLIENT_MODE_RW) {
				readClientCount++;
			}
			if (mode == SFCleintMode.CLIENT_MODE_W
					|| mode == SFCleintMode.CLIENT_MODE_RW) {
				writeClientCount++;
			}
			return true;
		}

		return false;
	}

}
