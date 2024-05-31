#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}

// Ziel-MAC-Adresse, die wir überwachen möchten
uint8_t target_mac[6] = {0x00, 0x22, 0xaa, 0x0b, 0xcb, 0x95};

// Prototypen der verwendeten Funktionen
void promiscuous_cb(uint8_t *buf, uint16_t len);
bool is_target_mac(uint8_t *mac);

// Struktur für RxControl entsprechend der ESP8266 SDK-Dokumentation
struct RxControl {
  signed rssi:8;
  unsigned rate:4;
  unsigned is_group:1;
  unsigned:1;
  unsigned sig_mode:2;
  unsigned legacy_length:12;
  unsigned damatch0:1;
  unsigned damatch1:1;
  unsigned bssidmatch0:1;
  unsigned bssidmatch1:1;
  unsigned MCS:7;
  unsigned CWB:1;
  unsigned HT_length:16;
  unsigned Smoothing:1;
  unsigned Not_Sounding:1;
  unsigned:1;
  unsigned Aggregation:1;
  unsigned STBC:2;
  unsigned FEC_CODING:1;
  unsigned SGI:1;
  unsigned rxend_state:8;
  unsigned ampdu_cnt:8;
  unsigned channel:4;
  unsigned:12;
};

// Struktur für 802.11-Header entsprechend der ESP8266 SDK-Dokumentation
struct ieee80211_hdr {
  uint16_t frame_control;
  uint16_t duration_id;
  uint8_t addr1[6];
  uint8_t addr2[6];
  uint8_t addr3[6];
  uint16_t seq_ctrl;
  uint8_t addr4[6];
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  wifi_set_opmode(STATION_MODE);
  wifi_set_promiscuous_rx_cb(promiscuous_cb);
  wifi_promiscuous_enable(1);
}

void loop() {
  // Hauptcode, hier wird nichts benötigt, da der Promiscuous-Callback die Pakete direkt verarbeitet und ausgibt
}

void promiscuous_cb(uint8_t *buf, uint16_t len) {
  struct RxControl *rx = (struct RxControl *)buf;
  struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)(buf + sizeof(struct RxControl));

  // Überprüfen, ob das Paket von der Ziel-MAC-Adresse stammt
  if (is_target_mac(hdr->addr2)) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Packet from target MAC: ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", hdr->addr2[i]);
      if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Das gesamte Paket ausgeben
    Serial.println("Packet content:");
    for (int i = 0; i < len; i++) {
      Serial.printf("%02X ", buf[i]);
      if ((i + 1) % 16 == 0) Serial.println();
    }
    Serial.println();
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

bool is_target_mac(uint8_t *mac) {
  for (int i = 0; i < 6; i++) {
    if (mac[i] != target_mac[i]) return false;
  }
  return true;
}
