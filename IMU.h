#ifndef IMU_H
#define IMU_H

#include "Register.h"
#include "Wire.h" // librairies pour la communication I2C
using namespace std;



/// @brief valeurs en sortie de l'accelerometre
struct t_Accelero
{
    int16_t x;
    int16_t y;
    int16_t z;
};

/// @brief valeurs en sortie du gyrometre
struct t_Gyro
{
    
    int16_t x;
    int16_t y;
    int16_t z;
};



/// @brief parametres de bases : frequence d'echantillonnage et echelles de mesure (full scale)
struct t_BasicParam
{
    int Fe; 
    uint8_t fs_accelero;
    uint8_t fs_gyro;
};


/// @brief parametres avances : filtres
struct t_AdvancedParam
{
    uint8_t HP_EN_G;
    uint8_t FTYPE ;
    uint8_t LOW_PASS_ON_6D ;
    uint8_t HPCF_XL ;
    uint8_t LPF2_XL_EN ;
    uint8_t USER_OFF_W ;
    uint8_t USER_OFF_ON_WU ;
    uint8_t SLOPE_FDS;
    uint8_t USER_OFF_ON_OUT ;
    uint8_t HP_SLOPE_XL_EN;
    uint8_t LPF1_SEL_G;
};


class IMU
{
    public:
        /// @brief constructeur
        /// @param usr_BUS_ADDRESS  : port du hub I2C sur lequel l'IMU est connectee
        /// @param usr_SELF_ADDRESS : adresse I2C de l'IMU
        IMU(uint8_t usr_BUS_ADDRESS, uint8_t usr_SELF_ADDRESS);

        /// @brief destructeur
        ~IMU();

        /// @brief mise a jour des structures de parametres et reglage des IMU
        /// @param new_basic_param : nouveaux parametres basiques souhaites
        /// @param new_advanced_param : nouveaux parametres avances souhaites
        void fn_update_param(t_BasicParam new_basic_param, t_AdvancedParam new_advanced_param);

        /// @brief reglage des IMU
        void fn_param_to_sensor(); 

        /// @brief accesseur pour BasicParam
        /// @return BasicParam
        t_BasicParam fn_get_basic_param();

        /// @brief accesseur pour AdvancedParam
        /// @return AdvancedParam
        t_AdvancedParam fn_get_advanced_param();

        /// @brief lecture de l'accelero
        /// @param output : stockage du resultat
        void fn_read_accel(uint8_t* output);

        /// @brief lecture du gyro
        /// @param output : stockage du resultat
        void fn_read_gyro(uint8_t* output) ;

        /// @brief lecture de l'accelero et du gyro
        /// @param output : stockage du resultat
        /// @param err : erreur de lecture
        void fn_read_accel_and_gyro(volatile uint8_t* output, uint8_t err);

        /// @brief ecriture d'un registre de l'IMU
        /// @param REG registre a modifier
        /// @param DATA donnees a ecrire
        void fn_write_one_register(uint8_t REG,  uint8_t DATA);

        /// @brief lecture d'un registre de l'IMU
        /// @param REG registre a lire
        /// @param read_value stockage du resultat
        void fn_read_one_register(uint8_t REG, volatile int16_t* read_value) ;

        /// @brief lecture de plusieurs registres d'adresses consecutives
        /// @param FIRST_REGISTER adresse du premier registre à lire
        /// @param bytes_to_read nombre d'octets à lire
        /// @param read_value stockage des resultats
        /// @return nombre d'octets lus
        uint8_t fn_read_registers(uint8_t FIRST_REGISTER, uint8_t bytes_to_read, volatile uint8_t* read_value);
       

    private: 
        struct t_Accelero m_acc; 
        struct t_Gyro m_gyr;
        struct t_BasicParam m_BasicParam;
        struct t_AdvancedParam m_AdvancedParam;
        uint8_t m_BUS_ADDRESS;
        uint8_t m_SELF_ADDRESS;
        
        
        /// @brief ecriture de plusieurs registres de l'IMU
        /// @param REG tableau contenant les registres a modifier
        /// @param DATA tableau contenant les donnees a ecrire dans chaque registre a l'adresse du registre correspondant
        void fn_write_registers(uint8_t* REG, uint8_t* DATA);

        /// @brief connexion I2C avec le registre a lire
        /// @param REG registre a lire
        void fn_register_to_read(uint8_t REG);
};
#endif //IMU_H