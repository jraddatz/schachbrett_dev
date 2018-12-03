void setup() {
    pinMode(PE_0, OUTPUT); 

}

int state = 0;
void loop() {
  delay(3);
  if (state == 0)
  {
    digitalWrite(PE_0, HIGH);
    state = 1;
  } else {
    digitalWrite(PE_0, LOW);
    state = 0;
  }
  
}
