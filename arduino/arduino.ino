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
  char a = s[idx];
  char b = s[idx + 1];
  char c = s[idx + 2];
  if (a < '0' || a > '9') return -1;
  if (b < '0' || b > '9') return -1;
  if (c < '0' || c > '9') return -1;
  return (a - '0') * 100 + (b - '0') * 10 + (c - '0');
}

void setup() {
  delay(500);

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

  // ---- CPU (display 0, right) ----
  if (cpuPos >= 0) {
    int cpu = parse3(line, cpuPos + 4);
    if (cpu >= 0) {
      lc.setDigit(0, 6, cpu / 100, false);
      lc.setDigit(0, 5, (cpu / 10) % 10, false);
      lc.setDigit(0, 4, cpu % 10, false);
    }
  }

  // ---- MEM (display 0, left) ----
  if (memPos >= 0) {
    int mem = parse3(line, memPos + 4);
    if (mem >= 0) {
      lc.setDigit(0, 2, mem / 100, false);
      lc.setDigit(0, 1, (mem / 10) % 10, false);
      lc.setDigit(0, 0, mem % 10, false);
    }
  }

  // ---- GPU (display 1, SAME AS MEM) ----
  if (gpuPos >= 0) {
    int gpu = parse3(line, gpuPos + 4);
    if (gpu >= 0) {
      lc.setDigit(1, 2, gpu / 100, false);
      lc.setDigit(1, 1, (gpu / 10) % 10, false);
      lc.setDigit(1, 0, gpu % 10, false);
    }
  }

  // ---- TEMP (display 1, right) ----
  if (tmpPos >= 0) {
    int tmp = parse3(line, tmpPos + 4);
    if (tmp >= 0) {
      lc.setDigit(1, 6, tmp / 100, false);
      lc.setDigit(1, 5, (tmp / 10) % 10, false);
      lc.setDigit(1, 4, tmp % 10, false);
    }
  }
}
