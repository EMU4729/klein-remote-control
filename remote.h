#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

String html = "<html><head><title>Klein</title><style>body{overflow-y:hidden;font-size:70px;font-family:'Arial';padding-top:35px;text-align:center;}#base,#stick{position:absolute;border-radius:50%;opacity:0;transition:opacity 200ms;}#base{width:400px;height:400px;background:#e6e6e6;}#stick{width:200px;height:200px;background:#a6a6a6;}</style></head><body>Press to create joystick<div id='base'></div><div id='stick'></div><script>var jx = 0;var jy = 0;var state = 0;var timeout = null;function sendData() {var http = new XMLHttpRequest();http.onload = function() {var newState = Number(http.responseText);if (newState != state && newState == 1) {navigator.vibrate(100);};state = newState;clearTimeout(timeout);sendData();};http.open('GET', '/data/' + jx.toFixed(2) + '/' + ((-jy).toFixed(2)), true);http.send();timeout = setTimeout(sendData, 1000);};var base = document.getElementById('base');var stick = document.getElementById('stick');var startX = 0;var startY = 0;window.addEventListener('touchstart', function(event) {event.preventDefault();event.stopImmediatePropagation();var touch = event.touches[0];var x = touch.clientX;var y = touch.clientY;base.style.left = x - base.offsetWidth / 2;base.style.top = y - base.offsetHeight / 2;stick.style.left = x - stick.offsetWidth / 2;stick.style.top = y - stick.offsetHeight / 2;base.style.opacity = 1;stick.style.opacity = 1;startX = x;startY = y;jx = 0;jy = 0;return false;}, {passive: false});;window.addEventListener('touchmove', function(event) {event.preventDefault();event.stopImmediatePropagation();var touch = event.touches[0];var x = touch.clientX;var y = touch.clientY;var angle = afp(startX, startY, x, y);var distance = dfp(startX, startY, x, y);if (distance > base.offsetWidth / 2) distance = base.offsetWidth / 2;var point = pfa(startX, startY, angle, distance);stick.style.left = point.x - stick.offsetWidth / 2;stick.style.top = point.y - stick.offsetHeight / 2;jx = (point.x - startX) / (base.offsetWidth / 2);jy = (point.y - startY) / (base.offsetHeight / 2);return false;}, {passive: false});function end(event) {base.style.opacity = 0;stick.style.opacity = 0;jx = 0;jy = 0;};window.ontouchend = end;window.ontouchcancel = end;sendData();function afp(x1, y1, x2, y2) {return Math.atan2(y2 - y1, x2 - x1);};function pfa(x, y, a, d) {return {x: Math.cos(a) * d + x, y: Math.sin(a) * d + y};};function dfp(x1, y1, x2, y2) {return Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));};</script></body></html>";

float joystickX = 0;
float joystickY = 0;
int remoteState = 0;

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleData() { 
  String uri = server.uri();
  String xString;
  String yString;
  int i = 6;
  char c = uri.charAt(i);
  while (c != '/' && i < uri.length()) {
    xString += c;
    i++;
    c = uri.charAt(i);
  }
  i++;
  c = uri.charAt(i);
  while (c != '/' && i < uri.length()) {
    yString += c;
    i++;
    c = uri.charAt(i);
  }
  joystickX = xString.toFloat();
  joystickY = yString.toFloat();
  server.send(200, "text/plain", String(remoteState));
}

void remoteSetup(char *ssid, char *password) {
  delay(1000);
  Serial.print("Creating Wi-Fi network... ");
  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  Serial.println("done\n");
  Serial.print("1. Connect to Wi-Fi network \"");
  Serial.print(ssid);
  Serial.println("\"");
  Serial.print("2. Enter password \"");
  Serial.print(password);
  Serial.println("\"");
  Serial.print("3. Open browser and go to \"");
  Serial.print(ip);
  Serial.println("\"\n");
  Serial.println("Note: make sure that \"Erase Flash:\nSketch + WiFi Settings\" is selected\nunder the \"Tools\" menu");
  server.on("/", handleRoot);
  server.onNotFound(handleData);
  server.begin();
}

void remoteUpdate(int state) {
  remoteState = state;
  server.handleClient();
}
