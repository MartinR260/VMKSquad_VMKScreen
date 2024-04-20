#include <SPI.h>
#include <UsbFat.h>
#include <masstorage.h>
#include <U8glib.h>
#include <string.h>

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE); 
// USB host objects.
USB usb;
BulkOnly bulk(&usb);

// File system.
UsbFat fs(&bulk);

#define BUTTON_UP 3
#define BUTTON_DOWN 2

int displayOffset = 0; 
int cursorPosition = 0; // Cursor position

//------------------------------------------------------------------------------
void setup() {
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  Serial.begin(9600);
  u8g.setFont(u8g_font_8x13);
  u8g.firstPage();
  
  while (!initUSB(&usb)) {  }
  
  if (!fs.begin()) {
    Serial.println(F("File system initialization failed"));
    return;
  }
  
  Serial.println(F("USB Mass Storage device initialized"));
  
  do {
    printFilesAndDirectories("/");
  } while( u8g.nextPage() );
}

void loop() {
  // Handle cursor movement with button presses
  if (digitalRead(BUTTON_UP) == LOW) {

      displayOffset -= 6; // Scroll up to show previous 6 elements
      if (displayOffset < 0) displayOffset = 0;
  }
  if (digitalRead(BUTTON_DOWN) == LOW) {
    displayOffset += 6; // Scroll up to show previous 6 elements
  }

  // Update display
  u8g.firstPage();
  do {
    printFilesAndDirectories("/");
  } while(u8g.nextPage());

  delay(50);
}

void printFilesAndDirectories(const char *path) {
  //Serial.println(F("Files and directories:"));
  
  File dir = fs.open(path);
  if (!dir) {
    Serial.println(F("Failed to open directory"));
    return;
  }

  int i = 0; 
  int currentPosition = 0; // Track current position
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;

    char fileName[256];
    entry.getName(fileName, sizeof(fileName));
    if (currentPosition >= displayOffset && currentPosition < displayOffset + 6) {
      // Draw file or directory indicator
      if(entry.isDirectory()) {
        u8g.drawStr(0, i + 9, "[D] ");
      } else {
        u8g.drawStr(0, i + 9, "[F] ");
      }
      u8g.drawStr(30, i + 9, fileName);
      i += 10;
    }
    // if(entry.isDirectory()) {
    //   char new_path[256];
    //   snprintf(new_path, sizeof(new_path), "%s/%s", path, fileName);
    //   printFilesAndDirectories(new_path);
    // }
    currentPosition++; // Increment current position
    entry.close();
  }
  dir.close();
}