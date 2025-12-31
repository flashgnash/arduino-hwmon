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

// ---- DISPLAY 3 DIGITS WITH LEADING ZERO BLANK ----
void show3(int disp, int d2, int d1, int d0, int val) {
  int h = val / 100;
  int t = (val / 10) % 10;
  int o = val % 10;

  if (h == 0)
    lc.setChar(disp, d2, ' ', false);
  else
    lc.setDigit(disp, d2, h, false);

  if (h == 0 && t == 0)
    lc.setChar(disp, d1, ' ', false);
  else
    lc.setDigit(disp, d1, t, false);

  lc.setDigit(disp, d0, o, false);
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
    int v = parse3(line, cpuPos + 4);
    if (v >= 0) show3(0, 6, 5, 4, v);
  }

  if (memPos >= 0) {
    int v = parse3(line, memPos + 4);
    if (v >= 0) show3(0, 2, 1, 0, v);
  }

  if (gpuPos >= 0) {
    int v = parse3(line, gpuPos + 4);
    if (v >= 0) show3(1, 2, 1, 0, v);
  }

  if (tmpPos >= 0) {
    int v = parse3(line, tmpPos + 4);
    if (v >= 0) show3(1, 6, 5, 4, v);
  }
}
