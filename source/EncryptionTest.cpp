/**
 * @copyright (c) Calliope gGmbH.
 * @author Matthias L. Jugel.
 *
 * Licensed under the Apache Software License 2.0 (ASL 2.0)
 */

#include <MicroBit.h>

extern "C" {
#include <nrf_soc.h>
}
MicroBit uBit;


int main(void) {
    uBit.init();
    uBit.serial.baud(115200);
    uBit.serial.send("Calliope Encryption Test v1.0\r\n");

    auto *cleartext = const_cast<char *>("012345678901234567890");
    printf("clear length: %d\r\n", strlen(cleartext));

    nrf_ecb_hal_data_t ecb_hal_data = {};
    memset(&ecb_hal_data, 0, sizeof(ecb_hal_data));
    memcpy(ecb_hal_data.key, (const uint8_t *) &NRF_FICR->DEVICEID, 8);
    memcpy(ecb_hal_data.key + 8, (const uint8_t *) &NRF_FICR->DEVICEID, 8);

    printf("key length: %d\r\n", sizeof(ecb_hal_data.key));
    for (unsigned int i = 0; i < sizeof(ecb_hal_data.key); i++) {
        printf("%02x", ecb_hal_data.key[i]);
    }
    printf("\r\n");

    // you need to allocate 16 byte multiples of the buffer and pad
    // to ensure security also pad using not just zero, i.e. 0x80 0x00 ...
    StringData *buffer;
    buffer = (StringData *) malloc(4 + (strlen(cleartext) / 16 + 1) * 16 + 1);
    buffer->len = 0;
    buffer->init();

    for (unsigned int i = 0; i < strlen(cleartext); i += 16) {
        memset(ecb_hal_data.cleartext, 0, 16);
        memset(ecb_hal_data.ciphertext, 0, 16);

        strncpy((char *) ecb_hal_data.cleartext, cleartext + i, 16);
        if (sd_ecb_block_encrypt(&ecb_hal_data) == NRF_SUCCESS) {
            printf("%03d: OK (%d)\r\n", i, buffer->len);
            memcpy(buffer->data + i, ecb_hal_data.ciphertext, 16);
            buffer->len += 16;
        } else {
            printf("%03d: ERROR\r\n", i);
        }
    }

    printf("cipher length: %d\r\n", buffer->len);
    for (unsigned int i = 0; i < buffer->len; i++) {
        printf("%02x", buffer->data[i]);
    }
    printf("\r\n");

    uBit.serial.send("OK\r\n");

    while (1) {
        fiber_sleep(100);
    }
}