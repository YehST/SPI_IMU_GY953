# GY-953 Library in STM32 HAL

## Abstract
- This library is developed for STM32 HAL library and uses GY-953 the IMU sensor.

- This library supports reading from multiple sensors, which can be achieved by assigning separate Chip Select (CS) pins when creating an IMU object structure. External interrupt pins can also be customized through the development of dedicated external interrupt routines, allowing you to decide whether to use the same pin for all sensors or different pins individually.
## Instructions For Use
- First, initialize the handle for communication processing and initialize IMUs by defining the structure as shown below for initialization arguments.
    ```C
      IMU_Struct IMU = {
        .CS_GPIO = GPIOE,
        .CS_PIN = GPIO_PIN_3
      }; // define the structure

      IMU_Init_Handle(&hspi3); // initialize the handle for communication
      IMU_Init_SET(&IMU, 200, 1, 1, 1); // initialize IMUs
    ```
- In terms of usage, you can place the ***IMU_ContinuousReadData()*** function within the main program loop. This function reads data from the IMU and copies it into a structure. It operates in continuous read mode, retrieving all available data from the IMU. For more details, you can refer to the datasheet. Additionally, make sure to include ***IMU_ResetFlag()*** within the external interrupt function to ensure proper data updating.
    ```C
      IMU_ContinuousReadData(&IMU); // read IMU data
      
      IMU_ResetFlag(&IMU); // ensure data updating
    ```

- NOTICE:
  1. If you encounter compilation failure due to the absence of the "printData" function, please replace "printData" with your own function for displaying data (e.g., printf), or copy it from the main file of the example program and use it accordingly.

## Example
- In this example, we have configured the GY-953 to update data at a rate of 200Hz. After reading the data, we transmit it back to the computer's serial port via UART.
  | STM32 PIN | Device PIN |
  | --- | --- |
  |PD8|PC RX|
  |PD9|PC TX|
  |PE3|CS|
  |PC10|SCK|
  |PC11|MISO|
  |PC12|MOSI|
  |PE0|INT|
  |5V|VCC|
  |GND|GND|

## Reference
- [GY-953 Datasheet]

[GY-953 Datasheet]:https://server4.eca.ir/eshop/000/GY-953%5B3011011012%5D/The%20GY953%20module%20User%20Manual%202.0.pdf
