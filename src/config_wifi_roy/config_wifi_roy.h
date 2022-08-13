
/* ====================================================== USO ====================================================== */
/*                                                                                                                   */
/* #include <config_wifi_roy.h>                                                                                      */
/* ESP8266WebServer server(80);                                                                                      */
/*                                                                                                                   */
/* #define EEPROM_ADDR_CONNECTED_SSID 1       // Start saving connected network SSID from this memory address        */
/* #define EEPROM_ADDR_CONNECTED_PASSWORD 30  // Start saving connected network Password from this memory address    */
/* #define AP_SSID "ESP8266_AP"               // Set your own Network Name (SSID)                                    */
/* #define AP_PASSWORD "12345678"             // Set your own password                                               */
/*                                                                                                                   */
/* setup() {                                                                                                         */
/* 	EEPROM.begin(4096);                                                                                              */
/* 	setupWifiConfigServer(server, EEPROM_ADDR_CONNECTED_SSID, EEPROM_ADDR_CONNECTED_PASSWORD, AP_SSID, AP_PASSWORD); */
/* }                                                                                                                 */
/*                                                                                                                   */
/* loop() {                                                                                                          */
/* 	wifiConfigLoop();                                                                                                */
/* }                                                                                                                 */
/*                                                                                                                   */
/* ================================================================================================================= */




#ifndef CONFIG_WIFI_ROY_H
#define CONFIG_WIFI_ROY_H



#include <blink_async.h>
#include <EEPROM_handler.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>





// THIS CODE WILL BE RETURNED BY THE SERVER WHEN CONNECTED TO ROOT (/)
#define WIFI_ROY_MAIN_1		"<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><style type=\"text\/css\">*{font-family: \"Trebuchet MS\";}*:focus{outline: none;}html, body{background-color: #F1F2F4;font-size: 14px;margin: 0;padding: 0;}h1{margin: 2em;color: #666;}.WiFi_selector{background-color: #F1F2F4;border-radius: 5px;box-shadow: 0px 10px 20px 0 rgba(0, 0, 0, 0.1);margin-left: auto;margin-right: auto;margin-bottom: 100px;padding: 1em;width: min-content;}.WiFi_selector *:not(:first-child){margin-top: 2em;}.WiFi_selector a{color: lightblue;display: block;margin-top: 1em !important;text-decoration: none;width: max-content;transition: all 0.2s;}.WiFi_selector label{display: block;}.WiFi_selector label *:not(:first-child){margin-top: 1em;}.WiFi_selector label input,.WiFi_selector label select{color: #666;}.WiFi_selector .l_title{color: #999;letter-spacing: 0.2em;}.WiFi_selector input,.WiFi_selector select,.WiFi_selector .nwk{-moz-box-sizing: border-box;-webkit-box-sizing: border-box;border-radius: 5px;border: 1px solid #F1F2F4;box-sizing: border-box;min-width: 20em;padding: 1em;transition: all 0.2s;width: 100%;}.WiFi_selector .nwk{padding: 0.5em !important;margin-top: 0.5em !important;display: flex;justify-content: space-between;}.WiFi_selector .nwk *{margin-top: 0 !important;}.WiFi_selector input:hover,.WiFi_selector select:hover{box-shadow: inset 0px 5px 10px 0 rgba(0, 0, 0, 0.1);}.WiFi_selector input:focus,.WiFi_selector select:focus{border-color: LightBlue;}.WiFi_selector .send{background-color: #999;color: #EEE;cursor: pointer;letter-spacing: 0.2em;transition: all 0.2s;}.WiFi_selector .send:hover{background-color: #666;box-shadow: 0px 5px 10px 0 rgba(0, 0, 0, 0.1);}.WiFi_selector .send:focus{background-color: #666;border-color: #F1F2F4;}.WiFi_selector .send:active{box-shadow: inset 0px 5px 10px 0 rgba(0, 0, 0, 0.1);color: LightBlue;}.WiFi_selector .output{-moz-box-sizing: border-box;-webkit-box-sizing: border-box;background-color: #333;border-radius: 5px;border: none;box-shadow: inset 0px 5px 10px 0 rgba(0, 0, 0, 0.1);box-sizing: border-box;color: #999;padding: 1em;}.WiFi_selector .fgt{color: indianred;opacity: 50%;}.WiFi_selector .fgt:hover{opacity: 100%;}<\/style><title>WiFi Config - "
/* IN BETWEEN (HERE) GOES THE ALIAS (NAME) OF THE BOARD/CONTROLLER */
#define WIFI_ROY_MAIN_2		" <\/title><link rel=\"shortcut icon\" type=\"image\/svg+xml\" href=\"data:image\/svg+xml;base64,PHN2ZyB2ZXJzaW9uPSIxLjEiIHZpZXdCb3g9IjAgMCAzMiAzMiIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48ZGVmcz48Y2xpcFBhdGggaWQ9ImNsaXBQYXRoMTgiPjxwYXRoIGQ9Im0wIDI0aDI0di0yNGgtMjR6Ii8+PC9jbGlwUGF0aD48L2RlZnM+PGcgdHJhbnNmb3JtPSJtYXRyaXgoMS4zMzMzIDAgMCAtMS4zMzMzIDAgMzIpIj48ZyBmaWxsPSIjYWRkOGU2Ij48ZyBjbGlwLXBhdGg9InVybCgjY2xpcFBhdGgxOCkiIGZpbGw9IiNhZGQ4ZTYiPjxnIHRyYW5zZm9ybT0idHJhbnNsYXRlKDQuMzYzOCAxMi44NjkpIj48cGF0aCBkPSJtMCAwIDIuMTgyLTIuMTgyYzMuMDEgMy4wMTEgNy44OTggMy4wMTEgMTAuOTA4IDBsMi4xODMgMi4xODJjLTQuMjExIDQuMjExLTExLjA1MiA0LjIxMS0xNS4yNzMgMG0tNC4zNjQgNC4zNjQgMi4xODItMi4xODJjNS40MjEgNS40MjEgMTQuMjE2IDUuNDIxIDE5LjYzNyAwbDIuMTgxIDIuMTgyYy02LjYyIDYuNjE5LTE3LjM2OCA2LjYxOS0yNCAwIiBmaWxsPSIjYWRkOGU2Ii8+PC9nPjxnIHRyYW5zZm9ybT0idHJhbnNsYXRlKDExLjk5NSA0LjMyMjMpIj48cGF0aCBkPSJtMCAwYy0wLjgyNiAwLTEuNTA2IDAuNjgtMS41MDYgMS41MTYgMCAwLjgyNiAwLjY4IDEuNTA2IDEuNTA2IDEuNTA2IDAuODM2IDAgMS41MTYtMC42OCAxLjUxNi0xLjUwNiAwLTAuODM2LTAuNjgtMS41MTYtMS41MTYtMS41MTZtMy4wMDIgMS4xMjNjMC4wMTggMC4xMTggMC4wMjggMC4yNTQgMC4wMjggMC4zOTMgMCAwLjEyOS0wLjAxIDAuMjY0LTAuMDI4IDAuMzkzbDAuODQ0IDAuNjU5YzAuMDgxIDAuMDY5IDAuMTAxIDAuMTY3IDAuMDYxIDAuMjU4bC0wLjgwNiAxLjM5NmMtMC4wNTEgMC4wODgtMC4xNTkgMC4xMjgtMC4yNTcgMC4wODhsLTEuMDAzLTAuNDA0Yy0wLjIwNyAwLjE2OS0wLjQzMyAwLjI5NS0wLjY3OSAwLjQwNGwtMC4xNSAxLjA2MmMtMC4wMTcgMC4wOTktMC4wOTcgMC4xNzktMC4yMDYgMC4xNjloLTEuNjEyYy0wLjA5OCAwLTAuMTc5LTAuMDctMC4xOTYtMC4xNjlsLTAuMTQ5LTEuMDYyYy0wLjI0Ny0wLjEwOS0wLjQ3MS0wLjI0NS0wLjY3OC0wLjQwNGwtMS4wMDUgMC40MDRjLTAuMDk4IDAuMDQtMC4xOTYgMC0wLjI0Ny0wLjA4OGwtMC44MDYtMS4zOTZjLTAuMDQ4LTAuMDkxLTAuMDMtMC4xOTkgMC4wNTEtMC4yNThsMC44NTYtMC42NTljLTAuMDItMC4xMjktMC4wNC0wLjI2NC0wLjA0LTAuMzkzIDAtMC4xMzkgMC4wMS0wLjI3NSAwLjAzLTAuMzkzbC0wLjg1Ni0wLjY3Yy0wLjA3MS0wLjA1OC0wLjA4OS0wLjE2Ni0wLjA1LTAuMjU1bDAuODA4LTEuMzk3YzAuMDQ3LTAuMDg4IDAuMTU2LTAuMTE4IDAuMjQ0LTAuMDg4bDEuMDA1IDAuNDAyYzAuMjE3LTAuMTU4IDAuNDMzLTAuMjk1IDAuNjg4LTAuMzkybDAuMTQ5LTEuMDc0YzAuMDE3LTAuMDk3IDAuMDk4LTAuMTY5IDAuMTk2LTAuMTY5aDEuNjEyYzAuMTA5IDAgMC4xODkgMC4wNzIgMC4xOTkgMC4xNjlsMC4xNTcgMS4wNzRjMC4yNDYgMC4wOTcgMC40NzIgMC4yMzQgMC42NzkgMC4zOTJsMS4wMDMtMC40MDJjMC4wODgtMC4wNDEgMC4xOTcgMCAwLjI0NiAwLjA4OGwwLjgwNyAxLjM5N2MwLjA1IDAuMDg5IDAuMDMgMC4xOTctMC4wNTEgMC4yNTV6IiBmaWxsPSIjYWRkOGU2Ii8+PC9nPjwvZz48L2c+PC9nPjwvc3ZnPgo=\"><\/head><body><h1>Connect "
/* IN BETWEEN (HERE) GOES THE ALIAS (NAME) OF THE BOARD/CONTROLLER */
#define WIFI_ROY_MAIN_3		" to a WiFi network<\/h1><div class=\"WiFi_selector\"><form action=\"set_wifi\" method=\'GET\' target=\"response\" id=\"form\" autocomplete=\"off\"><div><label><div class=\"l_title\">Network name:<\/div><input type=\"text\" list=\"networks\" name=\"ssid\" placeholder=\""
/* IN BETWEEN (HERE) GOES THE PLACEHOLDER TEXT DISPLAYING HOW MANY NETWORKS WERE FOUND. EJ. "3 Networks found..." (without double quotes) */
#define WIFI_ROY_MAIN_4		"\"><\/label><datalist id=\"networks\">"
/* IN BETWEEN (HERE) GOES THE SCANNED NETWORKS */
#define WIFI_ROY_MAIN_5		"<\/datalist><a href=\"/\" id=\"scanNwks\">Scan networks...</a><\/div><div><label><div class=\"l_title\">Password:<\/div><input type=\"password\" name=\"password\"><\/label><\/div><div><input type=\"submit\" name=\"submit\" class=\"send\" value=\"CONNECT\"><\/div><div class=\"output\"><span style=\"color: LightBlue;\" id=\"outputTxt\">No Js<\/span><noscript><iframe src=\"\/status\/html\" id=\'response\' name=\'response\' style=\"border:none;\"><\/iframe><br><br><small><b>Your browser does not support JavaScript!<\/b><br>Manually click to get the connection status.<\/small><\/noscript><\/div>"
/* IN BETWEEN (HERE) GOES THE BUTTON TO CLOSE/START THE ACCES POINT (DEPPENDING OF THE AP STATE) */
#define WIFI_ROY_MAIN_6		"<a href=\"\/restart\">Restart device</a><br><a href=\"\/forget\" class=\"fgt\">Forget saved network<\/a><\/form><\/div><script>const form=document.getElementById(\"form\"),opt=document.getElementById(\"outputTxt\"),scan=document.getElementById(\"scanNwks\"),nwList=document.getElementById(\"networks\");let nwkCont;function setNwkName(t){form.ssid.value=t,form.password.select()}function ajaxReq(){return window.XMLHttpRequest?new XMLHttpRequest:window.ActiveXObject?new ActiveXObject(\"Microsoft.XMLHTTP\"):(alert(\"Browser does not support XMLHTTP.\"),!1)}function getStatus(){const t=ajaxReq();t.open(\"GET\",\"\/status\/json\",!0),t.send(),t.onreadystatechange=(()=>{if(opt.innerHTML=\"Getting status...\",4==t.readyState)if(t.status>=200&&t.status<300)try{const e=JSON.parse(t.response);\"Connected\"==e.status?opt.innerHTML=`${e.status} to ${e.info.ssid}`:opt.innerHTML=e.status,nwkCont.innerHTML=\'<div class=\"l_title\" style=\"text-align: center;font-style: italic;\">Loading...<\/div>\';for(let t=0;t<e.networks.length;t++){const n=e.networks[t],s=`<div class=\"nwk send\" onclick=\"setNwkName(\'${n.ssid}\');\"><span class=\"n\">${n.ssid}<\/span><span class=\"q\">${Math.round((n.rssi+80)\/40*100)}%<\/span><\/div>`;0==t&&(nwkCont.innerHTML=\"\"),nwkCont.innerHTML+=s}setTimeout(getStatus,1e3)}catch(e){opt.innerHTML=\"Error. Check console.\",console.error(e),console.log(t.response)}else nwkCont.innerHTML=\'<div class=\"l_title\" style=\"text-align: center;font-style: italic;\">Error<\/div>\',opt.innerHTML=\"Error connecting to AP.\",setTimeout(getStatus,5e3)})}nwkDiv=document.createElement(\"div\"),nwkDiv.innerHTML=\'<div class=\"l_title\">Found networks:<\/div><div id=\"found\"><div class=\"l_title\" style=\"text-align: center;font-style: italic;\">Loading...<\/div><\/div>\',form.insertBefore(nwkDiv,form.firstChild),nwkCont=document.getElementById(\"found\"),scan.remove(),form.ssid.removeAttribute('placeholder'),nwList.innerHTML='',form.addEventListener(\"submit\",t=>{t.preventDefault(),form.submit.select(),opt.innerHTML=\"Sending data...\";const e=ajaxReq();e.open(\"GET\",encodeURI(`\/set_wifi?ssid=${form.ssid.value}&password=${form.password.value}`),!0),e.send(),e.onreadystatechange=(()=>{4==e.readyState&&(e.status<200||e.status>=300)&&(console.log(\"Error sending data.\"),opt.innerHTML=\"Error sending data.\")})}),window.onload=(()=>{getStatus()});<\/script><\/body><\/html>"

// COMMON CSS FILE FOR NO-JAVASCRIPT-ALLOWED-PAGE
#define WIFI_ROY_NO_JS_CSS	"*{font-family: \"Trebuchet MS\";}html, body{font-size: 14px;margin: 0;padding: 0;color: lightblue;}a{background-color: #f1f2f4;border-radius: 5px;color: #666;display: block;margin: 1em;padding: 0.5em;text-decoration: none;width: max-content;transition: all 0.2s;}a:hover{background-color: LightBlue;color: #333;}"

// THIS PAGE RETURNS THE CONNECTION STATUS OF THE DEVICE WHEN THERE IS NO JAVASCRIPT ALLOWED
#define WIFI_ROY_STATUS_HTML_1	"<!DOCTYPE html><html><head><meta charset=\"utf-8\"><noscript><meta http-equiv=\"refresh\" content=\"1\"><\/noscript><title><\/title><style type=\"text\/css\">"
/* THEN WIFI_ROY_NO_JS_CSS */
#define WIFI_ROY_STATUS_HTML_2	"<\/style><\/head><body>"
/* THEN getNetStatus() + (WiFi.isConnected() ? (" to " + EEPROM_READ(1) ) : "" ) */
#define WIFI_ROY_STATUS_HTML_3	"<br><a href=\"\/status\/html\">Click here to update status.<\/a><\/body><\/html>"

// THIS PAGE GETS THE SSID AND PASSWORD PASSED BY THE USER TO THE SERVER
#define WIFI_ROY_SET_WIFI_1		"<!DOCTYPE html><html><head><meta charset=\"utf-8\"><noscript><meta http-equiv=\"refresh\" content=\"3;URL=\'\/status\/html\'\"><\/noscript><title><\/title><style type=\"text\/css\">"
/* THEN WIFI_ROY_NO_JS_CSS */
#define WIFI_ROY_SET_WIFI_2		"<\/style><\/head><body>Data received!<br><a href=\"\/status\/html\">Click here to get status.<\/a><\/body><\/html>"





void setupWifiConfigServer(ESP8266WebServer &server, int EEPROM_ADDR_FOR_SSID, int EEPROM_ADDR_FOR_PASSWORD, char *ssid_for_AP, char *password_for_AP);

void wifiConfigLoop();

void connectWiFi();

bool wifiStatChanged(byte &compareVar);

void getNetworksAsync(bool shouldScan);

String getNetStatus();

void ESP_STATION(bool keepServerOpenInLAN);

void ESP_AP_STA();

void ESP_AP_TOGGLE(bool keepServerOpenInLAN);

bool captivePortal();

bool isIp(String str);

String toStringIp(IPAddress ip);





#endif