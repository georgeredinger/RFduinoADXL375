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

#ifndef _ADXL375_H_
#define _ADXL375_H_
#include <stdint.h>
//#pragma once

#define ADXL375_ADDRESS_ALT_LOW     0x53 // alt address pin low (GND)
#define ADXL375_ADDRESS_ALT_HIGH    0x1D // alt address pin high (VCC)
#define ADXL375_DEFAULT_ADDRESS     ADXL375_ADDRESS_ALT_LOW
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

#define ADXL375_AIC_ACT_AC_BIT      7
#define ADXL375_AIC_ACT_X_BIT       6
#define ADXL375_AIC_ACT_Y_BIT       5
#define ADXL375_AIC_ACT_Z_BIT       4
#define ADXL375_AIC_INACT_AC_BIT    3
#define ADXL375_AIC_INACT_X_BIT     2
#define ADXL375_AIC_INACT_Y_BIT     1
#define ADXL375_AIC_INACT_Z_BIT     0

#define ADXL375_TAPAXIS_SUP_BIT     3
#define ADXL375_TAPAXIS_X_BIT       2
#define ADXL375_TAPAXIS_Y_BIT       1
#define ADXL375_TAPAXIS_Z_BIT       0

#define ADXL375_TAPSTAT_ACTX_BIT    6
#define ADXL375_TAPSTAT_ACTY_BIT    5
#define ADXL375_TAPSTAT_ACTZ_BIT    4
#define ADXL375_TAPSTAT_ASLEEP_BIT  3
#define ADXL375_TAPSTAT_TAPX_BIT    2
#define ADXL375_TAPSTAT_TAPY_BIT    1
#define ADXL375_TAPSTAT_TAPZ_BIT    0

#define ADXL375_BW_LOWPOWER_BIT     4
#define ADXL375_BW_RATE_BIT         3
#define ADXL375_BW_RATE_LENGTH      4

#define ADXL375_PCTL_LINK_BIT       5
#define ADXL375_PCTL_AUTOSLEEP_BIT  4
#define ADXL375_PCTL_MEASURE_BIT    3
#define ADXL375_PCTL_SLEEP_BIT      2
#define ADXL375_PCTL_WAKEUP_BIT     1
#define ADXL375_PCTL_WAKEUP_LENGTH  2

#define ADXL375_INT_DATA_READY_BIT  7
#define ADXL375_INT_SINGLE_TAP_BIT  6
#define ADXL375_INT_DOUBLE_TAP_BIT  5
#define ADXL375_INT_ACTIVITY_BIT    4
#define ADXL375_INT_INACTIVITY_BIT  3
#define ADXL375_INT_FREE_FALL_BIT   2
#define ADXL375_INT_WATERMARK_BIT   1
#define ADXL375_INT_OVERRUN_BIT     0

#define ADXL375_FORMAT_SELFTEST_BIT 7
#define ADXL375_FORMAT_SPIMODE_BIT  6
#define ADXL375_FORMAT_INTMODE_BIT  5
#define ADXL375_FORMAT_FULL_RES_BIT 3
#define ADXL375_FORMAT_JUSTIFY_BIT  2
#define ADXL375_FORMAT_RANGE_BIT    1
#define ADXL375_FORMAT_RANGE_LENGTH 2

#define ADXL375_INT_MAP_REG          0x2F
#define ADXL375_INT_SOURCE_REG       0x30
#define ADXL345_DATA_FORMAT_REG     0x31

#define ADXL375_DATAX0           0x32
#define ADXL745_DATAX1           0x33
#define ADXL375_DATAY0           0x34
#define ADXL375_DATAY1           0x35
#define ADXL375_DATAZ0           0x36
#define ADXL375_DATAZ1           0x37
#define ADXL375_FIFO_CTL         0x38
#define ADXL375_FIFO_STATUS      0x39
#define ADXL375_FIFO_MODE_BIT       7
#define ADXL375_FIFO_MODE_LENGTH    2
#define ADXL375_FIFO_TRIGGER_BIT    5
#define ADXL375_FIFO_SAMPLES_BIT    4
#define ADXL375_FIFO_SAMPLES_LENGTH 5

#define ADXL375_FIFO_MODE_BYPASS    0b00
#define ADXL375_FIFO_MODE_FIFO      0b01
#define ADXL375_FIFO_MODE_STREAM    0b10
#define ADXL375_FIFO_MODE_TRIGGER   0b11
#define ADXL375_FIFOSTAT_TRIGGER_BIT        7
#define ADXL375_FIFOSTAT_LENGTH_BIT         5
#define ADXL375_FIFOSTAT_LENGTH_LENGTH      6
#define TO_READ (6)        //num of bytes we are going to read each time (two bytes for each axis)

class ADXL375 {
  public:
    ADXL375();
    ADXL375(uint8_t address);

    void initialize();
    bool testConnection();

    // DEVID register
    uint8_t getDeviceID();

    // THRESH_TAP register
    uint8_t getTapThreshold();
    void setTapThreshold(uint8_t threshold);

    // OFS* registers
    void getOffset(int8_t* x, int8_t* y, int8_t* z);
    void setOffset(int8_t x, int8_t y, int8_t z);
    int8_t getOffsetX();
    void setOffsetX(int8_t x);
    int8_t getOffsetY();
    void setOffsetY(int8_t y);
    int8_t getOffsetZ();
    void setOffsetZ(int8_t z);

    // DUR register
    uint8_t getTapDuration();
    void setTapDuration(uint8_t duration);

    // LATENT register
    uint8_t getDoubleTapLatency();
    void setDoubleTapLatency(uint8_t latency);

    // WINDOW register
    uint8_t getDoubleTapWindow();
    void setDoubleTapWindow(uint8_t window);

    // THRESH_ACT register
    uint8_t getActivityThreshold();
    void setActivityThreshold(uint8_t threshold);

    // THRESH_INACT register
    uint8_t getInactivityThreshold();
    void setInactivityThreshold(uint8_t threshold);

    // TIME_INACT register
    uint8_t getInactivityTime();
    void setInactivityTime(uint8_t time);

    // ACT_INACT_CTL register
    bool getActivityAC();
    void setActivityAC(bool enabled);
    bool getActivityXEnabled();
    void setActivityXEnabled(bool enabled);
    bool getActivityYEnabled();
    void setActivityYEnabled(bool enabled);
    bool getActivityZEnabled();
    void setActivityZEnabled(bool enabled);
    bool getInactivityAC();
    void setInactivityAC(bool enabled);
    bool getInactivityXEnabled();
    void setInactivityXEnabled(bool enabled);
    bool getInactivityYEnabled();
    void setInactivityYEnabled(bool enabled);
    bool getInactivityZEnabled();
    void setInactivityZEnabled(bool enabled);

    // THRESH_FF register
    uint8_t getFreefallThreshold();
    void setFreefallThreshold(uint8_t threshold);

    // TIME_FF register
    uint8_t getFreefallTime();
    void setFreefallTime(uint8_t time);

    // TAP_AXES register
    bool getTapAxisSuppress();
    void setTapAxisSuppress(bool enabled);
    bool getTapAxisXEnabled();
    void setTapAxisXEnabled(bool enabled);
    bool getTapAxisYEnabled();
    void setTapAxisYEnabled(bool enabled);
    bool getTapAxisZEnabled();
    void setTapAxisZEnabled(bool enabled);

    // ACT_TAP_STATUS register
    bool getActivitySourceX();
    bool getActivitySourceY();
    bool getActivitySourceZ();
    bool getAsleep();
    bool getTapSourceX();
    bool getTapSourceY();
    bool getTapSourceZ();

    // BW_RATE register
    bool getLowPowerEnabled();
    void setLowPowerEnabled(bool enabled);
    uint8_t getRate();
    void setRate(uint8_t rate);

    // POWER_CTL register
    bool getLinkEnabled();
    void setLinkEnabled(bool enabled);
    bool getAutoSleepEnabled();
    void setAutoSleepEnabled(bool enabled);
    bool getMeasureEnabled();
    void setMeasureEnabled(bool enabled);
    bool getSleepEnabled();
    void setSleepEnabled(bool enabled);
    uint8_t getWakeupFrequency();
    void setWakeupFrequency(uint8_t frequency);

    // INT_ENABLE register
    bool getIntDataReadyEnabled();
    void setIntDataReadyEnabled(bool enabled);
    bool getIntSingleTapEnabled();
    void setIntSingleTapEnabled(bool enabled);
    bool getIntDoubleTapEnabled();
    void setIntDoubleTapEnabled(bool enabled);
    bool getIntActivityEnabled();
    void setIntActivityEnabled(bool enabled);
    bool getIntInactivityEnabled();
    void setIntInactivityEnabled(bool enabled);
    bool getIntFreefallEnabled();
    void setIntFreefallEnabled(bool enabled);
    bool getIntWatermarkEnabled();
    void setIntWatermarkEnabled(bool enabled);
    bool getIntOverrunEnabled();
    void setIntOverrunEnabled(bool enabled);

    // INT_MAP register
    uint8_t getIntDataReadyPin();
    void setIntDataReadyPin(uint8_t pin);
    uint8_t getIntSingleTapPin();
    void setIntSingleTapPin(uint8_t pin);
    uint8_t getIntDoubleTapPin();
    void setIntDoubleTapPin(uint8_t pin);
    uint8_t getIntActivityPin();
    void setIntActivityPin(uint8_t pin);
    uint8_t getIntInactivityPin();
    void setIntInactivityPin(uint8_t pin);
    uint8_t getIntFreefallPin();
    void setIntFreefallPin(uint8_t pin);
    uint8_t getIntWatermarkPin();
    void setIntWatermarkPin(uint8_t pin);
    uint8_t getIntOverrunPin();
    void setIntOverrunPin(uint8_t pin);

    // INT_SOURCE register
    uint8_t getIntDataReadySource();
    uint8_t getIntSingleTapSource();
    uint8_t getIntDoubleTapSource();
    uint8_t getIntActivitySource();
    uint8_t getIntInactivitySource();
    uint8_t getIntFreefallSource();
    uint8_t getIntWatermarkSource();
    uint8_t getIntOverrunSource();

    // DATA_FORMAT register
    uint8_t getSelfTestEnabled();
    void setSelfTestEnabled(uint8_t enabled);
    uint8_t getSPIMode();
    void setSPIMode(uint8_t mode);
    uint8_t getInterruptMode();
    void setInterruptMode(uint8_t mode);
    uint8_t getFullResolution();
    void setFullResolution(uint8_t resolution);
    uint8_t getDataJustification();
    void setDataJustification(uint8_t justification);

    // DATA* registers
    void getAcceleration(int16_t* x, int16_t* y, int16_t* z);
    int16_t getAccelerationX();
    int16_t getAccelerationY();
    int16_t getAccelerationZ();

    // FIFO_CTL register
    uint8_t getFIFOMode();
    void setFIFOMode(uint8_t mode);
    uint8_t getFIFOTriggerInterruptPin();
    void setFIFOTriggerInterruptPin(uint8_t interrupt);
    uint8_t getFIFOSamples();
    void setFIFOSamples(uint8_t size);

    // FIFO_STATUS register
    bool getFIFOTriggerOccurred();
    uint8_t getFIFOLength();
    //Application methods
    void setupFIFO();
    void dumpSetup();
    void saveFIFO();
    void printFIFO();
    uint16_t maxFIFO(uint16_t *fifo);
  private:
    uint8_t devAddr;
    uint8_t buffer[6];
    uint16_t lastFiFo[96];
    uint16_t newistFiFo[96];

};
#endif


