#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <iostream>
#include "matrixMod.h"
using namespace std;
// Konfigurasi pin
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 21
#define G2_PIN 22
#define B2_PIN 23
#define A_PIN 12
#define B_PIN 16
#define C_PIN 17
#define D_PIN 18
#define LAT_PIN 32
#define OE_PIN 33
#define CLK_PIN 15

#define PANEL_RES_X 128 // Resolusi horizontal
#define PANEL_RES_Y 16  // Resolusi vertikal
#define PANEL_CHAIN 1   // Jumlah panel yang terhubung

#define DRIVER DP3246_SM5368;
int center1 = 0;
int center2 = 0;
int center3 = 0;
int center4 = 0;

String value1;
String value2;
String value3;
String value4;

HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
    A_PIN, B_PIN, C_PIN, D_PIN, -1, LAT_PIN, OE_PIN, CLK_PIN};

////////////////////////////

unsigned long lastmillisbutton;
void setup()
{
  Serial.begin(9600);
  HUB75_I2S_CFG mxconfig(
      PANEL_RES_X, // Lebar panel
      PANEL_RES_Y, // Tinggi panel
      PANEL_CHAIN, // Jumlah panel
      _pins        // Konfigurasi pin
  );

  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(255); // 0-255
  dma_display->clearScreen();

  myBLACK = dma_display->color565(0, 0, 0);
  myWHITE = dma_display->color565(255, 255, 255);
  myRED = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE = dma_display->color565(0, 0, 255);
  drawline();
}

void loop()
{

  if (Serial.available())
  {

    String valuee;
    int bole = 0;
    byte bufer[64];
    int len = Serial.readBytes(bufer, 64);
    drawline();
    for (int i = 0; i < len; i++)
    {

      if (bufer[i] == 40 && bufer[i + 1] == 112 && bufer[i + 2] == 105 && bufer[i + 3] == 110 && bufer[i + 4] == 103 && bufer[i + 5] == 41)
      {
        bole = 1;
        Serial.println("Modul Matrix Active");
      }

      if (bufer[i] == 40 && bufer[i + 1] == 99 && bufer[i + 2] == 108 && bufer[i + 3] == 101 && bufer[i + 4] == 97 && bufer[i + 5] == 114 && bufer[i + 6] == 41)
      {
        bole = 2;
        // Serial.println("clear");
        dma_display->clearScreen();
        delay(10);
        drawline();
      }

      if (bufer[i] == 40 && bufer[i + 1] == 66 && bufer[i + 2] == 44)
      {
        bole = 3;
        for (int j = i + 3; bufer[j] != 41; j++)
        {
          char charbuf = (char)bufer[j];
          String strbuf = String(charbuf);
          valuee += strbuf;
        }
        dma_display->setBrightness8(valuee.toInt()); // 0-255
      }

      // PANEL 1
      if (bufer[i] == 40 && bufer[i + 1] == 49 && bufer[i + 2] == 44)
      {
        bole = 4;
        drawText5x7(value1, 0 + center1 + 2, 9, dma_display->color565(0, 0, 0));
        center1 = 0;
        value1 = "";
        for (int j = i + 3; bufer[j] != 41; j++)
        {
          center1 += 6;
          char charbuf = (char)bufer[j];
          String strbuf = String(charbuf);
          value1 += strbuf;
        }
        center1 = ((30 / 2) - (center1 / 2));
        drawText5x7(value1, 0 + center1 + 2, 9, dma_display->color565(255, 255, 255));
        // Serial.println("{ \"led1\" : true }");
      }

      // PANEL 2
      if (bufer[i] == 40 && bufer[i + 1] == 50 && bufer[i + 2] == 44)
      {
        bole = 4;
        drawText5x7(value2, 33 + center2 - 2, 8, dma_display->color565(0, 0, 0));
        center2 = 0;
        value2 = "";
        for (int j = i + 3; bufer[j] != 41; j++)
        {
          center2 += 5;
          char charbuf = (char)bufer[j];
          String strbuf = String(charbuf);
          value2 += strbuf;
        }
        center2 = ((30 / 2) - (center2 / 2));
        drawText5x7(value2, 33 + center2 - 2, 8, dma_display->color565(255, 255, 255));
        // Serial.println("{ \"led2\" : true }");
      }

      // PANEL 3
      if (bufer[i] == 40 && bufer[i + 1] == 51 && bufer[i + 2] == 44)
      {
        bole = 4;
        drawText5x7(value3, 0 + center3 - 1, 25, dma_display->color565(0, 0, 0));
        center3 = 0;
        value3 = "";
        for (int j = i + 3; bufer[j] != 41; j++)
        {
          center3 += 5;
          char charbuf = (char)bufer[j];
          String strbuf = String(charbuf);
          value3 += strbuf;
        }
        center3 = ((30 / 2) - (center3 / 2));
        drawText5x7(value3, 0 + center3 - 1, 25, dma_display->color565(255, 255, 255));
        // Serial.println("{ \"led3\" : true }");
      }

      // PANEL 4
      if (bufer[i] == 40 && bufer[i + 1] == 52 && bufer[i + 2] == 44)
      {
        bole = 4;
        drawText5x7(value4, 33 + center4 - 2, 25, dma_display->color565(0, 0, 0));
        center4 = 0;
        value4 = "";
        for (int j = i + 3; bufer[j] != 41; j++)
        {
          center4 += 5;
          char charbuf = (char)bufer[j];
          String strbuf = String(charbuf);
          value4 += strbuf;
        }
        center4 = ((30 / 2) - (center4 / 2));
        drawText5x7(value4, 33 + center4 - 2, 25, dma_display->color565(255, 255, 255));
        // Serial.println("{ \"led4\" : true }");
      }
    }
    switch (bole)
    {
    case 1:
      break;
    case 2:
      Serial.print("{ \"clear\" : true }");
      break;
    case 3:
      Serial.print("{ \"Brightness\" : " + valuee + " }");
      break;
    case 4:
      Serial.print("{ \"led\" : true }");
      break;
    default:
      Serial.print("{ \"Error\" : Format command not found }");
      break;
    }
    // if (bole)
    // {
    //   Serial.println("{ \"led4\" : true }");
    // }
    // else
    // {
    //   Serial.println("{ \"Error\" : Format command not found }")
    // }
  }
}
