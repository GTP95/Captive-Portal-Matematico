/*
   Based on Captive Portal by: M. Ray Burnette 20150831
   See Notes tab for original code references and compile requirements
*/

//modifica per forzare sync
#include <ESP8266WiFi.h>
#include "./DNSServer.h"                  // Patched lib
#include <ESP8266WebServer.h>
#include "FS.h"

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  webServer(80);          // HTTP server

//Pagina di default contenente il test
String testHTML = ""
                      "<!DOCTYPE html><html lang=\"it\"><head><title>Disfida matematica</title></head><body><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
                      "<h1>1=0!</h1><p>L'utilizzo di questa rete WiFi è concesso solo a chi dimostra di avere almeno una conoscenza basilare della Matematica. "
                      "Di seguito troverete cinque domande per testare le vostre conoscenze, per poter usufruire della connessione dovrete rispondere correttamente ad "
                      "almeno tre domande</p>"
                      "<p>"
                      
                      "<form action=\"calcola_punteggio\">"
    "<fieldset>"
        "<legend><br><br><br><table style=\"border-spacing:0px; border-width:0px; font-family:verdana;\"><tr><td> &nbsp;</td><td>____</td></tr><tr><td style=\"padding:0px; font-size:larger\"> &radic;</td> <td style=\"padding:0px;\"> &nbsp;x<sup>2</sup> &nbsp;</td></tr></table></legend>"
        "|x| <input type=\"radio\" name=\"radiceQuadrata\" value=\"corretto\"/><br>"
        "x  <input type=\"radio\" name=\"radiceQuadrata\" value=\"sbagliato\"/><br>"
        "log<sub>2</sub>(x) <input type=\"radio\" name=\"radiceQuadrata\" value=\"sbagliato\"/>"
    "</fieldset>"



 "<fieldset>"
        "<legend><br><br><br>2<sup>log<sub>2</sub>3</sup></legend>"
        "1 <input type=\"radio\" name=\"logaritmo\" value=\"sbagliato\"/><br>"
        "2  <input type=\"radio\" name=\"logaritmo\" value=\"sbagliato\"/><br>"
        "3 <input type=\"radio\" name=\"logaritmo\" value=\"corretto\"/>"
    "</fieldset>"



 "<fieldset>"
        "<legend><br><br><br>0!</legend>"
        "0 <input type=\"radio\" name=\"fattoriale\" value=\"sbagliato\"/><br>"
        "1  <input type=\"radio\" name=\"fattoriale\" value=\"corretto\"/><br>"
        "2 <input type=\"radio\" name=\"fattoriale\" value=\"sbagliato\"/>"
    "</fieldset>"


 "<fieldset>"
        "<legend><br><br><br>1500<sup>0</sup></legend>"
        "0 <input type=\"radio\" name=\"potenza\" value=\"sbagliato\"/><br>"
        "1  <input type=\"radio\" name=\"potenza\" value=\"corretto\"/><br>"
        "15 <input type=\"radio\" name=\"potenza\" value=\"sbagliato\"/>"
    "</fieldset>"



 "<fieldset>"
        "<legend><br><br><br>La derivata di x<sup>&frac12+1</sup> è</legend>"
        "<table border=\"0\" cellspacing=\"0\" width=\"50\">"
"<tr> <td>"
"<i>3<span style=\"white-space: nowrap; font-size:larger\">"
"&radic;<span style=\"text-decoration:overline;\">&nbsp;X &nbsp;</span>"
"</span></i> <hr align=\"center\"> <i>2</i>"
"</td></tr>"
"</table>  <input type=\"radio\" name=\"derivata\" value=\"corretto\"/><br>"
        "<table border=\"0\" cellspacing=\"0\" width=\"50\">"
"<tr> <td>"
"<i><span style=\"white-space: nowrap; font-size:larger\">"
"&radic;<span style=\"text-decoration:overline;\">&nbsp;X &nbsp;</span>"
"</span></i> <hr align=\"center\"> <i>2</i>"
"</td></tr>"
"</table>  <input type=\"radio\" name=\"derivata\" value=\"sbagliato\"/><br>"
        "<table border=\"0\" cellspacing=\"0\" width=\"50\">"
"<tr> <td>"
"<i>2<span style=\"white-space: nowrap; font-size:larger\">"
"&radic;<span style=\"text-decoration:overline;\">&nbsp;X &nbsp;</span>"
"</span></i> <hr align=\"center\"> <i>3</i>"
"</td></tr>"
"</table> <input type=\"radio\" name=\"derivata\" value=\"sbagliato\"/>"
    "</fieldset>"


"<input type=\"submit\" value=\"Invia risposte\">"
"</form>"

"</p>"
                      "</body></html>";

//Pagina di default per il test superato
String testSuperatoHTML="<!DOCTYPE html><html><head><title>Disfida matematica</title></head><body><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
                      "<h1>Complimenti, hai superato il test!</h1>"
                      "<p>Tuttavia, almeno per il momento, questa rete non è in grado di fornire un servizio di connettività ad Internet. In futuro potrei esplorare la possibilità di un"
                      "tunnel DNS, ma per ora ti dovrai accontentare della gloria e dell'onore di aver superato questo test. Complimenti ancora!</p></body></html>";

//pagina di default per il test fallito
String testFallitoHTML="<!DOCTYPE html><html><head><title>Disfida matematica</title></head><body><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
                      "<h1>Test fallito...</h1>"
                      "<p>Mi dispiace (in realtà no), ma non ti sei dimostrato degno di utilizzare questa rete. Tuttavia questo non significa che tu non lo possa diventare in futuro: "
                      "il mio consiglio è di ripassare un po' di Matematica e ritentare domani.</p></body></html>"; 

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Disfida matematica");

  Serial.begin(115200); //serial debug

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", testHTML);
  });

webServer.on("/", []()
  {
    if(SPIFFS.exists("/index.html")){
     File paginaTest=SPIFFS.open("/index.html","r");
      webServer.streamFile(paginaTest, "text/html");
    }
    else webServer.send(200, "text/html", testHTML);
  });
  
  webServer.on("/calcola_punteggio", calcola_punteggio);  //specifico la funzione da chiamare corrispondente all'azione calcola_punteggio (evento pressione tasto "Invia risultati")
  webServer.on("/punteggi", []()
    {
      if(SPIFFS.exists("/punteggi")){
        File paginaPunteggi=SPIFFS.open("/punteggi", "r");
        webServer.streamFile(paginaPunteggi, "text/plain");
      }
      else webServer.send(404);
    });
  
  yield();
  SPIFFS.begin();   //monto il filesystem
  webServer.begin();  //avvio il server
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
