#include "Functions.h"


void check_GPS_sync_req(queue_t *recv_queue, queue_t *send_queue, struct parsed_nmea *nmea_ptr){
    if (!queue_is_empty(recv_queue)) 
    {
        uint8_t cmd;
        queue_remove_blocking(recv_queue, &cmd); //read the item in the queue
        if (cmd == GPS_SYNC_REQ)
        {
            if (GPS_sync(nmea_ptr, true)) exit(EXIT_FAILURE);
            cmd = GPS_SYNC_ACK;
            if (queue_try_add(send_queue, &cmd));
        }
    }
    return;
}