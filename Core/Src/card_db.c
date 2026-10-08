#include "card_db.h"
#include <string.h>

RFID_Card_t card_db[MAX_CARDS];

static CardDB_Status_t Save_To_Flash(void) {
    HAL_FLASH_Unlock(); 
    
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = FLASH_STORAGE_ADDR;
    EraseInitStruct.NbPages = 1;
    
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return CARD_FLASH_ERROR;
    }
    
    uint32_t current_addr = FLASH_STORAGE_ADDR;
    uint16_t *ram_ptr = (uint16_t *)card_db;         
    uint32_t total_half_words = sizeof(card_db) / 2; 
    
    for (uint32_t i = 0; i < total_half_words; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, current_addr, ram_ptr[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return CARD_FLASH_ERROR;
        }
        current_addr += 2; 
    }
    
    HAL_FLASH_Lock(); 
    return CARD_OK;
}

void CardDB_Init(void) {
    RFID_Card_t *flash_data = (RFID_Card_t *)FLASH_STORAGE_ADDR;
    if (flash_data[0].uid[0] == 0xFF && flash_data[0].is_valid == 0xFF) {
        memset(card_db, 0, sizeof(card_db));
    } else {
        memcpy(card_db, flash_data, sizeof(card_db));
    }
}

CardDB_Status_t CardDB_FindCard(uint8_t *uid, int8_t *found_index) {
    for (int i = 0; i < MAX_CARDS; i++) {
        if (card_db[i].is_valid == 1) {
            if (memcmp(card_db[i].uid, uid, UID_LENGTH) == 0) {
                *found_index = i; 
                return CARD_OK;     
            }
        }
    }
    return CARD_NOT_FOUND; 
}

CardDB_Status_t CardDB_AddCard(uint8_t *uid) {
    int8_t dummy_index;
    if (CardDB_FindCard(uid, &dummy_index) == CARD_OK) {
        return CARD_EXISTS; 
    }
    for (int i = 0; i < MAX_CARDS; i++) {
        if (card_db[i].is_valid == 0) {
            memcpy(card_db[i].uid, uid, UID_LENGTH);
            card_db[i].is_valid = 1; 
            return Save_To_Flash(); 
        }
    }
    return CARD_FULL; 
}

CardDB_Status_t CardDB_DeleteCard(uint8_t *uid) {
    int8_t found_index;
    if (CardDB_FindCard(uid, &found_index) == CARD_OK) {
        card_db[found_index].is_valid = 0; 
        return Save_To_Flash();
    }
    return CARD_NOT_FOUND; 
}