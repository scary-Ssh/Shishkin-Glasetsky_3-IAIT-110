void setup() {
	piMode(3, INPUT_PULLUP);
	Serial.begin(9600);
	pinMode(13, OUTPUT)
}

void loop() {
	butt = !digitalRead(3);

	if (butt == 1 && butt_flag == 0) {
		butt_flag = 1;
		Serial.println("Кнопка нажата");
		led_flag = !led_flag;
		digitalWrite(13, led_flag);
	}
	if (butt == 0 && butt_flag == 1) {
		butt_flag = 0;
		Serial.println("Кнопка не нажата");
	}
}
