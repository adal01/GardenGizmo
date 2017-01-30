void setup() 
{
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  digitalWrite(2,HIGH); // turn the LED on (Voltage High)
  delay(500);
  digitalWrite(2,LOW);  // turn the LED off (Voltage Low)
  delay(500);
}
