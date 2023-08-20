#include "DataManager.h"


DataManager::DataManager()
{
    

    
}

DataManager::~DataManager()
{
   
  delete m_gyr1;
  delete m_acc1;
  delete m_gyr2;
  delete m_acc2;
  delete m_gyr3;
  delete m_acc3;
  delete m_gyr4;
  delete m_acc4;
  delete m_tps;
    
  delete m_SERVER_IP;
    
}

void DataManager::fn_set_memory()
{
  
  m_gyr1 = (int16_t*)SDRAM.malloc(SIZE);
  m_acc1 = (int16_t*)SDRAM.malloc(SIZE);
  m_gyr2 = (int16_t*)SDRAM.malloc(SIZE);
  m_acc2 = (int16_t*)SDRAM.malloc(SIZE);
  m_gyr3 = (int16_t*)SDRAM.malloc(SIZE);
  m_acc3 = (int16_t*)SDRAM.malloc(SIZE);
  m_gyr4 = (int16_t*)SDRAM.malloc(SIZE);
  m_acc4 = (int16_t*)SDRAM.malloc(SIZE);
  m_tps = (uint32_t*)SDRAM.malloc(SIZE);

  
}

void DataManager::fn_free_memory()
{
  SDRAM.free((int16_t*)m_gyr1);
  SDRAM.free((int16_t*)m_acc1);
  SDRAM.free((int16_t*)m_gyr2);
  SDRAM.free((int16_t*)m_acc2);
  SDRAM.free((int16_t*)m_gyr3);
  SDRAM.free((int16_t*)m_acc3);
  SDRAM.free((int16_t*)m_gyr4);
  SDRAM.free((int16_t*)m_acc4);
  SDRAM.free((uint32_t*)m_tps);

}


bool DataManager::fn_connect_to_PC()
{
  return m_client.connect(m_SERVER_IP, m_SERVER_PORT);
}


void DataManager::fn_send_data_to_PC(uint16_t ind_time_list, t_BasicParam BParam, t_AdvancedParam AParam)
{
  
  int i = 0;
  uint16_t j = 0;
  String data_to_send = "";
  Serial.println("Debut SendToPC");
  
  // creation d'une chaine de caracteres avec le contenu de la SDRAM
  while (j < ind_time_list) {
    data_to_send.concat(String(m_tps[j] - m_tps[0]));  data_to_send.concat(",");
    data_to_send.concat(String(m_gyr1[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr1[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr1[i + 2])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc1[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc1[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc1[i + 2])); 
    data_to_send.concat(";");
    data_to_send.concat(String(m_tps[j + 1] - m_tps[0])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr2[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr2[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr2[i + 2])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc2[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc2[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc2[i + 2])); 
    data_to_send.concat(";");
    data_to_send.concat(String(m_tps[j + 2] - m_tps[0])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr3[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr3[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr3[i + 2])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc3[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc3[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc3[i + 2])); 
    data_to_send.concat(";");
    data_to_send.concat(String(m_tps[j + 3] - m_tps[0])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr4[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr4[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_gyr4[i + 2]));  data_to_send.concat(",");
    data_to_send.concat(String(m_acc4[i])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc4[i + 1])); data_to_send.concat(",");
    data_to_send.concat(String(m_acc4[i + 2])); data_to_send.concat("E");
    m_client.print(String(data_to_send.length())); // Envoi au serveur de la longueur de la chaine de caractere
    m_client.print(data_to_send); // Envoi au serveur dela chaine de caractere
    // Mise a jour des curseurs de la SDRAM et réinitialisation de la chaine de caractere
    i += 3;
    j+=4;
    data_to_send = "";
  }
  if (j >= ind_time_list)
  {
    m_client.print("EOS"); // signalement au serveur de la fin d'envoi des donnees de la SDRAM
    Serial.println("Fin de sequence");
  }

  fn_send_param_to_PC(BParam, AParam); // Envoi des parametres d'acquisition au serveur
 
}



uint8_t DataManager::fn_generate_FS_ACC(int param)
{
  uint8_t FS_ACC;
  switch (param)
  {
    case 2:
      FS_ACC = XL_FS_2;
      break;
    case 16:
      FS_ACC = XL_FS_16;
      break;
    case 4:
      FS_ACC = XL_FS_4;
      break;
    case 8:
      FS_ACC = XL_FS_8;
      break;  
    
  }

  
  return FS_ACC;
}

uint8_t DataManager::fn_generate_FS_GYR(int param)
{
  uint8_t FS_GYR;
  switch (param)
  {
    case 250:
      FS_GYR = G_FS_250;
      break;
    case 500:
      FS_GYR = G_FS_500;
      break;
    case 1000:
      FS_GYR = G_FS_1000;
      break;
    case 2000:
      FS_GYR = G_FS_2000;
      break;  
  }
  
  return FS_GYR;
}


uint8_t DataManager::fn_generate_FTYPE(int param)
{
  uint8_t FTYPE;
  switch (param)
  {
    case 304:
      FTYPE = 0b000;
      break;
    case 221:
      FTYPE = 0b001;
      break;
    case 166:
      FTYPE = 0b010;
      break;
    case 453:
      FTYPE = 0b011;
      break;  
    case 99:
      FTYPE = 0b100;
      break;
    case 49:
      FTYPE = 0b101;
      break;
    case 25:
      FTYPE = 0b110;
      break;
    case 12:
      FTYPE = 0b111;
      break;  
    
  }
  return FTYPE;
}

uint8_t DataManager::fn_generate_HPCF_XL(int param)
{
  uint8_t HPCF_XL;
  switch (param)
  {
    case 415:
      HPCF_XL = 0b000;
      break;
    case 166:
      HPCF_XL = 0b001;
      break;
    case 83:
      HPCF_XL = 0b010;
      break;
    case 36:
      HPCF_XL = 0b011;
      break;  
    case 16:
      HPCF_XL = 0b100;
      break;
    case 8:
      HPCF_XL = 0b101;
      break;
    case 4:
      HPCF_XL = 0b110;
      break;
    case 2:
      HPCF_XL = 0b111;
      break;  
    
  }
  return HPCF_XL;
}
  
  
void DataManager::fn_send_param_to_PC(t_BasicParam BParam, t_AdvancedParam AParam) // meme principe que pour fn_send_data_to_PC
{
  String data_to_send = "";
  data_to_send.concat(BParam.Fe);  data_to_send.concat(",");
  data_to_send.concat(BParam.fs_accelero); data_to_send.concat(",");
  data_to_send.concat(BParam.fs_gyro); data_to_send.concat(",");
  data_to_send.concat(AParam.LPF1_SEL_G); data_to_send.concat(",");
  data_to_send.concat(AParam.FTYPE); data_to_send.concat(",");
  data_to_send.concat(AParam.LPF2_XL_EN); data_to_send.concat(",");
  data_to_send.concat(AParam.HPCF_XL); 
  m_client.print(data_to_send);
   
}

t_BasicParam DataManager::fn_generate_new_basic_param_struct()
{

  t_BasicParam param;
  String Fe = "";
  String fs_acc = "";
  String fs_gyr = "";
 

  while(m_client.available() == 0)
  {

      delay(1);
  }
  while(m_client.available() > 0)
  {
    Fe.concat(char(m_client.read()));
  }
 
  
  param.Fe = Fe.toInt();
  Serial.println("Fe ok");

  while(m_client.available() == 0)
  {
      delay(1);
  }
  while(m_client.available() > 0)
  {
    fs_acc.concat(char(m_client.read()));
  }
  param.fs_accelero = fn_generate_FS_ACC(fs_acc.toInt());
  Serial.println("FS acc ok");

  while(m_client.available() == 0)
  {
      delay(1);
  }
  while(m_client.available() > 0)
  {
     fs_gyr.concat(char(m_client.read()));
  }
  param.fs_gyro = fn_generate_FS_GYR(fs_gyr.toInt());
  Serial.println("FS gyr ok");

  return param;
} 
 
t_AdvancedParam DataManager::fn_generate_new_advanced_param_struct(t_AdvancedParam AParam)
{
  String ftype = "";
  String hpcf_xl = "";
  String lpf_gyr = "";
  String lpf_acc = "";
  t_AdvancedParam res = AParam;
  while(m_client.available() == 0)
  {
      delay(1);
  }
  while(m_client.available() > 0)
  {
     lpf_gyr.concat(char(m_client.read()));
  }
  if (lpf_gyr == "2")
  {
    res.LPF1_SEL_G = 1;
    Serial.println("2 filtres en sortie de gyro");

    while(m_client.available() == 0)
    {
        delay(1);
    }
    while(m_client.available() > 0)
    {
      ftype.concat(char(m_client.read()));
    }
    res.FTYPE = fn_generate_FTYPE(ftype.toInt());
    Serial.println("BW 2° filtre gyro ok");
 
     while(m_client.available() == 0)
    {
        delay(1);
    }
    while(m_client.available() > 0)
    {
      lpf_acc.concat(char(m_client.read()));
    }
    if (lpf_acc == "2")
    {
      res.LPF2_XL_EN = 1;
      Serial.println("2 filtres en sortie de acc");
    
      while(m_client.available() == 0)
      {
          delay(1);
      }
      while(m_client.available() > 0)
      {
        hpcf_xl.concat(char(m_client.read()));
      }
    
      res.HPCF_XL = fn_generate_HPCF_XL(hpcf_xl.toInt());
      Serial.println("BW 2° filtres de acc ok");

    }
    else
    {
      res.LPF2_XL_EN = 0;
      Serial.println("1 filtre en sortie de acc");
    }

  }
  else
  {
    res.LPF1_SEL_G = 0;
    Serial.println("1 filtre en sortie de gyro");
    while(m_client.available() == 0)
    {
        delay(1);
    }
    while(m_client.available() > 0)
    {
      lpf_acc.concat(char(m_client.read()));
    }
    Serial.println(lpf_acc);
    if (lpf_acc == "2")
    {
      res.LPF2_XL_EN = 1;
      Serial.println("2 filtres en sortie d'acc");
    
      while(m_client.available() == 0)
      {
          delay(1);
      }
      while(m_client.available() > 0)
      {
        hpcf_xl.concat(char(m_client.read()));
      }
      
      res.HPCF_XL = fn_generate_HPCF_XL(hpcf_xl.toInt());
      Serial.println("BW 2° filtre acc ok");
    }
    else
    {
      res.LPF2_XL_EN = 0;
      Serial.println("1 filtre en sortie de acc");

     
    }
  } 
  return res;
}

void DataManager::fn_close_connection()
{
 m_client.stop();
}