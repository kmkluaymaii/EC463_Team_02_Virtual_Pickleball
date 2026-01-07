#pragma once
#include "Config.h"
#include "Audio.h"
#include "Haptics.h"
#include "LEDs.h"

void processUDPCommands() {
  int packetSize = udp.parsePacket();
  if (packetSize <= 0) return;

  char packet[255];
  int len = udp.read(packet, sizeof(packet) - 1);
  if (len <= 0) return;

  packet[len] = '\0';
  Serial.printf("RX len=%d raw='%s'\n", len, packet);

  float x, y, z;
  String msg;

  if (sscanf(packet, "%f,%f,%f", &x, &y, &z) == 3) {
    Serial.printf("📊 DATA: X: %.3f, Y: %.3f, Z: %.3f\n", x, y, z);
    msg = "combo:440:20:0:47:green";
  } else {
    msg = "combo:50:50:0:0:red";
  }

  if (msg.startsWith("combo:")) {
    int c1 = msg.indexOf(':');
    int c2 = msg.indexOf(':', c1 + 1);
    int c3 = msg.indexOf(':', c2 + 1);
    int c4 = msg.indexOf(':', c3 + 1);
    int c5 = msg.indexOf(':', c4 + 1);

    int freq = msg.substring(c1 + 1, c2).toInt();
    int duration = msg.substring(c2 + 1, c3).toInt();
    int channel = msg.substring(c3 + 1, c4).toInt();
    int effect = msg.substring(c4 + 1, c5).toInt();
    String color = msg.substring(c5 + 1);

    tcaSelect(channel);
    drv.setWaveform(0, effect);
    drv.setWaveform(1, 0);
    drv.go();

    playBeep(freq, duration);
    changeLedColor(color);
  }
}