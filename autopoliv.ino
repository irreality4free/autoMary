
//PH VARS
#define SensorPin        A2                                                                   // pH meter Analog output to Arduino Analog Input 0
#define Offset           -0.0                                                                // Компенсация смещения
#define LED              13                                                                   // Номер вывода светодиода, который является индикатором нормальной работы скетча
#define samplingInterval 20                                                                   // Интервал в мс между измерениями
#define printInterval    800                                                                  // Интервал в мс между выводами показаний
#define ArrayLenth       40                                                                   // Количество выборок
int pHArray[ArrayLenth];                                                                      // Массив для определения среднего показания напряжения считанного с датчика
int pHArrayIndex=0; 

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
int pre_delay = 20000;
int water_lvl_sensor_l_val;
int water_lvl_sensor_h_val;
float ph_val;
int hum_val;
long delF_Time = 0;
long delF_Period = 1000;


int lvl_h = 800;
int lvl_l = 800;
float phMax = 7.0;
float phMin = 6.5;
int ph_plus_doze = 30000;
int ph_minus_doze = 10000;
int fertilizer_doze = 500;
long fertilized_wtr_doze=30 * 1000;
long ph_delay = 300000;
int substrat_hum = 500;
int s_hum; 
long debug_current;
int debug_period = 1000;

long current_delF;
//int delF_period = 

int tank_state_val;
//tank state vars
int full = 3;
int not_full = 2;
int empty = 1;
int error = 0;

int check_tank_state(){
  int lvl_h_value = analogRead(water_lvl_sensor_h);
  int lvl_l_value = analogRead(water_lvl_sensor_l);
  if(lvl_h_value<lvl_h && lvl_l_value<lvl_l) {
    tank_state_val = 3;
    return 3;
  }
  if(lvl_h_value>=lvl_h && lvl_l_value<lvl_l){
    tank_state_val = 2;
    return 2;
  }
  if(lvl_h_value>=lvl_h && lvl_l_value>=lvl_l){
    tank_state_val = 1;
    return 1;
  }
  if(lvl_h_value<lvl_h && lvl_l_value>=lvl_l) {
    tank_state_val = 0;
    return 0;
  }
}

float check_ph(){
//  ph = analogRead(ph_sensor);
//  return ph
    static unsigned long samplingTime = millis();                                             // Определяем переменную samplingTime для хранения времени прошедшего с момента старта (переменная создаётся при первом проходе цикла loop и не теряется по его завершении)
    static unsigned long printTime    = millis();                                             // Определяем переменную printTime    для хранения времени прошедшего с момента старта (переменная создаётся при первом проходе цикла loop и не теряется по его завершении)
    static float pHValue, voltage;                                                            // Объявляем переменные для хранения значений напряжения и pH
//  Проводим измерения:                                                                       //
    if(millis() - samplingTime > samplingInterval){                                           // Выполняем код в теле оператора if через каждые samplingInterval мс
        pHArray[pHArrayIndex++] = analogRead(SensorPin);                                      // Читаем данные в очередной элемент массива pHArray
        if(pHArrayIndex==ArrayLenth) pHArrayIndex=0;                                          // Если достигли последнего элемента массива pHArray, то сбрасываем номер текущего элемента этого массива в 0
        voltage = averagearray(pHArray, ArrayLenth) * 5.0 / 1023;                             // Получаем среднее напряжение в мВ из массива напряжений pHArray
        pHValue = 3.5 * voltage + Offset;                                                     // Преобразуем мВ в pH
        samplingTime = millis();                                                              // Обновляем время для переменной samplingTime
    }                                                                                         //
//  Выводим показания в монитор:                                                              //
    if(millis() - printTime > printInterval){                                                 // Выполняем код в теле оператора if через каждые printInterval мс
//        Serial.print("Voltage:");                                                             // Выводим надпись "Voltage:"
//        Serial.print(voltage,2);                                                              // Выводим среднее напряжение
//        Serial.print("    pH value: ");                                                       // Выводим надпись "    pH value: "
//        Serial.println(pHValue,2);                                                            // Выводим значение pH
        digitalWrite(LED,digitalRead(LED)^1);                                                 // Меняем состояние светодиода
        printTime = millis();                                                                 // Обновляем время для переменной printTime
    }
    if (pHValue>14.9){
      pHValue = 6.7;
      Serial.println("ph sensor error: val is too high, check cable connection");
    }
    ph_val = pHValue;
    return pHValue;
}

int check_substrat_hum(){
  s_hum = analogRead(substrat_hum_sensor);
  hum_val = s_hum;
  return s_hum;
}

void pump_clear_wtr(int state){
  digitalWrite(clear_wtr_pump, state);
}

void fill_tank(){
//  Serial.println("1");
if (check_tank_state()!=full){
  pump_clear_wtr(0);
//  Serial.println("2");
  while(check_tank_state() != full){
//    Serial.println("3"); 
//    Serial.print("tank state - ");
//    Serial.println(check_tank_state());
    check_Sensors();
    Debug();
    if(check_tank_state() == error){
//      Serial.println("4");
      break;
    }
  }
//  Serial.println("5");
  pump_clear_wtr(1);
//  Serial.println("6");
}
else{
  Serial.println("tank is full");
  delay(500);
}
}


void inject_fertilizer(){
  digitalWrite(fertilizer_pump, LOW);
  delay(fertilizer_doze);
  digitalWrite(fertilizer_pump, HIGH);
  
}

void inject_ph_plus(){
  digitalWrite(ph_plus_pump, LOW);
  delF(ph_plus_doze);
  digitalWrite(ph_plus_pump, HIGH);
//  delay(ph_delay);
delF(ph_delay);
}

void inject_ph_minus(){
  digitalWrite(ph_minus_pump, LOW);
  delF(ph_minus_doze);
  digitalWrite(ph_minus_pump, HIGH);
//  delay(ph_delay);
delF(ph_delay);
}

void correct_ph(){
  delF(pre_delay);
  while(1){
    check_Sensors();
    Debug();
    Control();
    ph_val = check_ph();
    if(ph_val>phMax)inject_ph_minus();
    if(ph_val<phMin)inject_ph_plus();
    if(ph_val>=phMin && ph_val<=phMax)break;
  }
  Serial.println("ph is correct");
}

void pump_fwtr(){
  digitalWrite(fertilized_wtr_pump, LOW);
  delay(fertilized_wtr_doze);
  digitalWrite(fertilized_wtr_pump, HIGH);
}

//  Функция определения среднего значения напряжения                                          // Эта функция возвращает среднее арифметическое значение данных массива arr без учёта одного максимального и одного минимального значения массива.
double averagearray(int* arr, int number){                                                    //
    int i,max,min;                                                                            // Объявляем переменные для цикла и экстремумов
    double avg;                                                                               // Объявляем переменную для вывода среднего значения
    long amount=0;                                                                            // Определяем переменную для подсчёта среднего значения
    if(number<=0){ Serial.println("Error number for the array to avraging!/n");  return 0;}   // В массиве arr не может быть 0 и менее элементов
    if(number< 5){ for(i=0; i<number; i++){amount+=arr[i];} avg = amount/number; return avg;  // Если в массиве arr менее 5 элементов, то среднее значение является средним арифметическим значением
    }else{                                                                                    // Если в массиве arr более 5 элементов, то среднее значение считаем иначе ...
        if(arr[0]<arr[1]){ min = arr[0]; max=arr[1];}                                         // Определяем минимальное и максимальное число из первых двух элементов массива
        else             { min = arr[1]; max=arr[0];}                                         // Определяем минимальное и максимальное число из первых двух элементов массива
        for(i=2; i<number; i++){                                                              // Проходим по остальным элементам массива
                 if(arr[i]<min){ amount+=min; min=arr[i]; }                                   // Если значение очередного элемента меньше минимального,  то добавляем к значению amount предыдущее минимальное значение  и обновляем значение min
            else if(arr[i]>max){ amount+=max; max=arr[i]; }                                   // Если значение очередного элемента больше максимального, то добавляем к значению amount предыдущее максимальное значение и обновляем значение max
            else               { amount+=arr[i];          }                                   // Если значение очередного элемента находится в пределах между min и max, то добавляем значение этого элемента к amount
        }                                                                                     //
        avg = (double) amount/(number-2);                                                     // Получаем среднее арифметическое значение (без учета значений первых двух элементов массива arr, т.к. они не добавлялись к amount)
    }                                                                                         //
    return avg;                                                                               // Возвращаем полученное среднее значение
}  

void Run(){
    //need clear water if both water lvl sensors out of range
if(check_tank_state() == empty){
  fill_tank();
  inject_fertilizer();
  correct_ph();  
}
if(check_substrat_hum()<substrat_hum){
  pump_fwtr();
}
}


void PHPTest(){
  digitalWrite(ph_plus_pump ,LOW);
  delay(3000);
  digitalWrite(ph_plus_pump ,HIGH);
  
}

void PHMTest(){
  digitalWrite(ph_minus_pump ,LOW);
  delay(3000);
  digitalWrite(ph_minus_pump ,HIGH);
  
}

void Test(){
  PHPTest();
  PHMTest();
}

void Debug(){
  if (millis()- debug_current> debug_period){
  Serial.print("ph - ");
  Serial.println(ph_val);
  Serial.print("hum - ");
  Serial.println(hum_val);
  Serial.print("water_lvl_sensor_l_val - ");
  Serial.println(water_lvl_sensor_l_val);
  Serial.print("water_lvl_sensor_h_val - ");
  Serial.println(water_lvl_sensor_h_val);
  Serial.print("tank state - ");
  Serial.println(tank_state_val);
  Serial.println();
  Serial.print(" Time - ");
    Serial.println(delF_Time);
    Serial.print("Period - ");
    Serial.println(delF_Period);
    Serial.println();
  Serial.println("---------------------");
  Serial.println();
  debug_current= millis();
  }
}

void check_water_lvl_sensor(){
  water_lvl_sensor_l_val = analogRead(water_lvl_sensor_l);
  water_lvl_sensor_h_val = analogRead(water_lvl_sensor_h);
}
void check_Sensors(){
  check_tank_state();
  check_substrat_hum();
  check_water_lvl_sensor();
  check_ph();
}

void delF(long del){
  current_delF = millis();
  long locdeb_cur = millis();
  int period_loc = 1000;
  while(1){
    if (millis() - locdeb_cur>period_loc){
    
    delF_Time = millis() - current_delF;
    delF_Period = del;
    long locdeb_cur = millis();
    }
    
    check_Sensors();
    Debug();
    Control();
    if (millis() - current_delF>del){
      Serial.println("exit delay");
      break;
    }
  }
}

void Control(){
 if(Serial.available()>0){
    char a = Serial.read();
    if (a == '1'){
      digitalWrite(ph_plus_pump ,LOW);
    }
    if (a == '2'){
      digitalWrite(ph_plus_pump ,HIGH);
    }

    if (a == '3'){
      digitalWrite(ph_minus_pump ,LOW);
    }

    if (a == '4'){
      digitalWrite(ph_minus_pump ,HIGH);
    }
  } 
}



void setup() {
  Serial.begin(9600);
  // PIN MODE
  pinMode(clear_wtr_pump ,OUTPUT);
  pinMode(fertilized_wtr_pump ,OUTPUT);
  pinMode(fertilizer_pump ,OUTPUT);
  pinMode(ph_plus_pump ,OUTPUT);
  pinMode(ph_minus_pump ,OUTPUT);
  
  //SETUP PIN STATE
  digitalWrite(clear_wtr_pump ,HIGH);
  digitalWrite(fertilized_wtr_pump ,HIGH);
  digitalWrite(fertilizer_pump ,HIGH);
  digitalWrite(ph_plus_pump ,HIGH);
  digitalWrite(ph_minus_pump ,HIGH);
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
