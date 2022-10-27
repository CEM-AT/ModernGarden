/*
  =================Auto Watering=================;
  By: Abiel M. (@paodelonga)
  Date: 23/03/2022 - 8:00:58 PM
  Update: 28/05/2022 - 8:34:02 PM

  ==========Legenda==========
  TerminalIA: Terminal no Arduino.
  TerminalIM: Terminal no Módulo ou Sensor.

  Guia {
    [Conexões]
      [Módulos ou Sensores]
        [Modelo do Módulo/Sensor]
          TerminalIM = TerminalIA: Terminal
            About: Descrição.
  }


  [Conexões]
    [Módulos]
      [LCD]
        RS = TerminalIA: 12
        Enable = TerminalIA: 11
        DB4 = TerminalIA: 4
        DB5 = TerminalIA: 5
        DB6 = TerminalIA: 6
        DB7 = TerminalIA: 7

    [Sensores]
      [HD38]
        VCC = TerminalIA: 5V
        GND = TerminalIA: GND
        Signal = TerminalIA: A0
          About: Sensor de alta precisão para umidade do solo.


      [DHT11|DHT22]
        VCC = TerminalIA: 5V
        GND = TerminalIA: GND
        Signal = TerminalIA: A0
          About: Sensor de umidade e temperatura ambiente.

      [KY-019]
          About: Um relé para controle de alta voltagem.

  [Repositorio]
  https://github.com/paodelonga/Ardo/tree/main/Auto_Watering

  [TinkerCad]
  https://www.tinkercad.com/things/euZU69P87Dg-auto-watering-rega-inteligente

  ===============================================;
*/
#include <LiquidCrystal.h>
const int RS = 12, Enable = 11, DB4 = 4, DB5 = 5, DB6 = 6, DB7 = 7; // Definindo as conexões do LCD.
LiquidCrystal lcd(RS, Enable, DB4, DB5, DB6, DB7); // Definindo as conexões do LCD.

const int Moisture_Sensor = A0; // Sensor de umidade.
const int Solenoid_Valve = 2; // Válvula solenoide.
const int Minimal_Moisture = 65; // Umidade minima.
const long Watering_Time = 15 *1000; // Duração do tempo de irrigação. (segundos)
const long Watering_Interval = 60 *60000; // Intervalo de execução da checagem. (minutos)
const long Watering_Loop_Interval = 30 *60000; // Intervalo do Loop. (minutos)
int GroundMoisture = 0; // Umidade inicial.

// ======= Funções =======;
void dots(String _message, uint8_t _y, uint8_t _z ,uint8_t _n) {
  uint16_t _millisOne = millis();
  lcd.setCursor(_z, _y);
  lcd.print(_message);

  uint8_t _length = _message.length() + _z;
    for (uint8_t _i = 0; _i < 10; _i++) {
      for (uint8_t _x = _length; _x < _length+3; _x++) {
        lcd.setCursor(_x, _y);
        lcd.print(".");
        delay(_n*115/3.4120);
      }
      lcd.setCursor(_length, _y);
      lcd.print("   ");
    }
  //Serial.println(millis() - _millisOne);
}

void showMoisture() { // Exibe a atual umidade do solo.
  GroundMoisture = analogRead(Moisture_Sensor);
  GroundMoisture = map(GroundMoisture, 1023, 0, 0, 100);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Auto Watering.");
  delay(10);
  lcd.setCursor(2, 1);
  lcd.print((String)"Umidade " + GroundMoisture + "%");
  Serial.println("\n[showMoisture]");
  Serial.println((String)"Ground moisture: " + GroundMoisture + "%");
  delay(3000);
}

void checkMoisture() { // Checa a umidade e rega o solo
  GroundMoisture = analogRead(Moisture_Sensor);
  GroundMoisture = map(GroundMoisture, 1023, 0, 0, 100);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Auto Watering.");
  delay(10);
  lcd.setCursor(2, 1);
  lcd.print((String)"Umidade " + GroundMoisture + "%");
  Serial.println("\n\n[Loop]");
  Serial.println((String)"Ground moisture: " + GroundMoisture + "%");
  delay(3000);

  if(GroundMoisture < Minimal_Moisture) { // Se a umidade do solo é menor que a mínima; doanything.
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Auto Watering.");
    delay(10);
    Serial.println("\n[Watering]");
    Serial.println("Valve Open.");
    digitalWrite(Solenoid_Valve, LOW); // Abre a válvula.
    dots("   Regando", 1, Watering_Time / 1000); // Mantem a válvula aberta durante o intervalo.
    digitalWrite(Solenoid_Valve, HIGH); // Fecha a válvula.
    Serial.println("\n[Watering]");
    Serial.println("Valve Close.");
    delay(100);

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Auto Watering.");
    delay(10);
    lcd.setCursor(1, 1);
    lcd.print("Solo regado :)");
    Serial.println("\n[Watering]");
    Serial.println("Watered.");
    delay(2000);

    showMoisture(); // Mostra a umidade atual após irrigação.
  }

  else if (GroundMoisture >= Minimal_Moisture + 25) { // Se a umidade do solo é maior ou igual a umidade mínima +15; doanything
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Auto Watering.");
    delay(1);
    lcd.setCursor(0, 1);
    lcd.print("Solo encharcado!");
    Serial.println("\n[Watering]");
    Serial.println("Soggy Soil.");
    delay(1);

    delay(6000);
    showMoisture();
  }

  else if (GroundMoisture >= Minimal_Moisture) { // GroundMoisture > Minimal_Moisture && // Se a umidade do solo é maior que a umidade mínima; doanything
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Auto Watering.");
    delay(1);
    lcd.setCursor(2, 1);
    lcd.print("Solo umido :)");
    Serial.println("\n[Watering]");
    Serial.println("Wet Soil.");
    delay(1);

    delay(6000);
    showMoisture();
  }

  Serial.println("\n[Watering]");
  Serial.print("Next verification in: ");
  Serial.print(Watering_Interval / 60000);
  Serial.print("min");
}


// ====== Inicialização do código =======;
void setup() { // Inicialização.
  pinMode(Solenoid_Valve, OUTPUT);
  digitalWrite(Solenoid_Valve, HIGH);
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  lcd.print("Auto Watering.");
  delay(10);
  Serial.println("\n\n[Setup]");
  Serial.println("Starting...");
  dots("  Iniciando", 1, 3);
  delay(100);
  Serial.println("\n[Setup]");
  Serial.println("Successfully started!");

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Criado por");
  delay(10);
  lcd.setCursor(1, 1);
  lcd.print("git@paodelonga");
  delay(3000);

  Serial.println("\n[System]");
  Serial.print("Watering Time: ");
  Serial.print(Watering_Time / 1000);
  Serial.print(" sec\n");
  Serial.print("Watering Intervals: ");
  Serial.print(Watering_Interval / 60000);
  Serial.print(" min\n");
  Serial.print("Watering Loop Interval: ");
  Serial.print(Watering_Loop_Interval / 60000);
  Serial.print(" min\n");
}

void loop() {
  checkMoisture();
  delay(Watering_Interval);

}
