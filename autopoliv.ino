
#define NEW_LINE '\n'

#define PRE_DELAY_EE 0
#define LVL_H_EE = 2;
#define LVL_L_EE = 4;
#define PH_MAX_EE = 6;
#define PH_MIN_EE = 10;
#define PH_PLUS_DOZE_EE = 14;
#define PH_MINUS_DOZE_EE = 16;
#define FERTILIZER_DOZE_EE = 18;
#define PH_DELAY_EE = 20;
#define SUBSTRAT_HUM_EE = 24;


//PH VARS
#define SensorPin        A2                                                                   // pH meter Analog output to Arduino Analog Input 0
#define Offset           -0.0                                                                // Компенсация смещения
#define LED              13                                                                   // Номер вывода светодиода, который является индикатором нормальной работы скетча
#define samplingInterval 20                                                                   // Интервал в мс между измерениями
#define printInterval    800                                                                  // Интервал в мс между выводами показаний
#define ArrayLenth       40                                                                   // Количество выборок
int pHArray[ArrayLenth];                                                                      // Массив для определения среднего показания напряжения считанного с датчика
int pHArrayIndex = 0;

////PINS
int clear_wtr_pump = 3;
int fertilized_wtr_pump = 4;
int fertilizer_pump = 5;
int ph_plus_pump = 6;
int ph_minus_pump = 7;

int water_lvl_sensor_l = A0;
int water_lvl_sensor_h = A1;
int ph_sensor = A2;
int substrat_hum_sensor = A4;





/////VARS
bool debugB = false;

int water_lvl_sensor_l_val;
int water_lvl_sensor_h_val;
float ph_val;
int hum_val;

long delF_Time = 0;
long delF_Period = 1000;

int pre_delay = 20000;
int lvl_h = 800;
int lvl_l = 800;

float phMax = 7.0;
float phMin = 6.5;

int ph_plus_doze = 30000;
int ph_minus_doze = 10000;
int fertilizer_doze = 500;

long fertilized_wtr_doze = 30 * 1000;
long ph_delay = 300000;

int substrat_hum = 500;
int debug_period = 1000;

int s_hum;
long debug_current;
long current_delF;
int tank_state_val;

int full = 3;
int not_full = 2;
int empty = 1;
int error = 0;

String input_string = "";

void EEPROMWritelong(int address, long value)
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }

long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }


// чтение
int EEPROM_int_read(int addr) {   
  byte raw[2];
  for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr+i);
  int &num = (int&)raw;
  return num;
}
 
// запись
void EEPROM_int_write(int addr, int num) {
  byte raw[2];
  (int&)raw = num;
  for(byte i = 0; i < 2; i++) EEPROM.write(addr+i, raw[i]);
}



// чтение
float EEPROM_float_read(int addr) {   
  byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
  float &num = (float&)raw;
  return num;
}
 
// запись
void EEPROM_float_write(int addr, float num) {
  byte raw[4];
  (float&)raw = num;
  for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}





void SAVE_PRE_DELAY(){
  EEPROM_int_write(PRE_DELAY_EE, pre_delay);
}
void SAVE_LVL_H(){
  EEPROM_int_write(LVL_H_EE, lvl_h);
}
void SAVE_LVL_L(){
  EEPROM_int_write(LVL_L_EE, lvl_l);
}

void SAVE_PH_MAX(){
  EEPROM_float_write(PH_MAX_EE, phMax);
}
void SAVE_PH_MIN(){
  EEPROM_float_write(PH_MIN_EE, phMin);
}

void SAVE_PH_PLUS_DOZE(){
  EEPROM_int_write(PH_PLUS_DOZE_EE, ph_plus_doze);
}
void SAVE_PH_MINUS_DOZE(){
  EEPROM_int_write(PH_MINUS_DOZE_EE, ph_minus_doze);
}
void SAVE_FERTILIZER_DOZE(){
  EEPROM_int_write(FERTILIZER_DOZE_EE, fertilizer_doze);
}
void SAVE_PH_DELAY(){
  EEPROMWritelong(PH_DELAY_EE, ph_delay);
}
void SAVE_SUBSTRAT_HUM(){
  EEPROM_int_write(SUBSTRAT_HUM_EE, substrat_hum);
}




void READ_PRE_DELAY(){
pre_delay=  EEPROM_int_read(PRE_DELAY_EE );
}
void READ_LVL_H(){
lvl_h=  EEPROM_int_read(LVL_H_EE);
}
void READ_LVL_L(){
 lvl_l= EEPROM_int_read(LVL_L_EE);
}
//
void READ_PH_MAX(){
 phMax= EEPROM_float_read(PH_MAX_EE );
}
void READ_PH_MIN(){
 phMin= EEPROM_float_read(PH_MIN_EE );
}
//

void READ_PH_PLUS_DOZE(){
  ph_plus_doze=EEPROM_int_read(PH_PLUS_DOZE_EE);
}
void READ_PH_MINUS_DOZE(){
 ph_minus_doze= EEPROM_int_read(PH_MINUS_DOZE_EE );
}
void READ_FERTILIZER_DOZE(){
  fertilizer_doze=EEPROM_int_read(FERTILIZER_DOZE_EE );
}

//
void READ_PH_DELAY(){
 ph_delay= EEPROMReadlong(PH_DELAY_EE);
}

void READ_SUBSTRAT_HUM(){
 substrat_hum= EEPROM_int_read(SUBSTRAT_HUM_EE );
}







int check_tank_state() {
  int lvl_h_value = analogRead(water_lvl_sensor_h);
  int lvl_l_value = analogRead(water_lvl_sensor_l);
  if (lvl_h_value < lvl_h && lvl_l_value < lvl_l) {
    tank_state_val = 3;
    return 3;
  }
  if (lvl_h_value >= lvl_h && lvl_l_value < lvl_l) {
    tank_state_val = 2;
    return 2;
  }
  if (lvl_h_value >= lvl_h && lvl_l_value >= lvl_l) {
    tank_state_val = 1;
    return 1;
  }
  if (lvl_h_value < lvl_h && lvl_l_value >= lvl_l) {
    tank_state_val = 0;
    return 0;
  }
}
double averagearray(int* arr, int number) {                                                   //
  int i, max, min;                                                                          // Объявляем переменные для цикла и экстремумов
  double avg;                                                                               // Объявляем переменную для вывода среднего значения
  long amount = 0;                                                                          // Определяем переменную для подсчёта среднего значения
  if (number <= 0) {
    Serial.println("Error number for the array to avraging!/n");   // В массиве arr не может быть 0 и менее элементов
    return 0;
  }
  if (number < 5) {
    for (i = 0; i < number; i++) {
      amount += arr[i];
    } avg = amount / number; return avg; // Если в массиве arr менее 5 элементов, то среднее значение является средним арифметическим значением
  } else {                                                                                   // Если в массиве arr более 5 элементов, то среднее значение считаем иначе ...
    if (arr[0] < arr[1]) {
      min = arr[0];  // Определяем минимальное и максимальное число из первых двух элементов массива
      max = arr[1];
    }
    else             {
      min = arr[1];  // Определяем минимальное и максимальное число из первых двух элементов массива
      max = arr[0];
    }
    for (i = 2; i < number; i++) {                                                        // Проходим по остальным элементам массива
      if (arr[i] < min) {
        amount += min;  // Если значение очередного элемента меньше минимального,  то добавляем к значению amount предыдущее минимальное значение  и обновляем значение min
        min = arr[i];
      }
      else if (arr[i] > max) {
        amount += max;  // Если значение очередного элемента больше максимального, то добавляем к значению amount предыдущее максимальное значение и обновляем значение max
        max = arr[i];
      }
      else               {
        amount += arr[i];           // Если значение очередного элемента находится в пределах между min и max, то добавляем значение этого элемента к amount
      }
    }                                                                                     //
    avg = (double) amount / (number - 2);                                                 // Получаем среднее арифметическое значение (без учета значений первых двух элементов массива arr, т.к. они не добавлялись к amount)
  }                                                                                         //
  return avg;                                                                               // Возвращаем полученное среднее значение
}

float check_ph() {
  //  ph = analogRead(ph_sensor);
  //  return ph
  static unsigned long samplingTime = millis();                                             // Определяем переменную samplingTime для хранения времени прошедшего с момента старта (переменная создаётся при первом проходе цикла loop и не теряется по его завершении)
  static unsigned long printTime    = millis();                                             // Определяем переменную printTime    для хранения времени прошедшего с момента старта (переменная создаётся при первом проходе цикла loop и не теряется по его завершении)
  static float pHValue, voltage;                                                            // Объявляем переменные для хранения значений напряжения и pH
  //  Проводим измерения:                                                                       //
  if (millis() - samplingTime > samplingInterval) {                                         // Выполняем код в теле оператора if через каждые samplingInterval мс
    pHArray[pHArrayIndex++] = analogRead(SensorPin);                                      // Читаем данные в очередной элемент массива pHArray
    if (pHArrayIndex == ArrayLenth) pHArrayIndex = 0;                                     // Если достигли последнего элемента массива pHArray, то сбрасываем номер текущего элемента этого массива в 0
    voltage = averagearray(pHArray, ArrayLenth) * 5.0 / 1023;                             // Получаем среднее напряжение в мВ из массива напряжений pHArray
    pHValue = 3.5 * voltage + Offset;                                                     // Преобразуем мВ в pH
    samplingTime = millis();                                                              // Обновляем время для переменной samplingTime
  }                                                                                         //
  //  Выводим показания в монитор:                                                              //
  if (millis() - printTime > printInterval) {                                               // Выполняем код в теле оператора if через каждые printInterval мс
                                                            // Выводим значение pH
    digitalWrite(LED, digitalRead(LED) ^ 1);                                              // Меняем состояние светодиода
    printTime = millis();                                                                 // Обновляем время для переменной printTime
  }
  if (pHValue > 14.9) {
    pHValue = 6.7;
    if (debugB)Serial.println("ph sensor error: val is too high, check cable connection");
  }
  ph_val = pHValue;
  return pHValue;
}

int check_substrat_hum() {
  s_hum = analogRead(substrat_hum_sensor);
  hum_val = s_hum;
  return s_hum;
}

void pump_clear_wtr(int state) {
  digitalWrite(clear_wtr_pump, state);
}
void parseIt(String input_string) {

  Serial.println(input_string);
  if (input_string.substring(0, 4) == "com:") {

    if (input_string.substring(4, 5) == "9")ToPC();
    if (input_string.substring(4, 5) == "1")digitalWrite(ph_plus_pump , LOW);

    if (input_string.substring(4, 5) == "2")digitalWrite(ph_plus_pump , HIGH);


    if (input_string.substring(4, 5) == "3")digitalWrite(ph_minus_pump , LOW);
    if (input_string.substring(4, 5) == "4")digitalWrite(ph_minus_pump , HIGH);

    if (input_string.substring(4, 5) == "5")digitalWrite(fertilized_wtr_pump, LOW);
    if (input_string.substring(4, 5) == "6")digitalWrite(fertilized_wtr_pump, HIGH);

    if (input_string.substring(4, 5) == "7")digitalWrite(clear_wtr_pump, LOW);
    if (input_string.substring(4, 5) == "8")digitalWrite(clear_wtr_pump, HIGH);


  }

  //  mass = input_string.substring(5, 13).toFloat();


}



void Control() {
  while (Serial3.available() > 0 ) {
    char a = Serial3.read();
    Serial.println(a);
    if (a == NEW_LINE) { //считается, что команда завершена, если пришел символ перевода строки
      parseIt(input_string);
      input_string = "";
    }
    else {
      input_string += a;
    }
   
  }
}
void fill_tank() {
 
  Control();
  if (check_tank_state() != full && check_tank_state() != error) {
    pump_clear_wtr(0);
  
    while (check_tank_state() != full) {
   
      check_Sensors();
      Debug();
      Control();
      if (check_tank_state() == error) {
       
        break;
      }
    }
   
    pump_clear_wtr(1);
  }
  else {
    if (debugB)Serial3.println("tank is full");
    delay(500);
  }
}






void inject_fertilizer() {
  digitalWrite(fertilizer_pump, LOW);
  delay(fertilizer_doze);
  digitalWrite(fertilizer_pump, HIGH);

}

void inject_ph_plus() {
  digitalWrite(ph_plus_pump, LOW);
  delF(ph_plus_doze);
  digitalWrite(ph_plus_pump, HIGH);
  //  delay(ph_delay);
  delF(ph_delay);
}

void inject_ph_minus() {
  digitalWrite(ph_minus_pump, LOW);
  delF(ph_minus_doze);
  digitalWrite(ph_minus_pump, HIGH);
  //  delay(ph_delay);
  delF(ph_delay);
}

void correct_ph() {
  delF(pre_delay);
  while (1) {
    check_Sensors();
    Debug();
    Control();
    ph_val = check_ph();
    if (ph_val > phMax)inject_ph_minus();
    if (ph_val < phMin)inject_ph_plus();
    if (ph_val >= phMin && ph_val <= phMax)break;
  }
  if (debugB)Serial3.println("ph is correct");
}

void pump_fwtr() {
  digitalWrite(fertilized_wtr_pump, LOW);
  delay(fertilized_wtr_doze);
  digitalWrite(fertilized_wtr_pump, HIGH);
}

//  Функция определения среднего значения напряжения                                          // Эта функция возвращает среднее арифметическое значение данных массива arr без учёта одного максимального и одного минимального значения массива.

void Run() {
  //need clear water if both water lvl sensors out of range
  if (check_tank_state() == empty) {
    fill_tank();
    inject_fertilizer();
    correct_ph();
  }
  if (check_substrat_hum() < substrat_hum) {
    pump_fwtr();
  }
}


void PHPTest() {
  digitalWrite(ph_plus_pump , LOW);
  delay(3000);
  digitalWrite(ph_plus_pump , HIGH);

}

void PHMTest() {
  digitalWrite(ph_minus_pump , LOW);
  delay(3000);
  digitalWrite(ph_minus_pump , HIGH);

}

void Test() {
  PHPTest();
  PHMTest();
}

void Debug() {
  if (debugB) {
    if (millis() - debug_current > debug_period) {
      Serial3.print("ph - ");
      Serial3.println(ph_val);
      Serial3.print("hum - ");
      Serial3.println(hum_val);
      Serial3.print("water_lvl_sensor_l_val - ");
      Serial3.println(water_lvl_sensor_l_val);
      Serial3.print("water_lvl_sensor_h_val - ");
      Serial3.println(water_lvl_sensor_h_val);
      Serial3.print("tank state - ");
      Serial3.println(tank_state_val);
      Serial3.println();
      Serial3.print(" Time - ");
      Serial3.println(delF_Time);
      Serial3.print("Period - ");
      Serial3.println(delF_Period);
      Serial3.println();
      Serial3.println("---------------------");
      Serial3.println();
      debug_current = millis();
    }
  }
}

void ToPC() {
  check_Sensors();
  Serial3.print("ph:");
  Serial3.println(ph_val);
  Serial3.print("hum:");
  Serial3.println(hum_val);
  Serial3.print("lvl_l:");
  Serial3.println(water_lvl_sensor_l_val);
  Serial3.print("lvl_h:");
  Serial3.println(water_lvl_sensor_h_val);
  Serial3.print("tank:");
  Serial3.println(tank_state_val);
  Serial3.print("time:");
  Serial3.println(delF_Time);
  Serial3.print("period:");
  Serial3.println(delF_Period);


  Serial3.print("pre_delay:");
  Serial3.println(pre_delay);
  Serial3.print("lvl_hTR:");
  Serial3.println(lvl_h);
  Serial3.print("lvl_lTR:");
  Serial3.println(lvl_l);
  Serial3.print("phMax:");
  Serial3.println(phMax);
  Serial3.print("phMin:");
  Serial3.println(phMin);
  Serial3.print("ph_plus_doze:");
  Serial3.println(ph_plus_doze);
  Serial3.print("ph_minus_doze:");
  Serial3.println(ph_minus_doze);

  Serial3.print("fertilizer_doze:");
  Serial3.println(fertilizer_doze);
  Serial3.print("ph_delay:");
  Serial3.println(ph_delay);
  Serial3.print("substrat_hum:");
  Serial3.println(substrat_hum);



  int substrat_hum = 500;
  int s_hum;
  long debug_current;
  int debug_period = 1000;


}

void check_water_lvl_sensor() {
  water_lvl_sensor_l_val = analogRead(water_lvl_sensor_l);
  water_lvl_sensor_h_val = analogRead(water_lvl_sensor_h);
}
void check_Sensors() {
  check_tank_state();
  check_substrat_hum();
  check_water_lvl_sensor();
  check_ph();
}

void delF(long del) {
  current_delF = millis();
  long locdeb_cur = millis();
  int period_loc = 1000;
  while (1) {
    if (millis() - locdeb_cur > period_loc) {

      delF_Time = millis() - current_delF;
      delF_Period = del;
      long locdeb_cur = millis();
    }

    check_Sensors();
    Debug();
    Control();
    if (millis() - current_delF > del) {
      Serial.println("exit delay");
      break;
    }
  }
}







void setup() {
  Serial3.begin(9600);
  Serial.begin(9600);
  // PIN MODE
  pinMode(clear_wtr_pump , OUTPUT);
  pinMode(fertilized_wtr_pump , OUTPUT);
  pinMode(fertilizer_pump , OUTPUT);
  pinMode(ph_plus_pump , OUTPUT);
  pinMode(ph_minus_pump , OUTPUT);

  //SETUP PIN STATE
  digitalWrite(clear_wtr_pump , HIGH);
  digitalWrite(fertilized_wtr_pump , HIGH);
  digitalWrite(fertilizer_pump , HIGH);
  digitalWrite(ph_plus_pump , HIGH);
  digitalWrite(ph_minus_pump , HIGH);
  debug_current = millis();
  current_delF = millis();
  //Test();
}


void loop() {
  fill_tank();
  //Test();
  //check_ph();
  //check_water_lvl_sensor();
  //Debug();
  //Control();
  //correct_ph();



}
