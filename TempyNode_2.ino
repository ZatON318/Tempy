#include <ETH.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#include <DallasTemperature.h>
#include <SNMP_Agent.h>

char output_temp[10];  
char* outputchar = "0.00";

SNMPAgent snmp;
WiFiUDP udp;
//SNMPTrap* testTrap = new SNMPTrap("public", SNMP_VERSION_2C);

#define ONE_WIRE_BUS 4 // definuje pin na ktorom je pripojeny teplotny senzor
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

IPAddress staticIP(192, 168, 0, 20);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
String hostname;
String snmp_comm;
bool dhcp_on = true;

WebServer server(80);
Preferences preferences;

String loginUsername;
String loginPassword;
bool isAuthenticated = false;

void handleRoot() {
  if (server.method() == HTTP_POST) {
    String username = server.arg("username");
    String password = server.arg("password");

    if (username.equals(loginUsername) && password.equals(loginPassword)) {
      isAuthenticated = true;
      server.sendHeader("Location", "/config");
      server.send(302);
      return;
    }
  }

  String html = "<center><h1>Login</h1>";
  html += "<form method=\"post\">";
  html += "<label for=\"username\">Username:</label>";
  html += "<input type=\"text\" name=\"username\"><br>";
  html += "<label for=\"password\">Password:</label>";
  html += "<input type=\"password\" name=\"password\"><br>";
  html += "<input type=\"submit\" value=\"Login\">";
  html += "</form></center>";
  server.send(200, "text/html", html);
}

void handleConfig() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/");
    server.send(302);
    return;
  }

  if (server.method() == HTTP_GET) {
    String html = "<h1>TempyNode - firmware-1.2_220623</h1>";
    html += "<h2>Network Configuration</h2>";
    html += "<form method=\"post\" action=\"/config\">";

    html += "<label for=\"gateway\">Hostname:</label>";
    html += "<input type=\"text\" name=\"hostname\" value=\"" + hostname + "\"><br>";
    html += "<label for=\"dhcp\">Enable DHCP:</label>";
    html += "<input type=\"checkbox\" name=\"dhcp\"";
    if (dhcp_on) {
      html += " checked";
    }
    html += "><br>";
    html += "<label for=\"static_ip\">Static IP:</label>";
    html += "<input type=\"text\" name=\"static_ip\" value=\"" + staticIP.toString() + "\"><br>";
    html += "<label for=\"subnet_mask\">Subnet Mask:</label>";
    html += "<input type=\"text\" name=\"subnet_mask\" value=\"" + subnet.toString() + "\"><br>";
    html += "<label for=\"gateway\">Gateway:</label>";
    html += "<input type=\"text\" name=\"gateway\" value=\"" + gateway.toString() + "\"><br><br>";
    html += "<label for=\"gateway\">SNMP Community:</label>";
    html += "<input type=\"text\" name=\"snmp_comm\" value=\"" + snmp_comm + "\"><br>";
    html += "<h2>Network Configuration</h2><br>";
    html += "<label for=\"gateway\">Meno:</label>";
    html += "<input type=\"text\" name=\"username\" value=\"" + loginUsername + "\"><br><br>";
    html += "<label for=\"gateway\">Heslo:</label>";
    html += "<input type=\"password\" name=\"passwd\" value=\"" + loginPassword + "\"><br>";
    html += "<input type=\"submit\" value=\"Submit\">";
    html += "</form>";

    server.send(200, "text/html", html);
  } else if (server.method() == HTTP_POST) {
    if (server.hasArg("static_ip") && server.hasArg("subnet_mask") && server.hasArg("gateway")) {
      IPAddress newStaticIP;
      IPAddress newSubnet;
      IPAddress newGateway;

      Serial.println(server.arg("snmp_comm"));

      if (server.arg("dhcp") == "on") {
        dhcp_on = true;
      } else {
        dhcp_on = false;
      }

      if (newStaticIP.fromString(server.arg("static_ip")) &&
          newSubnet.fromString(server.arg("subnet_mask")) &&
          newGateway.fromString(server.arg("gateway"))) {
        
        staticIP = newStaticIP;
        subnet = newSubnet;
        gateway = newGateway;

        // Store configuration in flash memory
        preferences.begin("network_config", false);
        preferences.putString("static_ip", staticIP.toString());
        preferences.putString("subnet_mask", subnet.toString());
        preferences.putString("gateway", gateway.toString());
        preferences.putBool("dhcp_on", dhcp_on);
        preferences.putString("hostname", server.arg("hostname"));
        preferences.putString("snmp_comm", server.arg("snmp_comm"));
        preferences.putString("username", server.arg("username"));
        preferences.putString("passwd", server.arg("passwd"));
        preferences.end();

        String html = "<p>Configuration updated. Restarting...</p>";
        html += "<a href=\"/\">BACK</a>";

        server.send(200, "text/html", html);
        delay(3000);
        ESP.restart();
      } else {
        server.send(400, "text/html", "<p>Invalid IP address or subnet mask format.</p>");
      }
    }
  }
}

void WiFiEvent(WiFiEvent_t event) {
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
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
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
  
  
  //snmp.addIntegerHandler(".1.3.6.1.4.1.12345.0", &testint);
  //snmp.addIntegerHandler(".1.3.6.1.2.1.1.1.0", &test, false);
  snmp.addReadWriteStringHandler(".1.3.6.1.4.1.12345.1", &outputchar, false);
}

void setup() {
  Serial.begin(115200);
  pinMode(34, INPUT);

  if(digitalRead(34) == 0){
    preferences.begin("network_config", false);
    preferences.putString("static_ip", "192.168.0.20");
    preferences.putString("subnet_mask", "255.255.255.0");
    preferences.putString("gateway", "192.168.0.1");
    preferences.putBool("dhcp_on", false);
    preferences.putString("hostname", "TempyNode");
    preferences.putString("snmp_comm", "private");
    preferences.putString("username", "admin");
    preferences.putString("passwd", "pass");
    preferences.end();
  }

  WiFi.onEvent(WiFiEvent);
  ETH.begin();

  preferences.begin("network_config", true);
  staticIP.fromString(preferences.getString("static_ip", "192.168.0.20"));
  subnet.fromString(preferences.getString("subnet_mask", "255.255.255.0"));
  gateway.fromString(preferences.getString("gateway", "192.168.0.1"));
  dhcp_on = preferences.getBool("dhcp_on", false);
  hostname = preferences.getString("hostname", "TempyNode");
  snmp_comm = preferences.getString("snmp_comm", "private");
  loginUsername = preferences.getString("username", "admin");
  loginPassword = preferences.getString("passwd", "pass");
  preferences.end();

  ETH.setHostname(hostname.c_str());
  Serial.println(hostname);

  if (!dhcp_on) {
    ETH.config(staticIP, gateway, subnet);
  }

  Serial.println(ETH.localIP());
  Serial.println("snmp community: " + snmp_comm);
  snmp = SNMPAgent(snmp_comm.c_str());
  setupSNMP();
  sensors.begin();

  server.on("/", handleRoot);
  server.on("/config", HTTP_GET, []() {
    if (isAuthenticated) {
        handleConfig();
    } else {
        server.sendHeader("Location", "/");
        server.send(302);
    }
});
server.on("/config", HTTP_POST, handleConfig);

  server.begin();

  Serial.println("Server started");
}

void loop() {
  server.handleClient();
  snmp.loop(); 
  getTemp();
  delay(20);
}

float getTemp(){ 
  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  dtostrf(temp, 5, 2, output_temp);
  outputchar = output_temp;
  return 0;
  }
