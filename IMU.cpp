#include "IMU.h"

IMU::IMU(uint8_t usr_BUS_ADDRESS, uint8_t usr_SELF_ADDRESS)
{
    m_BUS_ADDRESS = usr_BUS_ADDRESS;
    m_SELF_ADDRESS = usr_SELF_ADDRESS;
    m_acc.x = 0;
    m_acc.y = 0;
    m_acc.z = 0;
    m_gyr.x = 0;
    m_gyr.y = 0;
    m_gyr.z = 0;

    
    m_BasicParam.Fe = 100;
    m_BasicParam.fs_accelero = 0;
    m_BasicParam.fs_gyro = 0;

    m_AdvancedParam.HP_EN_G = 1;
    m_AdvancedParam.FTYPE = 0;
    m_AdvancedParam.LOW_PASS_ON_6D = 1;
    m_AdvancedParam.HPCF_XL = 1;
    m_AdvancedParam.LPF2_XL_EN = 1;
    m_AdvancedParam.USER_OFF_W = 0;
    m_AdvancedParam.USER_OFF_ON_WU = 1;
    m_AdvancedParam.SLOPE_FDS = 1;
    m_AdvancedParam.USER_OFF_ON_OUT = 0;
    m_AdvancedParam.HP_SLOPE_XL_EN = 0;
    m_AdvancedParam.LPF1_SEL_G = 1;
    
}

IMU::~IMU()
{
  
}



void IMU::fn_register_to_read(uint8_t REG) 
{
  Wire.beginTransmission(m_SELF_ADDRESS);
  Wire.write(REG);
  Wire.endTransmission();
}
 


void IMU::fn_read_one_register(uint8_t REG, volatile int16_t* read_value) 
{ 
  fn_register_to_read(REG);
  Wire.requestFrom(m_SELF_ADDRESS, (uint8_t)1);
  *read_value = Wire.read();                            
  
}

uint8_t IMU::fn_read_registers(uint8_t FIRST_REGISTER, uint8_t bytes_to_read, volatile uint8_t* read_value) 
{
  fn_register_to_read(FIRST_REGISTER);
  uint8_t bytes_read = Wire.requestFrom(m_SELF_ADDRESS, bytes_to_read);
  for (uint8_t i = 0; i < bytes_to_read; i++) 
  {
    *read_value++ = Wire.read();
  }
  return bytes_read;
}

void IMU::fn_write_one_register(uint8_t REG, uint8_t DATA) 
{
  Wire.beginTransmission(m_SELF_ADDRESS);
  Wire.write(REG);
  Wire.write(DATA);
  Wire.endTransmission();
  delay(10);
}

void IMU::fn_write_registers(uint8_t* REG, uint8_t* DATA) 
{
    int taille = sizeof(*REG)/sizeof(REG[0]);
    for(uint8_t i = 0; i < taille; i++)
    {
        fn_write_one_register(REG[i], DATA[i]);
    }
}



void IMU::fn_update_param(t_BasicParam new_basic_param, t_AdvancedParam new_advanced_param) 
{
  //mise a jour des structures de parametres
  m_BasicParam = new_basic_param;
  m_AdvancedParam = new_advanced_param;
  //reglage des IMU
  fn_param_to_sensor();
  
}

void IMU::fn_param_to_sensor() // fqce et FS G+A
{                  
    // remise a zero des registres
    fn_write_one_register(CTRL1_XL, CTRL1_XL_DEFAULT);
    fn_write_one_register(CTRL2_G, CTRL2_G_DEFAULT);
    fn_write_one_register(CTRL4_C, CTRL4_C_DEFAULT);
    fn_write_one_register(CTRL6_C, CTRL6_C_DEFAULT);
    fn_write_one_register(CTRL8_XL, CTRL8_XL_DEFAULT);
    fn_write_one_register(CTRL7_G, CTRL7_G_DEFAULT);
    fn_write_one_register(TAP_CFG0, TAP_CFG0_DEFAULT);
    fn_write_one_register(WAKE_UP_THS, WAKE_UP_THS_DEFAULT);
    //  constructions des mots binaires a ecrire dans les registres
    uint8_t ctrl2 = (m_BasicParam.fs_gyro << 2) | CTRL2_G_DEFAULT ;
    uint8_t ctrl1 = (m_AdvancedParam.LPF2_XL_EN << 1) |(m_BasicParam.fs_accelero << 2) | CTRL1_XL_DEFAULT ;
    uint8_t ctrl4 = (m_AdvancedParam.LPF1_SEL_G << 1) | CTRL4_C_DEFAULT;
    uint8_t ctrl6 = m_AdvancedParam.FTYPE | (m_AdvancedParam.USER_OFF_W << 3) | CTRL6_C_DEFAULT ;
    uint8_t ctrl7 = (m_AdvancedParam.HP_EN_G << 6) | (m_AdvancedParam.USER_OFF_ON_OUT << 1) | CTRL7_G_DEFAULT;
    uint8_t ctrl8 = m_AdvancedParam.LOW_PASS_ON_6D | (m_AdvancedParam.HP_SLOPE_XL_EN << 2) | CTRL8_XL_DEFAULT;
    uint8_t tap = (m_AdvancedParam.SLOPE_FDS << 4) | TAP_CFG0_DEFAULT;
    uint8_t wake = m_AdvancedParam.USER_OFF_ON_WU << 6 | WAKE_UP_THS_DEFAULT;
    // ecriture des registres
    fn_write_one_register(CTRL1_XL, ctrl1);
    fn_write_one_register(CTRL2_G, ctrl2);
    fn_write_one_register(CTRL4_C, ctrl4);
    fn_write_one_register(CTRL6_C, ctrl6);
    fn_write_one_register(CTRL8_XL, ctrl8);
    fn_write_one_register(CTRL7_G, ctrl7);
    fn_write_one_register(TAP_CFG0, tap);
    fn_write_one_register(WAKE_UP_THS, wake);
}


t_BasicParam IMU::fn_get_basic_param() 
{
  return m_BasicParam;
}

t_AdvancedParam IMU::fn_get_advanced_param() 
{
  return m_AdvancedParam;
}
void IMU::fn_read_accel(uint8_t* output) 
{

  fn_read_registers(OUTX_L_A, 6, output);

}

void IMU::fn_read_gyro(uint8_t* output) 
{
  
  fn_read_registers(OUTX_L_G, 6, output);

}

void IMU::fn_read_accel_and_gyro(volatile uint8_t* output, uint8_t err) 
{
  uint8_t bytes_read = fn_read_registers(OUTX_L_G, 12, output);
  if (bytes_read != 12) {err +=1;}
}
