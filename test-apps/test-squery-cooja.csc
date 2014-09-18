<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project>[CONTIKI_DIR]/tools/cooja/apps/mrm</project>
  <project>[CONTIKI_DIR]/tools/cooja/apps/mspsim</project>
  <project>[CONTIKI_DIR]/tools/cooja/apps/avrora</project>
  <project>[CONTIKI_DIR]/tools/cooja/apps/serial_forwarder</project>
  <simulation>
    <title>Quickstarted simulation: test-squery.c</title>
    <delaytime>0</delaytime>
    <randomseed>123456</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      se.sics.cooja.radiomediums.UDGM
      <transmitting_range>60.0</transmitting_range>
      <interference_range>100.0</interference_range>
      <success_ratio_tx>1.0</success_ratio_tx>
      <success_ratio_rx>1.0</success_ratio_rx>
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      se.sics.cooja.contikimote.ContikiMoteType
      <identifier>mtype151</identifier>
      <description>Contiki Mote Type (test-squery.c)</description>
      <contikiapp>[CONFIG_DIR]/test-squery.c</contikiapp>
      <commands>make test-squery.cooja TARGET=cooja</commands>
      <moteinterface>se.sics.cooja.interfaces.Position</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.Battery</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiVib</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiMoteID</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiRS232</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiBeeper</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiIPAddress</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiRadio</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiButton</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiPIR</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiClock</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiLED</moteinterface>
      <moteinterface>se.sics.cooja.contikimote.interfaces.ContikiCFS</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <symbols>false</symbols>
      <commstack>Rime</commstack>
    </motetype>
    <mote>
      se.sics.cooja.contikimote.ContikiMote
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>-3.641590579280537</x>
        <y>102.66439555551898</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.contikimote.interfaces.ContikiMoteID
        <id>1</id>
      </interface_config>
      <motetype_identifier>mtype151</motetype_identifier>
    </mote>
    <mote>
      se.sics.cooja.contikimote.ContikiMote
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>36.95472599819956</x>
        <y>78.0290239572191</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.contikimote.interfaces.ContikiMoteID
        <id>2</id>
      </interface_config>
      <motetype_identifier>mtype151</motetype_identifier>
    </mote>
    <mote>
      se.sics.cooja.contikimote.ContikiMote
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>20.299826889489772</x>
        <y>65.19087256092196</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.contikimote.interfaces.ContikiMoteID
        <id>3</id>
      </interface_config>
      <motetype_identifier>mtype151</motetype_identifier>
    </mote>
    <mote>
      se.sics.cooja.contikimote.ContikiMote
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>-20.580380013706954</x>
        <y>56.992054409561355</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.contikimote.interfaces.ContikiMoteID
        <id>4</id>
      </interface_config>
      <motetype_identifier>mtype151</motetype_identifier>
    </mote>
    <mote>
      se.sics.cooja.contikimote.ContikiMote
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>24.810528731432004</x>
        <y>117.9313864051696</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.contikimote.interfaces.ContikiMoteID
        <id>5</id>
      </interface_config>
      <motetype_identifier>mtype151</motetype_identifier>
    </mote>
    <mote>
      se.sics.cooja.contikimote.ContikiMote
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>-6.0704300326340475</x>
        <y>74.906230374336</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.contikimote.interfaces.ContikiMoteID
        <id>6</id>
      </interface_config>
      <motetype_identifier>mtype151</motetype_identifier>
    </mote>
  </simulation>
  <plugin>
    se.sics.cooja.plugins.SimControl
    <width>290</width>
    <z>0</z>
    <height>172</height>
    <location_x>395</location_x>
    <location_y>214</location_y>
    <minimized>false</minimized>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.Visualizer
    <plugin_config>
      <skin>se.sics.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.TrafficVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.LEDVisualizerSkin</skin>
      <skin>se.sics.cooja.plugins.skins.UDGMVisualizerSkin</skin>
      <viewport>2.8820348707425136 0.0 0.0 2.8820348707425136 105.49519103445394 -119.88236797470843</viewport>
    </plugin_config>
    <width>300</width>
    <z>2</z>
    <height>300</height>
    <location_x>823</location_x>
    <location_y>85</location_y>
    <minimized>false</minimized>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.LogListener
    <plugin_config>
      <filter />
    </plugin_config>
    <width>1116</width>
    <z>3</z>
    <height>212</height>
    <location_x>11</location_x>
    <location_y>398</location_y>
    <minimized>false</minimized>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.SerialForwarder
    <mote_arg>0</mote_arg>
    <plugin_config>
      <serverPort>25601</serverPort>
      <isRunning>false</isRunning>
    </plugin_config>
    <width>292</width>
    <z>1</z>
    <height>127</height>
    <location_x>13</location_x>
    <location_y>259</location_y>
    <minimized>false</minimized>
  </plugin>
</simconf>
