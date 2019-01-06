/*
		Here we Have the Web Server (httpd) and the update client over http 

		Credit where credit is due: Some parts of the code taken from : .... TODO 

		the html files are located in the data subfolder and need to be sent in with 
		ESP8266 Sketch Data Upload howto = HTTP:// ....... TODO


*/

#include "httpd.h"
//#include <WiFiClient.h>					//required for other libs

#ifdef _MSC_VER  
	#include <ESP8266WiFi\src\ESP8266WiFi.h>
	#include <ESP8266WebServer\src\ESP8266WebServer.h>
	#include <ESP8266HTTPUpdateServer\src\ESP8266HTTPUpdateServer.h>
	#include <ESP8266mDNS\ESP8266mDNS.h>
#else
	#include <WiFi.h>				// REquired for other libs
	#include <ESP8WebServer.h>			// the Webserver
	#include <ESPHTTPUpdateServer.h>	// the HTTP update server http://IP/update
	#include <ESPmDNS.h>				// mDNS 
	
#endif
#include <FS.h>							// for file system  SPIFFS access
#include "tools.h"						// for bools reading/writing
#include "wifi-ota.h"					// get the wifi structures




// ********* External Functions
// From tools.cpp
extern boolean get_bool(uint8_t bit_nr);
extern void write_bool(uint8_t bit_nr, boolean value);

	// from wifi-ota.cpp
	// add the Debug functions   --     send to debug   MSG to  Serial or telnet --- Line == true  add a CR at the end.
	extern void debugMe(String input, boolean line = true);
	extern void debugMe(float input, boolean line = true);
	extern void debugMe(uint8_t input, boolean line = true);
	extern void debugMe(int input, boolean line = true);


// from config_fs.cpp
extern void FS_wifi_write(uint8_t conf_nr);
extern void FS_dht_write(uint8_t conf_nr);

//from DHT.mos.cpp
		#include "dht_mos.h"
		extern dht_sensor_strucht dht_sensor[DHT_NR_SENSORS]; // , { false ,0,0,DHT_0_TYPE } };
		extern float dht_get_temp_stage0_Min();
		extern float dht_get_temp_stage0_Avg();
		extern float dht_get_temp_stage0_Max();
		extern float dht_get_humid_stage0_Max();
		extern float dht_get_humid_stage0_Min();
		extern float dht_get_humid_stage0_Avg();

#include "wemos_relay.h"
		extern relay_cfg_struct relay[NR_OF_RELAYS];


// *********** External Variables 
// from wifi-ota.cpp
extern wifi_Struct wifi_cfg;
extern String ntp_get_resettime();

ESP8266WebServer httpd(80);					// The Web Server 
File fsUploadFile;							// Variable to hold a file upload

ESP8266HTTPUpdateServer httpUpdater;		// The HTTP update Server


boolean httpd_is_authentified()
{
	if (!httpd.authenticate(HTTPD_ACCESS_USERNAME, HTTPD_ACCESS_PASSWORD))
	{
		httpd.sendHeader("Location", "/login");
		httpd.sendHeader("Cache-Control", "no-cache");
		httpd.send(301);
		debugMe("Web Auth failed");
		return false;
	}
	
	//debugMe("Web Auth OK");
	return true;

}


String httpd_getContentType(String filename) {
	if (httpd.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	else if (filename.endsWith(".json")) return "text/json";
	return "text/plain";
}


bool httpd_handleFileRead(String path) {
	
	 debugMe("handleFileRead: " + path);
	 if (!httpd_is_authentified()) return false;

	if (path.endsWith("/")) path += "index.html";
	String contentType = httpd_getContentType(path);
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
		if (SPIFFS.exists(pathWithGz))
			path += ".gz";
		File file = SPIFFS.open(path, "r");
		size_t sent = httpd.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

void httpd_handleFileUpload() {
	if (!httpd_is_authentified()) return;
	if (httpd.uri() != "/edit") return;
	HTTPUpload& upload = httpd.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/" + filename;
              
		debugMe("handleFileUpload Name: ",false);
		debugMe(filename);
            
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE) {
		//DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	}
	else if (upload.status == UPLOAD_FILE_END) {
		if (fsUploadFile)
			fsUploadFile.close();

                 
		debugMe("handleFileUpload Size: ",false);
		debugMe(String(upload.totalSize));
           
	}
}



void httpd_handleFileDelete() {
	if (!httpd_is_authentified()) return;
	if (httpd.args() == 0) return httpd.send(500, "text/plain", "BAD ARGS");
	String path = httpd.arg(0);

             
	 debugMe("handleFileDelete: " + path);

	if (path == "/")
		return httpd.send(500, "text/plain", "BAD PATH");
	if (!SPIFFS.exists(path))
		return httpd.send(404, "text/plain", "FileNotFound");
	SPIFFS.remove(path);
	httpd.send(200, "text/plain", "");
	path = String();
}

void httpd_handleFileCreate() {
	if (!httpd_is_authentified()) return;
	if (httpd.args() == 0)
		return httpd.send(500, "text/plain", "BAD ARGS");
	String path = httpd.arg(0);

	 debugMe("handleFileCreate: " + path);

	if (path == "/")
		return httpd.send(500, "text/plain", "BAD PATH");
	if (SPIFFS.exists(path))
		return httpd.send(500, "text/plain", "FILE EXISTS");
	File file = SPIFFS.open(path, "w");
	if (file)
		file.close();
	else
		return httpd.send(500, "text/plain", "CREATE FAILED");
	httpd.send(200, "text/plain", "");
	path = String();
}

void httpd_handleFileList() {
	if (!httpd_is_authentified()) return;
	if (!httpd.hasArg("dir")) { httpd.send(500, "text/plain", "BAD ARGS"); return; }
	String path = httpd.arg("dir");
 
	 debugMe("handleFileList: " + path);

	Dir dir = SPIFFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next()) {
		File entry = dir.openFile("r");
		if (output != "[") output += ',';
		bool isDir = false;
		output += "{\"type\":\"";
		output += (isDir) ? "dir" : "file";
		output += "\",\"name\":\"";
		output += String(entry.name()).substring(1);
		output += "\"}";
		entry.close();
	}

	output += "]";
	httpd.send(200, "text/json", output);
}


void httpd_handle_default_args()
{
	if (!httpd_is_authentified()) return;
	if (httpd.args() > 0)
	{
    
		 debugMe("set args present");
		 debugMe(httpd.uri());
		 debugMe(httpd.argName(0));


		// set the brightness


		if (httpd.hasArg("ssid") && httpd.hasArg("password") && httpd.hasArg("wifiMode")) {
			String ssid_STR = httpd.arg("ssid");
			String PWD_STR = httpd.arg("password");
			String wifiMmode_STR = httpd.arg("wifiMode");

			ssid_STR.toCharArray(wifi_cfg.ssid, sizeof(wifi_cfg.ssid));
			PWD_STR.toCharArray(wifi_cfg.pwd, sizeof(wifi_cfg.pwd));
			write_bool(WIFI_MODE, 1);
			//wifiMode =  wifiMmode_STR[0]; 
			FS_wifi_write(0);



			debugMe("Setting ssid to ", false);
			 debugMe(wifi_cfg.ssid);

			debugMe("Setting password to ", false);
			debugMe(wifi_cfg.pwd);

			debugMe("Setting wifi_mode to ", false);
			debugMe(get_bool(WIFI_MODE));
			

		}

		if (httpd.hasArg("APname") && httpd.hasArg("wifiMode")) {
			String APname_STR = httpd.arg("APname");
			String wifiMmode_STR = httpd.arg("wifiMode");

			APname_STR.toCharArray(wifi_cfg.APname, 32);
			write_bool(WIFI_MODE, 0 );
			FS_wifi_write(0);

			debugMe("Setting APname to ", false);
			debugMe(wifi_cfg.APname);

			debugMe("Setting wifi_mode to ", false);
			debugMe(get_bool(WIFI_MODE));
			

		}

		if (httpd.hasArg("delete")) {
			// with http://172.16.222.40/index.html?delete=/conf/0.dht.txt
			String path = httpd.arg("delete");

			// path.toCharArray(ePassword,64);    
			SPIFFS.remove(path);

			debugMe("requested delete : ", false);
			debugMe(path);
			

		}

		if (httpd.hasArg("tempMin")) {
			// with http://172.16.222.40/index.html?tempMin=5
			String tempMin_str = httpd.arg("tempMin");
			uint8_t  tempMin = tempMin_str.toInt();
			
			dht_sensor[0].mintemp = constrain(tempMin, DHT_MINTEMP_MIN, DHT_MINTEMP_MAX);
			debugMe("requested tempmin : ", false);
			debugMe(tempMin);
		}

		if (httpd.hasArg("humidMin")) {
			// with http://172.16.222.40/index.html?humidMin=60
			String humidMin_str = httpd.arg("humidMin");
			uint8_t  humidMin = humidMin_str.toInt();

			dht_sensor[0].minHumid = constrain(humidMin, DHT_MINHUMID_MIN, DHT_MINHUMID_MAX);
			debugMe("requested humid min : ", false);
			debugMe(humidMin);
		}

		if (httpd.hasArg("DHTsave")) {
			
			String tempMin_str = httpd.arg("DHTsave");
			
			debugMe("requested dhtsave : ", true);
			FS_dht_write(0);
		}
	}

}


void httpd_handleRequestSettings() 
{
	//if (!httpd_is_authentified()) return;
	//String  output_bufferZ = "-" ;
	httpd.on("/reset", []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String("rebooting")); ESP.restart();    });

	httpd.on("/wifiMode", []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(get_bool(WIFI_MODE)));   });
	httpd.on("/ssid", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", wifi_cfg.ssid);  });
	httpd.on("/password", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", wifi_cfg.pwd);   });
	httpd.on("/APname", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", wifi_cfg.APname);   });

	httpd.on("/temp", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].temp));   });
	httpd.on("/errors", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].totalErrors));   });
	httpd.on("/humid", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].humidity));   });
	httpd.on("/tempMin", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].mintemp));   });
	httpd.on("/humidMin", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].minHumid));   });
	httpd.on("/tempOff", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].tempDiff));   });
	httpd.on("/humidOff", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].HumiDiff));   });

	httpd.on("/MAXtemp", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_get_temp_stage0_Max()));   });
	httpd.on("/MINtemp", HTTP_GET, []() {if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_get_temp_stage0_Min()));   });
	httpd.on("/AVGtemp", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_get_temp_stage0_Avg()));   });
	httpd.on("/MAXhumid", HTTP_GET, []() {if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_get_humid_stage0_Max()));   });
	httpd.on("/MINhumid", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_get_humid_stage0_Min()));   });
	httpd.on("/AVGhumid", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_get_humid_stage0_Avg()));   });

	
	httpd.on("/DHTups", HTTP_GET, []() { if (!httpd_is_authentified()) return; httpd.send(200, "text/plain", String(dht_sensor[0].update_sec));   });

	


	httpd.on("/localdht.json", HTTP_GET, []() {
		if (!httpd_is_authentified()) return;
		String json = "{";
			json += "\"APname\": \"" + String(wifi_cfg.APname)+ String("\"");
			json += ", \"location\":\"" + String("Here") + String("\"");
			json += ", \"errors\":" + String(dht_sensor[0].totalErrors);
			json += ", \"relay\":";
			if (relay[dht_sensor[0].relay_no].state == true)
				json += "\"On\"";
			else 		
				json += "\"Off\"";

			json += ", \"ups\":" + String(dht_sensor[0].update_sec);
			json += ", \"rst\":\"" + String(ntp_get_resettime()) + String("\"");
			json += " , \"dht\":";
			json += "[";
				json += "{";
					json += "\"name\": \"Temp\"" ;
					json += ", \"last\":" + String(dht_sensor[0].temp);
					json += ", \"on\":" + String(dht_sensor[0].mintemp);
					json += ", \"off\":" + String(dht_sensor[0].tempDiff);
					json += ", \"avg\":" + String(dht_get_temp_stage0_Avg());
					json += ", \"max\":" + String(dht_get_temp_stage0_Max());
					json += ", \"min\":" + String(dht_get_temp_stage0_Min());
					json += ", \"onName\": \"tempMin\"";
					json += ", \"offName\": \"tempOff\"";
				json += "},";
				json += "{";
					json += "\"name\": \"Humidity\"";
					json += ", \"last\":" + String(dht_sensor[0].humidity);
					json += ", \"on\":" + String(dht_sensor[0].minHumid);
					json += ", \"off\":" + String(dht_sensor[0].HumiDiff);
					json += ", \"avg\":" + String(dht_get_humid_stage0_Avg());
					json += ", \"max\":" + String(dht_get_humid_stage0_Max());
					json += ", \"min\":" + String(dht_get_humid_stage0_Min());
					json += ", \"onName\": \"humidMin\"";
					json += ", \"offName\": \"humidOff\"";
				json += "}";
			json += "]";

		json += "}";
		httpd.send(200, "text/json", json);
		json = String();
	});

}


void httpd_toggle_webserver()
{
	if (get_bool(HTTP_ENABLED) == true)
	{
		httpd.stop();
		write_bool(HTTP_ENABLED, false);
		 debugMe("httpd turned off");
	}
	else
	{
		httpd.begin();
		write_bool(HTTP_ENABLED, true);
		 debugMe("httpd turned on");
		httpUpdater.setup(&httpd);
		// debugMe("HTTP server started");
		MDNS.begin(wifi_cfg.APname);
		MDNS.addService("http", "tcp", 80);
	}


}




void httpd_setup()
{
	httpd.on("/", []()  { if (!httpd_is_authentified()) return; httpd_handleFileRead("/index.html"); });

	httpd.on("/login", []() {
		if (!httpd.authenticate(HTTPD_ACCESS_USERNAME, HTTPD_ACCESS_PASSWORD))
			return httpd.requestAuthentication();
		httpd_handleFileRead("/index.html");
	});
	
	






	// Setup Handlers
	httpd.on("/list", HTTP_GET, httpd_handleFileList);
	//load editor
	httpd.on("/edit", HTTP_GET, []() { if (!httpd_handleFileRead("/edit.html")) httpd.send(404, "text/plain", "edit_FileNotFound"); });
	httpd.on("/edit", HTTP_DELETE, httpd_handleFileDelete);
	httpd.on("/edit", HTTP_POST, []() { httpd.send(200, "text/plain", ""); }, httpd_handleFileUpload);

	httpd.onNotFound([]() {if (!httpd_handleFileRead(httpd.uri()))  httpd.send(404, "text/plain", "FileNotFound im sorry check in the next 2'n dimension on the left"); });

	//get heap status, analog input value and all GPIO statuses in one json call
	httpd.on("/all", HTTP_GET, []() {
		if (!httpd_is_authentified()) return;
		String json = "{";
		json += "\"heap\":" + String(ESP.getFreeHeap());
		json += ", \"analog\":" + String(analogRead(A0));
		json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
		json += "}";
		httpd.send(200, "text/json", json);
		json = String();
	});

	// end FS handlers
	//httpd.on("/edit", HTTP_DELETE, handleFileDelete);
	//httpd.serveStatic("/index.html", SPIFFS, "/index.html");  
	//httpd.on( "/set", handle_default_args ); 

	httpd.on("/settings.html", []() {   httpd_handleFileRead("/settings.html");	      httpd_handle_default_args();   });


	httpd.on("/index.html", []() {
#ifdef ARTNET_ENABLED
		if (get_bool(ARTNET_ENABLE) == true)
			handleFileRead("/artnet.html");
		else
#endif
		httpd_handleFileRead("/index.html");
		httpd_handle_default_args();
	});
	//httpd.on("/all", HTTP_GET, [](){

	httpd_handleRequestSettings();

	if (get_bool(HTTP_ENABLED) == true)
	{
		httpd.begin();					// Switch on the HTTP Server
		httpUpdater.setup(&httpd);
		 debugMe("HTTP server started");
		MDNS.begin(wifi_cfg.APname);
		MDNS.addService("http", "tcp", 80);
	}
}

void http_loop()
{
	if (get_bool(HTTP_ENABLED) == true)
		httpd.handleClient();
	
}




