#include "stm32sunidMainSmall.h"
#include "VTimer.h"
#include "uart.h"
/*
 * This demo provides an example application using DESFire
 * to create an employee card that hold working hour infomation
 * and other access rules
 * the employee information will be store in Application with ID of 0xAB
 * the application will consist of 4 file
 * file ID 0: card type - len: 1 byte
 * file ID 1: Employee ID	- len: 10byte
 * file ID 2: Date Issue (4bytes) & Date Expire (4bytes) 
 * file ID 3: Working day (1byte) & Working hour (4byte)
 * file ID 4: 32 bytes for user function
 */

