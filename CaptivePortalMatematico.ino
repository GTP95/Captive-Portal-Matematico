/*
   Based on Captive Portal by: M. Ray Burnette 20150831
   See Notes tab for original code references and compile requirements
*/

#include <ESP8266WiFi.h>
#include "./DNSServer.h"                  // Patched lib
#include <ESP8266WebServer.h>
#include "FS.h"

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  webServer(80);          // HTTP server

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

String testSuperatoHTML;


String testFallitoHTML="<!DOCTYPE html><html><head><title>Disfida matematica</title></head><body><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
                      "<h1>Test fallito...</h1>"
       
               
                      "<img src='data:image/jpeg;base64,/9j/4AAQSkZJRgABAQAAAQABAAD/4QB+RXhpZgAASUkqAAgAAAACADEBAgAHAAAAJgAAAGmHBAABAAAALgAAAAAAAABHb29nbGUAAAMAAJAHAAQAAAAwMjIwAaADAAEAAAABAAAABaAEAAEAAABYAAAAAAAAAAIAAQACAAQAAABSOTg"
                      "AAgAHAAQAAAAwMTAwAAAAAP/bAIQAAwICCAoKCAoKCAgICAgICAcICAgICAgICAgHCAgICAgIBwgICAgICAgHCAcICggHBwgJCQkIBw0NCggNBwgJCAEDBAQGBQYKBgYKDQ0IDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0NDQ0N/8AA"
                      "EQgBBwDAAwEiAAIRAQMRAf/EAB4AAAEEAwEBAQAAAAAAAAAAAAYEBQcIAAMJAgEK/8QAWRAAAQMCAwUEBAgGDAkNAAAAAgADBAUSAQYiBxETMkIIIVJyFCNikgkVMUFRgqKyM2NzwtLwFiQ2RFNhdZGjtNPiFxg0NXGTobPyJkNUVmZ2gYOVpLHB4//EABsBAAID"
                      "AQEBAAAAAAAAAAAAAAMFAgQGAAEH/8QAJxEAAgICAQMEAgMBAAAAAAAAAAIDEgEEIgUTMhEUQlIGIxUxM0H/2gAMAwEAAhEDEQA/AOcx5SK1NZ5fNFZ13TpWyMxctUsfoPvbq4BPRbV43KQplC0oIqMa0kVMizZ1KGphE8KOhUHEW5ZqfSt10TYVBDOh5mtpsNGMyPcmKVTF"
                      "sc5uVvQbQRFSXFuy3lE3CUrZU2dexfd4F5Psxa6Wdi3DrOwFelgSaZWHEctHX9RWcy9snj3ao5fbUtUGiMNiAi1YsZtflkUXFFGcWh9znRKhfryJkmtrqY1s+hvlqaFCO0Dsqx329ADd5yWY2PyX3HxDeywpzMXjDFTHts2ASqe53h6kuUhvP371E4QT8Cpd7DlZ4XQS3LLkvcy"
                      "+74CTcbSDYHQ+r1gvK9Yo0TAjZvWvB1YvqvIxx6uWbl6WcJHuSoPUWOjalN6Unp8ACW2oerFKJXuarV4Hqp1IBQLUW7iMk6yn7khko6RANlrjAbadKK+k77STxpFqYaU3akWxn9iHiTLQaRxBT5Q8ikbwNiBH5QIvuIi2J5EkSRDRYP8ACnyq2eSMpRYYjbbxeov+NMepfkiwL6"
                      "J5BNTS+5GWz7s8u8zukPAQEJeVTFAyS01yjavVQzoPTqQ5PzK4a+abHWZ9jyYfdpVDB7hD4U1PTmvZQZJqPiJMsyse0lTzBkQkQJgiVwkKI4GdvFaoFPMHtL6zXS8SB3gvaJ4zVRIc5ghdC+4C6+XSqxZO7OTTdwnq4WgD1a1KNCzES3S62rS7oNogc/wLtcoqF9rXZrwwHiNAXX"
                      "0H41ZqiVzUiSSAuCr0WwUpUOUeYMsOslaQl7liaMVfvbRsDCQ2ZtAN/h1eNUfzDlh1ly0gT7WmuK5oajRjgt8SNclTMZFeXIYJz2+Nz2FLuMMKh+JO8PLwEnqtxLeVM9PqdpIV/UZ9lEC74lMVrrMS5tS7Usv6eXoBAtfpulZ/1o3MbovEiR9J0szC3aSZQkLQJKtBRL5Hs21LW"
                      "x3YtxSB5/8ABdAEGk0k2U7OuOQOOcg9HjVs6NRGmxG+0Gx6RWV6l1D4oGXX+w4ZejkIiDDVgD4R0osZoxDzue8mMM9CI2tN6fEmmXWyMu8i8upZn1v5B6BNOlNBy2l5UyVKo3JtccJNUmcpHhskzkyTniJbJM1Np1Ai7kB3LKIeGWyRBSMtuEleWqRdzKQKdTrUvaUaRwDRT6RaK"
                      "Hpkq0lI8iF3IRqVBXJKSaA00iXci2BKUcQZNho7hOabk3ilEsqD8ctV12/7H2nB44D5rA9v++p+hv8ASvDtLEtBjeBJrr7FWKjJY5m1GHaVq30qXaSkzbNs7OM+6PRfeJ+dRa2GpfUdSs0Qobg4dm3cKFZMC0k+0uXpWTIFyq6+u12GzcyykxnT9QVG2awtUwPMaQ8ijTOkHSaVb"
                      "urZbIMYnK/5r/CJry9SeM8DfiNOmbo+pH+wjI54lxy5Rts8SzM2w2ugCWLmTXk+ADDQCI22gnt+rb/lTc+tIYrKO92LPxCOnY3+yKIgeEB7kBnmLdypXSsHT1F9VTXIEfZ8wiWuJSLuZb+KIcyb52Y7tIojOciCiXBHlXqkZW9lb8tZeIyuUp0WiJRPKOYIBqoGUfMiTCh7kRwYO"
                      "5bHmFSZhovEGBiJqqkHmRqcVM1RYQ7E/Ih6tRNSdaEelKMxxuZJsnzRLT4U61WEW0g6sydSfg1D3JtrFPHmFbssSrhV6wrADbls5CWxcI+tb19PQBqlzmWT6l0flMdKq9tZyXwn7h5CW46L1Nk/UUZILsQ1Q8vXKSKNk3xCnvJ2VeT9etSRGoi+oasaynngCD2agtDV0Ag7MWYBI"
                      "TUPVPOLtyaDzM74ljonevMnYIZtF9IeaEQ5jC/yKfKJDBoBAdNoCCjLZi3pNwvIiuTmbVasB1qVWk4jGII6jVBFMMyt3JnOdctrDSzBaoOcHEixRTFrFmm7UhmFgRaRRjl7J/V1KNqnnaPrdxowyvk0i1Wokyps/wB/MpYouURHDcqskpfjhwDuX8tW4ItjUtOTcG1ezNU2YvCF1"
                      "21aTfWTH0jKUuJHxxxMkqVvSudMQvPmqB6MmY3OZAOXZ1r/AL33SRRXHCtJA9JPWmuuK9liWmqpcK1Zf0mheNUSRDBf+dWnFIeTabp3qP8AP2XBMVKlGLihuQ9W4Nw2kmelLRiuxFNBoVop44CR4SOGRj4VlSlWjcvtvS91XiKDFRJWV7kxVnJ9vKCk+mAJe4ij9jYkKW7EfHiMu0"
                      "R3QJ3DYtLpSOHPHmtRBmSkg3pUeVB8BvG6zyr5b1KL9rFlOIdwtXKimm0Yi5iTHlSRGjCPFK64BIbdQ2miPDaNTv4XBZ1i8pI+TsqNFbqU7ZSyS0A78dSqjT9pzAY3NPtl7N4qVMkdoLA9Jfr9hQZQmMllKWw2lpPCOPmUbws4CWI44dSU1qrkOInh0qtVgllqSJIaSF1pRJVdsrQ"
                      "cx2+a1DH+HoLvwo++P6CH2Du8ThNgpmkxtyBI20/A+8HG/eFPsHNN/diu7RaWSwqmAhucz3ommpkbf34qtRidgNzG3aKDaI3qUi5zMbN2HMou9OsPyptEKNkI5ulL6LWO7cmSrVYbUwQKtaSuCwsPkSt9Kea+oiy3X7SUhhXbhUojwAsyU7UDnt2L5WIPqvqJbmOXqAfbTvUqb6v"
                      "6i+jfj8/3KjqUtpk62xF0bNweNRjJO1N51Y0wfa4jWobbQp3Ebu8KGKHQxGMEm3im4dg39AB/xJqjZhuLhl1I3ytRjfgcMdHoc+w/I9yfdWV2E78xTd6m3Y9M4kg7tIiCkLMGZIw3X26dOkdRJoyNkDTKtK91sx1D5ATgWR2jAwPq6uoVld2LtT1GkXNRpdyjxQvHiCP4wRtL3EC"
                      "ZqpxsjxWCIDHWQipw2e0FqFjvH15F0v3GPumh7aJR2+BKO20uC6Wnl5EGyhKka0DtPTgtG663xW/2adZHaRrEn1TTpBxNHI1+gi/Yf2RWpTAPyHHB4gXiAj7XnTZn7Y98VTotpE6w+Z26LbLLNP2l7lk/4ckGag7Mym6X4d0pBdV2nV9RLwy21ptjCXtESlOTlMTC5N8XLA7+8tK"
                      "rd0nXiNtHyxTi54g3fi3SEvvosayUzaXoclyK7b8rlz/N5z9lAkfZLJu0ut2c3Jqt8yd63l+U1aYukVvvaFzMcthlr2da7AcAfTClNct1jQD4k60TtGStQusCBdJXj+YgfajWnXWLbdQnze8hnLGSnx5QJ0y5kS6KpD9krEkS9v79x+qEreq9ANd23Om4Gi1F0TYrKdG23hXJFnHs"
                      "/cIBLi8vNo/voaSodPAwQDmriCJLe7PQNDYt0p4CYr1hdQkugV1HdPr+lQfSKjpRZCrNoqCHj4JBrU2/UjKLVQ4Q3eBRc3UrhTjNB3hhb4E/0NhojxUuVClJgrsm0UrmVbSgis1S5PZJeIKaaiHj4z1KfNgufYrD9r/+SzmQZkWXnwj6HbA62rzVbcMVJ2wKncSoRRLl4omX1EkeW"
                      "nMX6z3cudkKnxbpRMPi6Dp6ekrLdNwGlVRy4W/SK+ztl4u/gh4BeJr1X3E2NbEquPJMet9qQWP31lNrZ7slmNhFBVTYGWHMceVMm0KlBxIsXqkGNw/igIbyTs5stzAP78wt9orkq2Z5ALikTrrkiTyk67qt9kblXCVJ4ypQrWxt5RHSo5255W9IjO4APr2wI2POKmSNEtAcPZQbWW9"
                      "2Khks4wRLs/h8eKB9P4IvZdDSY++tNRyw4HKNyXZvy46NxwnSYO+8mLv2s6XURteNBnx9mQP3rEfHp9U7/aKKkO2OrMg+oSFOoUcj6SQl+zHMXz0qL/qnf7RKYlezE5pcitsCX8GwY/a4ilY9WAbsyZHaeksRQ1OcUZEn2Wv1IFLw5baDuFsRSrIuQxZ9adxSXA9Y45qLy+RP84BV"
                      "ZmCrHUFyiIdze1cBImnPobqcW4SUFPJSu9ebtIk2xpWpFWbqX6zchOXEIStTZGFMqiyG/aaJRfQpGw1JZIqvhVtBa4cxaxpBTaMsOAz+R/MVb6H6zhD4j/PU0Y3CIDq0gn2mSXBQ2oNnahZ/BG1X5UFSXFpt/VWBMGfnluxpbbVj+zPlT14OeFm/37FA1BgXGP8ApFXG2CQLbvIAL"
                      "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'"
" alt=\"Vittorio Sgarbi che ti sta dando della capra\" />"
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
    webServer.send(200, "text/html", testHTML);
  });
  
  webServer.on("/calcola_punteggio", calcola_punteggio);  //specifico la funziona da chiamare corrispondente all'azione calcola_punteggio (evento pressione tasto "Invia risultati")
  
  yield();
  SPIFFS.begin();   //monto il filesystem
  //SPIFFS.format();  //RIMUOVRE!
  yield();

  
  webServer.begin();  //avvio il server
  
  

}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}



