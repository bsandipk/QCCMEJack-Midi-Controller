/**
 * --------------------------------------------------------
 * Modified from @auth RobertoHE 
 * modified 
 * --------------------------------------------------------
 */

//#include <Arduino.h>
#include <BLEMIDI_Transport.h>
//#include <MIDI.h>
//#include <hardware/BLEMIDI_Client_ESP32.h>

#include <hardware/BLEMIDI_ESP32_NimBLE.h>
//#include <hardware/BLEMIDI_ESP32.h>
//#include <hardware/BLEMIDI_nRF52.h>
//#include <hardware/BLEMIDI_ArduinoBLE.h>

//BLEMIDI_CREATE_DEFAULT_INSTANCE(); //Connect to first server found
//BLEMIDI_CREATE_INSTANCE("",MIDI)                  //Connect to the first server found
//BLEMIDI_CREATE_INSTANCE("fc:54:9a:99:67:9f",MIDI) //Connect to a specific BLE address server
BLEMIDI_CREATE_INSTANCE("QCWIDI",MIDI)       //Connect to a specific name server

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 //modify for match with yout board
#endif

void ReadCB(void *parameter);       //Continuos Read function (See FreeRTOS multitasks)

unsigned long t0 = millis();
bool isConnected = false;

/**
 * -----------------------------------------------------------------------------
 * When BLE is connected, LED will turn on (indicating that connection was successful)
 * When receiving a NoteOn, LED will go out, on NoteOff, light comes back on.
 * This is an easy and conveniant way to show that the connection is alive and working.
 * -----------------------------------------------------------------------------
*/
void setup()
{
  Serial.begin(115200);
  MIDI.begin(1);

  BLEMIDI.setHandleConnected([]()
                             {
                               Serial.println("---------CONNECTED---------");
                               isConnected = true;
                               digitalWrite(LED_BUILTIN, HIGH);
                             });

  BLEMIDI.setHandleDisconnected([]()
                                {
                                  Serial.println("---------NOT CONNECTED---------");
                                  isConnected = false;
                                  digitalWrite(LED_BUILTIN, LOW);
                                });

  

  xTaskCreatePinnedToCore(ReadCB,           //See FreeRTOS for more multitask info  
                          "MIDI-READ",
                          3000,
                          NULL,
                          1,
                          NULL,
                          1); //Core0 or Core1

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  if (isConnected) // && (millis() - t0) > 1000)
  {
    MIDI.sendControlChange(44,127,1); // this is not sending any control change (tap temppo on my gear), I have tried program change too
    vTaskDelay(250/portTICK_PERIOD_MS);
    //MIDI.sendNoteOff(60, 0, 1);
  }
}

void ReadCB(void *parameter)
{
//  Serial.print("READ Task is started on core: ");
//  Serial.println(xPortGetCoreID());
  for (;;)
  {
    MIDI.read(); 
    vTaskDelay(1 / portTICK_PERIOD_MS); //Feed the watchdog of FreeRTOS.
   }
  vTaskDelay(1);
}
