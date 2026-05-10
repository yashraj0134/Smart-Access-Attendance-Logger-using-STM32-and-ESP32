#define BLYNK_TEMPLATE_ID "TMPL36bAqCuIl"
#define BLYNK_TEMPLATE_NAME "RFID BASED ATTENDENCE"
#define BLYNK_AUTH_TOKEN "7zoopszf2i5j5EDggyv2k5HKLNWjw4R0"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <HardwareSerial.h>
#include <HTTPClient.h>

char ssid[] = "Yashraj";
char pass[] = "00010304";

HardwareSerial mySerial(1);

String rxData = "";

String nameData;
String tagData;
String dateData;
String timeData;
String statusData;

int lineCount = 0;

void setup()
{
    Serial.begin(115200);

    // UART1
    // RX = GPIO16
    // TX = GPIO17
    mySerial.begin(115200, SERIAL_8N1, 16, 17);

    Serial.println("UART Ready");

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    while(Blynk.connected() == false)
    {
    }

    Serial.println("Blynk Connected");
}

void loop()
{
    Blynk.run();

    while(mySerial.available())
    {
        char data = mySerial.read();

        if(data == '\n')
        {
            rxData.trim();

            Serial.println("Received:");
            Serial.println(rxData);

            storeData(rxData);

            rxData = "";
        }
        else if(data != '\r')
        {
            rxData += data;
        }
    }
}

void storeData(String data)
{
    lineCount++;

    if(lineCount == 1)
    {
        nameData = data;
    }
    else if(lineCount == 2)
    {
        tagData = data;
    }
    else if(lineCount == 3)
    {
        dateData = data;
    }
    else if(lineCount == 4)
    {
        timeData = data;
    }
    else if(lineCount == 5)
    {
        statusData = data;

        sendToBlynk();

        lineCount = 0;
    }
}

void sendToBlynk()
{
    Serial.println("Sending To Blynk");

    Blynk.virtualWrite(V0, nameData);
    delay(200);

    Blynk.virtualWrite(V1, tagData);
    delay(200);

    Blynk.virtualWrite(V2, dateData);
    delay(200);

    Blynk.virtualWrite(V3, timeData);
    delay(200);

    Blynk.virtualWrite(V4, statusData);
    delay(200);


    sendToGoogleSheet();
    // LED indication

    if(statusData == "Check-In")
    {
        Blynk.setProperty(V5, "color", "#00FF00");

        Blynk.virtualWrite(V5, 1);
    }
    else if(statusData == "Check-Out")
    {
        Blynk.setProperty(V5, "color", "#FF0000");

        Blynk.virtualWrite(V5, 1);
    }

    Serial.println("Data Sent");
}


void sendToGoogleSheet()
{
    if(WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        String url = "https://script.google.com/macros/s/AKfycbyosetG8ZSfl4bedr1ol4YRYKY9IC5lHvDRLTyp9ZFwU-AJ-tX-LMOcGXyHIJFeMTnj/exec";

        url = url + "?name=" + nameData;
        url = url + "&tag=" + tagData;
        url = url + "&date=" + dateData;
        url = url + "&time=" + timeData;
        url = url + "&status=" + statusData;
        
        url.replace(" ", "%20");

        Serial.println(url);

        http.begin(url);

        int httpCode = http.GET();

        if(httpCode > 0)
        {
            Serial.print("HTTP Responce :");
            Serial.println(httpCode);
        }

        else
        {
             Serial.print("HTTP error"); 
        }

        http.end();
    }
}