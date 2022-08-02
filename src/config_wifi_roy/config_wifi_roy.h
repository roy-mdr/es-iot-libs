
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





// THIS CODE WILL BE RETURNED BY THE SERVER WHEN CONNECTED TO ROOT (/)
#define WIFI_ROY_MAIN_1		"<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><style type=\"text\/css\">*{font-family: \"Trebuchet MS\";}*:focus{outline: none;}html, body{background-color: #F1F2F4;font-size: 14px;margin: 0;padding: 0;}.WiFi_selector{background-color: #F1F2F4;border-radius: 5px;box-shadow: 0px 10px 20px 0 rgba(0, 0, 0, 0.1);margin-left: auto;margin-right: auto;margin-top: 100px;padding: 1em;width: min-content;}.WiFi_selector *:not(:first-child){margin-top: 2em;}.WiFi_selector a{color: lightblue;display: block;margin-top: 1em !important;text-decoration: none;width: max-content;transition: all 0.2s;}.WiFi_selector label{display: block;}.WiFi_selector label *:not(:first-child){margin-top: 1em;}.WiFi_selector label input,.WiFi_selector label select{color: #666;}.WiFi_selector .l_title{color: #999;letter-spacing: 0.2em;}.WiFi_selector input,.WiFi_selector select,.WiFi_selector .nwk{-moz-box-sizing: border-box;-webkit-box-sizing: border-box;border-radius: 5px;border: 1px solid #F1F2F4;box-sizing: border-box;min-width: 20em;padding: 1em;transition: all 0.2s;width: 100%;}.WiFi_selector .nwk{padding: 0.5em !important;margin-top: 0.5em !important;display: flex;justify-content: space-between;}.WiFi_selector .nwk *{margin-top: 0 !important;}.WiFi_selector input:hover,.WiFi_selector select:hover{box-shadow: inset 0px 5px 10px 0 rgba(0, 0, 0, 0.1);}.WiFi_selector input:focus,.WiFi_selector select:focus{border-color: LightBlue;}.WiFi_selector .send{background-color: #999;color: #EEE;cursor: pointer;letter-spacing: 0.2em;transition: all 0.2s;}.WiFi_selector .send:hover{background-color: #666;box-shadow: 0px 5px 10px 0 rgba(0, 0, 0, 0.1);}.WiFi_selector .send:focus{background-color: #666;border-color: #F1F2F4;}.WiFi_selector .send:active{box-shadow: inset 0px 5px 10px 0 rgba(0, 0, 0, 0.1);color: LightBlue;}.WiFi_selector .output{-moz-box-sizing: border-box;-webkit-box-sizing: border-box;background-color: #333;border-radius: 5px;border: none;box-shadow: inset 0px 5px 10px 0 rgba(0, 0, 0, 0.1);box-sizing: border-box;color: #999;padding: 1em;}.WiFi_selector .fgt{color: indianred;opacity: 50%;}.WiFi_selector .fgt:hover{opacity: 100%;}<\/style><title>WiFi Config<\/title><\/head><body><div class=\"WiFi_selector\"><form action=\"config\" method=\'GET\' target=\"response\" id=\"form\" autocomplete=\"off\"><div><label><div class=\"l_title\">Network name:<\/div><input type=\"text\" list=\"networks\" name=\"ssid\" placeholder=\""
/* IN BETWEEN (HERE) GOES THE PLACEHOLDER TEXT DISPLAYING HOW MANY NETWORKS WERE FOUND. EJ. "3 Networks found..." (without double quotes) */
#define WIFI_ROY_MAIN_2		"\"><\/label><datalist id=\"networks\">"
/* IN BETWEEN (HERE) GOES THE SCANNED NETWORKS */
#define WIFI_ROY_MAIN_3		"<\/datalist><a href=\"/\" id=\"scanNwks\">Scan networks...</a><\/div><div><label><div class=\"l_title\">Password:<\/div><input type=\"password\" name=\"password\"><\/label><\/div><div><input type=\"submit\" name=\"submit\" class=\"send\" value=\"CONNECT\"><\/div><div class=\"output\"><span style=\"color: LightBlue;\" id=\"outputTxt\">No Js<\/span><noscript><iframe src=\"\/status\/html\" id=\'response\' name=\'response\' style=\"border:none;\"><\/iframe><br><br><small><b>Your browser does not support JavaScript!<\/b><br>Manually click to get the connection status.<\/small><\/noscript><\/div>"
/* IN BETWEEN (HERE) GOES THE BUTTON TO CLOSE/START THE ACCES POINT (DEPPENDING OF THE AP STATE) */
#define WIFI_ROY_MAIN_4		"<a href=\"\/restart\">Restart device</a><br><a href=\"\/forget\" class=\"fgt\">Forget saved network<\/a><\/form><\/div><script>const form=document.getElementById(\"form\"),opt=document.getElementById(\"outputTxt\"),scan=document.getElementById(\"scanNwks\"),nwList=document.getElementById(\"networks\");let nwkCont;function setNwkName(t){form.ssid.value=t,form.password.select()}function ajaxReq(){return window.XMLHttpRequest?new XMLHttpRequest:window.ActiveXObject?new ActiveXObject(\"Microsoft.XMLHTTP\"):(alert(\"Browser does not support XMLHTTP.\"),!1)}function getStatus(){const t=ajaxReq();t.open(\"GET\",\"\/status\/json\",!0),t.send(),t.onreadystatechange=(()=>{if(opt.innerHTML=\"Getting status...\",4==t.readyState)if(t.status>=200&&t.status<300)try{const e=JSON.parse(t.response);\"Connected\"==e.status?opt.innerHTML=`${e.status} to ${e.info.ssid}`:opt.innerHTML=e.status,nwkCont.innerHTML=\'<div class=\"l_title\" style=\"text-align: center;font-style: italic;\">Loading...<\/div>\';for(let t=0;t<e.networks.length;t++){const n=e.networks[t],s=`<div class=\"nwk send\" onclick=\"setNwkName(\'${n.ssid}\');\"><span class=\"n\">${n.ssid}<\/span><span class=\"q\">${Math.round((n.rssi+80)\/40*100)}%<\/span><\/div>`;0==t&&(nwkCont.innerHTML=\"\"),nwkCont.innerHTML+=s}setTimeout(getStatus,1e3)}catch(e){opt.innerHTML=\"Error. Check console.\",console.error(e),console.log(t.response)}else nwkCont.innerHTML=\'<div class=\"l_title\" style=\"text-align: center;font-style: italic;\">Error<\/div>\',opt.innerHTML=\"Error connecting to AP.\",setTimeout(getStatus,5e3)})}nwkDiv=document.createElement(\"div\"),nwkDiv.innerHTML=\'<div class=\"l_title\">Found networks:<\/div><div id=\"found\"><div class=\"l_title\" style=\"text-align: center;font-style: italic;\">Loading...<\/div><\/div>\',form.insertBefore(nwkDiv,form.firstChild),nwkCont=document.getElementById(\"found\"),scan.remove(),form.ssid.removeAttribute('placeholder'),nwList.innerHTML='',form.addEventListener(\"submit\",t=>{t.preventDefault(),form.submit.select(),opt.innerHTML=\"Sending data...\";const e=ajaxReq();e.open(\"GET\",encodeURI(`\/config?ssid=${form.ssid.value}&password=${form.password.value}`),!0),e.send(),e.onreadystatechange=(()=>{4==e.readyState&&(e.status<200||e.status>=300)&&(console.log(\"Error sending data.\"),opt.innerHTML=\"Error sending data.\")})}),window.onload=(()=>{getStatus()});<\/script><\/body><\/html>"

// COMMON CSS FILE FOR NO-JAVASCRIPT-ALLOWED-PAGE
#define WIFI_ROY_NO_JS_CSS	"*{font-family: \"Trebuchet MS\";}html, body{font-size: 14px;margin: 0;padding: 0;color: lightblue;}a{background-color: #f1f2f4;border-radius: 5px;color: #666;display: block;margin: 1em;padding: 0.5em;text-decoration: none;width: max-content;transition: all 0.2s;}a:hover{background-color: LightBlue;color: #333;}"

// THIS PAGE RETURNS THE CONNECTION STATUS OF THE DEVICE WHEN THERE IS NO JAVASCRIPT ALLOWED
#define WIFI_ROY_STATUS_HTML_1	"<!DOCTYPE html><html><head><meta charset=\"utf-8\"><noscript><meta http-equiv=\"refresh\" content=\"1\"><\/noscript><title><\/title><style type=\"text\/css\">"
/* THEN WIFI_ROY_NO_JS_CSS */
#define WIFI_ROY_STATUS_HTML_2	"<\/style><\/head><body>"
/* THEN getNetStatus() + (WiFi.isConnected() ? (" to " + EEPROM_READ(1) ) : "" ) */
#define WIFI_ROY_STATUS_HTML_3	"<br><a href=\"\/status\/html\">Click here to update status.<\/a><\/body><\/html>"

// THIS PAGE GETS THE SSID AND PASSWORD PASSED BY THE USER TO THE SERVER
#define WIFI_ROY_CONFIG_1		"<!DOCTYPE html><html><head><meta charset=\"utf-8\"><noscript><meta http-equiv=\"refresh\" content=\"3;URL=\'\/status\/html\'\"><\/noscript><title><\/title><style type=\"text\/css\">"
/* THEN WIFI_ROY_NO_JS_CSS */
#define WIFI_ROY_CONFIG_2		"<\/style><\/head><body>Data received!<br><a href=\"\/status\/html\">Click here to get status.<\/a><\/body><\/html>"





void setupWifiConfigServer(ESP8266WebServer &server, int EEPROM_ADDR_FOR_SSID, int EEPROM_ADDR_FOR_PASSWORD, char *ssid_for_AP, char *password_for_AP);

void wifiConfigLoop(ESP8266WebServer &server);

void connectWiFi();

bool wifiStatChanged(byte &compareVar);

void getNetworksAsync(bool shouldScan);

String getNetStatus();

void ESP_STATION(bool keepServerOpenInLAN);

void ESP_AP_STA();

void ESP_AP_TOGGLE(bool keepServerOpenInLAN);





#endif