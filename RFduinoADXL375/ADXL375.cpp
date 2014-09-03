// based on Jeff Rowbergs i2clib code for the ADXL345

#include "ADXL375.h"
#include "I2Cdev.h"
#include "BitTwiddle.h"
#include "FiFo.h"

ADXL375::ADXL375() {
    devAddr =  ADXL375_DEFAULT_ADDRESS;
    for(int i=0;i<sizeof(lastFiFo);i++){
      lastFiFo[i] =  newistFiFo[i] = 0;
    }
}

/** Specific address constructor.
 * @param address I2C address
 * @see ADXL375_DEFAULT_ADDRESS
 * @see ADXL375_ADDRESS_ALT_LOW
 * @see ADXL375_ADDRESS_ALT_HIGH
 */
ADXL375::ADXL375(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This will activate the accelerometer, so be sure to adjust the power settings
 * after you call this method if you want it to enter standby mode, or another
 * less demanding mode of operation.
 */
void ADXL375::initialize() {
    I2Cdev::writeByte(devAddr, ADXL375_POWER_CTL_REG, 0); // reset all power settings
    setAutoSleepEnabled(true);
    setMeasureEnabled(true);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool ADXL375::testConnection() {
    return getDeviceID() == 0xE5;
}

// DEVID register

/** Get Device ID.
 * The DEVID register holds a fixed device ID code of 0xE5 (375 octal).
 * @return Device ID (should be 0xE5, 229 dec, 375 oct)
 * @see ADXL375_DEVID
 */
uint8_t ADXL375::getDeviceID() {
    I2Cdev::readByte(devAddr, ADXL375_DEVID_REG, buffer);
    return buffer[0];
}

// THRESH_TAP register

/** Get tap threshold.
 * The THRESH_TAP register is eight bits and holds the threshold value for tap
 * interrupts. The data format is unsigned, therefore, the magnitude of the tap
 * event is compared with the value in THRESH_TAP for normal tap detection. The
 * scale factor is 62.5 mg/LSB (that is, 0xFF = 16 g). A value of 0 may result
 * in undesirable behavior if single tap/double tap interrupts are enabled.
 * @return Tap threshold (scaled at 62.5 mg/LSB)
 * @see ADXL375_THRESH_TAP
 */
uint8_t ADXL375::getTapThreshold() {
    I2Cdev::readByte(devAddr, ADXL375_THRESH_TAP_REG, buffer);
    return buffer[0];
}
/** Set tap threshold.
  * @param threshold Tap magnitude threshold (scaled at 62.5 mg/LSB)
  * @see ADXL375_THRESH_TAP
  * @see getTapThreshold()
  */
void ADXL375::setTapThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, ADXL375_THRESH_TAP_REG, threshold);
}

// OFS* registers

/** Get axis offsets.
 * The OFSX, OFSY, and OFSZ registers are each eight bits and offer user-set
 * offset adjustments in twos complement format with a scale factor of 15.6
 * mg/LSB (that is, 0x7F = 2 g). The value stored in the offset registers is
 * automatically added to the acceleration data, and the resulting value is
 * stored in the output data registers. For additional information regarding
 * offset calibration and the use of the offset registers, refer to the Offset
 * Calibration section of the datasheet.
 * @param x X axis offset container
 * @param y Y axis offset container
 * @param z Z axis offset container
 * @see ADXL375_OFSX
 * @see ADXL375_OFSY
 * @see ADXL375_OFSZ
 */
void ADXL375::getOffset(int8_t* x, int8_t* y, int8_t* z) {
    I2Cdev::readBytes(devAddr, ADXL375_OFSX_REG, 3, buffer);
    *x = buffer[0];
    *y = buffer[1];
    *z = buffer[2];
}
/** Set axis offsets.
 * @param x X axis offset value
 * @param y Y axis offset value
 * @param z Z axis offset value
 * @see getOffset()
 * @see ADXL375_OFSX
 * @see ADXL375_OFSY
 * @see ADXL375_OFSZ
 */
void ADXL375::setOffset(int8_t x, int8_t y, int8_t z) {
    I2Cdev::writeByte(devAddr, ADXL375_OFSX_REG, x);
    I2Cdev::writeByte(devAddr, ADXL375_OFSY_REG, y);
    I2Cdev::writeByte(devAddr, ADXL375_OFSZ_REG, z);
}
/** Get X axis offset.
 * @return X axis offset value
 * @see getOffset()
 * @see ADXL375_OFSX
 */
int8_t ADXL375::getOffsetX() {
    I2Cdev::readByte(devAddr, ADXL375_OFSX_REG, buffer);
    return buffer[0];
}
/** Set X axis offset.
 * @param x X axis offset value
 * @see getOffset()
 * @see ADXL375_OFSX
 */
void ADXL375::setOffsetX(int8_t x) {
    I2Cdev::writeByte(devAddr, ADXL375_OFSX_REG, x);
}
/** Get Y axis offset.
 * @return Y axis offset value
 * @see getOffset()
 * @see ADXL375_OFSY
 */
int8_t ADXL375::getOffsetY() {
    I2Cdev::readByte(devAddr, ADXL375_OFSY_REG, buffer);
    return buffer[0];
}
/** Set Y axis offset.
 * @param y Y axis offset value
 * @see getOffset()
 * @see ADXL375_OFSY
 */
void ADXL375::setOffsetY(int8_t y) {
    I2Cdev::writeByte(devAddr, ADXL375_OFSY_REG, y);
}
/** Get Z axis offset.
 * @return Z axis offset value
 * @see getOffset()
 * @see ADXL375_OFSZ
 */
int8_t ADXL375::getOffsetZ() {
    I2Cdev::readByte(devAddr, ADXL375_OFSZ_REG, buffer);
    return buffer[0];
}
/** Set Z axis offset.
 * @param z Z axis offset value
 * @see getOffset()
 * @see ADXL375_OFSZ
 */
void ADXL375::setOffsetZ(int8_t z) {
    I2Cdev::writeByte(devAddr, ADXL375_OFSZ_REG, z);
}

// DUR register

/** Get tap duration.
 * The DUR register is eight bits and contains an unsigned time value
 * representing the maximum time that an event must be above the THRESH_TAP
 * threshold to qualify as a tap event. The scale factor is 625 us/LSB. A value
 * of 0 disables the single tap/ double tap functions.
 * @return Tap duration (scaled at 625 us/LSB)
 * @see ADXL375_DUR
 */
uint8_t ADXL375::getTapDuration() {
    I2Cdev::readByte(devAddr, ADXL375_DUR_REG, buffer);
    return buffer[0];
}
/** Set tap duration.
 * @param duration Tap duration (scaled at 625 us/LSB)
 * @see getTapDuration()
 * @see ADXL375_DUR
 */
void ADXL375::setTapDuration(uint8_t duration) {
    I2Cdev::writeByte(devAddr, ADXL375_DUR_REG, duration);
}

// LATENT register

/** Get tap duration.
 * The latent register is eight bits and contains an unsigned time value
 * representing the wait time from the detection of a tap event to the start of
 * the time window (defined by the window register) during which a possible
 * second tap event can be detected. The scale factor is 1.25 ms/LSB. A value of
 * 0 disables the double tap function.
 * @return Tap latency (scaled at 1.25 ms/LSB)
 * @see ADXL375_LATENT
 */
uint8_t ADXL375::getDoubleTapLatency() {
    I2Cdev::readByte(devAddr, ADXL375_LATENT_REG, buffer);
    return buffer[0];
}
/** Set tap duration.
 * @param latency Tap latency (scaled at 1.25 ms/LSB)
 * @see getDoubleTapLatency()
 * @see ADXL375_LATENT
 */
void ADXL375::setDoubleTapLatency(uint8_t latency) {
    I2Cdev::writeByte(devAddr, ADXL375_LATENT_REG, latency);
}

// WINDOW register

/** Get double tap window.
 * The window register is eight bits and contains an unsigned time value
 * representing the amount of time after the expiration of the latency time
 * (determined by the latent register) during which a second valid tap can
 * begin. The scale factor is 1.25 ms/LSB. A value of 0 disables the double tap
 * function.
 * @return Double tap window (scaled at 1.25 ms/LSB)
 * @see ADXL375_WINDOW
 */
uint8_t ADXL375::getDoubleTapWindow() {
    I2Cdev::readByte(devAddr, ADXL375_WINDOW_REG, buffer);
    return buffer[0];
}
/** Set double tap window.
 * @param window Double tap window (scaled at 1.25 ms/LSB)
 * @see getDoubleTapWindow()
 * @see ADXL375_WINDOW
 */
void ADXL375::setDoubleTapWindow(uint8_t window) {
    I2Cdev::writeByte(devAddr, ADXL375_WINDOW_REG, window);
}

// THRESH_ACT register

/** Get activity threshold.
 * The THRESH_ACT register is eight bits and holds the threshold value for
 * detecting activity. The data format is unsigned, so the magnitude of the
 * activity event is compared with the value in the THRESH_ACT register. The
 * scale factor is 62.5 mg/LSB. A value of 0 may result in undesirable behavior
 * if the activity interrupt is enabled.
 * @return Activity threshold (scaled at 62.5 mg/LSB)
 * @see ADXL375_THRESH_ACT
 */
uint8_t ADXL375::getActivityThreshold() {
    I2Cdev::readByte(devAddr, ADXL375_THRESH_ACT_REG, buffer);
    return buffer[0];
}
/** Set activity threshold.
 * @param threshold Activity threshold (scaled at 62.5 mg/LSB)
 * @see getActivityThreshold()
 * @see ADXL375_THRESH_ACT
 */
void ADXL375::setActivityThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, ADXL375_THRESH_ACT_REG, threshold);
}

// THRESH_INACT register

/** Get inactivity threshold.
 * The THRESH_INACT register is eight bits and holds the threshold value for 
 * detecting inactivity. The data format is unsigned, so the magnitude of the
 * inactivity event is compared with the value in the THRESH_INACT register. The
 * scale factor is 62.5 mg/LSB. A value of 0 may result in undesirable behavior
 * if the inactivity interrupt is enabled.
 * @return Inactivity threshold (scaled at 62.5 mg/LSB)
 * @see ADXL375_THRESH_INACT
 */
uint8_t ADXL375::getInactivityThreshold() {
    I2Cdev::readByte(devAddr, ADXL375_THRESH_INACT_REG, buffer);
    return buffer[0];
}
/** Set inactivity threshold.
 * @param threshold Inctivity threshold (scaled at 62.5 mg/LSB)
 * @see getInctivityThreshold()
 * @see ADXL375_THRESH_INACT
 */
void ADXL375::setInactivityThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, ADXL375_THRESH_INACT_REG, threshold);
}

// TIME_INACT register

/** Set inactivity time.
 * The TIME_INACT register is eight bits and contains an unsigned time value
 * representing the amount of time that acceleration must be less than the value
 * in the THRESH_INACT register for inactivity to be declared. The scale factor
 * is 1 sec/LSB. Unlike the other interrupt functions, which use unfiltered data
 * (see the Threshold sectionof the datasheet), the inactivity function uses
 * filtered output data. At least one output sample must be generated for the
 * inactivity interrupt to be triggered. This results in the function appearing
 * unresponsive if the TIME_INACT register is set to a value less than the time
 * constant of the output data rate. A value of 0 results in an interrupt when
 * the output data is less than the value in the THRESH_INACT register.
 * @return Inactivity time (scaled at 1 sec/LSB)
 * @see ADXL375_TIME_INACT
 */
uint8_t ADXL375::getInactivityTime() {
    I2Cdev::readByte(devAddr, ADXL375_TIME_INACT_REG, buffer);
    return buffer[0];
}
/** Set inactivity time.
 * @param time Inactivity time (scaled at 1 sec/LSB)
 * @see getInctivityTime()
 * @see ADXL375_TIME_INACT
 */
void ADXL375::setInactivityTime(uint8_t time) {
    I2Cdev::writeByte(devAddr, ADXL375_TIME_INACT_REG, time);
}

// ACT_INACT_CTL register

/** Get activity AC/DC coupling.
 * A setting of 0 selects dc-coupled operation, and a setting of 1 enables
 * ac-coupled operation. In dc-coupled operation, the current acceleration
 * magnitude is compared directly with THRESH_ACT and THRESH_INACT to determine
 * whether activity or inactivity is detected.
 *
 * In ac-coupled operation for activity detection, the acceleration value at the
 * start of activity detection is taken as a reference value. New samples of
 * acceleration are then compared to this reference value, and if the magnitude
 * of the difference exceeds the THRESH_ACT value, the device triggers an
 * activity interrupt.
 *
 * Similarly, in ac-coupled operation for inactivity detection, a reference
 * value is used for comparison and is updated whenever the device exceeds the
 * inactivity threshold. After the reference value is selected, the device
 * compares the magnitude of the difference between the reference value and the
 * current acceleration with THRESH_INACT. If the difference is less than the
 * value in THRESH_INACT for the time in TIME_INACT, the device is considered
 * inactive and the inactivity interrupt is triggered.
 *
 * @return Activity coupling (0 = DC, 1 = AC)
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_AC_BIT
 */
bool ADXL375::getActivityAC() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_AC_BIT, buffer);
    return buffer[0];
}
/** Set activity AC/DC coupling.
 * @param enabled Activity AC/DC coupling (TRUE for AC, FALSE for DC)
 * @see getActivityAC()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_AC_BIT
 */
void ADXL375::setActivityAC(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_AC_BIT, enabled);
}
/** Get X axis activity monitoring inclusion.
 * For all "get[In]Activity*Enabled()" methods: a setting of 1 enables x-, y-,
 * or z-axis participation in detecting activity or inactivity. A setting of 0
 * excludes the selected axis from participation. If all axes are excluded, the
 * function is disabled. For activity detection, all participating axes are
 * logically OR�ed, causing the activity function to trigger when any of the
 * participating axes exceeds the threshold. For inactivity detection, all
 * participating axes are logically AND�ed, causing the inactivity function to
 * trigger only if all participating axes are below the threshold for the
 * specified time.
 * @return X axis activity monitoring enabled value
 * @see getActivityAC()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_X_BIT
 */
bool ADXL375::getActivityXEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_X_BIT, buffer);
    return buffer[0];
}
/** Set X axis activity monitoring inclusion.
 * @param enabled X axis activity monitoring inclusion value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_X_BIT
 */
void ADXL375::setActivityXEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_X_BIT, enabled);
}
/** Get Y axis activity monitoring.
 * @return Y axis activity monitoring enabled value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_Y_BIT
 */
bool ADXL375::getActivityYEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_Y_BIT, buffer);
    return buffer[0];
}
/** Set Y axis activity monitoring inclusion.
 * @param enabled Y axis activity monitoring inclusion value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_Y_BIT
 */
void ADXL375::setActivityYEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_Y_BIT, enabled);
}
/** Get Z axis activity monitoring.
 * @return Z axis activity monitoring enabled value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_Z_BIT
 */
bool ADXL375::getActivityZEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_Z_BIT, buffer);
    return buffer[0];
}
/** Set Z axis activity monitoring inclusion.
 * @param enabled Z axis activity monitoring inclusion value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_ACT_Z_BIT
 */
void ADXL375::setActivityZEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_ACT_Z_BIT, enabled);
}
/** Get inactivity AC/DC coupling.
 * @return Inctivity coupling (0 = DC, 1 = AC)
 * @see getActivityAC()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_AC_BIT
 */
bool ADXL375::getInactivityAC() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_AC_BIT, buffer);
    return buffer[0];
}
/** Set inctivity AC/DC coupling.
 * @param enabled Inactivity AC/DC coupling (TRUE for AC, FALSE for DC)
 * @see getActivityAC()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_AC_BIT
 */
void ADXL375::setInactivityAC(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_AC_BIT, enabled);
}
/** Get X axis inactivity monitoring.
 * @return Y axis inactivity monitoring enabled value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_X_BIT
 */
bool ADXL375::getInactivityXEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_X_BIT, buffer);
    return buffer[0];
}
/** Set X axis activity monitoring inclusion.
 * @param enabled X axis inactivity monitoring inclusion value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_X_BIT
 */
void ADXL375::setInactivityXEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_X_BIT, enabled);
}
/** Get Y axis inactivity monitoring.
 * @return Y axis inactivity monitoring enabled value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_Y_BIT
 */
bool ADXL375::getInactivityYEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_Y_BIT, buffer);
    return buffer[0];
}
/** Set Y axis inactivity monitoring inclusion.
 * @param enabled Y axis inactivity monitoring inclusion value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_Y_BIT
 */
void ADXL375::setInactivityYEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_Y_BIT, enabled);
}
/** Get Z axis inactivity monitoring.
 * @return Z axis inactivity monitoring enabled value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_Z_BIT
 */
bool ADXL375::getInactivityZEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_Z_BIT, buffer);
    return buffer[0];
}
/** Set Z axis inactivity monitoring inclusion.
 * @param enabled Z axis activity monitoring inclusion value
 * @see getActivityAC()
 * @see getActivityXEnabled()
 * @see ADXL375_ACT_INACT_CTL
 * @see ADXL375_AIC_INACT_Z_BIT
 */
void ADXL375::setInactivityZEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_ACT_INACT_CTL_REG, ADXL375_AIC_INACT_Z_BIT, enabled);
}

// THRESH_FF register

/** Get freefall threshold value.
 * The THRESH_FF register is eight bits and holds the threshold value, in
 * unsigned format, for free-fall detection. The acceleration on all axes is
 * compared with the value in THRESH_FF to determine if a free-fall event
 * occurred. The scale factor is 62.5 mg/LSB. Note that a value of 0 mg may
 * result in undesirable behavior if the free-fall interrupt is enabled. Values
 * between 300 mg and 600 mg (0x05 to 0x09) are recommended.
 * @return Freefall threshold value (scaled at 62.5 mg/LSB)
 * @see ADXL375_THRESH_FF
 */
uint8_t ADXL375::getFreefallThreshold() {
    I2Cdev::readByte(devAddr, ADXL375_THRESH_FF_REG, buffer);
    return buffer[0];
}
/** Set freefall threshold value.
 * @param threshold Freefall threshold value (scaled at 62.5 mg/LSB)
 * @see getFreefallThreshold()
 * @see ADXL375_THRESH_FF
 */
void ADXL375::setFreefallThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, ADXL375_THRESH_FF_REG, threshold);
}

// TIME_FF register

/** Get freefall time value.
 * The TIME_FF register is eight bits and stores an unsigned time value
 * representing the minimum time that the value of all axes must be less than
 * THRESH_FF to generate a free-fall interrupt. The scale factor is 5 ms/LSB. A
 * value of 0 may result in undesirable behavior if the free-fall interrupt is
 * enabled. Values between 100 ms and 350 ms (0x14 to 0x46) are recommended.
 * @return Freefall time value (scaled at 5 ms/LSB)
 * @see getFreefallThreshold()
 * @see ADXL375_TIME_FF
 */
uint8_t ADXL375::getFreefallTime() {
    I2Cdev::readByte(devAddr, ADXL375_TIME_FF_REG, buffer);
    return buffer[0];
}
/** Set freefall time value.
 * @param threshold Freefall time value (scaled at 5 ms/LSB)
 * @see getFreefallTime()
 * @see ADXL375_TIME_FF
 */
void ADXL375::setFreefallTime(uint8_t time) {
    I2Cdev::writeByte(devAddr, ADXL375_TIME_FF_REG, time);
}

// TAP_AXES register

/** Get double-tap fast-movement suppression.
 * Setting the suppress bit suppresses double tap detection if acceleration
 * greater than the value in THRESH_TAP is present between taps. See the Tap
 * Detection section in the datasheet for more details.
 * @return Double-tap fast-movement suppression value
 * @see getTapThreshold()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_SUP_BIT
 */
bool ADXL375::getTapAxisSuppress() {
    I2Cdev::readBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_SUP_BIT, buffer);
    return buffer[0];
}
/** Set double-tap fast-movement suppression.
 * @param enabled Double-tap fast-movement suppression value
 * @see getTapAxisSuppress()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_SUP_BIT
 */
void ADXL375::setTapAxisSuppress(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_SUP_BIT, enabled);
}
/** Get double-tap fast-movement suppression.
 * A setting of 1 in the TAP_X enable bit enables x-axis participation in tap
 * detection. A setting of 0 excludes the selected axis from participation in
 * tap detection.
 * @return Double-tap fast-movement suppression value
 * @see getTapThreshold()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_X_BIT
 */
bool ADXL375::getTapAxisXEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_X_BIT, buffer);
    return buffer[0];
}
/** Set tap detection X axis inclusion.
 * @param enabled X axis tap detection enabled value
 * @see getTapAxisXEnabled()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_X_BIT
 */
void ADXL375::setTapAxisXEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_X_BIT, enabled);
}
/** Get tap detection Y axis inclusion.
 * A setting of 1 in the TAP_Y enable bit enables y-axis participation in tap
 * detection. A setting of 0 excludes the selected axis from participation in
 * tap detection.
 * @return Double-tap fast-movement suppression value
 * @see getTapThreshold()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_Y_BIT
 */
bool ADXL375::getTapAxisYEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_Y_BIT, buffer);
    return buffer[0];
}
/** Set tap detection Y axis inclusion.
 * @param enabled Y axis tap detection enabled value
 * @see getTapAxisYEnabled()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_Y_BIT
 */
void ADXL375::setTapAxisYEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_Y_BIT, enabled);
}
/** Get tap detection Z axis inclusion.
 * A setting of 1 in the TAP_Z enable bit enables z-axis participation in tap
 * detection. A setting of 0 excludes the selected axis from participation in
 * tap detection.
 * @return Double-tap fast-movement suppression value
 * @see getTapThreshold()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_Z_BIT
 */
bool ADXL375::getTapAxisZEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_Z_BIT, buffer);
    return buffer[0];
}
/** Set tap detection Z axis inclusion.
 * @param enabled Z axis tap detection enabled value
 * @see getTapAxisZEnabled()
 * @see ADXL375_TAP_AXES
 * @see ADXL375_TAPAXIS_Z_BIT
 */
void ADXL375::setTapAxisZEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_TAP_AXES_REG, ADXL375_TAPAXIS_Z_BIT, enabled);
}

// ACT_TAP_STATUS register

/** Get X axis activity source flag.
 * These bits indicate the first axis involved in a tap or activity event. A
 * setting of 1 corresponds to involvement in the event, and a setting of 0
 * corresponds to no involvement. When new data is available, these bits are not
 * cleared but are overwritten by the new data. The ACT_TAP_STATUS register
 * should be read before clearing the interrupt. Disabling an axis from
 * participation clears the corresponding source bit when the next activity or
 * single tap/double tap event occurs.
 * @return X axis activity source flag
 * @see ADXL375_ACT_TAP_STATUS
 * @see ADXL375_TAPSTAT_ACTX_BIT
 */
bool ADXL375::getActivitySourceX() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_TAP_STATUS_REG, ADXL375_TAPSTAT_ACTX_BIT, buffer);
    return buffer[0];
}
/** Get Y axis activity source flag.
 * @return Y axis activity source flag
 * @see getActivitySourceX()
 * @see ADXL375_ACT_TAP_STATUS
 * @see ADXL375_TAPSTAT_ACTY_BIT
 */
bool ADXL375::getActivitySourceY() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_TAP_STATUS_REG, ADXL375_TAPSTAT_ACTY_BIT, buffer);
    return buffer[0];
}
/** Get Z axis activity source flag.
 * @return Z axis activity source flag
 * @see getActivitySourceX()
 * @see ADXL375_ACT_TAP_STATUS
 * @see ADXL375_TAPSTAT_ACTZ_BIT
 */
bool ADXL375::getActivitySourceZ() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_TAP_STATUS_REG, ADXL375_TAPSTAT_ACTZ_BIT, buffer);
    return buffer[0];
}
/** Get sleep mode flag.
 * A setting of 1 in the asleep bit indicates that the part is asleep, and a
 * setting of 0 indicates that the part is not asleep. This bit toggles only if
 * the device is configured for auto sleep. See the AUTO_SLEEP Bit section of
 * the datasheet for more information on autosleep mode.
 * @return Sleep mode enabled flag
 * @see ADXL375_ACT_TAP_STATUS
 * @see ADXL375_TAPSTAT_ASLEEP_BIT
 */
bool ADXL375::getAsleep() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_TAP_STATUS_REG, ADXL375_TAPSTAT_ASLEEP_BIT, buffer);
    return buffer[0];
}
/** Get X axis tap source flag.
 * @return X axis tap source flag
 * @see getActivitySourceX()
 * @see ADXL375_ACT_TAP_STATUS
 * @see ADXL375_TAPSTAT_TAPX_BIT
 */
bool ADXL375::getTapSourceX() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_TAP_STATUS_REG, ADXL375_TAPSTAT_TAPX_BIT, buffer);
    return buffer[0];
}
/** Get Y axis tap source flag.
 * @return Y axis tap source flag
 * @see getActivitySourceX()
 * @see ADXL375_ACT_TAP_STATUS
 * @see ADXL375_TAPSTAT_TAPY_BIT
 */
bool ADXL375::getTapSourceY() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_TAP_STATUS_REG, ADXL375_TAPSTAT_TAPY_BIT, buffer);
    return buffer[0];
}
/** Get Z axis tap source flag.
 * @return Z axis tap source flag
 * @see getActivitySourceX()
 * @see ADXL375_ACT_TAP_STATUS
 * @see ADXL375_TAPSTAT_TAPZ_BIT
 */
bool ADXL375::getTapSourceZ() {
    I2Cdev::readBit(devAddr, ADXL375_ACT_TAP_STATUS_REG, ADXL375_TAPSTAT_TAPZ_BIT, buffer);
    return buffer[0];
}

// BW_RATE register

/** Get low power enabled status.
 * A setting of 0 in the LOW_POWER bit selects normal operation, and a setting
 * of 1 selects reduced power operation, which has somewhat higher noise (see
 * the Power Modes section of the datasheet for details).
 * @return Low power enabled status
 * @see ADXL375_BW_RATE
 * @see ADXL375_BW_LOWPOWER_BIT
 */
bool ADXL375::getLowPowerEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_BW_RATE_REG, ADXL375_BW_LOWPOWER_BIT, buffer);
    return buffer[0];
}
/** Set low power enabled status.
 * @see getLowPowerEnabled()
 * @param enabled Low power enable setting
 * @see ADXL375_BW_RATE
 * @see ADXL375_BW_LOWPOWER_BIT
 */
void ADXL375::setLowPowerEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_BW_RATE_REG, ADXL375_BW_LOWPOWER_BIT, enabled);
}
/** Get measurement data rate.
 * These bits select the device bandwidth and output data rate (see Table 7 and
 * Table 8 in the datasheet for details). The default value is 0x0A, which
 * translates to a 100 Hz output data rate. An output data rate should be
 * selected that is appropriate for the communication protocol and frequency
 * selected. Selecting too high of an output data rate with a low communication
 * speed results in samples being discarded.
 * @return Data rate (0x0 - 0xF)
 * @see ADXL375_BW_RATE
 * @see ADXL375_BW_RATE_BIT
 * @see ADXL375_BW_RATE_LENGTH
 */
uint8_t ADXL375::getRate() {
    I2Cdev::readBits(devAddr, ADXL375_BW_RATE_REG, ADXL375_BW_RATE_BIT, ADXL375_BW_RATE_LENGTH, buffer);
    return buffer[0];
}
/** Set measurement data rate.
 * 0x0 = 0.1 Hz
 * 0x01 = 0.2 Hz
 * 0x02 = 0.39 Hz
 * 0x03 = 0.78 Hz
 * 0x04 = 1.56 Hz
 * 0x05 = 3.13 Hz
 * 0x06 = 6.25 Hz
 * 0x7 =  12.5Hz
 * 0x8 =  25Hz, increasing or decreasing by factors of 2, so:
 * 0x9 =  50Hz
 * 0xA = 100Hz
 * 0xB = 200 Hz
 * 0xC = 400 Hz
 * 0xD = 800 Hz
 * 0XE = 1600 Hz
 * 0XF = 3200 Hz
 * @param rate New data rate (0x0 - 0xF)
 * @see ADXL375_RATE_100
 * @see ADXL375_BW_RATE
 * @see ADXL375_BW_RATE_BIT
 * @see ADXL375_BW_RATE_LENGTH
 */
void ADXL375::setRate(uint8_t rate) {
    I2Cdev::writeBits(devAddr, ADXL375_BW_RATE_REG, ADXL375_BW_RATE_BIT, ADXL375_BW_RATE_LENGTH, rate);
}

// POWER_CTL register

/** Get activity/inactivity serial linkage status.
 * A setting of 1 in the link bit with both the activity and inactivity
 * functions enabled delays the start of the activity function until
 * inactivity is detected. After activity is detected, inactivity detection
 * begins, preventing the detection of activity. This bit serially links the
 * activity and inactivity functions. When this bit is set to 0, the inactivity
 * and activity functions are concurrent. Additional information can be found
 * in the Link Mode section of the datasheet.
 *
 * When clearing the link bit, it is recommended that the part be placed into
 * standby mode and then set back to measurement mode with a subsequent write.
 * This is done to ensure that the device is properly biased if sleep mode is
 * manually disabled; otherwise, the first few samples of data after the link
 * bit is cleared may have additional noise, especially if the device was asleep
 * when the bit was cleared.
 *
 * @return Link status
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_LINK_BIT
 */
bool ADXL375::getLinkEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_LINK_BIT, buffer);
    return buffer[0];
}
/** Set activity/inactivity serial linkage status.
 * @param enabled New link status
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_LINK_BIT
 */
void ADXL375::setLinkEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_LINK_BIT, enabled);
}
/** Get auto-sleep enabled status.
 * If the link bit is set, a setting of 1 in the AUTO_SLEEP bit enables the
 * auto-sleep functionality. In this mode, the ADXL375 auto-matically switches
 * to sleep mode if the inactivity function is enabled and inactivity is
 * detected (that is, when acceleration is below the THRESH_INACT value for at
 * least the time indicated by TIME_INACT). If activity is also enabled, the
 * ADXL375 automatically wakes up from sleep after detecting activity and
 * returns to operation at the output data rate set in the BW_RATE register. A
 * setting of 0 in the AUTO_SLEEP bit disables automatic switching to sleep
 * mode. See the description of the Sleep Bit in this section of the datasheet
 * for more information on sleep mode.
 *
 * If the link bit is not set, the AUTO_SLEEP feature is disabled and setting
 * the AUTO_SLEEP bit does not have an impact on device operation. Refer to the
 * Link Bit section or the Link Mode section for more information on utilization
 * of the link feature.
 *
 * When clearing the AUTO_SLEEP bit, it is recommended that the part be placed
 * into standby mode and then set back to measure-ment mode with a subsequent
 * write. This is done to ensure that the device is properly biased if sleep
 * mode is manually disabled; otherwise, the first few samples of data after the
 * AUTO_SLEEP bit is cleared may have additional noise, especially if the device
 * was asleep when the bit was cleared.
 *
 * @return Auto-sleep enabled status
 * @see getActivityThreshold()
 * @see getInactivityThreshold()
 * @see getInactivityTime()
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_AUTOSLEEP_BIT
 */
bool ADXL375::getAutoSleepEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_AUTOSLEEP_BIT, buffer);
    return buffer[0];
}
/** Set auto-sleep enabled status.
 * @param enabled New auto-sleep status
 * @see getAutoSleepEnabled()
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_AUTOSLEEP_BIT
 */
void ADXL375::setAutoSleepEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_AUTOSLEEP_BIT, enabled);
}
/** Get measurement enabled status.
 * A setting of 0 in the measure bit places the part into standby mode, and a
 * setting of 1 places the part into measurement mode. The ADXL375 powers up in
 * standby mode with minimum power consumption.
 * @return Measurement enabled status
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_MEASURE_BIT
 */
bool ADXL375::getMeasureEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_MEASURE_BIT, buffer);
    return buffer[0];
}
/** Set measurement enabled status.
 * @param enabled Measurement enabled status
 * @see getMeasureEnabled()
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_MEASURE_BIT
 */
void ADXL375::setMeasureEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_MEASURE_BIT, enabled);
}
/** Get sleep mode enabled status.
 * A setting of 0 in the sleep bit puts the part into the normal mode of
 * operation, and a setting of 1 places the part into sleep mode. Sleep mode
 * suppresses DATA_READY, stops transmission of data to FIFO, and switches the
 * sampling rate to one specified by the wakeup bits. In sleep mode, only the
 * activity function can be used. When the DATA_READY interrupt is suppressed,
 * the output data registers (Register 0x32 to Register 0x37) are still updated
 * at the sampling rate set by the wakeup bits (D1:D0).
 *
 * When clearing the sleep bit, it is recommended that the part be placed into
 * standby mode and then set back to measurement mode with a subsequent write.
 * This is done to ensure that the device is properly biased if sleep mode is
 * manually disabled; otherwise, the first few samples of data after the sleep
 * bit is cleared may have additional noise, especially if the device was asleep
 * when the bit was cleared.
 *
 * @return Sleep enabled status
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_SLEEP_BIT
 */
bool ADXL375::getSleepEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_SLEEP_BIT, buffer);
    return buffer[0];
}
/** Set sleep mode enabled status.
 * @param Sleep mode enabled status
 * @see getSleepEnabled()
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_SLEEP_BIT
 */
void ADXL375::setSleepEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_SLEEP_BIT, enabled);
}
/** Get wakeup frequency.
 * These bits control the frequency of readings in sleep mode as described in
 * Table 20 in the datasheet. (That is, 0 = 8Hz, 1 = 4Hz, 2 = 2Hz, 3 = 1Hz)
 * @return Wakeup frequency (0x0 - 0x3, indicating 8/4/2/1Hz respectively)
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_SLEEP_BIT
 */
uint8_t ADXL375::getWakeupFrequency() {
    I2Cdev::readBits(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_WAKEUP_BIT, ADXL375_PCTL_WAKEUP_LENGTH, buffer);
    return buffer[0];
}
/** Set wakeup frequency.
 * @param frequency Wakeup frequency (0x0 - 0x3, indicating 8/4/2/1Hz respectively)
 * @see getWakeupFrequency()
 * @see ADXL375_POWER_CTL
 * @see ADXL375_PCTL_SLEEP_BIT
 */
void ADXL375::setWakeupFrequency(uint8_t frequency) {
    I2Cdev::writeBits(devAddr, ADXL375_POWER_CTL_REG, ADXL375_PCTL_WAKEUP_BIT, ADXL375_PCTL_WAKEUP_LENGTH, frequency);
}

// INT_ENABLE register

/** Get DATA_READY interrupt enabled status.
 * Setting bits in this register to a value of 1 enables their respective
 * functions to generate interrupts, whereas a value of 0 prevents the functions
 * from generating interrupts. The DATA_READY, watermark, and overrun bits
 * enable only the interrupt output; the functions are always enabled. It is
 * recommended that interrupts be configured before enabling their outputs.
 * @return DATA_READY interrupt enabled status.
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_DATA_READY_BIT
 */
bool ADXL375::getIntDataReadyEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_DATA_READY_BIT, buffer);
    return buffer[0];
}
/** Set DATA_READY interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_DATA_READY_BIT
 */
void ADXL375::setIntDataReadyEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_DATA_READY_BIT, enabled);
}
/** Set SINGLE_TAP interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_SINGLE_TAP_BIT
 */
bool ADXL375::getIntSingleTapEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_SINGLE_TAP_BIT, buffer);
    return buffer[0];
}
/** Set SINGLE_TAP interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_SINGLE_TAP_BIT
 */
void ADXL375::setIntSingleTapEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_SINGLE_TAP_BIT, enabled);
}
/** Get DOUBLE_TAP interrupt enabled status.
 * @return Interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_DOUBLE_TAP_BIT
 */
bool ADXL375::getIntDoubleTapEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_DOUBLE_TAP_BIT, buffer);
    return buffer[0];
}
/** Set DOUBLE_TAP interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_DOUBLE_TAP_BIT
 */
void ADXL375::setIntDoubleTapEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_DOUBLE_TAP_BIT, enabled);
}
/** Set ACTIVITY interrupt enabled status.
 * @return Interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_ACTIVITY_BIT
 */
bool ADXL375::getIntActivityEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_ACTIVITY_BIT, buffer);
    return buffer[0];
}
/** Set ACTIVITY interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_ACTIVITY_BIT
 */
void ADXL375::setIntActivityEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_ACTIVITY_BIT, enabled);
}
/** Get INACTIVITY interrupt enabled status.
 * @return Interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_INACTIVITY_BIT
 */
bool ADXL375::getIntInactivityEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_INACTIVITY_BIT, buffer);
    return buffer[0];
}
/** Set INACTIVITY interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_INACTIVITY_BIT
 */
void ADXL375::setIntInactivityEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_INACTIVITY_BIT, enabled);
}
/** Get FREE_FALL interrupt enabled status.
 * @return Interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_FREE_FALL_BIT
 */
bool ADXL375::getIntFreefallEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_FREE_FALL_BIT, buffer);
    return buffer[0];
}
/** Set FREE_FALL interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_FREE_FALL_BIT
 */
void ADXL375::setIntFreefallEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_FREE_FALL_BIT, enabled);
}
/** Get WATERMARK interrupt enabled status.
 * @return Interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_WATERMARK_BIT
 */
bool ADXL375::getIntWatermarkEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_WATERMARK_BIT, buffer);
    return buffer[0];
}
/** Set WATERMARK interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_WATERMARK_BIT
 */
void ADXL375::setIntWatermarkEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_WATERMARK_BIT, enabled);
}
/** Get OVERRUN interrupt enabled status.
 * @return Interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_OVERRUN_BIT
 */
bool ADXL375::getIntOverrunEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_OVERRUN_BIT, buffer);
    return buffer[0];
}
/** Set OVERRUN interrupt enabled status.
 * @param enabled New interrupt enabled status
 * @see getIntDataReadyEnabled()
 * @see ADXL375_INT_ENABLE
 * @see ADXL375_INT_OVERRUN_BIT
 */
void ADXL375::setIntOverrunEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_ENABLE_REG, ADXL375_INT_OVERRUN_BIT, enabled);
}

// INT_MAP register

/** Get DATA_READY interrupt pin.
 * Any bits set to 0 in this register send their respective interrupts to the
 * INT1 pin, whereas bits set to 1 send their respective interrupts to the INT2
 * pin. All selected interrupts for a given pin are OR'ed.
 * @return Interrupt pin setting
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_DATA_READY_BIT
 */
uint8_t ADXL375::getIntDataReadyPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_DATA_READY_BIT, buffer);
    return buffer[0];
}
/** Set DATA_READY interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_DATA_READY_BIT
 */
void ADXL375::setIntDataReadyPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_DATA_READY_BIT, pin);
}
/** Get SINGLE_TAP interrupt pin.
 * @return Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_SINGLE_TAP_BIT
 */
uint8_t ADXL375::getIntSingleTapPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_SINGLE_TAP_BIT, buffer);
    return buffer[0];
}
/** Set SINGLE_TAP interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_SINGLE_TAP_BIT
 */
void ADXL375::setIntSingleTapPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_SINGLE_TAP_BIT, pin);
}
/** Get DOUBLE_TAP interrupt pin.
 * @return Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_DOUBLE_TAP_BIT
 */
uint8_t ADXL375::getIntDoubleTapPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_DOUBLE_TAP_BIT, buffer);
    return buffer[0];
}
/** Set DOUBLE_TAP interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_DOUBLE_TAP_BIT
 */
void ADXL375::setIntDoubleTapPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_DOUBLE_TAP_BIT, pin);
}
/** Get ACTIVITY interrupt pin.
 * @return Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_ACTIVITY_BIT
 */
uint8_t ADXL375::getIntActivityPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_ACTIVITY_BIT, buffer);
    return buffer[0];
}
/** Set ACTIVITY interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_ACTIVITY_BIT
 */
void ADXL375::setIntActivityPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_ACTIVITY_BIT, pin);
}
/** Get INACTIVITY interrupt pin.
 * @return Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_INACTIVITY_BIT
 */
uint8_t ADXL375::getIntInactivityPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_INACTIVITY_BIT, buffer);
    return buffer[0];
}
/** Set INACTIVITY interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_INACTIVITY_BIT
 */
void ADXL375::setIntInactivityPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_INACTIVITY_BIT, pin);
}
/** Get FREE_FALL interrupt pin.
 * @return Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_FREE_FALL_BIT
 */
uint8_t ADXL375::getIntFreefallPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_FREE_FALL_BIT, buffer);
    return buffer[0];
}
/** Set FREE_FALL interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_FREE_FALL_BIT
 */
void ADXL375::setIntFreefallPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_FREE_FALL_BIT, pin);
}
/** Get WATERMARK interrupt pin.
 * @return Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_WATERMARK_BIT
 */
uint8_t ADXL375::getIntWatermarkPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_WATERMARK_BIT, buffer);
    return buffer[0];
}
/** Set WATERMARK interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_WATERMARK_BIT
 */
void ADXL375::setIntWatermarkPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_WATERMARK_BIT, pin);
}
/** Get OVERRUN interrupt pin.
 * @return Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_OVERRUN_BIT
 */
uint8_t ADXL375::getIntOverrunPin() {
    I2Cdev::readBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_OVERRUN_BIT, buffer);
    return buffer[0];
}
/** Set OVERRUN interrupt pin.
 * @param pin Interrupt pin setting
 * @see getIntDataReadyPin()
 * @see ADXL375_INT_MAP
 * @see ADXL375_INT_OVERRUN_BIT
 */
void ADXL375::setIntOverrunPin(uint8_t pin) {
    I2Cdev::writeBit(devAddr, ADXL375_INT_MAP_REG, ADXL375_INT_OVERRUN_BIT, pin);
}

// INT_SOURCE register

/** Get DATA_READY interrupt source flag.
 * Bits set to 1 in this register indicate that their respective functions have
 * triggered an event, whereas a value of 0 indicates that the corresponding
 * event has not occurred. The DATA_READY, watermark, and overrun bits are
 * always set if the corresponding events occur, regardless of the INT_ENABLE
 * register settings, and are cleared by reading data from the DATAX, DATAY, and
 * DATAZ registers. The DATA_READY and watermark bits may require multiple
 * reads, as indicated in the FIFO mode descriptions in the FIFO section. Other
 * bits, and the corresponding interrupts, are cleared by reading the INT_SOURCE
 * register.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_DATA_READY_BIT
 */
uint8_t ADXL375::getIntDataReadySource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_DATA_READY_BIT, buffer);
    return buffer[0];
}
/** Get SINGLE_TAP interrupt source flag.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_SINGLE_TAP_BIT
 */
uint8_t ADXL375::getIntSingleTapSource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_SINGLE_TAP_BIT, buffer);
    return buffer[0];
}
/** Get DOUBLE_TAP interrupt source flag.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_DOUBLE_TAP_BIT
 */
uint8_t ADXL375::getIntDoubleTapSource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_DOUBLE_TAP_BIT, buffer);
    return buffer[0];
}
/** Get ACTIVITY interrupt source flag.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_ACTIVITY_BIT
 */
uint8_t ADXL375::getIntActivitySource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_ACTIVITY_BIT, buffer);
    return buffer[0];
}
/** Get INACTIVITY interrupt source flag.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_INACTIVITY_BIT
 */
uint8_t ADXL375::getIntInactivitySource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_INACTIVITY_BIT, buffer);
    return buffer[0];
}
/** Get FREE_FALL interrupt source flag.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_FREE_FALL_BIT
 */
uint8_t ADXL375::getIntFreefallSource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_FREE_FALL_BIT, buffer);
    return buffer[0];
}
/** Get WATERMARK interrupt source flag.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_WATERMARK_BIT
 */
uint8_t ADXL375::getIntWatermarkSource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_WATERMARK_BIT, buffer);
    return buffer[0];
}
/** Get OVERRUN interrupt source flag.
 * @return Interrupt source flag
 * @see ADXL375_INT_SOURCE
 * @see ADXL375_INT_OVERRUN_BIT
 */
uint8_t ADXL375::getIntOverrunSource() {
    I2Cdev::readBit(devAddr, ADXL375_INT_SOURCE_REG, ADXL375_INT_OVERRUN_BIT, buffer);
    return buffer[0];
}

// DATA_FORMAT register

/** Get self-test force enabled.
 * A setting of 1 in the SELF_TEST bit applies a self-test force to the sensor,
 * causing a shift in the output data. A value of 0 disables the self-test
 * force.
 * @return Self-test force enabled setting
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_SELFTEST_BIT
 */
uint8_t ADXL375::getSelfTestEnabled() {
    I2Cdev::readBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_SELFTEST_BIT, buffer);
    return buffer[0];
}
/** Set self-test force enabled.
 * @param enabled New self-test force enabled setting
 * @see getSelfTestEnabled()
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_SELFTEST_BIT
 */
void ADXL375::setSelfTestEnabled(uint8_t enabled) {
    I2Cdev::writeBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_SELFTEST_BIT, enabled);
}
/** Get SPI mode setting.
 * A value of 1 in the SPI bit sets the device to 3-wire SPI mode, and a value
 * of 0 sets the device to 4-wire SPI mode.
 * @return SPI mode setting
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_SELFTEST_BIT
 */
uint8_t ADXL375::getSPIMode() {
    I2Cdev::readBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_SPIMODE_BIT, buffer);
    return buffer[0];
}
/** Set SPI mode setting.
 * @param mode New SPI mode setting
 * @see getSPIMode()
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_SELFTEST_BIT
 */
void ADXL375::setSPIMode(uint8_t mode) {
    I2Cdev::writeBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_SPIMODE_BIT, mode);
}
/** Get interrupt mode setting.
 * A value of 0 in the INT_INVERT bit sets the interrupts to active high, and a
 * value of 1 sets the interrupts to active low.
 * @return Interrupt mode setting
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_INTMODE_BIT
 */
uint8_t ADXL375::getInterruptMode() {
    I2Cdev::readBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_INTMODE_BIT, buffer);
    return buffer[0];
}
/** Set interrupt mode setting.
 * @param mode New interrupt mode setting
 * @see getInterruptMode()
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_INTMODE_BIT
 */
void ADXL375::setInterruptMode(uint8_t mode) {
    I2Cdev::writeBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_INTMODE_BIT, mode);
}
/** Get full resolution mode setting.
 * When this bit is set to a value of 1, the device is in full resolution mode,
 * where the output resolution increases with the g range set by the range bits
 * to maintain a 4 mg/LSB scale factor. When the FULL_RES bit is set to 0, the
 * device is in 10-bit mode, and the range bits determine the maximum g range
 * and scale factor.
 * @return Full resolution enabled setting
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_FULL_RES_BIT
 */
uint8_t ADXL375::getFullResolution() {
    I2Cdev::readBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_FULL_RES_BIT, buffer);
    return buffer[0];
}
/** Set full resolution mode setting.
 * @param resolution New full resolution enabled setting
 * @see getFullResolution()
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_FULL_RES_BIT
 */
void ADXL375::setFullResolution(uint8_t resolution) {
    I2Cdev::writeBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_FULL_RES_BIT, resolution);
}
/** Get data justification mode setting.
 * A setting of 1 in the justify bit selects left-justified (MSB) mode, and a
 * setting of 0 selects right-justified mode with sign extension.
 * @return Data justification mode
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_JUSTIFY_BIT
 */
uint8_t ADXL375::getDataJustification() {
    I2Cdev::readBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_JUSTIFY_BIT, buffer);
    return buffer[0];
}
/** Set data justification mode setting.
 * @param justification New data justification mode
 * @see getDataJustification()
 * @see ADXL375_DATA_FORMAT
 * @see ADXL375_FORMAT_JUSTIFY_BIT
 */
void ADXL375::setDataJustification(uint8_t justification) {
    I2Cdev::writeBit(devAddr, ADXL375_DATA_FORMAT_REG, ADXL375_FORMAT_JUSTIFY_BIT, justification);
}


// DATA* registers

/** Get 3-axis accleration measurements.
 * These six bytes (Register 0x32 to Register 0x37) are eight bits each and hold
 * the output data for each axis. Register 0x32 and Register 0x33 hold the
 * output data for the x-axis, Register 0x34 and Register 0x35 hold the output
 * data for the y-axis, and Register 0x36 and Register 0x37 hold the output data
 * for the z-axis. The output data is twos complement, with DATAx0 as the least
 * significant byte and DATAx1 as the most significant byte, where x represent
 * X, Y, or Z. The DATA_FORMAT register (Address 0x31) controls the format of
 * the data. It is recommended that a multiple-byte read of all registers be
 * performed to prevent a change in data between reads of sequential registers.
 * 
 * The DATA_FORMAT register controls the presentation of data to Register 0x32
 * through Register 0x37. All data, except that for the +/-16 g range, must be
 * clipped to avoid rollover.
 *
 * @param x 16-bit signed integer container for X-axis acceleration
 * @param y 16-bit signed integer container for Y-axis acceleration
 * @param z 16-bit signed integer container for Z-axis acceleration
 * @see ADXL375_DATAX0
 */
void ADXL375::getAcceleration(int16_t* x, int16_t* y, int16_t* z) {
    I2Cdev::readBytes(devAddr, ADXL375_DATAX0, 6, buffer);
    *x = (((int16_t)buffer[1]) << 8) | buffer[0];
    *y = (((int16_t)buffer[3]) << 8) | buffer[2];
    *z = (((int16_t)buffer[5]) << 8) | buffer[4];
}
/** Get X-axis accleration measurement.
 * @return 16-bit signed X-axis acceleration value
 * @see ADXL375_DATAX0
 */
int16_t ADXL375::getAccelerationX() {
    I2Cdev::readBytes(devAddr, ADXL375_DATAX0, 2, buffer);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}
/** Get Y-axis accleration measurement.
 * @return 16-bit signed Y-axis acceleration value
 * @see ADXL375_DATAY0
 */
int16_t ADXL375::getAccelerationY() {
    I2Cdev::readBytes(devAddr, ADXL375_DATAY0, 2, buffer);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}
/** Get Z-axis accleration measurement.
 * @return 16-bit signed Z-axis acceleration value
 * @see ADXL375_DATAZ0
 */
int16_t ADXL375::getAccelerationZ() {
    I2Cdev::readBytes(devAddr, ADXL375_DATAZ0, 2, buffer);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}

// FIFO_CTL register

/** Get FIFO mode.
 * These bits set the FIFO mode, as described in Table 22. That is:
 *
 * 0x0 = Bypass (FIFO is bypassed.)
 *
 * 0x1 = FIFO (FIFO collects up to 32 values and then stops collecting data,
 *       collecting new data only when FIFO is not full.)
 *
 * 0x2 = Stream (FIFO holds the last 32 data values. When FIFO is full, the
 *       oldest data is overwritten with newer data.)
 *
 * 0x3 = Trigger (When triggered by the trigger bit, FIFO holds the last data
 *       samples before the trigger event and then continues to collect data 
 *       until full. New data is collected only when FIFO is not full.)
 *
 * @return Curent FIFO mode
 * @see ADXL375_FIFO_CTL
 * @see ADXL375_FIFO_MODE_BIT
 * @see ADXL375_FIFO_MODE_LENGTH
 */
uint8_t ADXL375::getFIFOMode() {
    I2Cdev::readBits(devAddr, ADXL375_FIFO_CTL, ADXL375_FIFO_MODE_BIT, ADXL375_FIFO_MODE_LENGTH, buffer);
    return buffer[0];
}
/** Set FIFO mode.
 * @param mode New FIFO mode
 * @see getFIFOMode()
 * @see ADXL375_FIFO_CTL
 * @see ADXL375_FIFO_MODE_BIT
 * @see ADXL375_FIFO_MODE_LENGTH
 */
void ADXL375::setFIFOMode(uint8_t mode) {
    I2Cdev::writeBits(devAddr, ADXL375_FIFO_CTL, ADXL375_FIFO_MODE_BIT, ADXL375_FIFO_MODE_LENGTH, mode);
}
/** Get FIFO trigger interrupt setting.
 * A value of 0 in the trigger bit links the trigger event of trigger mode to
 * INT1, and a value of 1 links the trigger event to INT2.
 * @return Current FIFO trigger interrupt setting
 * @see ADXL375_FIFO_CTL
 * @see ADXL375_FIFO_TRIGGER_BIT
 */
uint8_t ADXL375::getFIFOTriggerInterruptPin() {
    I2Cdev::readBit(devAddr, ADXL375_FIFO_CTL, ADXL375_FIFO_TRIGGER_BIT, buffer);
    return buffer[0];
}
/** Set FIFO trigger interrupt pin setting.
 * @param interrupt New FIFO trigger interrupt pin setting
 * @see ADXL375_FIFO_CTL
 * @see ADXL375_FIFO_TRIGGER_BIT
 */
void ADXL375::setFIFOTriggerInterruptPin(uint8_t interrupt) {
    I2Cdev::writeBit(devAddr, ADXL375_FIFO_CTL, ADXL375_FIFO_TRIGGER_BIT, interrupt);
}
/** Get FIFO samples setting.
 * The function of these bits depends on the FIFO mode selected (see Table 23).
 * Entering a value of 0 in the samples bits immediately sets the watermark
 * status bit in the INT_SOURCE register, regardless of which FIFO mode is
 * selected. Undesirable operation may occur if a value of 0 is used for the
 * samples bits when trigger mode is used.
 *
 * MODE    | EFFECT
 * --------+-------------------------------------------------------------------
 * Bypass  | None.
 * FIFO    | FIFO entries needed to trigger a watermark interrupt.
 * Stream  | FIFO entries needed to trigger a watermark interrupt.
 * Trigger | Samples are retained in the FIFO buffer before a trigger event.
 *
 * @return Current FIFO samples setting
 * @see ADXL375_FIFO_CTL
 * @see ADXL375_FIFO_SAMPLES_BIT
 * @see ADXL375_FIFO_SAMPLES_LENGTH
 */
uint8_t ADXL375::getFIFOSamples() {
    I2Cdev::readBits(devAddr, ADXL375_FIFO_CTL, ADXL375_FIFO_SAMPLES_BIT, ADXL375_FIFO_SAMPLES_LENGTH, buffer);
    return buffer[0];
}
/** Set FIFO samples setting.
 * @param size New FIFO samples setting (impact depends on FIFO mode setting)
 * @see getFIFOSamples()
 * @see getFIFOMode()
 * @see ADXL375_FIFO_CTL
 * @see ADXL375_FIFO_SAMPLES_BIT
 * @see ADXL375_FIFO_SAMPLES_LENGTH
 */
void ADXL375::setFIFOSamples(uint8_t size) {
    I2Cdev::writeBits(devAddr, ADXL375_FIFO_CTL, ADXL375_FIFO_SAMPLES_BIT, ADXL375_FIFO_SAMPLES_LENGTH, size);
}

// FIFO_STATUS register

/** Get FIFO trigger occurred status.
 * A 1 in the FIFO_TRIG bit corresponds to a trigger event occurring, and a 0
 * means that a FIFO trigger event has not occurred.
 * @return FIFO trigger occurred status
 * @see ADXL375_FIFO_STATUS
 * @see ADXL375_FIFOSTAT_TRIGGER_BIT
 */
bool ADXL375::getFIFOTriggerOccurred() {
    I2Cdev::readBit(devAddr, ADXL375_FIFO_STATUS, ADXL375_FIFOSTAT_TRIGGER_BIT, buffer);
    return buffer[0];
}
/** Get FIFO length.
 * These bits report how many data values are stored in FIFO. Access to collect
 * the data from FIFO is provided through the DATAX, DATAY, and DATAZ registers.
 * FIFO reads must be done in burst or multiple-byte mode because each FIFO
 * level is cleared after any read (single- or multiple-byte) of FIFO. FIFO
 * stores a maximum of 32 entries, which equates to a maximum of 33 entries
 * available at any given time because an additional entry is available at the
 * output filter of the I2Cdev::
 * @return Current FIFO length
 * @see ADXL375_FIFO_STATUS
 * @see ADXL375_FIFOSTAT_LENGTH_BIT
 * @see ADXL375_FIFOSTAT_LENGTH_LENGTH
 */
uint8_t ADXL375::getFIFOLength() {
    I2Cdev::readBits(devAddr, ADXL375_FIFO_STATUS, ADXL375_FIFOSTAT_LENGTH_BIT, ADXL375_FIFOSTAT_LENGTH_LENGTH, buffer);
    return buffer[0];
}



void ADXL375::dumpSetup(){
  Serial.println("ADXL375_DATA_FORMAT_REG:");
    Serial.print("getSelfTestEnabled()");Serial.println(getSelfTestEnabled());
    Serial.print("getSPIMode()");Serial.println(getSPIMode());
    Serial.print("getInterruptMode()");Serial.println(getInterruptMode());
    Serial.print("getFullResolution()"); Serial.println(getFullResolution());
    Serial.print("getDataJustification()");Serial.println(getDataJustification());
  Serial.print("getTapThreshold():");Serial.println(getTapThreshold());
  Serial.print("getTapDuration():");Serial.println(getTapDuration()); 
  Serial.print("getDoubleTapLatency():");Serial.println(getDoubleTapLatency());
  Serial.print("setDoubleTapWindow():");Serial.println(getDoubleTapWindow());
  Serial.print("getTapAxisXEnabled():");Serial.println(getTapAxisXEnabled());
  Serial.print("getTapAxisYEnabled():");Serial.println(getTapAxisYEnabled());
  Serial.print("getTapAxisZEnabled():");Serial.println(getTapAxisZEnabled());
  Serial.print("getLowPowerEnabled():");Serial.println(getLowPowerEnabled());
  Serial.print("getRate():");Serial.println(getRate());
  Serial.print("getIntSingleTapEnabled():");Serial.println(getIntSingleTapEnabled());
  Serial.print("getIntDoubleTapEnabled():");Serial.println(getIntDoubleTapEnabled());
  Serial.print("getIntActivityEnabled():");Serial.println(getIntActivityEnabled());
  Serial.print("getIntInactivityEnabled():"); Serial.println(getIntInactivityEnabled());
  Serial.print("getIntSingleTapPin():");Serial.println(getIntSingleTapPin());
}

void ADXL375::setupFIFO(){
  setSelfTestEnabled(0);
  setSPIMode(0);
  setInterruptMode(0);
  setFullResolution(1);
  setDataJustification(0);
  setTapThreshold(255/4);
  setTapDuration(60);
  setDoubleTapLatency(0);
  //setDoubleTapWindow(0x1F);
  setTapAxisXEnabled(true);
  setTapAxisYEnabled(true);
  setTapAxisZEnabled(true);
  setLowPowerEnabled(true);
  setRate(0xF);
  setTriggerPosition(32);
  setIntSingleTapEnabled(true);
  setIntDoubleTapEnabled(false);
  setIntActivityEnabled(false);
  setIntInactivityEnabled(false);
  setIntSingleTapPin(1);
}

//Because the measured output for each axis is expressed in LSBs, 
//XST, YST, and ZST are also expressed in LSBs. These values can be 
//converted to acceleration (g) by multiplying each value by the 
//49 mg/LSB scale factor 
// meaning, multiply each measurement by 49e-3, or in integer math divide by 1/49e-3, or 1/.0049 or ~ 20 
// confirming this, given 49mG/LSB (.049 G/ LSB) FSR 200 G, 200/.049 == 4081 , or very close to 14 bits (plus sign)
// 4095 (14 bits) / 200 (FSR) == 20 , so dividing the data registers (x,7,z by 20 should give Gs)
#define GDIVISOR 20

void scaleXYZ(short *x,short *y,short *z) {
      *x = *x/GDIVISOR,
      *y = *y/GDIVISOR, 
      *z = *z/GDIVISOR;  
}

uint16_t ADXL375::maxFIFO(uint16_t fifo[]){
   short x, y, z;
   int fifo_index =0;
   unsigned maxx=0,maxy=0,maxz=0;
   unsigned maxmax=0;
   int i=0;
    for(i=0;i<32;i++){
      x = fifo[fifo_index++];  
      y = fifo[fifo_index++];  
      z = fifo[fifo_index++];  
  
      maxx=max(abs(x),maxx);
      maxy=max(abs(y),maxy);
      maxz=max(abs(z),maxz);
    }
    maxmax=max(maxx,maxy);
    maxmax = max(maxmax,maxz);
    return(maxmax);
  
  }
  

void ADXL375::saveFIFO(){
   int i=0;
   int fifo_index;
   short x, y, z;
   fifo_index=0;  
    readFrom(DEVICE,ADXL375_INT_SOURCE_REG,1,buff);
    for(i=0;i<32;i++){
      readFrom(DEVICE, ADXL375_DATAX0_REG, TO_READ, buff); //read the acceleration data from the ADXL345
      x = (((short)buff[1]) << 8) | buff[0];   
      y = (((short)buff[3])<< 8) | buff[2];
      z = (((short)buff[5]) << 8) | buff[4];
      scaleXYZ(&x,&y,&z);
      newistFiFo[fifo_index++] = x;
      newistFiFo[fifo_index++] = y;
      newistFiFo[fifo_index++] = z;
    }
    //keep the biggist shock only
    if(maxFIFO(newistFiFo) > maxFIFO(lastFiFo)) {
     for(int i=0;i<sizeof(lastFiFo);i++){
      lastFiFo[i] =  newistFiFo[i];
    } 
    }
 }
 void ADXL375::printFIFO(){
   int i=0;
      int fifo_index;
     short x, y, z;
   fifo_index=0;  

    for(i=0;i<32;i++){
      x = lastFiFo[fifo_index++];  
      x = lastFiFo[fifo_index++];  
      x = lastFiFo[fifo_index++];  

      sprintf(str, "%+03d,%+03d,%+03d ",x, y, z);  

      Serial.print(str);
      Serial.write(10);

    }
 }

