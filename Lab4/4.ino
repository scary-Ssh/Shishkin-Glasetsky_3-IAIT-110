#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Настройки LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Настройки Клавиатуры ---
const int rowPins[4] = {8, 7, 6, 5}; // Строки (подключены к диодам)
const int colPins[4] = {11, 10, 9, 4}; // Столбцы
const int KEY_INT_PIN = 2; // D2 - Внешнее прерывание 0
const int rowPins[4] = {8, 7, 6, 5}; 
const int colPins[4] = {11, 10, 9, 4}; 
const int KEY_INT_PIN = 2; 

char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

volatile unsigned long timerCount = 0;
volatile bool scanFlag = false; // Флаг, установленный INT0
volatile bool scanFlag = false;

// --- Обработчик прерывания Таймера1 (Обновление LCD каждые 500 мс) ---
ISR(TIMER1_COMPA_vect) {
    timerCount++;
}

// --- Обработчик внешнего прерывания (Запуск сканирования) ---
void startScan() {
    scanFlag = true;
}

void setupKeyboardPins() {
    for(int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH); // Строки по умолчанию HIGH
        pinMode(colPins[i], INPUT_PULLUP); // Используем внутреннюю подтяжку
        digitalWrite(rowPins[i], HIGH); 
        pinMode(colPins[i], INPUT_PULLUP); 
    }
}

void setupTimer1() {
    // Настройка Timer1 для прерывания 500 мс (OCR1A = 31249 при Prescaler 1024)
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 31249; // Для 500 мс при 16 МГц и prescaler 1024
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
    TIMSK1 |= (1 << OCIE1A); // Включение прерывания
    OCR1A = 31249; 
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10); 
    TIMSK1 |= (1 << OCIE1A); 
    interrupts();
}

// Функция полного сканирования клавиатуры
char scanKeyboard() {
    for (int i = 0; i < 4; i++) {
        // Активируем строку i (подаем LOW)
        digitalWrite(rowPins[i], LOW);

        // Небольшая задержка для стабилизации
        delayMicroseconds(100);

        for (int j = 0; j < 4; j++) {
            if (digitalRead(colPins[j]) == LOW) {
                // Клавиша найдена: keys[i][j]
                
                // Ждем отпускания кнопки (антидребезг)

                while(digitalRead(colPins[j]) == LOW) {
                    delay(10);
                }

                // Отключаем строку перед возвратом
                digitalWrite(rowPins[i], HIGH);
                return keys[i][j];
            }
        }
        // Деактивируем строку перед переходом к следующей
        digitalWrite(rowPins[i], HIGH);
    }
    return 0; // Ничего не найдено
    return 0; 
}

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    setupKeyboardPins();
    setupTimer1();

    // --- Настройка Внешнего прерывания (D2) ---
    pinMode(KEY_INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(KEY_INT_PIN), startScan, FALLING);

    // Включаем глобальные прерывания
    interrupts();

    lcd.setCursor(0, 0);
    lcd.print("System Ready   ");
}

void loop() {
    // 1. Обновление счетчика (из таймера)
    static unsigned long lastDisplayUpdate = 0;
    static unsigned long lastTimerCount = 0;

    if (timerCount != lastTimerCount) {
        if (timerCount - lastDisplayUpdate >= 2) { // Обновляем каждую секунду (2 * 500 мс)
        if (timerCount - lastDisplayUpdate >= 2) { 
            lcd.setCursor(0, 0);
            lcd.print("Count: ");
            lcd.print(timerCount);
            lcd.print("   ");
            lastDisplayUpdate = timerCount;
        }
        lastTimerCount = timerCount;
    }

    // 2. Проверка флага сканирования (установлен внешним прерыванием)
    if (scanFlag) {
        char pressedKey = scanKeyboard();

        if (pressedKey != 0) {
            lcd.setCursor(0, 1);
            lcd.print("Key: ");
            lcd.print(pressedKey);
            lcd.print("           ");
            Serial.print("Key Press Triggered Scan: ");
            Serial.println(pressedKey);

            // Дополнительная логика: сброс счетчика по нажатию 'A'
            if (pressedKey == 'A') {
                timerCount = 0;
                lcd.setCursor(0, 0);
                lcd.print("Count: 0       ");
            }
        } else {
            lcd.setCursor(0, 1);
            lcd.print("No Key         ");
        }
        scanFlag = false;
    }
}
