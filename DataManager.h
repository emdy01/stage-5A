#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#define SIZE 906*906 // taille des partitions de la SDRAM
#include "SDRAM.h"
#include <WiFi.h>
#include <iostream>
#include "IMU.h"
using namespace std;

class DataManager
{
    private:
      const char* m_SERVER_IP = "192.168.0.99";
      const int m_SERVER_PORT = 6789;  

      /// @brief conversion de la valeur recue du serveur en parametre fs_acc
      /// @param param valeur recue du serveur
      /// @return fs_acc
      uint8_t fn_generate_FS_ACC(int param);

      /// @brief conversion de la valeur recue du serveur en parametre fs_gyr
      /// @param param valeur recue du serveur
      /// @return fs_gyr
      uint8_t fn_generate_FS_GYR(int param); 

      /// @brief conversion de la valeur recue du serveur en parametre ftype
      /// @param param valeur recue du serveur
      /// @return ftype
      uint8_t fn_generate_FTYPE(int param);

      /// @brief conversion de la valeur recue du serveur en parametre hpcf_xl 
      /// @param param valeur recue du serveur
      /// @return hpcf_xl 
      uint8_t fn_generate_HPCF_XL(int param);      
      
    public:

      // tableaux representant les partitions de la SDRAM
      int16_t* m_gyr1;
      int16_t* m_acc1;
      int16_t* m_gyr2;
      int16_t* m_acc2;
      int16_t* m_gyr3;
      int16_t* m_acc3;
      int16_t* m_gyr4;
      int16_t* m_acc4;
      uint32_t* m_tps;

      /// @brief constructeur
      DataManager();

      /// @brief destructeur
      ~DataManager();

      /// @brief initialisation de la memoire RAM
      void fn_set_memory();

      /// @brief liberation de la memoire RAM
      void fn_free_memory();

      /// @brief connexion au serveur
      /// @return true si connecte, false sinon
      bool fn_connect_to_PC();

      /// @brief envoi des donnees et parametres d'acquisition au serveur
      /// @param ind_time_list quantite d'echantillons collectes
      /// @param BParam parametres basiques utilises
      /// @param AParam parametres avancees utilises
      void fn_send_data_to_PC(uint16_t ind_time_list, t_BasicParam BParam, t_AdvancedParam AParam);

      /// @brief envoi des parametres d'acquisition au serveur
      /// @param BParam parametres basiques a envoyer
      /// @param AParam parametres avancees a envoyer
      void fn_send_param_to_PC(t_BasicParam BParam, t_AdvancedParam AParam);

      /// @brief creation d'une structure BasicParam a partir de valeurs recues du serveur
      /// @return la structure BasicParam creee
      t_BasicParam fn_generate_new_basic_param_struct();

      /// @brief creation d'une structure AdvancedParam a partir de valeurs recues du serveur et de la structure courante
      /// @param AParam structure courante
      /// @return la structure AdvancedParam creee
      t_AdvancedParam fn_generate_new_advanced_param_struct(t_AdvancedParam AParam);

      /// @brief fermeture de la connection avec le serveur
      void fn_close_connection();

      WiFiClient m_client; // client TCP

  
};



#endif //SCREENMANAGER_H