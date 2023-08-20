// adresse du hub I2C
#define HUB_ADDRESS (uint8_t)0x70

/* Toutes les valeurs ci-dessous sont definies dans la documentation des  IMU LSM6DSOX */

// adresses des registres de controle du FIFO 
#define FIFO_CTRL1 (uint8_t)0x07
#define FIFO_CTRL2 (uint8_t)0x08
#define FIFO_CTRL3 (uint8_t)0x09
#define FIFO_CTRL4 (uint8_t)0x0A

// valeur par defaut des registres de controle du FIFO 
#define FIFO_CTRL1_DEFAULT (uint8_t)0b00000000
#define FIFO_CTRL2_DEFAULT (uint8_t)0b00000000
#define FIFO_CTRL3_DEFAULT (uint8_t)0b00000000
#define FIFO_CTRL4_DEFAULT (uint8_t)0b00000001

// adresses des registres de controle des capteurs
#define CTRL1_XL (uint8_t)0x10
#define CTRL2_G (uint8_t)0x11
#define CTRL4_C (uint8_t)0x13
#define CTRL5_C (uint8_t)0x14
#define CTRL6_C (uint8_t)0x15
#define CTRL7_G (uint8_t)0x16
#define CTRL8_XL (uint8_t)0x17
#define CTRL9_XL (uint8_t)0x18
#define TAP_CFG0 (uint8_t)0x56
#define WAKE_UP_THS (uint8_t)0x5B

// valeurs des registres de controle des capteurs 
#define CTRL1_XL_DEFAULT (uint8_t)0b10000000
#define CTRL2_G_DEFAULT (uint8_t)0b10000000
#define CTRL4_C_DEFAULT (uint8_t)0b00000010
#define CTRL5_C_DEFAULT (uint8_t)0b01100000
#define CTRL6_C_DEFAULT (uint8_t)0b00000000
#define CTRL7_G_DEFAULT (uint8_t)0b00000000
#define CTRL8_XL_DEFAULT (uint8_t)0b00000000
#define CTRL9_XL_DEFAULT (uint8_t)0b11100000
#define TAP_CFG0_DEFAULT (uint8_t)0b00000000
#define WAKE_UP_THS_DEFAULT (uint8_t)0b01000000

#define RST_REG {FIFO_CTRL1, FIFO_CTRL2, FIFO_CTRL3, FIFO_CTRL4, CTRL1_XL, CTRL2_G, CTRL4_C, CTRL5_C, CTRL6_C, CTRL7_G, CTRL8_XL, CTRL9_XL, TAP_CFG0, WAKE_UP_THS}
#define RST_VAL {FIFO_CTRL1_DEFAULT, FIFO_CTRL2_DEFAULT, FIFO_CTRL3_DEFAULT, FIFO_CTRL4_DEFAULT, CTRL1_XL_DEFAULT, CTRL2_G_DEFAULT, CTRL4_C_DEFAULT, CTRL5_C_DEFAULT, CTRL6_C_DEFAULT, CTRL7_G_DEFAULT, CTRL8_XL_DEFAULT, CTRL9_XL_DEFAULT, TAP_CFG0_DEFAULT, WAKE_UP_THS_DEFAULT}

// codes binaires des valeurs d'ODR 
#define ODR_0 (uint8_t)0b0000
#define ODR_12_5 (uint8_t)0b0001
#define ODR_26 (uint8_t)0b0010
#define ODR_52 (uint8_t)0b0011
#define ODR_104 (uint8_t)0b0100
#define ODR_208 (uint8_t)0b0101
#define ODR_416 (uint8_t)0b0110
#define ODR_833 (uint8_t)0b0111
#define ODR_1_66_K (uint8_t)0b1000
#define ODR_3_33_K (uint8_t)0b1001
#define ODR_6_66_K (uint8_t)0b1010

// codes binaires des valeurs d'echelles de mesures de l'accelero
#define XL_FS_2 (uint8_t)0b00
#define XL_FS_16 (uint8_t)0b01
#define XL_FS_4 (uint8_t)0b10
#define XL_FS_8 (uint8_t)0b11

// codes binaires des valeurs d'echelles de mesures du gyro
#define G_FS_250 (uint8_t)0b00
#define G_FS_500 (uint8_t)0b01
#define G_FS_1000 (uint8_t)0b10
#define G_FS_2000 (uint8_t)0b11

// adresses des registres de sortie des capteurs 
#define OUTX_L_G 0x22
#define OUTX_H_G 0x23
#define OUTY_L_G 0x24
#define OUTY_H_G 0x25
#define OUTZ_L_G 0x26
#define OUTZ_H_G 0x27
#define OUTX_L_A 0x28
#define OUTX_H_A 0x29
#define OUTY_L_A 0x2A
#define OUTY_H_A 0x2B
#define OUTZ_L_A 0x2C
#define OUTZ_H_A 0x2D

// adresses des registres de statut du FIFO 
#define FIFO_STATUS1 = 0x3A
#define FIFO_STATUS2 = 0x3B

// adresses des registres de sortie du FIFO 
#define FIFO_DATA_OUT_TAG = 0x78
#define FIFO_DATA_OUT_X_L = 0x79
#define FIFO_DATA_OUT_X_H = 0x7A
#define FIFO_DATA_OUT_Y_L = 0x7B
#define FIFO_DATA_OUT_Y_H = 0x7C
#define FIFO_DATA_OUT_Z_L = 0x7D
#define FIFO_DATA_OUT_Z_H = 0x7E




