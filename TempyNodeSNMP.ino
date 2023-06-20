// Arduino IDE kniznice
#include <ETH.h>
#include <WiFiUdp.h>
#include <DallasTemperature.h>
#include <SNMP_Agent.h>

SNMPAgent snmp = SNMPAgent("private");
WiFiUDP udp;
SNMPTrap* testTrap = new SNMPTrap("public", SNMP_VERSION_2C);

TimestampCallback* timestampCallback;

  int testint = 10;
  int test = 10;

  char output_temp[10];  
  char* outputchar = "0.00";



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
  
  
  snmp.addIntegerHandler(".1.3.6.1.4.1.12345.0", &testint);
  snmp.addIntegerHandler(".1.3.6.1.2.1.1.1.0", &test, false);
  snmp.addReadWriteStringHandler(".1.3.6.1.4.1.12345.1", &outputchar, false);
}

void setup()
{
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  ETH.setHostname("TempyNode_v1.1"); // hostname pre tempynode
  Serial.println(ETH.localIP());
  setupSNMP();

  sensors.begin(); 
}


void loop()
{
  snmp.loop(); 
  getTemp();
  delay(2);
}

float getTemp(){ 
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  dtostrf(temp, 5, 2, tempchar);
  outputchar = output_temp;
  return 0;
  }
