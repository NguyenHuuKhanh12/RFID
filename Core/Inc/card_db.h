#ifndef CARD_DB_H
#define CARD_DB_H

#include "stm32f1xx_hal.h" 
#include <stdint.h>
#include <stdbool.h>

#define MAX_CARDS           20       
#define UID_LENGTH          5        
#define FLASH_STORAGE_ADDR  0x0800FC00 

typedef enum {
    CARD_OK = 0,
    CARD_EXISTS,       
    CARD_NOT_FOUND,    
    CARD_FULL,         
    CARD_FLASH_ERROR   
} CardDB_Status_t;

typedef struct {
    uint8_t uid[UID_LENGTH];
    uint8_t is_valid;    
} RFID_Card_t;

void CardDB_Init(void);
CardDB_Status_t CardDB_FindCard(uint8_t *uid, int8_t *found_index);
CardDB_Status_t CardDB_AddCard(uint8_t *uid);
CardDB_Status_t CardDB_DeleteCard(uint8_t *uid);

#endif 