#define ADXL375_DEVID_REG          0x00
#define ADXL375_THRESH_TAP_REG     0x1D
#define ADXL375_OFSX_REG           0x1E
#define ADXL375_OFSY_REG           0x1F
#define ADXL375_OFSZ_REG           0x20
#define ADXL375_DUR_REG            0x21
#define ADXL375_LATENT_REG         0x22
#define ADXL375_WINDOW_REG         0x23
#define ADXL375_THRESH_ACT_REG     0x24
#define ADXL375_THRESH_INACT_REG   0x25
#define ADXL375_TIME_INACT_REG     0x26
#define ADXL375_ACT_INACT_CTL_REG  0x27
#define ADXL375_THRESH_FF_REG      0x28
#define ADXL375_TIME_FF_REG        0x29
#define ADXL375_TAP_AXES_REG       0x2A
#define ADXL375_ACT_TAP_STATUS_REG 0x2B
#define ADXL375_BW_RATE_REG        0x2C
#define ADXL375_POWER_CTL_REG      0x2D
#define ADXL375_INT_ENABLE_REG     0x2E
#define ADXL375_INT_MAP_REG        0x2F
#define ADXL375_INT_SOURCE_REG     0x30
#define ADXL375_DATA_FORMAT_REG    0x31
#define ADXL375_DATAX0_REG         0x32
#define ADXL375_DATAX1_REG         0x33
#define ADXL375_DATAY0_REG         0x34
#define ADXL375_DATAY1_REG         0x35
#define ADXL375_DATAZ0_REG         0x36
#define ADXL375_DATAZ1_REG         0x37
#define ADXL375_FIFO_CTL           0x38
#define ADXL375_FIFO_STATUS        0x39
 
//Data rate codes.
#define ADXL375_3200HZ      0x0F
#define ADXL375_1600HZ      0x0E
#define ADXL375_800HZ       0x0D
#define ADXL375_400HZ       0x0C
#define ADXL375_200HZ       0x0B
#define ADXL375_100HZ       0x0A
#define ADXL375_50HZ        0x09
#define ADXL375_25HZ        0x08
#define ADXL375_12HZ5       0x07
#define ADXL375_6HZ25       0x06
 
#define ADXL375_SPI_READ    0x80
#define ADXL375_SPI_WRITE   0x00
#define ADXL375_MULTI_BYTE  0x60
 
#define ADXL375_X           0x00
#define ADXL375_Y           0x01
#define ADXL375_Z           0x02

#define DEVICE (0x53)    //ADXL345 device address


#define TO_READ (6)        //num of bytes we are going to read each time (two bytes for each axis)

