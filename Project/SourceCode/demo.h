#include "stm32sunid.h"
#include "VTimer.h"
#include "pn532.h"
#include "pn532_drvr.h"
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
#define APP_ID			0x332211
#define FTYPE_ID		0x0
#define FEMPID_ID		0x1
#define FDATE_ID		0x2
#define FWORKING_ID	0x3
#define FUSER_ID		0x1
#define FVALUE_ID		0x2

#define FTYPE_SZ		1
#define FEMPID_SZ		10
#define FDATE_SZ		8
#define FWORKING_SZ	5
#define FUSER_SZ		32
//Define card type
#define	ADMIN_CARD			0x00
#define	HEADDEP_CARD		0x01
#define EMPLOYEE_CARD		0x02
#define GUEST_CARD			0x03
//Define working day
#define	MONDAY		0X01
#define	TUESDAY		0X02
#define	WEDNESDAY	0X04
#define	THURSDAY	0X08
#define	FRIDAY		0X10
#define	SATURDAY	0X20
#define	SUNDAY		0X40
#define	REGULAR		0X1F

//struct hold all data
typedef struct
{
	uint8_t	type;
	uint8_t empID[FEMPID_SZ];
	uint8_t date[FDATE_SZ];
	uint8_t working[FWORKING_SZ];
	uint8_t user[FUSER_SZ];
	int32_t	value;
}EmployeeCard_st;
typedef enum DemoErrorCode_e 
{
	DER_NONE 		= 0x0,
	DER_NOCARD  = 0x1,
	DER_CARD		= 0x2,
	DER_READER	= 0x3,
	DER_INVPASS	= 0x4,
	DER_VERIFY	= 0x5,
}DemoErrorCode_t;
DemoErrorCode_t StartReader(void);
DemoErrorCode_t RestoreToDefaultCard(void);
DemoErrorCode_t FormatNewCard(void);
DemoErrorCode_t FormatOldCard(void);
DemoErrorCode_t CreateCard(const EmployeeCard_st * detail);
DemoErrorCode_t WriteType(uint8_t type);
DemoErrorCode_t WriteEmpID(const uint8_t * empID);
DemoErrorCode_t WriteDate(const uint8_t * date);
DemoErrorCode_t WriteWorking(uint8_t day, uint16_t start, uint16_t end);
DemoErrorCode_t WriteUser(const uint8_t * userData);
DemoErrorCode_t CreditValue(const int32_t data);
DemoErrorCode_t DebitValue(const int32_t data);
DemoErrorCode_t ReadCard(EmployeeCard_st * detail);
void DemoTest(void);
void DateToByteArray(uint32_t dateIssue,uint32_t dateExpire,uint8_t * buf);
void WorkingToByteArray(uint8_t day,uint16_t start, uint16_t end,uint8_t * buf);
void InitEmployee(EmployeeCard_st * detail,uint8_t type,uint8_t * empID, 
									uint32_t dateIssue,uint32_t dateExpire,
									uint8_t day,uint32_t start, uint16_t end, uint8_t * user, uint32_t value);
