#include <LedControl.h>

int din = 13;
int clk = 11;
int cs = 12;
int num_of_displays = 2;

LedControl lc = LedControl(din, clk, cs, num_of_displays); // DIN, CLK, CS, # of MAX7219

void setupDisplay(int displayNum) {
  lc.shutdown(displayNum, false);
  lc.setIntensity(displayNum, 8);
  lc.clearDisplay(displayNum);

}

void setNumber(long num, int start, int displayNum){

  String num_string = String(num);
  int i = 0;
  // Serial.println(num_string);
  // Serial.println(num);


  for (char c: num_string) {
    delay(1000);

    int digit = 7-i+start;
    int value = c - '0';

    Serial.println(i+start);


    lc.setDigit(displayNum, digit, value, false); // show 7 on digit 0
    i++;
    delay(1000);
  }
}


void setup() {

  delay(1000);
  setupDisplay(0);
  setupDisplay(1);

  delay(1000);

  Serial.begin(9600);
  Serial.setTimeout(50);
  Serial.println("Hello world");

  // setNumber(12345678, 0, 0);
  // setNumber(87654321, 0, 1);

  // lc.setDigit(0, 0, 8, false); // show 7 on digit 0
  // lc.setDigit(0, 1, 7, false); // show 7 on digit 0
  // lc.setDigit(0, 2, 6, false); // show 7 on digit 0

  // delay(30);

  // lc.setDigit(0, 3, 5, false); // show 7 on digit 0
  // lc.setDigit(0, 4, 4, false); // show 7 on digit 0
  // lc.setDigit(0, 5, 3, false); // show 7 on digit 0
  // lc.setDigit(0, 6, 2, false); // show 7 on digit 0
  // lc.setDigit(0, 7, 1, false); // show 7 on digit 0


}

int parse2(const String& s, int idx) {
  if (idx + 1 >= s.length()) return -1;
  char a = s[idx];
  char b = s[idx + 1];
  if (a < '0' || a > '9') return -1;
  if (b < '0' || b > '9') return -1;
  return (a - '0') * 10 + (b - '0');
}


void loop() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil(';');
  line.trim();

  
  // ---- CPU + MEM + GPU ----
  int cpuPos = line.indexOf("CPU ");
  int memPos = line.indexOf("MEM ");
  int gpuPos = line.indexOf("GPU ");

  if (cpuPos >= 0) {
    int cpu = parse2(line, cpuPos + 4);
    if (cpu >= 0) {
      lc.setDigit(0, 5, cpu / 10, false);
      lc.setDigit(0, 4, cpu % 10, false);
    }
  }

  if (memPos >= 0) {
    int mem = parse2(line, memPos + 4);
    if (mem >= 0) {
      lc.setDigit(0, 2, mem / 10, false);
      lc.setDigit(0, 1, mem % 10, false);
    }
  }

  if (gpuPos >= 0) {
    int gpu = parse2(line, gpuPos + 4);
    if (gpu >= 0) {
      lc.setDigit(1, 2, gpu / 10, false);
      lc.setDigit(1, 1, gpu % 10, false);
    }
  }

  // ---- TEMP ----
  int tmpPos = line.indexOf("TMP ");
  if (tmpPos >= 0) {
    int tmp = parse2(line, tmpPos + 4);
    if (tmp >= 0) {
      lc.setDigit(1, 5, tmp / 10, false);
      lc.setDigit(1, 4, tmp % 10, false);
    }
  }
}
