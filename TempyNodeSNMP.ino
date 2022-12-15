// Arduino IDE kniznice
#include <ETH.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <SNMP_Agent.h>

SNMPAgent snmp = SNMPAgent("public");
WiFiUDP udp;
SNMPTrap* testTrap = new SNMPTrap("public", SNMP_VERSION_2C);

TimestampCallback* timestampCallback;
int tensOfMillisCounter = 0;


#define ONE_WIRE_BUS 4 // definuje pin na ktorom je pripojeny teplotny senzor
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

WiFiClient ethClient;

static bool eth_connected = false;

void WiFiEvent(WiFiEvent_t event)// funkcia ktora sa stara o Ethernetove pripojenie
{
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      ETH.setHostname("TempyNode_v1.1");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}


void verifySNMP()
{
  snmp.loop();

  if(snmp.setOccurred)
  {
    snmp.resetSetOccurred();
  }
}

void setupSNMP()
{
  snmp.setUDP(&udp);
  snmp.begin();

  int testint = 5;
  
  //Adiciona o OID para umidade (apenas leitura)
  snmp.addIntegerHandler(".1.3.6.1.4.1.12345.0", &testint, false);
  //Adiciona o OID para temperatura (apenas leitura)
  //snmp.addStringHandler(".1.3.6.1.4.1.12345.1", &strTemperature, false);
  //Adiciona o OID para umidade máxima (leitura e escrita)
  //snmp.addIntegerHandler(".1.3.6.1.4.1.12345.2", &maxHumidity, true);
  //Adiciona o OID para umidade mínima (leitura e escrita)
  //snmp.addIntegerHandler(".1.3.6.1.4.1.12345.3", &minHumidity, true);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("BOOTING TempyNode_v1.1");
  Serial.println("I turned myself into pickle Morty , I AM PICKLE RICK !!!");
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  ETH.setHostname("TempyNode_v1.1");
  setupSNMP();

  sensors.begin(); 
}


void loop()
{
  snmp.loop(); 

  // Update our timestamp value
  tensOfMillisCounter = millis()/10;
  
  // Send the trap to the specified IP address
  
  IPAddress destinationIP = IPAddress(10, 65, 1, 86);
  
  if(snmp.sendTrapTo(testTrap, destinationIP, true, 2, 5000) != INVALID_SNMP_REQUEST_ID){ 
      Serial.println("Sent SNMP Trap");
  } else {
      Serial.println("Couldn't send SNMP Trap");
      Serial.println(ETH.localIP());
  }
  
  //char tempString[8];
  //dtostrf(getTemp(), 1, 2, tempString);
  
  //delay(5000);
}

float getTemp(){
   Serial.print(" \nTemp req..."); 
   sensors.requestTemperatures();
   Serial.print("Teplota je: "); 
   Serial.print(sensors.getTempCByIndex(0));
  
   return(sensors.getTempCByIndex(0));
  }
