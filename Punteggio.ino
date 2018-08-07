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
    //webServer.send(200, "text/html", testSuperatoHTML);
    if (SPIFFS.exists("/test_superato.html")){
      File f = SPIFFS.open("/test_superato.html", "r");
      webServer.streamFile(f, "text/html");
          f.close();
  }

  else{
    Serial.println("File test_superato.html not found, falling back to default page");
    testSuperatoHTML="<!DOCTYPE html><html><head><title>Disfida matematica</title></head><body><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
                      "<h1>Complimenti, hai superato il test!</h1>"
                      "<p>Tuttavia, almeno per il momento, questa rete non è in grado di fornire un servizio di connettività ad Internet. In futuro potrei esplorare la possibilità di un"
                      "tunnel DNS, ma per ora ti dovrai accontentare della gloria e dell'onore di aver superato questo test. Complimenti ancora!</p></body></html>";


  }
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

