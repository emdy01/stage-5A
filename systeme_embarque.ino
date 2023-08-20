 
#include "DataManager.h"
#include "IMU.h"
#include <Portenta_H7_TimerInterrupt.h> // librairie ISR
#include <Adafruit_SH110X.h> // librairie ecran
#include <Adafruit_GFX.h>

// Debug et erreurs
char c_DEBUGGING;
uint8_t n_err_mem = 0;
uint8_t n_err_hub = 0;
uint8_t n_err_read = 0;
uint8_t n_err_send = 0;

// Timer pour ISR
Portenta_H7_Timer ReadClock(TIM15); // Lecture IMU
Portenta_H7_Timer SendClock(TIM16); // Envoi au serveur
// Flags
volatile bool b_reading = false;
volatile bool b_sending = false;

// IMU
IMU LSM1(0, 0x6A);
IMU LSM2(0, 0x6B);
IMU LSM3(7, 0x6A);
IMU LSM4(7, 0x6B);

uint8_t u_Te = 10;  // Periode d'echantillonage

int u_tps_seq; // Duree de la sequence
int u_start_time; // debut de sequence en ms
int u_stop_time; // variable actualisee au fur et a mesure du deroulement de la sequence, contribue à la mesure de la duree de celle-ci
volatile uint8_t* a_data_imu1 = new uint8_t[12]; 
volatile uint8_t* a_data_imu2 = new uint8_t[12];
volatile uint8_t* a_data_imu3 = new uint8_t[12];
volatile uint8_t* a_data_imu4 = new uint8_t[12]; // memoires tampon pour les donnees brutes issues des IMU 
int u_dataset_start = 0; // curseur pour placer correctement les blocs dans la SDRAM
uint16_t u_ind_time_list = 0; // Mesure de la quantite d'echantillons collectes (pour eviter la saturation de la SDRAM)
bool b_sdramFULL = false; // indication de la saturation de la SDRAM

DataManager liaison;
const char* pc_SSID = "IMU_DYN"; // Nom du reseau
const char* pc_password = "paraperf"; // Mot de passe du reseau
int u_WIFI_STATUS;
bool b_SERVER_CONNECTED = 0;
String s_server_msg; // Message brut du serveur

Adafruit_SH1107 display(128, 128, &Wire, -1); // ecran

void fn_read_time() { 
  /* ISR de gestion du flag de lecture des IMU (lui affecte "true" quand la lecture doit etre effectuee) */
  b_reading = true;
}


/// @brief selection du port i2c via lequel la carte veut communiquer
/// @param port : le port souhaite 
void fn_select_port(uint8_t port) { 
  Wire.beginTransmission(HUB_ADDRESS);
  uint8_t BytesWritten = Wire.write((1 << port) & 0xFF);
  Wire.endTransmission(true);
  if (!(BytesWritten == 1)) {
    n_err_hub += 1;
  }
}

/// @brief lecture des IMU et traitement des valeurs lues

void fn_IMUtoSDRAM() { 
  /* Stockage des valeurs de temps dans la SDRAM et transfert des valeurs brutes issues des capteurs dans les memoires tampon */
  fn_select_port(0);
  liaison.m_tps[u_ind_time_list] = millis();
  LSM1.fn_read_accel_and_gyro(a_data_imu1, n_err_read);
  liaison.m_tps[u_ind_time_list + 1] = millis();
  LSM2.fn_read_accel_and_gyro(a_data_imu2, n_err_read);
  fn_select_port(7);
  liaison.m_tps[u_ind_time_list + 2] = millis();
  LSM3.fn_read_accel_and_gyro(a_data_imu3, n_err_read);
  liaison.m_tps[u_ind_time_list + 3] = millis();
  LSM4.fn_read_accel_and_gyro(a_data_imu4, n_err_read);
  // Traitement des valeurs brutes
  fn_parse_data(a_data_imu1, liaison.m_gyr1, liaison.m_acc1);
  fn_parse_data(a_data_imu2, liaison.m_gyr2, liaison.m_acc2);
  fn_parse_data(a_data_imu3, liaison.m_gyr3, liaison.m_acc3);
  fn_parse_data(a_data_imu4, liaison.m_gyr4, liaison.m_acc4);
  u_dataset_start += 3; // Deplacement du curseur de positionnement des blocs dans la SDRAM
  u_ind_time_list += 4; // Mise a jour de la quantite d'echantillons collectes
  if (u_ind_time_list > 102604) { b_sdramFULL = true; }  // verification de la non-saturation de la SDRAM
}

/// @brief  Post-traitement des donnees brutes issues des capteurs et transfert des resultats dans la SDRAM
/// @param input : les donnees brutes  des capteurs
/// @param gyr : le tableau de valeurs de vitesses stocke dans la SDRAM
/// @param acc : le tableau de valeurs d'acceleration stocke dans la SDRAM
void fn_parse_data(volatile uint8_t* input, volatile int16_t* gyr, volatile int16_t* acc) {
  gyr[u_dataset_start] = input[0] | input[1] << 8;
  gyr[u_dataset_start + 1] = input[2] | input[3] << 8;
  gyr[u_dataset_start + 2] = input[4] | input[5] << 8;
  acc[u_dataset_start] = input[6] | input[7] << 8;
  acc[u_dataset_start + 1] = input[8] | input[9] << 8;
  acc[u_dataset_start + 2] = input[10] | input[11] << 8;
}


/// @brief ISR de gestion du flag de transfert des donnees vers le serveur (lui affecte "true" quand le transfert doit etre effectue)
void fn_send_time() { 
  b_sending = true;
}


void setup() {
  // Initialisation du moniteur série
  Serial.begin(9600);

  // Connection au réseau wifi
  u_WIFI_STATUS = WiFi.begin(pc_SSID, pc_password);
  while (u_WIFI_STATUS != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
    u_WIFI_STATUS = WiFi.begin(pc_SSID, pc_password);
  }
  Serial.println("Wifi connected !");
  Serial.print("ip carte = "); Serial.println(WiFi.localIP());

  // Connection au serveur
  Serial.println("Waiting for the server");
  while (b_SERVER_CONNECTED == 0) {
    b_SERVER_CONNECTED = liaison.fn_connect_to_PC();
  }

  Serial.println("Server connected !");

  // Reception et memorisation de l'ordre d'activation (ou pas) du debug
  while (liaison.m_client.available() == 0) {
    delay(1000);
  }
  c_DEBUGGING = liaison.m_client.read();
  if (c_DEBUGGING == '1') {
    Serial.print("debug=");
    Serial.println(c_DEBUGGING);
  }

  // Initialisation de la SDRAM
  SDRAM.begin();
  liaison.fn_set_memory();

  // Initialisation de la communication i2c
  Wire.setClock(1000000);
  Wire.begin();
  
  //Reglages des IMU et de la frequence d'echantillonage par defaut
  t_BasicParam DEFAULT_PARAM;
  DEFAULT_PARAM.Fe = 100;
  DEFAULT_PARAM.fs_accelero = XL_FS_2;
  DEFAULT_PARAM.fs_gyro = G_FS_500;
  fn_select_port(0);
  LSM1.fn_update_param(DEFAULT_PARAM, LSM1.fn_get_advanced_param());
  LSM2.fn_update_param(DEFAULT_PARAM, LSM1.fn_get_advanced_param());
  fn_select_port(7);
  LSM3.fn_update_param(DEFAULT_PARAM, LSM1.fn_get_advanced_param());
  LSM4.fn_update_param(DEFAULT_PARAM, LSM1.fn_get_advanced_param());

  // Reglage du timer regulant la lecture des IMU
  ReadClock.attachInterruptInterval(u_Te * 1000, fn_read_time);

  // Initialisation de l'ecran
  display.begin();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
}


void loop() {
  // put your main code here, to run repeatedly:
  char c_mode; //mode de fonctionnement selectionne par l'utilisateur
  // information : en attente des ordres du serveur
  Serial.println("Attente de donnees du serveur...");
  display.setCursor(0, 0);
  display.println("En attente...");
  display.display();
  while (liaison.m_client.available() == 0) { // attente des ordres du serveur
    if (c_DEBUGGING == '1') {
      Serial.print("liaison.m_client.connected():");
      Serial.println(liaison.m_client.connected());
    }

    delay(1000);
  }
  c_mode = liaison.m_client.read(); // reception du mode souhaite par l'utilisateur
  if (c_DEBUGGING == '1') {
    Serial.print("mode=");
    Serial.println(c_mode);
  }
  if (c_mode == 'a') {  // si le mode acquisition est active
    // reception de la duree d'acquisition souhaite
    s_server_msg = "";
    while (liaison.m_client.available() == 0) { delay(1); }
    while (liaison.m_client.available() > 0) {
      s_server_msg.concat(char(liaison.m_client.read()));
    }
    u_tps_seq = s_server_msg.toInt();
    // reglage du timer declenchant l'envoi des donnees au serveur
    SendClock.attachInterruptInterval(u_tps_seq * 1000000, fn_send_time);
    // information : demarrage de l'acquisition
    Serial.println("start");
    if (c_DEBUGGING == '1') { Serial.println("Démarrage des acquisitions de blocs"); }
    display.println("Enregistrement...");
    display.display();
    // initialisation des variables servant a la mesure du temps d'acquisition ecoule en ms
    u_start_time = millis();
    u_stop_time = u_start_time;
    b_reading = false; // forcage du flag de lecture des IMU à false
    while (u_stop_time - u_start_time <= u_tps_seq*1000) // tant que le temps d'acquisition ecoule est inferieur a la duree d'acquisition demandee
    //while (u_stop_time - u_start_time <= 50) // tant que le temps d'acquisition ecoule est inferieur a 50 ms (simulateur)

    {

      if (b_reading == true) // si le flag de lecture des IMU est passe a true (ISR)

      {
        fn_IMUtoSDRAM(); // lecture des IMU 
        b_reading = false; // reinitialisation du flag de lecture des IMU
      }
  
      u_stop_time = millis(); // actualisation du temps d'acquisition ecoule
      if (b_sdramFULL == true) { // si la RAM est saturee
        b_reading = false; // reinitialisation du flag de lecture des IMU

        display.println("!! sat.mem !!");
        display.display(); // information : arret de l'acquisition
        u_stop_time = u_tps_seq*1000 + u_start_time + 1; // arret de la boucle while => arret des acquisitions
    
      }
      
    }
    // information : arret de l'acquisition
    Serial.println("stop");
    Serial.print("u_ind_time_list=");
    Serial.println(u_ind_time_list);
    // information : envoi des donnees au serveur
    display.println("Envoi au serveur...");
    display.display();
    // attente du passage du flag d'envoi au serveur a true
    while (b_sending == false) { delay(1); } 
    // envoi des donnees au serveur
    liaison.fn_send_data_to_PC(u_ind_time_list, LSM1.fn_get_basic_param(), LSM1.fn_get_advanced_param());
    Serial.print("code retour de l'envoi vers le serveur (1:le json n'est pas envoye completement, 2 le top E de end n'est pas envoye completement) n_err_send:");
    Serial.println(n_err_send);
    // desactivation du timer declenchant l'envoi des donnees au serveur
    SendClock.detachInterrupt();
    // réinitialisation de la quantite d'echantillons collectes, du curseur de positionnement dans la SDRAM et de la SDRAM
    u_ind_time_list = 0;
    u_dataset_start = 0;
    liaison.fn_free_memory();
    liaison.fn_set_memory();

    if (c_DEBUGGING == '1') { Serial.println("Memory renewed !"); }
    // information : fin d'execution du mode
    Serial.println("Done.");
    display.clearDisplay();
  }
  if (c_mode == 'p') { // si le mode parametres est active
    // information : 1ere etape mode p
    Serial.println("c_Mode p active");
    display.println("Recup. param.");
    display.display();
    // recuperation des parametres d'acquisition
    fn_select_port(0);
    t_BasicParam CurrentBParam = LSM1.fn_get_basic_param();
    t_AdvancedParam CurrentAParam = LSM1.fn_get_advanced_param(); 
    // information : 2eme etape mode p
    display.println("Envoi au serveur");
    display.display();
    // envoi des parametres d'acquisition
    liaison.fn_send_param_to_PC(CurrentBParam, CurrentAParam);
    display.clearDisplay();
  }
  if (c_mode == 'r') { // si le mode reglages est active
    // envoi des parametres actuels au serveur
    t_BasicParam BParam;
    t_AdvancedParam AParam;
    fn_select_port(0);
    t_BasicParam CurrentBParam = LSM1.fn_get_basic_param();
    t_AdvancedParam CurrentAParam = LSM1.fn_get_advanced_param();
    liaison.fn_send_param_to_PC(CurrentBParam, CurrentAParam);
    if (c_DEBUGGING == '1') { Serial.println("parametres actuels envoyes au serveur"); }
    // information : reception des nouveaux parametres
    display.println("Recup. nouv. param.");
    display.display();
    // reception des nouveaux parametres
    BParam = liaison.fn_generate_new_basic_param_struct();
    AParam = liaison.fn_generate_new_advanced_param_struct(CurrentAParam);
    if (c_DEBUGGING == '1') { Serial.println("nouveaux parametres recus"); }
    // information : reglage des IMU
    display.println("Reglage IMU");
    display.display();
    // reglage des IMU
    fn_select_port(0);
    LSM1.fn_update_param(BParam, AParam);
    LSM2.fn_update_param(BParam, AParam);
    fn_select_port(7);
    LSM3.fn_update_param(BParam, AParam);
    LSM4.fn_update_param(BParam, AParam);
    ReadClock.attachInterruptInterval(((int)((1.0/(BParam.Fe)) * 1000000)), fn_read_time);
    if (c_DEBUGGING == '1') { Serial.println("IMU reglees"); }
    display.clearDisplay();
  }
  if (c_mode == 's') { // si le mode stop est active
    // fermeture de la connection avec le serveur
    liaison.fn_close_connection();
    // information : arret du systeme
    display.clearDisplay();
    display.println("Arret.");
    display.display();
    // blocage de l'execution de la loop Arduino
    while (true) {}
  }
 
}
