#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

int punteggioRadiceQuadrata, punteggioLogaritmo, punteggioFattoriale, punteggioPotenza, punteggioDerivata;  //qui non verrebbero inizializzati a 0

void calcola_punteggio() {
punteggioRadiceQuadrata=0, punteggioLogaritmo=0, punteggioFattoriale=0, punteggioPotenza=0, punteggioDerivata=0;
  
radiceQuadrata();
logaritmo();
fattoriale();
potenza();
derivata();
  
int punteggioTotale = punteggioRadiceQuadrata+punteggioLogaritmo+punteggioFattoriale+punteggioPotenza+punteggioDerivata;
  Serial.println("Punteggio totale: "+punteggioTotale);
  if(punteggioTotale>=3){
    webServer.send(200, "text/html", testSuperatoHTML);
  }
  else webServer.send(200, "text/html", testFallitoHTML);
}

void radiceQuadrata(){
  if(webServer.arg("radiecQuadrata").equals("corretto")) punteggioRadiceQuadrata=1;
  Serial.println(webServer.arg("risultato"));
}

void logaritmo(){
  if(webServer.arg("logaritmo").equals("corretto")) punteggioLogaritmo=1;
  Serial.println(webServer.arg("risultato"));
}

void fattoriale(){
  if(webServer.arg("fattoriale").equals("corretto")) punteggioFattoriale=1;
  Serial.println(webServer.arg("risultato"));
}

void potenza(){
  if(webServer.arg("potenza").equals("corretto")) punteggioPotenza=1;
  Serial.println(webServer.arg("risultato"));
}

void derivata(){
  if(webServer.arg("derivata").equals("corretto")) punteggioDerivata=1;
  Serial.println(webServer.arg("risultato"));
}

