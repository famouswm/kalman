#ifndef _hmc_h_
#define _hmc_h_
#include "mbed.h"

const int HMC_CONFIGURATION_REGISTER_A = 0x00;
const int HMC_CONFIGURATION_REGISTER_B = 0x01;
const int HMC_MODE_REGISTER            = 0x02;
const int HMC_WRITE_ADDRESS            = 0x3C;
const int HMC_READ_ADDRESS             = 0x3D;

const int HMC_DATA_OUTPUT_X_MSB = 0x03;
const int HMC_DATA_OUTPUT_X_LSB = 0x04;
const int HMC_DATA_OUTPUT_Z_MSB = 0x05;
const int HMC_DATA_OUTPUT_Z_LSB = 0x06;
const int HMC_DATA_OUTPUT_Y_MSB = 0x07;
const int HMC_DATA_OUTPUT_Y_LSB = 0x08;

const int HMC_ID_REGISTER_A = 0x0A;
const int HMC_ID_REGISTER_B = 0x0B;
const int HMC_ID_REGISTER_C = 0x0C;
const int HMC_ID_A_DEFAULT = 0x48;
const int HMC_ID_B_DEFAULT = 0x34;
const int HMC_ID_C_DEFAULT = 0x33;

// Number of samples averaged per measurement output
const int HMC_AVERAGING_SAMPLES_1      = 0x00;
const int HMC_AVERAGING_SAMPLES_2      = 0x01;
const int HMC_AVERAGING_SAMPLES_4      = 0x02;
const int HMC_AVERAGING_SAMPLES_8      = 0x03;

// Selectable output rates in continuous-measurement mode.
// All three channels are measured with the same rate.
// The number xx_yy denotes the rate xx.yy Hz.
const int HMC_DATA_OUTPUT_RATE_0_75    = 0x00;
const int HMC_DATA_OUTPUT_RATE_1_5     = 0x01;
const int HMC_DATA_OUTPUT_RATE_3       = 0x02;
const int HMC_DATA_OUTPUT_RATE_7_5     = 0x03;
const int HMC_DATA_OUTPUT_RATE_15      = 0x04;
const int HMC_DATA_OUTPUT_RATE_30      = 0x05;
const int HMC_DATA_OUTPUT_RATE_75      = 0x06;

const int HMC_FREQUENCY_100KHZ         = 100000;
const int HMC_FREQUENCY_400KHZ         = 400000;

// Normal measurement mode (default).
// In normal measurement configuration the positive and
// negative pins of the resistive load are left floating
// and high impedance.
const int HMC_BIAS_NONE                = 0x00;

// Positive bias configuration for the X, Y and Z axes.
// In this configuration a positive current is forced
// across the resistive load for each axis.
const int HMC_BIAS_POSITIVE            = 0x01;

// Negative bias configuration for the X, Y and Z axes.
// In this configuratin a negative current is forced
// across the resistive load for each axis.
const int HMC_BIAS_NEGATIVE            = 0x02;

// These configure the common gain for each axis.
// The output range of the sensor in each axis is
// 0xF800 to 0x07FF (-2048, 2047);
const int HMC_GAIN_1370               = 0x00; // 0.73 mG / LSb
const int HMC_GAIN_1090               = 0x01; // 0.92 mG / LSb
const int HMC_GAIN_820                = 0x02; // 1.22 mG / LSb
const int HMC_GAIN_660                = 0x03; // 1.52 mG / LSb
const int HMC_GAIN_440                = 0x04; // 2.27 mG / LSb
const int HMC_GAIN_390                = 0x05; // 2.56 mG / LSb
const int HMC_GAIN_330                = 0x06; // 3.03 mG / LSb
const int HMC_GAIN_230                = 0x07; // 4.35 mG / LSb

// In continuous-measurement mode the device continuously
// performs measurements and places the result in the data
// register. RDY goes high when new data is placed in all
// three registers. Measurements will become available at
// a frequency equal to the set DATA_OUTPUT_FREQUENCY.
const int HMC_CONTINUOUS_MODE         = 0x00;

// In single-measurement mode the device performs a single
// measurement, sets RDY high and returns to idle mode.
const int HMC_SINGLE_MODE             = 0x01; // Default
const int HMC_IDLE_MODE               = 0x02;

class HMC5883
{
public:
    HMC5883(PinName sda, PinName scl);
    bool init(int frequency, 
             int num_averaging_samples, 
             int data_output_rate, 
             int bias_mode,
             int gain,
             int measurement_mode);

    void getXYZ(int16_t *x, int16_t *y, int16_t *z);

    // TODO: Set settings as functions
private:
    I2C hmc_i2c;

    int writeByte(int where, int what);
    int readBytes(int where, char *data, int length);
    void swapBytes(int16_t *v);
};

#endif