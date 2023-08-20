#include "main.h"
#include "IMU_GY953.h"

/******************************************************************************/

/******************************************************************************/
SPI_HandleTypeDef* IMU_hSPI;
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
void IMU_Init_Handle(SPI_HandleTypeDef *hspi){
	IMU_hSPI = hspi;
}

void IMU_Init_SET(IMU_Struct *IMUx, int frequency, uint8_t MagIO, uint8_t GyroIO, uint8_t AccIO){
	HAL_Delay(1000);
	IMU_Calibration(IMUx);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(IMUx->GPIO, IMUx->GPIO_PIN, GPIO_PIN_RESET);
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
	
	
	HAL_GPIO_WritePin(IMUx->GPIO, IMUx->GPIO_PIN, GPIO_PIN_SET);
}

void IMU_Calibration(IMU_Struct *IMUx){
	HAL_GPIO_WritePin(IMUx->GPIO, IMUx->GPIO_PIN, GPIO_PIN_RESET);
	uint8_t data[2];
	data[0] = WriteIMU | ConReg;
	data[1] = 0x04 | RegisterB; // Mag
	HAL_SPI_Transmit(IMU_hSPI, data, 2, 50);
	
	data[1] = 0x08 | RegisterB; // Gyro & Acc
	HAL_SPI_Transmit(IMU_hSPI, data, 2, 50);
	
	HAL_GPIO_WritePin(IMUx->GPIO, IMUx->GPIO_PIN, GPIO_PIN_SET);
}

void IMU_ContinuousReadData(IMU_Struct *IMUx){
	uint8_t cmd = ContinuousReadIMU; /* 連續讀取模式 */
	
	HAL_GPIO_WritePin(IMUx->GPIO, IMUx->GPIO_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(IMU_hSPI, &cmd, 1, 50);
	HAL_SPI_Receive(IMU_hSPI, IMUx->TMP_buf, 41, 150);
	HAL_GPIO_WritePin(IMUx->GPIO, IMUx->GPIO_PIN, GPIO_PIN_SET);
	IMU_handleData(IMUx);
}

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
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(GPIO_Pin);
//  /* NOTE: This function Should not be modified, when the callback is needed,
//           the HAL_GPIO_EXTI_Callback could be implemented in the user file
//   */
//		// Åª¨ú GY953 ¼Æ¾Ú¦b¤¤Â_Ä²µo®ÉÅª¨ú IMU ¸ê°T 
//		if(!Flag){
//			IMU_ContinuousReadData(&IMU1);
//			Flag=1;
//			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
//		}
//}
