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

  String l1 = Serial.readStringUntil('\n');
  String l2 = Serial.readStringUntil('\n');

  lc.clearDisplay(0);
  lc.clearDisplay(1);

  // ---- CPU + MEM line ----
  if (l1.startsWith("CPU")) {
    int cpu = parse2(l1, 4);   // CPU XX
    int mem = parse2(l1, 10);  // MEM YY

    if (cpu >= 0) {
      lc.setDigit(0, 5, cpu / 10, false);
      lc.setDigit(0, 4, cpu % 10, false);
    }

    if (mem >= 0) {
      lc.setDigit(0, 1, mem / 10, false);
      lc.setDigit(0, 2, mem % 10, false);
    }
  }

  // ---- TEMP line ----
  if (l2.startsWith("TMP")) {
    int tmp = parse2(l2, 4);   // TMP ZZ

    if (tmp >= 0) {
      lc.setDigit(1, 5, tmp / 10, false);
      lc.setDigit(1, 4, tmp % 10, false);
    }
  }
}

