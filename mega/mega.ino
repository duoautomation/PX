void setup(){
    Serial.begin(9600);
    Serial3.begin(9600);

    
    Serial.println("Tudo certo.");
}

void loop(){
    Serial3.print("Hello");
    delay(5000);
}