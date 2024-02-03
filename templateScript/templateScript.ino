#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
ESP8266WebServer server(80);

const char id[] = "AuraHub_27d435b37a58";
bool setupIsRunning = false;
bool accesPointIsRunning = false;

struct settings
{
    char ssid[30];
    char password[30];

} user_wifi = {};

void setup_wifi()
{
    setupIsRunning = true;
    accesPointIsRunning = false;
    delay(100);

    // Turn on wifi and try to connect with credentials from EEPROM
    Serial.println();
    Serial.print("Trying to connect to ");
    Serial.println(user_wifi.ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(user_wifi.ssid, user_wifi.password);

    // Try to connect to wifi (with data from EEPROM)
    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(BUILTIN_LED, LOW);
        Serial.print(".");
        delay(600);

        // If not connected after 15 seconds, run access point, turn on LED and break from while loop
        if (tries++ > 15)
        {
            accesPointIsRunning = true;
            Serial.println("");
            Serial.println("Starting Access Point Mode");
            digitalWrite(BUILTIN_LED, LOW);
            WiFi.mode(WIFI_AP);
            WiFi.softAP(id, "");
            break;
        }
        digitalWrite(BUILTIN_LED, HIGH);
        delay(400);
    }

    // If connect print IP and turn off LED
    if (tries < 15)
    {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(BUILTIN_LED, HIGH);
    }

    // Turn on web server and wait
    server.on("/", handlePortal);
    server.begin();

    setupIsRunning = false;
}

void setup()
{
    // Initializing BUILTIN_LED and serial
    pinMode(BUILTIN_LED, OUTPUT);
    Serial.begin(115200);

    // Start EEPROM and retrive data
    EEPROM.begin(sizeof(struct settings));
    EEPROM.get(0, user_wifi);

    // Setup access point config
    IPAddress localIP(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 0);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(localIP, gateway, subnet);

    setup_wifi();
}

void loop()
{
    // Handle web server request
    server.handleClient();

    // If not connected to wifi run wifi setup sequence
    if (WiFi.status() != WL_CONNECTED)
    {
        if (setupIsRunning == false && accesPointIsRunning == false)
        {
            setup_wifi();
        }
    }
}

void handlePortal()
{
    if (server.method() == HTTP_POST)
    {
        // Parse data from HTTP POST request
        strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
        strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
        user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';

        // Put data to EEPROM memory
        EEPROM.put(0, user_wifi);
        EEPROM.commit();

        // Rerun wifi setup sequence after getting new data
        setup_wifi();

        server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!</main></body></html>");
    }
    else
    {
        server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style> </head> <body><main class='form-signin'> <form action='/' method='post'> <h1 class=''>Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'></p></form></main> </body></html>");
    }
}