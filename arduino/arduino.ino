#include <LedControl.h>

int din = 13;
int clk = 11;
int cs  = 12;
int num_of_displays = 2;

LedControl lc = LedControl(din, clk, cs, num_of_displays);

// ---- SETUP DISPLAY ----
void setupDisplay(int d) {
  lc.shutdown(d, false);
  lc.setIntensity(d, 8);
  lc.clearDisplay(d);
}

// ---- PARSE 3 DIGITS ----
int parse3(const String& s, int idx) {
  if (idx + 2 >= s.length()) return -1;
  char a = s[idx], b = s[idx+1], c = s[idx+2];
  if (!isDigit(a) || !isDigit(b) || !isDigit(c)) return -1;
  return (a - '0') * 100 + (b - '0') * 10 + (c - '0');
}

// Display 3-digit number aligned left starting at "start" digit
void show3(int disp, int start, int val) {
  int h = val / 100;
  int t = (val / 10) % 10;
  int o = val % 10;

  // Decide which digits to write based on value
  if (h > 0) {
    lc.setDigit(disp, start, h, false);
    lc.setDigit(disp, start + 1, t, false);
    lc.setDigit(disp, start + 2, o, false);
  } else if (t > 0) {
    lc.setChar(disp, start, ' ', false);
    lc.setDigit(disp, start + 1, t, false);
    lc.setDigit(disp, start + 2, o, false);
  } else {
    lc.setChar(disp, start, ' ', false);
    lc.setChar(disp, start + 1, ' ', false);
    lc.setDigit(disp, start + 2, o, false);
  }
}

void setup() {
  setupDisplay(0);
  setupDisplay(1);


  Serial.begin(9600);
  Serial.setTimeout(50);
}

// ---- MAIN LOOP ----
void loop() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil(';');
  line.trim();

  int cpuPos = line.indexOf("CPU ");
  int memPos = line.indexOf("MEM ");
  int gpuPos = line.indexOf("GPU ");
  int tmpPos = line.indexOf("TMP ");

  if (cpuPos >= 0) {
    lc.setRow(0,7,0x4E); // C
    int v = parse3(line, cpuPos + 4);
    if (v >= 0) show3(0, 4, v); // start at digit 4
  }

  if (memPos >= 0) {
    lc.setRow(0,3,0x46); // R
    int v = parse3(line, memPos + 4);
    if (v >= 0) show3(0, 0, v); // start at digit 0
  }

  if (gpuPos >= 0) {
    lc.setRow(1,3,0x3E); // U
    int v = parse3(line, gpuPos + 4);
    if (v >= 0) show3(1, 0, v); // start at digit 0
  }

  if (tmpPos >= 0) {
    lc.setChar(1,7,'H',false); // TEMP
    int v = parse3(line, tmpPos + 4);
    if (v >= 0) show3(1, 4, v); // start at digit 4
  }

}
