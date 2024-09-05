/**
 * Device Calibrations
 * Author: jk1806
 * Created: 2024-08-20
 * 
 * Factory calibration data management
 * Stores sensor offsets, gains, and correction factors
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define CALIBRATION_VERSION "1.0.0"
#define MAX_CALIBRATIONS 32

struct calibration_data {
    char sensor_name[32];
    float offset;
    float gain;
    float temperature_coeff;
    u32 timestamp;
    bool valid;
};

static struct calibration_data calibrations[MAX_CALIBRATIONS];
static int calibration_count = 0;

/**
 * Set calibration data
 */
int calibration_set(const char *sensor, float offset, float gain, float temp_coeff)
{
    int i;
    
    if (calibration_count >= MAX_CALIBRATIONS) {
        return -ENOSPC;
    }
    
    for (i = 0; i < calibration_count; i++) {
        if (strcmp(calibrations[i].sensor_name, sensor) == 0) {
            break;
        }
    }
    
    if (i == calibration_count) {
        calibration_count++;
    }
    
    strncpy(calibrations[i].sensor_name, sensor, sizeof(calibrations[i].sensor_name) - 1);
    calibrations[i].offset = offset;
    calibrations[i].gain = gain;
    calibrations[i].temperature_coeff = temp_coeff;
    calibrations[i].timestamp = jiffies;
    calibrations[i].valid = true;
    
    pr_info("Calibration: Set for %s (offset=%.3f, gain=%.3f)\n",
            sensor, offset, gain);
    
    return 0;
}

/**
 * Get calibration data
 */
int calibration_get(const char *sensor, float *offset, float *gain, float *temp_coeff)
{
    int i;
    
    for (i = 0; i < calibration_count; i++) {
        if (strcmp(calibrations[i].sensor_name, sensor) == 0 && calibrations[i].valid) {
            if (offset) *offset = calibrations[i].offset;
            if (gain) *gain = calibrations[i].gain;
            if (temp_coeff) *temp_coeff = calibrations[i].temperature_coeff;
            return 0;
        }
    }
    
    return -ENOENT;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jk1806");
MODULE_DESCRIPTION("Device Calibrations");
MODULE_VERSION(CALIBRATION_VERSION);

