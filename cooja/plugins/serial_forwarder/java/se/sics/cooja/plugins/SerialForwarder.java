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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Collection;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import org.apache.log4j.Logger;
import org.jdom.Element;

import se.sics.cooja.ClassDescription;
import se.sics.cooja.GUI;
import se.sics.cooja.Mote;
import se.sics.cooja.MotePlugin;
import se.sics.cooja.PluginType;
import se.sics.cooja.Simulation;
import se.sics.cooja.VisPlugin;
import se.sics.cooja.interfaces.SerialPort;

@ClassDescription("Serial Forwarder")
@PluginType(PluginType.MOTE_PLUGIN)
public class SerialForwarder extends VisPlugin implements MotePlugin {
	private static final long serialVersionUID = 1L;
	private static Logger logger = Logger.getLogger(SerialForwarder.class);

	private final static int LABEL_WIDTH = 170;
	private final static int LABEL_HEIGHT = 20;

	private final static int PORT_START = 25600;

	private Mote mote;
	private SerialPort serialPort = null;
	private SFController sfController = null;
	private int tcpPort = PORT_START;

	private JTextField txtPort = null;
	private JLabel lblRunning = null;
	private JButton btnStartStop = null;

	private boolean isSFServerRunnig = false;

	public SerialForwarder(Mote mote, Simulation simulation, final GUI gui) {
		super("Serial Forwarder (" + mote + ")", gui, false);
		this.mote = mote;
		tcpPort = PORT_START + mote.getID();

		/* Mote serial port */
		serialPort = (SerialPort) mote.getInterfaces().getLog();
		if (serialPort == null) {
			throw new RuntimeException("No mote serial port");
		}

		/* GUI components */
		if (GUI.isVisualized()) {
			JPanel mainPane = new JPanel();
			JPanel smallPane = null;
			JLabel label = null;
			mainPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
			mainPane.setLayout(new BoxLayout(mainPane, BoxLayout.Y_AXIS));

			smallPane = new JPanel(new BorderLayout());
			label = new JLabel("Port");
			label.setPreferredSize(new Dimension(LABEL_WIDTH, LABEL_HEIGHT));
			smallPane.add(BorderLayout.WEST, label);
			txtPort = new JTextField(8);
			txtPort.setText(String.valueOf(tcpPort));
			smallPane.add(BorderLayout.EAST, txtPort);
			mainPane.add(smallPane);

			mainPane.add(Box.createRigidArea(new Dimension(1, 5)));

			smallPane = new JPanel(new BorderLayout());
			label = new JLabel("Status");
			label.setPreferredSize(new Dimension(LABEL_WIDTH, LABEL_HEIGHT));
			smallPane.add(BorderLayout.WEST, label);
			lblRunning = new JLabel();
			smallPane.add(BorderLayout.EAST, lblRunning);
			mainPane.add(smallPane);

			mainPane.add(Box.createRigidArea(new Dimension(1, 5)));

			smallPane = new JPanel(new BorderLayout());
			btnStartStop = new JButton();
			btnStartStop.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent arg0) {

					if (sfController != null) {
						sfController.stopServers();
						setStatus(false);
						sfController = null;
					} else {
						try {
							tcpPort = Integer.parseInt(txtPort.getText());
						} catch (NumberFormatException e) {

						}
						sfController = new SFController(serialPort, tcpPort);
						if (!sfController.startServers()) {
							setStatus(false);
							sfController = null;
							return;
						}
						setStatus(true);
					}

					/*
					 * if(sfServer != null) { sfServer.stopServer();
					 * setStatus(false); sfServer = null; } else { try { sfPort
					 * = Integer.parseInt(txtPort.getText()); }
					 * catch(NumberFormatException e) {
					 * 
					 * } sfServer = new
					 * SFServer(SerialForwarder.this.serialPort);
					 * if(!sfServer.startServer(sfPort)) { setStatus(false);
					 * sfServer = null; return; } setStatus(true);
					 * 
					 * }
					 */
				}
			});
			smallPane.add(BorderLayout.WEST, btnStartStop);
			mainPane.add(smallPane);
			setStatus(false);

			getContentPane().add(BorderLayout.NORTH, mainPane);

			this.setMinimumSize(new Dimension(280, 150));
			this.setMaximizable(false);
			this.setResizable(false);

			pack();
			// setSize(getWidth()+10, getHeight()+10);
		}

	}

	private void setStatus(boolean isRunning) {
		if (GUI.isVisualized()) {
			if (isRunning) {
				lblRunning.setForeground(Color.blue);
				lblRunning.setText("Running");
				btnStartStop.setText("Stop");
				txtPort.setEnabled(false);
			} else {
				lblRunning.setForeground(Color.black);
				lblRunning.setText("Not Running");
				btnStartStop.setText("Start");
				txtPort.setEnabled(true);
			}
		}
	}

	@Override
	public boolean setConfigXML(Collection<Element> configXML,
			boolean visAvailable) {

		boolean shouldStart = false;

		for (Element element : configXML) {
			if (element.getName().equals("serverPort")) {
				try {
					tcpPort = Integer.parseInt(element.getText());
				} catch (NumberFormatException e) {
					tcpPort = PORT_START + mote.getID();
				}
			}
			if (element.getName().equals("isRunning")) {
				shouldStart = Boolean.valueOf(element.getText());
			}
		}

		if (shouldStart) {
			sfController = new SFController(serialPort, tcpPort);
			if (sfController.startServers()) {
				setStatus(true);
			} else {
				logger.warn("Can not start Servers");
				setStatus(false);
				sfController = null;
			}
			/*
			 * sfServer = new SFServer(SerialForwarder.this.serialPort); if
			 * (sfServer.startServer(sfPort)) { setStatus(true); } else {
			 * logger.warn("Can not start Serial Server"); setStatus(false);
			 * sfServer = null; }
			 */
		}
		return true;
	}

	@Override
	public Collection<Element> getConfigXML() {
		Vector<Element> config = new Vector<Element>();

		Element element;

		element = new Element("serverPort");
		element.setText(String.valueOf(this.tcpPort));
		config.add(element);

		element = new Element("isRunning");

		if (sfController == null) {
			element.setText(String.valueOf(false));
		} else {
			element.setText(String.valueOf(true));
		}
		config.add(element);

		return config;
	}

	@Override
	public void closePlugin() {

		if (sfController != null) {
			sfController.stopServers();
		}

	}
	
	@Override
	public Mote getMote() {
		return mote;
	}

}
