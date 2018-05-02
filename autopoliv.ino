////PINS
int clear_wtr_pump = 3;
int fertilized_wtr_pump = 4;
int fertilizer_pump = 5;
int ph_plus_pump = 6;
int ph_minus_pump = 7;

int water_lvl_sensor_l = A0;
int water_lvl_sensor_h = A1;
int ph_sensor = A2;
int substrat_hum_sensor = A3;

/////VARS

int lvl_h = 500;
int lvl_l = 500;
float phMax = 7.0;
float phMin = 6.5;
int ph_plus_doze = 100;
int ph_minus_doze = 100;
int fertilizer_doze = 500;
long fertilized_wtr_doze=30 * 1000;
long ph_delay = 15*60*1000;
int substrat_hum = 500;


//tank state vars
int full = 3;
int not_full = 2;
int empty = 1;
int error = 0;

int check_tank_state(){
  int lvl_h_value = analogRead(water_lvl_sensor_h);
  int lvl_l_value = analogRead(water_lvl_sensor_l);
  if(lvl_h_value>lvl_h && lvl_l_value>lvl_l) return 3;
  if(lvl_h_value<=lvl_h && lvl_l_value>lvl_l) return 2;
  if(lvl_h_value<=lvl_h && lvl_l_value<=lvl_l) return 1;
  if(lvl_h_value>lvl_h && lvl_l_value<=lvl_l) return 0;
}

float check_ph(){
  ph = analogRead(ph_sensor);
  return ph
}

int check_substrat_hum(){
  s_hum = analogRead(substrat_hum_sensor);
  return s_hum;
}

void pump_clear_wtr(int state){
  digitalWrite(clear_wtr_pump, state);
}

void fill_tank(){
  pump_clear_wtr(1);
  while(check_tank_state() != full){
    Serial.print("tank state - ");
    Serial.print(check_tank_state());
  }
  pump_clear_wtr(1);
}


void inject_fertilizer(){
  digitalWrite(fertilizer_pump, HIGH);
  delay(fertilizer_doze);
  digitalWrite(fertilizer_pump, LOW);
  
}

void inject_ph_plus(){
  digitalWrite(ph_plus_pump, HIGH);
  delay(ph_plus_doze);
  digitalWrite(ph_plus_pump, LOW);
  delay(ph_delay);
}

void inject_ph_minus(){
  digitalWrite(ph_minus_pump, HIGH);
  delay(ph_minus_doze);
  digitalWrite(ph_minus_pump, LOW);
  delay(ph_delay);
}

void correct_ph(){
  while(1){
    int ph_val = check_ph();
    if(ph_val>phMax)inject_ph_minus();
    if(ph_val<phMin)inject_ph_plus();
    if(ph_val>=phMin && ph_val<=phMax)break;
  }
}

void pump_fwtr(){
  digitalWrite(fertilized_wtr_pump, HIGH);
  delay(fertilized_wtr_doze);
  digitalWrite(fertilized_wtr_pump, LOW);
}



void setup() {
  // PIN MODE
  pinMode(clear_wtr_pump ,OUTPUT);
  pinMode(fertilized_wtr_pump ,OUTPUT);
  pinMode(fertilizer_pump ,OUTPUT);
  pinMode(ph_plus_pump ,OUTPUT);
  pinMode(ph_minus_pump ,OUTPUT);
  
  //SETUP PIN STATE
  digitalWrite(clear_wtr_pump ,LOW);
  digitalWrite(fertilized_wtr_pump ,LOW);
  digitalWrite(fertilizer_pump ,LOW);
  digitalWrite(ph_plus_pump ,LOW);
  digitalWrite(ph_minus_pump ,LOW);

}

void loop() {
  //need clear water if both water lvl sensors out of range
if(check_tank_state() = empty){
  fill_tank();
  inject_fertilizer();
  correct_ph();  
}
if(check_substrat_hum()<substrat_hum){
  pump_fwtr();
}
}
