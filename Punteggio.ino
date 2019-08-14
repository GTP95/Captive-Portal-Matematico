#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

int punteggioTotale;  //qui non verrebbe inizializzato a 0
void calcola_punteggio() {
  punteggioTotale=0;
  File punteggi=(File)NULL;
  
  radiceQuadrata();
  logaritmo();
  fattoriale();
  potenza();
  derivata();
  
  Serial.print("Punteggio totale: ");
  Serial.println(punteggioTotale);
  if(punteggioTotale>=3){
    if (SPIFFS.exists("/test_superato.html")){
      File paginaTestSuperato = SPIFFS.open("/test_superato.html", "r");
      webServer.streamFile(paginaTestSuperato, "text/html");
      paginaTestSuperato.close();
    }
         
  

  else{
    Serial.println("File test_superato.html not found, falling back to default page");
    webServer.send(200, "text/html", testSuperatoHTML);


  }
  }
  else{
    if(SPIFFS.exists("/test_fallito.html")){
      File paginaTestFallito = SPIFFS.open("/test_fallito.html", "r");
      webServer.streamFile(paginaTestFallito, "text/html");
      paginaTestFallito.close();
    }
    else webServer.send(200, "text/html", testFallitoHTML);
  }
  if(punteggi==NULL) punteggi=SPIFFS.open("/punteggi","a");
  registraPunteggio(punteggi, punteggioTotale);
}

void radiceQuadrata(){
  if(webServer.arg("radiceQuadrata").equals("corretto")) punteggioTotale++;
}

void logaritmo(){
  if(webServer.arg("logaritmo").equals("corretto")) punteggioTotale++;
}

void fattoriale(){
  if(webServer.arg("fattoriale").equals("corretto")) punteggioTotale++;
}

void potenza(){
  if(webServer.arg("potenza").equals("corretto")) punteggioTotale++;

}

void derivata(){
  if(webServer.arg("derivata").equals("corretto")) punteggioTotale++;
  
}

void registraPunteggio(File file, int punteggio){ //flush&close?
    file.print(punteggio);
    file.flush();
}
