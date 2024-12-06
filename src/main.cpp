#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <iostream>
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

HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
    A_PIN, B_PIN, C_PIN, D_PIN, -1, LAT_PIN, OE_PIN, CLK_PIN};

// MatrixPanel_I2S_DMA *dma_display;

// Example sketch which shows how to display some patterns
// on a 64x32 LED matrix
//

// MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK, myWHITE, myRED, myGREEN, myBLUE;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos)
{
  if (pos < 85)
  {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  }
  else if (pos < 170)
  {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  }
  else
  {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

void drawText(int colorWheelOffset)
{

  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(5, 0); // start at top left, with 8 pixel of spacing
  uint8_t w = 0;
  const char *str = "ESP32";
  for (w = 0; w < strlen(str); w++)
  {
    dma_display->setTextColor(colorWheel((w * 32) + colorWheelOffset));
    dma_display->print(str[w]);
  }

  dma_display->println();

  dma_display->println();

  dma_display->setTextColor(dma_display->color444(15, 15, 15));
  dma_display->println("LED");
  // Jump a half character
  dma_display->setCursor(34, 24);
  dma_display->setTextColor(dma_display->color444(0, 15, 15));
  dma_display->print("*");
  dma_display->setTextColor(dma_display->color444(15, 0, 0));
  dma_display->print('R');
  dma_display->setTextColor(dma_display->color444(0, 15, 0));
  dma_display->print('G');
  dma_display->setTextColor(dma_display->color444(0, 0, 15));
  dma_display->print("B");
  dma_display->setTextColor(dma_display->color444(15, 0, 8));
  dma_display->println("*");
}

int boot;
bool tf;

void errdrawPixel(int x, int y, uint16_t color)
{
  // Hitung posisi pixel berdasarkan susunan 32x64
  int panelX = x;
  int panelY = y;
  int panelIndex = 0;

  if (y <= 7)
  {
    // panelY = y + 8;
    panelX = x + 64;
    dma_display->drawPixel(panelX, panelY, color);
  }
  else if (y <= 15)
  {
    panelY = y - 8;
    dma_display->drawPixel(panelX, panelY, color);
  }
  else if (y <= 23)
  {
    panelY = y - 8;
    panelX = x + 64;
    dma_display->drawPixel(panelX, panelY, color);
  }
  else if (y <= 31)
  {
    panelY = y - 16;
    dma_display->drawPixel(panelX, panelY, color);
  }
}
////////////////////////////
const uint8_t font[36][5] = {
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x41, 0x3E}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x41, 0x41, 0x7F, 0x41, 0x41}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x62, 0x51, 0x49, 0x49, 0x46}, // 2
    {0x22, 0x41, 0x49, 0x49, 0x36}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}  // 9
};

const uint8_t smallFont[36][3] = {
    {0x1F, 0x05, 0x1F}, // A
    {0x1F, 0x15, 0x0A}, // B
    {0x0E, 0x11, 0x11}, // C
    {0x1F, 0x11, 0x0E}, // D
    {0x1F, 0x15, 0x15}, // E
    {0x1F, 0x05, 0x05}, // F
    {0x0E, 0x11, 0x1D}, // G
    {0x1F, 0x04, 0x1F}, // H
    {0x11, 0x1F, 0x11}, // I
    {0x08, 0x10, 0x0F}, // J
    {0x1F, 0x04, 0x1B}, // K
    {0x1F, 0x10, 0x10}, // L
    {0x1F, 0x06, 0x1F}, // M
    {0x1F, 0x0E, 0x1F}, // N
    {0x0E, 0x11, 0x0E}, // O
    {0x1F, 0x05, 0x02}, // P
    {0x0E, 0x11, 0x1E}, // Q
    {0x1F, 0x05, 0x1A}, // R
    {0x12, 0x15, 0x09}, // S
    {0x01, 0x1F, 0x01}, // T
    {0x0F, 0x10, 0x0F}, // U
    {0x07, 0x18, 0x07}, // V
    {0x1F, 0x0C, 0x1F}, // W
    {0x1B, 0x04, 0x1B}, // X
    {0x03, 0x1C, 0x03}, // Y
    {0x19, 0x15, 0x13}, // Z
    {0x0E, 0x11, 0x0E}, // 0
    {0x00, 0x1F, 0x00}, // 1
    {0x19, 0x15, 0x12}, // 2
    {0x11, 0x15, 0x0A}, // 3
    {0x07, 0x04, 0x1F}, // 4
    {0x17, 0x15, 0x09}, // 5
    {0x1E, 0x15, 0x1D}, // 6
    {0x01, 0x01, 0x1F}, // 7
    {0x1F, 0x15, 0x1F}, // 8
    {0x17, 0x15, 0x0F}  // 9
};

const uint8_t font4x6[36][4] = {
    {0x1E, 0x9, 0x9, 0x1E},   // A
    {0x1F, 0x15, 0x15, 0x0A}, // B
    {0x0E, 0x11, 0x11, 0x11}, // C
    {0x1F, 0x11, 0x11, 0x0E}, // D
    {0x1F, 0x15, 0x15, 0x15}, // E
    {0x1F, 0x05, 0x05, 0x05}, // F
    {0x0E, 0x11, 0x15, 0x0D}, // G
    {0x1F, 0x04, 0x04, 0x1F}, // H
    {0x11, 0x1F, 0x11, 0x00}, // I
    {0x08, 0x10, 0x10, 0x0F}, // J
    {0x1F, 0x04, 0x0A, 0x11}, // K
    {0x1F, 0x10, 0x10, 0x10}, // L
    {0x1F, 0x02, 0x04, 0x02}, // M
    {0x1F, 0x02, 0x04, 0x1F}, // N
    {0x0E, 0x11, 0x11, 0x0E}, // O
    {0x1F, 0x05, 0x05, 0x02}, // P
    {0x0E, 0x11, 0x19, 0x16}, // Q
    {0x1F, 0x05, 0x05, 0x1A}, // R
    {0x12, 0x15, 0x15, 0x09}, // S
    {0x01, 0x1F, 0x01, 0x01}, // T
    {0x0F, 0x10, 0x10, 0x0F}, // U
    {0x07, 0x18, 0x18, 0x07}, // V
    {0x1F, 0x08, 0x08, 0x1F}, // W
    {0x1B, 0x04, 0x04, 0x1B}, // X
    {0x03, 0x1C, 0x1C, 0x03}, // Y
    {0x19, 0x15, 0x15, 0x13}, // Z
    {0x0E, 0x11, 0x11, 0x0E}, // 0
    {0x00, 0x12, 0x1F, 0x10}, // 1
    {0x19, 0x15, 0x15, 0x12}, // 2
    {0x11, 0x15, 0x15, 0x0A}, // 3
    {0x07, 0x04, 0x1F, 0x04}, // 4
    {0x17, 0x15, 0x15, 0x09}, // 5
    {0x0E, 0x15, 0x15, 0x09}, // 6
    {0x01, 0x01, 0x1F, 0x01}, // 7
    {0x0A, 0x15, 0x15, 0x0A}, // 8
    {0x12, 0x15, 0x15, 0x0E}  // 9
};
const uint8_t font5x5[36][5] = {
    {0x1E, 0x05, 0x05, 0x05, 0x1E}, // A
    {0x1F, 0x15, 0x15, 0x15, 0x0A}, // B
    {0x0E, 0x11, 0x11, 0x11, 0x00}, // C
    {0x1F, 0x11, 0x11, 0x11, 0x0E}, // D
    {0x1F, 0x15, 0x15, 0x15, 0x11}, // E
    {0x1F, 0x05, 0x05, 0x05, 0x01}, // F
    {0x0E, 0x11, 0x15, 0x15, 0x1C}, // G
    {0x1F, 0x04, 0x04, 0x04, 0x1F}, // H
    {0x11, 0x1F, 0x11, 0x11, 0x00}, // I
    {0x10, 0x10, 0x11, 0x11, 0x0F}, // J
    {0x1F, 0x04, 0x0A, 0x11, 0x00}, // K
    {0x1F, 0x10, 0x10, 0x10, 0x10}, // L
    {0x1F, 0x02, 0x04, 0x02, 0x1F}, // M
    {0x1F, 0x02, 0x04, 0x08, 0x1F}, // N
    {0x0E, 0x11, 0x11, 0x11, 0x0E}, // O
    {0x1F, 0x05, 0x05, 0x05, 0x02}, // P
    {0x0E, 0x11, 0x11, 0x19, 0x1E}, // Q
    {0x1F, 0x05, 0x05, 0x0D, 0x12}, // R
    {0x12, 0x15, 0x15, 0x15, 0x09}, // S
    {0x01, 0x01, 0x1F, 0x01, 0x01}, // T
    {0x0F, 0x10, 0x10, 0x10, 0x0F}, // U
    {0x07, 0x08, 0x10, 0x08, 0x07}, // V
    {0x1F, 0x08, 0x04, 0x08, 0x1F}, // W
    {0x11, 0x0A, 0x04, 0x0A, 0x11}, // X
    {0x01, 0x02, 0x1C, 0x02, 0x01}, // Y
    {0x11, 0x19, 0x15, 0x13, 0x11}, // Z
    {0x0E, 0x11, 0x11, 0x11, 0x0E}, // 0
    {0x00, 0x12, 0x1F, 0x10, 0x00}, // 1
    {0x19, 0x15, 0x15, 0x15, 0x12}, // 2
    {0x11, 0x15, 0x15, 0x15, 0x0A}, // 3
    {0x07, 0x04, 0x1F, 0x04, 0x04}, // 4
    {0x17, 0x15, 0x15, 0x15, 0x09}, // 5
    {0x1E, 0x15, 0x15, 0x15, 0x08}, // 6
    {0x01, 0x01, 0x1D, 0x03, 0x01}, // 7
    {0x0A, 0x15, 0x15, 0x15, 0x0A}, // 8
    {0x02, 0x15, 0x15, 0x15, 0x0E}  // 9
};

// Fungsi untuk menggambar pixel pada konfigurasi 32x64

// Fungsi untuk menggambar satu karakter dengan ukuran yang dapat diubah
void drawChar(char c, int x, int y, uint16_t color, int scale = 1)
{
  if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
  {
    const uint8_t *bitmap = (c >= 'A') ? font[c - 'A'] : font[c - '0' + 26];

    for (int col = 0; col < 5; col++)
    {
      for (int row = 0; row < 7; row++)
      {
        if (bitmap[col] & (1 << row))
        {
          // Gambar piksel dengan ukuran diperbesar sesuai skala
          for (int dx = 0; dx < scale; dx++)
          {
            for (int dy = 0; dy < scale; dy++)
            {
              errdrawPixel(x + col * scale + dx, y + row * scale + dy, color);
            }
          }
        }
      }
    }
  }
}
void drawChar4x6(char c, int x, int y, uint16_t color)
{
  if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
  {
    const uint8_t *bitmap = (c >= 'A') ? font4x6[c - 'A'] : font4x6[c - '0' + 26];

    for (int col = 0; col < 4; col++)
    {
      for (int row = 0; row < 6; row++)
      {
        if (bitmap[col] & (1 << row))
        {
          errdrawPixel(x + col, y + row, color);
        }
      }
    }
  }
}
void drawText4x6(String text, int x, int y, uint16_t color)
{
  for (size_t i = 0; i < text.length(); i++)
  {
    drawChar4x6(text[i], x + i * 5, y, color); // Jarak antar karakter adalah 5 piksel
  }
}
void drawSmallChar(char c, int x, int y, uint16_t color)
{
  if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
  {
    const uint8_t *bitmap = (c >= 'A') ? smallFont[c - 'A'] : smallFont[c - '0' + 26];

    for (int col = 0; col < 3; col++)
    {
      for (int row = 0; row < 5; row++)
      {
        if (bitmap[col] & (1 << row))
        {
          errdrawPixel(x + col, y + row, color);
        }
      }
    }
  }
}
void drawSmallText(String text, int x, int y, uint16_t color)
{
  for (size_t i = 0; i < text.length(); i++)
  {
    drawSmallChar(text[i], x + i * 4, y, color); // Jarak antar karakter adalah 4 piksel
  }
}
// Fungsi untuk menggambar teks dengan ukuran yang dapat diubah
void drawText(String text, int x, int y, uint16_t color, int scale = 1)
{
  for (size_t i = 0; i < text.length(); i++)
  {
    drawChar(text[i], x + i * (5 * scale + scale), y, color, scale); // Spasi antar karakter sesuai skala
  }
}
void drawChar5x5(char c, int x, int y, uint16_t color)
{
  if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
  {
    const uint8_t *bitmap = (c >= 'A') ? font5x5[c - 'A'] : font5x5[c - '0' + 26];

    for (int col = 0; col < 5; col++)
    {
      for (int row = 0; row < 5; row++)
      {
        if (bitmap[col] & (1 << row))
        {
          errdrawPixel(x + col, y + row, color);
        }
      }
    }
  }
}
void drawText5x5(String text, int x, int y, uint16_t color)
{
  for (size_t i = 0; i < text.length(); i++)
  {
    drawChar5x5(text[i], x + i * 6, y, color); // Jarak antar karakter adalah 6 piksel
  }
}
void scrollText(String text, int speed, uint16_t color)
{
  int textWidth = text.length() * 6; // Lebar total teks dalam piksel
  int panelWidth = 64;               // Lebar panel dalam piksel
  int x = panelWidth;                // Mulai dari luar sisi kanan panel

  while (x > -textWidth)
  {
    dma_display->clearScreen();

    // Gambar teks berjalan
    drawText5x5(text, x, 0, color);

    //  dma_display->show(); // Perbarui layar
    delay(speed); // Kecepatan scrolling

    x--; // Gerakkan teks ke kiri
  }
}
void drawline()
{
  dma_display->clearScreen();
  for (int i = 0; i < 32; i++)
  {
    errdrawPixel(32, i, myRED);
    delay(1);
  }
  for (int i = 0; i < 64; i++)
  {
    errdrawPixel(i, 16, myRED);
    delay(1);
  }

  drawText5x5("P1", 0, 0, dma_display->color565(255, 255, 255));
  drawText5x5("P2", 33, 0, dma_display->color565(255, 255, 255));
  drawText5x5("P3", 0, 17, dma_display->color565(255, 255, 255));
  drawText5x5("P4", 33, 17, dma_display->color565(255, 255, 255));
}
String command;
////////////////////////////
String p1String, p2String, p3String, p4String; // Untuk menampung string setelah /p1:, /p2:, /p3:, /p4:

void setup()
{
  Serial.begin(115200);
  if (tf == 1)
  {
    tf = 0;
    boot++;
  }
  else
  {
    tf = 1;
  }
  HUB75_I2S_CFG mxconfig(
      PANEL_RES_X, // Lebar panel
      PANEL_RES_Y, // Tinggi panel
      PANEL_CHAIN, // Jumlah panel
      _pins        // Konfigurasi pin
  );

  // mxconfig.driver = HUB75_I2S_CFG::MBI5124; // Driver FM6126A
  // dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // mxconfig.gpio.e = 18;

  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;
  // mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;
  // mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_15M;
  //   mxconfig.mx_height = 32;
  //   mxconfig.mx_width = 32;
  // mxconfig.latch_blanking = 0;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); // 0-255
  dma_display->clearScreen();

  // drawText(0);

  myBLACK = dma_display->color565(0, 0, 0);
  myWHITE = dma_display->color565(255, 255, 255);
  myRED = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE = dma_display->color565(0, 0, 255);
  drawline();
}

uint8_t wheelval = 0;

int ex = 0;
int ye = 0;
int n = 0;
void loop()
{
  // Menampilkan beberapa pixel
  // errdrawPixel(0, 0, myRED);   // Pixel di panel 1
  // errdrawPixel(63, 31, myGREEN); // Pixel di panel 2
  // errdrawPixel(63, 0, myBLUE);  // Pixel di panel 3
  // errdrawPixel(0, 31, myWHITE); // Pixel di panel 4
  //
  // dma_display->drawPixel(0, 0, myRED);
  // dma_display->drawPixel(127, 0, myGREEN);
  // dma_display->drawPixel(0, 15, myBLUE);
  // dma_display->drawPixel(127,15 , myWHITE);
  //
  // errdrawPixel(0, 0, myRED);
  // errdrawPixel(127, 0, myGREEN);
  // errdrawPixel(0, 15, myBLUE);
  // dma_display->drawPixel(ex, ye, myWHITE);
  // errdrawPixel(ex, ye, myWHITE);
  //
  // // Contoh menggambar teks
  // drawText4x6("ABCDEFGHIJKL", ex, ye, dma_display->color565(255, 255, 255));
  // drawText4x6("MNOPQRSTUVWX", ex, ye + 7, dma_display->color565(255, 255, 255));
  // drawText4x6("YZ 1234567890", ex, ye + 7 + 7, dma_display->color565(255, 255, 255));
  //
  // // // Contoh menggambar teks
  //
  // String allChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // Semua karakter A-Z dan 0-9
  // uint16_t textColor = dma_display->color565(0, 255, 0);   // Warna teks (hijau)
  //
  // scrollText(allChars, 100, textColor); // Teks berjalan dengan kecepatan 50 ms per langkah
  // if (Serial.available())
  // {
  //   command = Serial.readString();
  //   if (command.find("/p1:"))
  //   {
  //     size_t pos = command.find("/p1:");
  //     command.erase(pos, 4);
  //     n = 0;
  //     Serial.println(ex + " ex");
  //   }
  //   else
  //   {
  //     Serial.println(ye + " ye");
  //   }
  //   dma_display->clearScreen();
  // }
  // Menampilkan teks
  // drawText("ESP32", myWHITE, 2, 4); // Teks di seluruh panel

  // delay(100); // Delay untuk memperbarui tampilan

  if (Serial.available())
  {
    String command ="   " + Serial.readString();

    drawText5x5(p1String, 0, 7, dma_display->color565(0, 0, 0));
    drawText5x5(p2String, 33, 7, dma_display->color565(0, 0, 0));
    drawText5x5(p3String, 0, 24, dma_display->color565(0, 0, 0));
    drawText5x5(p4String, 33, 24, dma_display->color565(0, 0, 0));
    // Kondisi pertama: jika input mengandung /p1:
    if (command.indexOf("/p1:") != 0)
    {
      size_t start = command.indexOf("/p1:") + 4;
      size_t end = command.indexOf("/p2:", start);
      p1String = command.substring(start, (end != -1) ? end : command.length());
      // Serial.println("Kondisi satu dijalankan, isi p1: " + p1String);
    }

    // Kondisi kedua: jika input mengandung /p2:
    if (command.indexOf("/p2:") != 0)
    {
      size_t start = command.indexOf("/p2:") + 4;
      size_t end = command.indexOf("/p3:", start);
      p2String = command.substring(start, (end != -1) ? end : command.length());
      // Serial.println("Kondisi dua dijalankan, isi p2: " + p2String);
    }

    // Kondisi ketiga: jika input mengandung /p3:
    if (command.indexOf("/p3:") != 0)
    {
      size_t start = command.indexOf("/p3:") + 4;
      size_t end = command.indexOf("/p4:", start);
      p3String = command.substring(start, (end != -1) ? end : command.length());
      // Serial.println("Kondisi tiga dijalankan, isi p3: " + p3String);
    }

    // Kondisi keempat: jika input mengandung /p4:
    if (command.indexOf("/p4:") != 0)
    {
      size_t start = command.indexOf("/p4:") + 4;
      p4String = command.substring(start); // Ambil string setelah "/p4:"
      // Serial.println("Kondisi empat dijalankan, isi p4: " + p4String);
    }

    // Menampilkan string yang dimasukkan setelah /p1:, /p2:, /p3:, atau /p4:      drawText5x5(p1String, 0, 7, dma_display->color565(255, 255, 255));
    // drawline();
    drawText5x5(p1String, 0, 7, dma_display->color565(255, 255, 255));
    drawText5x5(p2String, 33, 7, dma_display->color565(255, 255, 255));
    drawText5x5(p3String, 0, 24, dma_display->color565(255, 255, 255));
    drawText5x5(p4String, 33, 24, dma_display->color565(255, 255, 255));

    Serial.println("Isi command: " + command);
  }
}