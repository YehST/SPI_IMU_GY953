#ifndef IMU_GY953_H
#define IMU_GY953_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
typedef struct {
	// CS Pin Set 
	GPIO_TypeDef *GPIO; 
	uint16_t GPIO_PIN;
	
	uint8_t data_buf[41];
	uint8_t TMP_buf[41];

	
	float ROLL, PITCH, YAW;
	float ACCxyz[3];
	float Gyroxyz[3];
	float Magxyz[3];
	
	
} IMU_Struct;

#define RegisterA 0x08
#define RegisterB 0x11

#define SetReg 0x01
#define ConReg 0x02

#define WriteIMU 0x40
#define ReadIMU 0x80
#define ContinuousReadIMU 0xc1
/******************************************************************************/
void IMU_Init_Handle(SPI_HandleTypeDef *hspi);
void IMU_Init_SET(IMU_Struct *IMUx, int frequency, uint8_t MagIO, uint8_t GyroIO, uint8_t AccIO);
void IMU_Calibration(IMU_Struct *IMUx);
void IMU_ContinuousReadData(IMU_Struct *IMUx);
void IMU_handleData(IMU_Struct *IMUx);
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif