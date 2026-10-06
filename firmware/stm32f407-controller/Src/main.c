/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "tim.h"

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  GPIO_TypeDef *DIR_Port;
  uint16_t      DIR_Pin;

  GPIO_TypeDef *EN_Port;
  uint16_t      EN_Pin;
  uint8_t       Has_EN;

  GPIO_TypeDef *PUL_Port;
  uint16_t      PUL_Pin;
} StepperMotor_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define HIGH GPIO_PIN_SET
#define LOW  GPIO_PIN_RESET

/* Low level enable */
#define ON   LOW
#define OFF  HIGH

#define CW   GPIO_PIN_SET
#define CCW  GPIO_PIN_RESET

#define M1_STEP_US      5000U
#define M2_STEP_US      1000U
#define M3_STEP_US      1000U
#define M4_STEP_US      1000U
#define M5_STEP_US      800U
#define M6_STEP_US      300U
#define STEP_SUBDIVIDE  8.0f

/* Sensor1: J40 PH2 */
#define ROKO1_PORT GPIOH
#define ROKO1_PIN  GPIO_PIN_2

/* Sensor2: J40 PH3 */
#define ROKO2_PORT GPIOH
#define ROKO2_PIN  GPIO_PIN_3

/* LED for Sensor1: PE2 */
#define LED1_PORT  GPIOE
#define LED1_PIN   GPIO_PIN_2

/* LED for Sensor2: PG15 */
#define LED2_PORT  GPIOG
#define LED2_PIN   GPIO_PIN_15

#define LED_ON     GPIO_PIN_RESET
#define LED_OFF    GPIO_PIN_SET

/* Relay */
#define RELAY1_PORT GPIOF
#define RELAY1_PIN  GPIO_PIN_3   /* PF3 */

#define RELAY2_PORT GPIOF
#define RELAY2_PIN  GPIO_PIN_4   /* PF4 */

/*
   Relay output logic used by this program:
   GPIO_PIN_SET   = relay ON
   GPIO_PIN_RESET = relay OFF
*/
#define RELAY_ON    GPIO_PIN_SET
#define RELAY_OFF   GPIO_PIN_RESET

/* Motor1: J25 */
#define M1_DIR_PORT GPIOE
#define M1_DIR_PIN  GPIO_PIN_1
#define M1_EN_PORT  GPIOE
#define M1_EN_PIN   GPIO_PIN_0
#define M1_PUL_PORT GPIOI
#define M1_PUL_PIN  GPIO_PIN_5

/* Motor2: J26 */
#define M2_DIR_PORT GPIOI
#define M2_DIR_PIN  GPIO_PIN_8
#define M2_EN_PORT  GPIOE
#define M2_EN_PIN   GPIO_PIN_4
#define M2_PUL_PORT GPIOI
#define M2_PUL_PIN  GPIO_PIN_6

/* Motor3: J31 */
#define M3_DIR_PORT GPIOI
#define M3_DIR_PIN  GPIO_PIN_11
#define M3_EN_PORT  GPIOI
#define M3_EN_PIN   GPIO_PIN_10
#define M3_PUL_PORT GPIOI
#define M3_PUL_PIN  GPIO_PIN_7

/* Motor4: J32 */
#define M4_DIR_PORT GPIOF
#define M4_DIR_PIN  GPIO_PIN_2
#define M4_EN_PORT  GPIOF
#define M4_EN_PIN   GPIO_PIN_1
#define M4_PUL_PORT GPIOC
#define M4_PUL_PIN  GPIO_PIN_9

/* Motor5: External MD4CH */
#define M5_DIR_PORT GPIOE
#define M5_DIR_PIN  GPIO_PIN_15
#define M5_EN_PORT  NULL
#define M5_EN_PIN   GPIO_PIN_0
#define M5_PUL_PORT GPIOE
#define M5_PUL_PIN  GPIO_PIN_13

/* Motor6: External MD4CH */
#define M6_DIR_PORT GPIOF
#define M6_DIR_PIN  GPIO_PIN_11
#define M6_EN_PORT  NULL
#define M6_EN_PIN   GPIO_PIN_0
#define M6_PUL_PORT GPIOD
#define M6_PUL_PIN  GPIO_PIN_9

/*
   Servo 1 connection: J28 -> PD12 -> TIM4_CH1

   After Motor 5 finishes, Servo 1 rotates outward for about 90 degrees.
   At the end of the sequence, Servo 1 returns to its original position.
*/
#define SERVO1_STOP_US          1500U
#define SERVO1_CCW_US           1350U
#define SERVO1_CW_US            1650U
#define SERVO1_ROTATE_TIME_MS   900U

/*
   Servo 2 connection: J29 -> PD13 -> TIM4_CH2

   After Motor 3 finishes its motion, Servo 2 performs one forward-and-return action.
*/
#define SERVO2_STOP_US          1500U
#define SERVO2_CCW_US           1350U
#define SERVO2_CW_US            1650U
#define SERVO2_ROTATE_TIME_MS   1500U

/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

static StepperMotor_t motor1 =
{
  M1_DIR_PORT, M1_DIR_PIN,
  M1_EN_PORT,  M1_EN_PIN, 1,
  M1_PUL_PORT, M1_PUL_PIN
};

static StepperMotor_t motor2 =
{
  M2_DIR_PORT, M2_DIR_PIN,
  M2_EN_PORT,  M2_EN_PIN, 1,
  M2_PUL_PORT, M2_PUL_PIN
};

static StepperMotor_t motor3 =
{
  M3_DIR_PORT, M3_DIR_PIN,
  M3_EN_PORT,  M3_EN_PIN, 1,
  M3_PUL_PORT, M3_PUL_PIN
};

static StepperMotor_t motor4 =
{
  M4_DIR_PORT, M4_DIR_PIN,
  M4_EN_PORT,  M4_EN_PIN, 1,
  M4_PUL_PORT, M4_PUL_PIN
};

static StepperMotor_t motor5 =
{
  M5_DIR_PORT, M5_DIR_PIN,
  M5_EN_PORT,  M5_EN_PIN, 0,
  M5_PUL_PORT, M5_PUL_PIN
};

static StepperMotor_t motor6 =
{
  M6_DIR_PORT, M6_DIR_PIN,
  M6_EN_PORT,  M6_EN_PIN, 0,
  M6_PUL_PORT, M6_PUL_PIN
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
static void delay_us_init(void);
static void delay_us(uint32_t us);

static void Stepper_GPIO_Manual_Init(void);
static void Sensor_LED_Manual_Init(void);

static void Stepper_SetDir(StepperMotor_t *m, GPIO_PinState dir);
static void Stepper_SetEn(StepperMotor_t *m, GPIO_PinState en_state);
static void Stepper_Pulse(StepperMotor_t *m, uint32_t tim_us);
static void Stepper_Init(StepperMotor_t *m);
static void Stepper_Turn(StepperMotor_t *m, uint32_t tim_us, float angle, float subdivide, GPIO_PinState dir);

static uint8_t Sensor1_IsTriggered(void);
static uint8_t Sensor2_IsTriggered(void);
static void Sensor_LED_Update(void);

static void Relay_Set(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);
static void Relay_All_Off(void);

static void Stepper_Run_CW_Until_Sensor1(StepperMotor_t *m, uint32_t tim_us);
static void Wait_Until_Sensor2_Triggered(void);
static void Linkage_Run_Once(void);

/* J28 Servo1 */
static void Servo1_Start(void);
static void Servo1_SetPulse(uint16_t pulse_us);
static void Servo1_Stop(void);
static void Servo1_CCW(void);
static void Servo1_CW(void);
static void Servo1_TurnOut_After_Motor5(void);
static void Servo1_Return_At_End(void);

/* J29 Servo2 */
static void Servo2_Start(void);
static void Servo2_SetPulse(uint16_t pulse_us);
static void Servo2_Stop(void);
static void Servo2_CCW(void);
static void Servo2_CW(void);
static void Servo2_Action_After_Motor3(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

static void Stepper_GPIO_Manual_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /*
     GPIOE:
     PE0  -> M1_EN
     PE1  -> M1_DIR
     PE4  -> M2_EN
     PE13 -> M5_PUL
     PE15 -> M5_DIR
  */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1 | GPIO_PIN_13 | GPIO_PIN_15, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_13 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*
     GPIOI:
     PI5  -> M1_PUL
     PI6  -> M2_PUL
     PI8  -> M2_DIR
     PI7  -> M3_PUL
     PI10 -> M3_EN
     PI11 -> M3_DIR
  */
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_10, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_11, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                        GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*
     GPIOC:
     PC9 -> M4_PUL
  */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*
     GPIOF:
     PF1  -> M4_EN
     PF2  -> M4_DIR
     PF3  -> Relay1
     PF4  -> Relay2
     PF11 -> M6_DIR
  */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2 | GPIO_PIN_11, GPIO_PIN_RESET);

  /*
     Initialize both relay outputs to the OFF state:
     GPIO_PIN_RESET = relay OFF
  */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*
     GPIOD:
     PD9 -> M6_PUL

     Important:
     PD12 is used by J28 Servo 1 through TIM4_CH1 and must be configured by CubeMX.
     PD13 is used by J29 Servo 2 through TIM4_CH2 and must be configured by CubeMX.
     Do not reconfigure PD12 or PD13 as ordinary GPIO outputs here.
  */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void Sensor_LED_Manual_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*
     PH2 -> Sensor1
     PH3 -> Sensor2

     Sensor input logic:
     GPIO_PIN_RESET = sensor triggered
     GPIO_PIN_SET   = sensor not triggered
  */
  GPIO_InitStruct.Pin = ROKO1_PIN | ROKO2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*
     PE2  -> Sensor1 LED
     PG15 -> Sensor2 LED

     LED:
     GPIO_PIN_RESET = LED ON
     GPIO_PIN_SET   = LED OFF
  */
  HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, LED_OFF);
  HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, LED_OFF);

  GPIO_InitStruct.Pin = LED1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_PORT, &GPIO_InitStruct);
}

static void delay_us_init(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us(uint32_t us)
{
  uint32_t ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000U);
  uint32_t start = DWT->CYCCNT;

  while ((DWT->CYCCNT - start) < ticks)
  {
  }
}

static void Stepper_SetDir(StepperMotor_t *m, GPIO_PinState dir)
{
  HAL_GPIO_WritePin(m->DIR_Port, m->DIR_Pin, dir);
}

static void Stepper_SetEn(StepperMotor_t *m, GPIO_PinState en_state)
{
  if (m->Has_EN && m->EN_Port != NULL)
  {
    HAL_GPIO_WritePin(m->EN_Port, m->EN_Pin, en_state);
  }
}

static void Stepper_Pulse(StepperMotor_t *m, uint32_t tim_us)
{
  HAL_GPIO_WritePin(m->PUL_Port, m->PUL_Pin, HIGH);
  delay_us(tim_us / 2);

  HAL_GPIO_WritePin(m->PUL_Port, m->PUL_Pin, LOW);
  delay_us(tim_us / 2);
}

static void Stepper_Init(StepperMotor_t *m)
{
  Stepper_SetEn(m, OFF);
  HAL_GPIO_WritePin(m->PUL_Port, m->PUL_Pin, LOW);
  Stepper_SetDir(m, CW);
}

static void Stepper_Turn(StepperMotor_t *m, uint32_t tim_us, float angle, float subdivide, GPIO_PinState dir)
{
  int n;
  int i;

  n = (int)(angle / (1.8f / subdivide));

  Stepper_SetDir(m, dir);
  HAL_Delay(5);

  Stepper_SetEn(m, ON);

  for (i = 0; i < n; i++)
  {
    Stepper_Pulse(m, tim_us);
  }

  Stepper_SetEn(m, OFF);
}

static uint8_t Sensor1_IsTriggered(void)
{
  return (HAL_GPIO_ReadPin(ROKO1_PORT, ROKO1_PIN) == GPIO_PIN_RESET);
}

static uint8_t Sensor2_IsTriggered(void)
{
  return (HAL_GPIO_ReadPin(ROKO2_PORT, ROKO2_PIN) == GPIO_PIN_RESET);
}

static void Sensor_LED_Update(void)
{
  if (Sensor1_IsTriggered())
  {
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, LED_ON);
  }
  else
  {
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, LED_OFF);
  }

  if (Sensor2_IsTriggered())
  {
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, LED_ON);
  }
  else
  {
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, LED_OFF);
  }
}

static void Relay_Set(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state)
{
  HAL_GPIO_WritePin(port, pin, state);
}

static void Relay_All_Off(void)
{
  Relay_Set(RELAY1_PORT, RELAY1_PIN, RELAY_OFF);
  Relay_Set(RELAY2_PORT, RELAY2_PIN, RELAY_OFF);
}

/*
   Run Motor 4 in the configured direction
   until Sensor 1 on PH2 is triggered.
*/
static void Stepper_Run_CW_Until_Sensor1(StepperMotor_t *m, uint32_t tim_us)
{
  Stepper_SetDir(m, CCW);
  HAL_Delay(5);

  Stepper_SetEn(m, ON);

  while (!Sensor1_IsTriggered())
  {
    Stepper_Pulse(m, tim_us);
    Sensor_LED_Update();
  }

  Stepper_SetEn(m, OFF);
}

/*
   Wait until Sensor 2 on PH3 is triggered.
*/
static void Wait_Until_Sensor2_Triggered(void)
{
  while (!Sensor2_IsTriggered())
  {
    Sensor_LED_Update();
    HAL_Delay(10);
  }
}

/*
   Servo 1 connection: J28 -> PD12 -> TIM4_CH1
*/
static void Servo1_Start(void)
{
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

  Servo1_Stop();
  HAL_Delay(300);
}

static void Servo1_SetPulse(uint16_t pulse_us)
{
  /*
     This board uses an inverting level-shifter for the servo signal.
     With TIM4 channel polarity configured correctly in CubeMX,
     write the desired pulse width directly to the compare register.
  */
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pulse_us);
}

static void Servo1_Stop(void)
{
  Servo1_SetPulse(SERVO1_STOP_US);
}

static void Servo1_CCW(void)
{
  Servo1_SetPulse(SERVO1_CCW_US);
}

static void Servo1_CW(void)
{
  Servo1_SetPulse(SERVO1_CW_US);
}

/*
   Action after Motor 5 finishes:
   rotate Servo 1 outward for the preset time, then stop it.
*/
static void Servo1_TurnOut_After_Motor5(void)
{
  Servo1_CW();
  HAL_Delay(SERVO1_ROTATE_TIME_MS);

  Servo1_Stop();
  HAL_Delay(300);
}

/*
   Final action of the sequence:
   rotate Servo 1 back to its original position, then stop it.
*/
static void Servo1_Return_At_End(void)
{
  Servo1_CCW();
  HAL_Delay(SERVO1_ROTATE_TIME_MS);

  Servo1_Stop();
  HAL_Delay(300);
}

/*
   Servo 2 connection: J29 -> PD13 -> TIM4_CH2
*/
static void Servo2_Start(void)
{
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

  Servo2_Stop();
  HAL_Delay(4000);
}

static void Servo2_SetPulse(uint16_t pulse_us)
{
  /*
     This board uses an inverting level-shifter for the servo signal.
     With TIM4 channel polarity configured correctly in CubeMX,
     write the desired pulse width directly to the compare register.
  */
  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pulse_us);
}

static void Servo2_Stop(void)
{
  Servo2_SetPulse(SERVO2_STOP_US);
}

static void Servo2_CCW(void)
{
  Servo2_SetPulse(SERVO2_CCW_US);
}

static void Servo2_CW(void)
{
  Servo2_SetPulse(SERVO2_CW_US);
}

/*
   Action after Motor 3 finishes:
   rotate Servo 2 in the CCW direction,
   hold the stopped state for 3 seconds,
   then rotate it back in the CW direction and stop.
*/
static void Servo2_Action_After_Motor3(void)
{
  Servo2_CCW();
  HAL_Delay(SERVO2_ROTATE_TIME_MS);

  Servo2_Stop();
  HAL_Delay(3000);

  Servo2_CW();
  HAL_Delay(SERVO2_ROTATE_TIME_MS);

  Servo2_Stop();
  HAL_Delay(300);
}

/*
   Execute one complete linkage sequence.
*/
static void Linkage_Run_Once(void)
{
  /* Reset all outputs to a safe initial state before starting the sequence. */
  Relay_All_Off();
  Servo1_Stop();
  Servo2_Stop();
  Sensor_LED_Update();
  HAL_Delay(100);

  /* 1. Rotate Motor 5 by the specified angle. */
  Stepper_Turn(&motor5, M5_STEP_US, 90.0f, STEP_SUBDIVIDE, CCW);
  HAL_Delay(1500);

	  /* 1. Rotate Motor 5 by the specified angle. */
  Stepper_Turn(&motor5, M5_STEP_US, 100.0f, STEP_SUBDIVIDE, CCW);
  HAL_Delay(1000);

	
  /* 2. After Motor 5 stops, rotate Servo 1 outward. */
  Servo1_TurnOut_After_Motor5();

  /* 3. Turn Relay 1 on and run Motor 4 until Sensor 1 is triggered. */
  Relay_Set(RELAY1_PORT, RELAY1_PIN, RELAY_ON);
  Stepper_Run_CW_Until_Sensor1(&motor4, M4_STEP_US);

  /*
     4. After Sensor 1 is triggered:
        stop Motor 4,
        turn Relay 1 off,
        and turn Relay 2 on.
  */
  Relay_Set(RELAY1_PORT, RELAY1_PIN, RELAY_OFF);
  Relay_Set(RELAY2_PORT, RELAY2_PIN, RELAY_ON);
  Sensor_LED_Update();
  HAL_Delay(2000);

  /* 5. Rotate Motor 6 by one revolution. */
  Stepper_Turn(&motor6, M6_STEP_US, 360.0f, STEP_SUBDIVIDE, CCW);
  HAL_Delay(2000);

  /* 6. Rotate Motor 1 clockwise by 90 degrees. */
  Stepper_Turn(&motor1, M1_STEP_US, 90.0f, STEP_SUBDIVIDE, CW);
  HAL_Delay(2000);

  /* 7. Rotate Motor 2 by one revolution. */
  Stepper_Turn(&motor2, M2_STEP_US, 360.0f, STEP_SUBDIVIDE, CCW);
  HAL_Delay(300);

  /* 8. Wait until Sensor 2 on PH3 is triggered. */
  Wait_Until_Sensor2_Triggered();

  /* 9. After Sensor 2 is triggered, turn Relay 2 off. */
  Relay_Set(RELAY2_PORT, RELAY2_PIN, RELAY_OFF);
  Sensor_LED_Update();
  HAL_Delay(300);

  /* 10. Rotate Motor 3 one revolution in the CCW direction. */
  Stepper_Turn(&motor3, M3_STEP_US, 200.0f, STEP_SUBDIVIDE, CW);
  HAL_Delay(300);

  /* 11. Rotate Motor 3 one revolution in the CW direction. */
  Stepper_Turn(&motor3, M3_STEP_US, 200.0f, STEP_SUBDIVIDE, CCW);
  HAL_Delay(300);

  /* 12. After Motor 3 finishes, run the Servo 2 forward-and-return action. */
  Servo2_Action_After_Motor3();

  /* 13. Rotate Motor 1 counterclockwise by 90 degrees. */
  Stepper_Turn(&motor1, M1_STEP_US, 90.0f, STEP_SUBDIVIDE, CCW);
  HAL_Delay(300);

  /* 14. At the end of the sequence, return Servo 1 to its original position. */
  Servo1_Return_At_End();

  /* Finish the sequence by switching off the relays and stopping both servos. */
  Relay_All_Off();
  Servo1_Stop();
  Servo2_Stop();
  Sensor_LED_Update();
}

/* USER CODE END 0 */

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM4_Init();

  /* USER CODE BEGIN 2 */

  Stepper_GPIO_Manual_Init();
  Sensor_LED_Manual_Init();
  delay_us_init();

  Stepper_Init(&motor1);
  Stepper_Init(&motor2);
  Stepper_Init(&motor3);
  Stepper_Init(&motor4);
  Stepper_Init(&motor5);
  Stepper_Init(&motor6);

  Relay_All_Off();

  Servo1_Start();
  Servo2_Start();

  HAL_Delay(1000);

  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Linkage_Run_Once();

    HAL_Delay(3000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();

  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}

#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/