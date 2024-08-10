#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include "time.h"
#include <AnimatedGIF.h>
#include "images/hyperspace.h"
#define GIF_IMAGE hyperspace

AnimatedGIF gif;
// Replace with your network credentials
const char* ssid = "tatooine";
const char* password = "behringer";

// NTP server settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -25200; // Adjust for your timezone (e.g., -18000 for EST)
const int daylightOffset_sec = 3600; // Adjust for daylight savings if applicable

TFT_eSPI tft = TFT_eSPI();

// Constants for watch face
#define CENTER_X 120
#define CENTER_Y 120
#define RADIUS 100

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  gif.begin(BIG_ENDIAN_PIXELS);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // tft.println("Connecting to WiFi...");
    drawMessageCenter("Starting up...");
    if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
    {
      Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
      tft.startWrite();
      while (gif.playFrame(true, NULL))
      {
        yield();
      }
      gif.close();
      tft.endWrite(); 
    }
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  tft.fillScreen(TFT_BLACK); //screen clear
  drawWatchFace();
}

void loop() {
  drawTime();
  delay(1000);
}

void drawWatchFace() {
  tft.drawCircle(CENTER_X, CENTER_Y, RADIUS, TFT_WHITE);

  for (int i = 0; i < 12; i++) {
    float angle = i * 30 * DEG_TO_RAD;
    int x0 = CENTER_X + (RADIUS - 10) * cos(angle);
    int y0 = CENTER_Y + (RADIUS - 10) * sin(angle);
    int x1 = CENTER_X + RADIUS * cos(angle);
    int y1 = CENTER_Y + RADIUS * sin(angle);
    tft.drawLine(x0, y0, x1, y1, TFT_WHITE);
  }
}

void drawTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(2);
    tft.println("Time not set");
    return;
  }

  int hours = timeinfo.tm_hour % 12;
  int minutes = timeinfo.tm_min;
  int seconds = timeinfo.tm_sec;

  tft.fillCircle(CENTER_X, CENTER_Y, RADIUS - 19, TFT_BLACK);

  float secAngle = seconds * 6 * DEG_TO_RAD;
  float minAngle = minutes * 6 * DEG_TO_RAD + seconds * 0.1 * DEG_TO_RAD;
  float hourAngle = hours * 30 * DEG_TO_RAD + minutes * 0.5 * DEG_TO_RAD;

  int hx = CENTER_X + (RADIUS - 50) * cos(hourAngle);
  int hy = CENTER_Y + (RADIUS - 50) * sin(hourAngle);
  tft.drawLine(CENTER_X, CENTER_Y, hx, hy, TFT_RED);

  int mx = CENTER_X + (RADIUS - 30) * cos(minAngle);
  int my = CENTER_Y + (RADIUS - 30) * sin(minAngle);
  tft.drawLine(CENTER_X, CENTER_Y, mx, my, TFT_GREEN);

  int sx = CENTER_X + (RADIUS - 20) * cos(secAngle);
  int sy = CENTER_Y + (RADIUS - 20) * sin(secAngle);
  tft.drawLine(CENTER_X, CENTER_Y, sx, sy, TFT_BLUE);
}

void drawMessageCenter(const char* text) {
  // Get screen dimensions
  int screenWidth = tft.width();
  int screenHeight = tft.height();
  
  // Calculate approximate width and height of the text
  int textWidth = strlen(text) * 6 * 2;  // 6 is the width of the default character, 2 is the text size
  int textHeight = 8 * 2;  // 8 is the height of the default character, 2 is the text size

  // Calculate center position
  int centerX = (screenWidth - textWidth) / 2;
  int centerY = (screenHeight - textHeight) / 2;

  // Set cursor to center
  tft.setCursor(centerX, centerY);

  // Print text to the screen
  tft.println(text);
}
