/*
 * FSM.c
 *
 *  Created on: Nov 20, 2023
 *      Author: admin
 */
#include "timer.h"
#include "FSM.h"
#include "main.h"
#include <stdio.h>

extern ADC_HandleTypeDef hadc1;

extern UART_HandleTypeDef huart2;

uint8_t temp;
uint8_t idx = 0;
uint8_t Data[MAX_BUFFER_LENGTH];
uint8_t cmdError[] = "ERROR COMMAND!!!\r";

int ADC_value = 0;
extern int timer0_flag;

typedef enum COMMAND{
	START,
	R,
	S,
	T,
	O,
	K,
	END,
	INVALID,
	FREE
} command_t;

typedef enum PROCESS{
	GET_RST,
	ERROR_CMD,
	GET_DATA,
	WAIT_CONFORM
} process_t;

command_t CMD_state = FREE;
process_t PROC_state = GET_RST;

void command_fsm(){
	if (idx == MAX_BUFFER_LENGTH){
		HAL_UART_Transmit(&huart2, cmdError, sizeof(cmdError), 100);
		CMD_state = FREE;
		idx = 0;
		return;
	}
	switch (CMD_state)
	{
	case FREE:
		switch(temp)
		{
		case '!':
			CMD_state = START;
			Data[idx++] = temp;
			HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
			break;
		case '\r':
		case 8:
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
			break;
		}
		break;
	case START:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case 'R':
			CMD_state = R;
			Data[idx++] = temp;
			break;
		case 'O':
			CMD_state = O;
			Data[idx++] = temp;
			break;
		case 8:
			CMD_state = FREE;
			idx--;
			break;
		case '\r':
			CMD_state = FREE;
			idx = 0;
			PROC_state = ERROR_CMD;
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			break;
		}
		break;
	case R:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case 'S':
			CMD_state = S;
			Data[idx++] = temp;
			break;
		case 8:
			CMD_state = START;
			idx--;
			break;
		case '\r':
			CMD_state = FREE;
			idx = 0;
			PROC_state = ERROR_CMD;
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			break;
		}
		break;
	case S:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case 'T':
			CMD_state = T;
			Data[idx++] = temp;
			break;
		case 8:
			CMD_state = R;
			idx--;
			break;
		case '\r':
			CMD_state = FREE;
			idx = 0;
			PROC_state = ERROR_CMD;
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			break;
		}
		break;
	case T:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case '#':
			CMD_state = END;
			Data[idx++] = temp;
			break;
		case 8:
			CMD_state = S;
			idx--;
			break;
		case '\r':
			CMD_state = FREE;
			idx = 0;
			PROC_state = ERROR_CMD;
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			break;
		}
		break;
	case O:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case 'K':
			CMD_state = K;
			Data[idx++] = temp;
			break;
		case 8:
			CMD_state = START;
			idx--;
			break;
		case '\r':
			CMD_state = FREE;
			idx = 0;
			PROC_state = ERROR_CMD;
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			break;
		}
		break;
	case K:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case '#':
			CMD_state = END;
			Data[idx++] = temp;
			break;
		case 8:
			CMD_state = O;
			idx--;
			break;
		case '\r':
			CMD_state = FREE;
			idx = 0;
			PROC_state = ERROR_CMD;
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			break;
		}
		break;
	case END:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case '\r':
			CMD_state = FREE;
			idx = 0;
			if (Data[1] == 'O')
				if (ADC_value != 0){
					PROC_state = GET_RST;
					ADC_value = 0;
				}
				else {
					PROC_state = ERROR_CMD;
				}
			else
				PROC_state = GET_DATA;
			break;
		case 8:
			if (Data[1] == 'O')
				CMD_state = K;
			else
				CMD_state = T;
			idx --;
			break;
		default:
			CMD_state = INVALID;
			Data[idx++] = temp;
			break;
		}
		break;
	case INVALID:
		HAL_UART_Transmit(&huart2, &temp, sizeof(temp), 100);
		switch(temp)
		{
		case '\r':
			CMD_state = FREE;
			idx = 0;
			PROC_state = ERROR_CMD;
			break;
		case 8:
			idx--;
			if (idx == 5 &&
					Data[0] == '!' &&
					Data[1] == 'R' &&
					Data[2] == 'S' &&
					Data[3] == 'T' &&
					Data[4] == '#'){
				CMD_state = END;
			}
			else if (idx == 2 &&
					Data[0] == '!' &&
					Data[1] == 'R'){
				CMD_state = R;
			}
			else if (idx == 3 &&
					Data[0] == '!' &&
					Data[1] == 'R' &&
					Data[2] == 'S'){
				CMD_state = S;
			}
			else if (idx == 4 &&
					Data[0] == '!' &&
					Data[1] == 'R' &&
					Data[2] == 'S' &&
					Data[3] == 'T'){
				CMD_state = T;
			}
			if (idx == 4 &&
					Data[0] == '!' &&
					Data[1] == 'O' &&
					Data[2] == 'K' &&
					Data[3] == '#'){
				CMD_state = END;
			}
			else if (idx == 3 &&
					Data[0] == '!' &&
					Data[1] == 'O' &&
					Data[2] == 'K'){
				CMD_state = K;
			}
			else if (idx == 2 &&
					Data[0] == '!' &&
					Data[1] == 'O'){
				CMD_state = O;
			}
			else if (idx == 1 && Data[0] == '!'){
				CMD_state = START;
			}
			else if (idx == 0){
				CMD_state = FREE;
			}
			break;
		default:
			Data[idx++] = temp;
			break;
		}
		break;
	}
}

void proc_fsm(){
	switch(PROC_state)
	{
	case GET_DATA:
		ADC_value = HAL_ADC_GetValue(&hadc1);
		PROC_state = WAIT_CONFORM;
		setTimer0(10);
		break;
	case WAIT_CONFORM:
		if (timer0_flag == 1){
			if (idx != 0)
				HAL_UART_Transmit(&huart2, (uint8_t*)("\r"), 1, 100);
			HAL_UART_Transmit(&huart2, Data, sprintf((char*)Data, "!ADC=%d#\r", ADC_value), 3000);
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			idx = 0;
			CMD_state = FREE;
			setTimer0(3000);
		}
		break;
	case GET_RST:
		break;
	case ERROR_CMD:
		if (ADC_value == 0)
			PROC_state = GET_RST;
		else
			PROC_state = WAIT_CONFORM;
		HAL_UART_Transmit(&huart2, cmdError, sizeof(cmdError), 100);
		break;
	}
}

