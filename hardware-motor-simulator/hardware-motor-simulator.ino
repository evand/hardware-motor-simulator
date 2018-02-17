/*
 * Hardware Motor Simulator
 * 
 * This code runs on the P2/V4.4 Hardware Motor Simulator
 * 
 * All of the important code is in .cpp files in this directory.
 * This main program simply calls the various modules.
 */
void setup() {
  Serial.begin();
  log_init();
  lcd_init();
}

void loop() {
  // put your main code here, to run repeatedly:

}
