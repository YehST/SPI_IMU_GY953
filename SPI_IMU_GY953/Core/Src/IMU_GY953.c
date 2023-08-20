#include "main.h"
#include "IMU_GY953.h"

/******************************************************************************/
SPI_HandleTypeDef* IMU_hSPI;
/******************************************************************************/
/** @brief Connect the spi handler to the IMU.
  * 
  * @param hspi: The spi handler.  
  * @return None. */
void IMU_Init_Handle(SPI_HandleTypeDef *hspi){
	IMU_hSPI = hspi;
}

/** @brief Intialize the IMU.
  * 
  * @param IMUx: The IMU structure.  
  * @param frequency: The data output frequency of IMU.  
  * @param MagIO: The switch of magnetic sensor.  
  * @param GyroIO: The switch of Gyro sensor.  
  * @param AccIO: The switch of acceleration sensor.   
  * Can only disable one of the three sensors at the same time.
  * @return None. */
void IMU_Init_SET(IMU_Struct *IMUx, int frequency, uint8_t MagIO, uint8_t GyroIO, uint8_t AccIO){
	HAL_Delay(1000);
	IMU_Calibration(IMUx);
	HAL_Delay(1000);
	IMU_SPI_Enable;
	uint8_t data[2];
	data[0] = WriteIMU | SetReg;
	if (frequency == 100){
		data[1] = RegisterA | 0x04 | MagIO<<6 | GyroIO<<5 | AccIO<<4;
	}
	else if (frequency == 200){
		data[1] = RegisterA | 0x05 | MagIO<<6 | GyroIO<<5 | AccIO<<4;
	}
	else data[1] = RegisterA | 0x03 | MagIO<<6 | GyroIO<<5 | AccIO<<4;
	HAL_SPI_Transmit(IMU_hSPI, data, 2, 50);
	
	IMU_SPI_Disable;
}

/** @brief Calibrating the IMU.
  * 
  * @param IMUx: The IMU structure.  
  * @return None. */
void IMU_Calibration(IMU_Struct *IMUx){
	IMU_SPI_Enable;
	uint8_t data[2];
	data[0] = WriteIMU | ConReg;
	data[1] = 0x04 | RegisterB; // Mag
	HAL_SPI_Transmit(IMU_hSPI, data, 2, 50);
	
	data[1] = 0x08 | RegisterB; // Gyro & Acc
	HAL_SPI_Transmit(IMU_hSPI, data, 2, 50);
	
	IMU_SPI_Disable;
}

/** @brief Read data from IMU with continuous mode.
  * 
  * @param IMUx: The IMU structure.  
  * @return the flag if it read data success. */
uint8_t IMU_ContinuousReadData(IMU_Struct *IMUx){
	if (IMUx->read_Flag){
	uint8_t cmd = ContinuousReadIMU; /* 連續讀取模式 */
	
	IMU_SPI_Enable;
	HAL_SPI_Transmit(IMU_hSPI, &cmd, 1, 50);
	HAL_SPI_Receive(IMU_hSPI, IMUx->TMP_buf, 41, 150);
	IMU_SPI_Disable;
	IMU_handleData(IMUx);

	return 1;
	}
	else return 0;
}

/** @brief Handle the data from IMU.
  * 
  * @param IMUx: The IMU structure.  
  * @return None. */
void IMU_handleData(IMU_Struct *IMUx){
	memcpy(IMUx->data_buf, IMUx->TMP_buf, sizeof(IMUx->TMP_buf)); // 由IMU擷取的資料複製
	
	int16_t tmp[3];
	tmp[0] = (IMUx->data_buf[20]<<8)|IMUx->data_buf[21];
	tmp[1] = (IMUx->data_buf[22]<<8)|IMUx->data_buf[23];
	tmp[2] = (IMUx->data_buf[24]<<8)|IMUx->data_buf[25];
	IMUx->ROLL = (float)tmp[0]/100.;
	IMUx->PITCH = (float)tmp[1]/100.;
	IMUx->YAW = (float)tmp[2]/100.;
	
	tmp[0] = (IMUx->data_buf[2]<<8)|IMUx->data_buf[3];
	tmp[1] = (IMUx->data_buf[4]<<8)|IMUx->data_buf[5];
	tmp[2] = (IMUx->data_buf[6]<<8)|IMUx->data_buf[7];
	IMUx->ACCxyz[0] = (float)tmp[0]/16383.;
	IMUx->ACCxyz[1] = (float)tmp[1]/16383.;
	IMUx->ACCxyz[2] = (float)tmp[2]/16383.;
	
	tmp[0] = (IMUx->data_buf[8]<<8)|IMUx->data_buf[9];
	tmp[1] = (IMUx->data_buf[10]<<8)|IMUx->data_buf[11];
	tmp[2] = (IMUx->data_buf[12]<<8)|IMUx->data_buf[13];
	IMUx->Gyroxyz[0] = (float)tmp[0]/16.4;
	IMUx->Gyroxyz[1] = (float)tmp[1]/16.4;
	IMUx->Gyroxyz[2] = (float)tmp[2]/16.4;
	
	tmp[0] = (IMUx->data_buf[14]<<8)|IMUx->data_buf[15];
	tmp[1] = (IMUx->data_buf[16]<<8)|IMUx->data_buf[17];
	tmp[2] = (IMUx->data_buf[18]<<8)|IMUx->data_buf[19];
	IMUx->Magxyz[0] = (float)tmp[0]/6.7;
	IMUx->Magxyz[1] = (float)tmp[1]/6.7;
	IMUx->Magxyz[2] = (float)tmp[2]/6.7;
}

/** @brief Reset the read flag of IMU.
  * It should be called in the GPIO_EXTI function.
  * @param IMUx: The IMU structure.  
  * @return None. */
void IMU_ResetFlag(IMU_Struct *IMUx){
	IMUx->read_Flag = IMUx->read_Flag | 1;
}
/******************************************************************************/
