#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "queue.h"
#include "network.h"
#include "rtp.h"

/**
 * PLESE ENTER YOUR INFORMATION BELOW TO RECEIVE MANUAL GRADING CREDITS
 * Name: Vy Mai
 * GTID: 903681630
 * Fall 2023
 */

typedef struct message {
    char *buffer;
    int length;
} message_t;

/* ================================================================ */
/*                  H E L P E R    F U N C T I O N S                */
/* ================================================================ */

/**
 * --------------------------------- PROBLEM 1 --------------------------------------
 * 
 * Convert the given buffer into an array of PACKETs and returns the array.  The 
 * value of (*count) should be updated so that it contains the number of packets in 
 * the array
 * 
 * @param buffer pointer to message buffer to be broken up packets
 * @param length length of the message buffer.
 * @param count number of packets in the returning array
 * 
 * @returns array of packets
 */
packet_t *packetize(char *buffer, int length, int *count) {

    packet_t *packets;

    /* ----  FIXME  ---- */
    short even_packet_size = (length % MAX_PAYLOAD_LENGTH == 0);
    int last_packet_length = even_packet_size ? MAX_PAYLOAD_LENGTH : length % MAX_PAYLOAD_LENGTH;
    /*
        even_packet_size == 0 --> length is not a multiple of MAX_PAYLOAD_LENGTH
        even_packet_size == 1 --> length is a multiple of MAX_PAYLOAD_LENGTH
    */
    length = length / MAX_PAYLOAD_LENGTH + (even_packet_size ? 0 : 1);
    packets = (packet_t*) malloc(sizeof(packet_t) * ((size_t) length));
    int i = 0;
    while(i < length - 1){
        packets[i].type = DATA;
        packets[i].checksum = checksum(buffer, MAX_PAYLOAD_LENGTH);
        packets[i].payload_length = MAX_PAYLOAD_LENGTH;
        memcpy(packets[i].payload, buffer, MAX_PAYLOAD_LENGTH);
        buffer += MAX_PAYLOAD_LENGTH;
        i++;
    }
    packets[i].type = LAST_DATA;
    packets[i].checksum = checksum(buffer,last_packet_length);
    packets[i].payload_length =  last_packet_length;
    memcpy(packets[i].payload, buffer, last_packet_length);
    *count = length;
    return packets;
}

/**
 * --------------------------------- PROBLEM 2 --------------------------------------
 * 
 * Compute a checksum based on the data in the buffer.
  * 
 * Checksum calcuation: For the first two ASCII characters of the buffer, 
 * multiply each of their values by three and add them together. Then for the next 
 * two characters, divide each of their values by three and add them to the total. 
 * Repeat this for each group of two characters until the buffer is empty.
 * 
 * Example: "abcd" checksum = (a * 3) + (b * 3) + (c / 3) + (d / 3)
 * 
 * @param buffer pointer to the char buffer that the checksum is calculated from
 * @param length length of the buffer
 * 
 * @returns calcuated checksum
 */
int checksum(char *buffer, int length) {

    /* ----  FIXME  ---- */
    int sum = 0;
    int i = 0;
    while(i < length - 1){
        sum += buffer[i] * 3 + buffer[i + 1] * 3;
        sum += buffer[i + 2] / 3 + buffer[i + 3] / 3;
        i += 4;
    }
    return sum;
}


/* ================================================================ */
/*                      R T P       T H R E A D S                   */
/* ================================================================ */

static void *rtp_recv_thread(void *void_ptr) {

    rtp_connection_t *connection = (rtp_connection_t *) void_ptr;

    do {
        message_t *message;
        int buffer_length = 0;
        char *buffer = NULL;
        packet_t packet;

        /* Put messages in buffer until the last packet is received  */
        do {
            if (net_recv_packet(connection->net_connection_handle, &packet) <= 0 || packet.type == TERM) {
                /* remote side has disconnected */
                connection->alive = 0;
                pthread_cond_signal(&connection->recv_cond);
                pthread_cond_signal(&connection->send_cond);
                break;
            }

            /*  ----  FIXME  ----
            *
            * 1. check to make sure payload of packet is correct
            * 2. send an ACK or a NACK, whichever is appropriate
            * 3. if this is the last packet in a sequence of packets
            *    and the payload was corrupted, make sure the loop
            *    does not terminate
            * 4. if the payload matches, add the payload to the buffer
            */

           if (DATA == packet.type || LAST_DATA == packet.type){
                packet_t * return_packet;
                if (NULL == (return_packet = malloc(sizeof(packet_t)))){
                    exit(EXIT_FAILURE);
                }
                // Ensure we don't end early if the last packet is corrupted
                if (checksum(packet.payload, packet.payload_length) != packet.checksum) {
                    if (LAST_DATA == packet.type){
                        packet.type = DATA;
                    }
                    return_packet->type = NACK;
                    return_packet->checksum = 0;
                    return_packet->payload_length = 0; // are these necessary
                }
                else {
                    return_packet->type = ACK;
                    return_packet->checksum = 0;
                    return_packet->payload_length = 0;
                    buffer_length += packet.payload_length;
                    char * temp_buffer = buffer;
                    if (NULL == (buffer = realloc(buffer, buffer_length))){
                        // wtf am i supposed to do here
                        buffer = temp_buffer;
                        free(return_packet);
                        // free(buffer);
                        exit(EXIT_FAILURE);
                    }
                    temp_buffer = NULL;
                    for (int i = 0; i < packet.payload_length; i++){
                        buffer[buffer_length - packet.payload_length + i] = packet.payload[i];
                    }
                    // memcpy(buffer + buffer_length - packet.payload_length, packet.payload, packet.payload_length);
                }
                net_send_packet(connection->net_connection_handle, return_packet);
                free(return_packet);
           }
            /*
            *  What if the packet received is not a data packet?
            *  If it is a NACK or an ACK, the sending thread should
            *  be notified so that it can finish sending the message.
            *   
            *  1. add the necessary fields to the CONNECTION data structure
            *     in rtp.h so that the sending thread has a way to determine
            *     whether a NACK or an ACK was received
            *  2. signal the sending thread that an ACK or a NACK has been
            *     received.
            */
           else if (ACK == packet.type || NACK == packet.type){
                pthread_mutex_lock(&connection->recv_mutex);
                connection->ack = (packet.type == ACK) ? 1 : 0;
                pthread_cond_signal(&connection->ack_cond);
                pthread_mutex_unlock(&connection->recv_mutex);
           }


        } while (packet.type != LAST_DATA);

        if (connection->alive == 1) {
            /*  ----  FIXME: Part II-C ----
            *
            * Now that an entire message has been received, we need to
            * add it to the queue to provide to the rtp client.
            *
            * 1. Add message to the received queue.
            * 2. Signal the client thread that a message has been received.
            */


        } else{
            free(buffer);
        }

    } while (connection->alive == 1);

    return NULL;

}

static void *rtp_send_thread(void *void_ptr) {

    rtp_connection_t *connection = (rtp_connection_t *) void_ptr;
    message_t *message;
    int array_length = 0;
    int i;
    packet_t *packet_array;

    do {
        /* Extract the next message from the send queue */
        pthread_mutex_lock(&connection->send_mutex);
        while (queue_size(&connection->send_queue) == 0 && connection->alive == 1) {
            pthread_cond_wait(&connection->send_cond, &connection->send_mutex);
        }

        if (connection->alive == 0) {
            pthread_mutex_lock(&connection->send_mutex);
            break;
        }

        message = queue_extract(&connection->send_queue);

        pthread_mutex_unlock(&connection->send_mutex);

        /* Packetize the message and send it */
        packet_array = packetize(message->buffer, message->length, &array_length);

        for (i = 0; i < array_length; i++) {

            /* Start sending the packetized messages */
            if (net_send_packet(connection->net_connection_handle, &packet_array[i]) <= 0) {
                /* remote side has disconnected */
                connection->alive = 0;
                break;
            }

            /*  ----FIX ME: Part II-D ---- 
             * 
             *  1. wait for the recv thread to notify you of when a NACK or
             *     an ACK has been received
             *  2. check the data structure for this connection to determine
             *     if an ACK or NACK was received.  (You'll have to add the
             *     necessary fields yourself)
             *  3. If it was an ACK, continue sending the packets.
             *  4. If it was a NACK, resend the last packet
             */


        }

        free(packet_array);
        free(message->buffer);
        free(message);
    } while (connection->alive == 1);
    return NULL;
}

/* ================================================================ */
/*                           R T P    A P I                         */
/* ================================================================ */

static rtp_connection_t *rtp_init_connection(int net_connection_handle) {
    rtp_connection_t *rtp_connection = malloc(sizeof(rtp_connection_t));

    if (rtp_connection == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(EXIT_FAILURE);
    }

    rtp_connection->net_connection_handle = net_connection_handle;

    queue_init(&rtp_connection->recv_queue);
    queue_init(&rtp_connection->send_queue);

    pthread_mutex_init(&rtp_connection->ack_mutex, NULL);
    pthread_mutex_init(&rtp_connection->recv_mutex, NULL);
    pthread_mutex_init(&rtp_connection->send_mutex, NULL);
    pthread_cond_init(&rtp_connection->ack_cond, NULL);
    pthread_cond_init(&rtp_connection->recv_cond, NULL);
    pthread_cond_init(&rtp_connection->send_cond, NULL);

    rtp_connection->alive = 1;

    pthread_create(&rtp_connection->recv_thread, NULL, rtp_recv_thread,
                   (void *) rtp_connection);
    pthread_create(&rtp_connection->send_thread, NULL, rtp_send_thread,
                   (void *) rtp_connection);

    return rtp_connection;
}

rtp_connection_t *rtp_connect(char *host, int port) {

    int net_connection_handle;

    if ((net_connection_handle = net_connect(host, port)) < 1)
        return NULL;

    return (rtp_init_connection(net_connection_handle));
}

int rtp_disconnect(rtp_connection_t *connection) {

    message_t *message;
    packet_t term;

    term.type = TERM;
    term.payload_length = term.checksum = 0;
    net_send_packet(connection->net_connection_handle, &term);
    connection->alive = 0;

    net_disconnect(connection->net_connection_handle);
    pthread_cond_signal(&connection->send_cond);
    pthread_cond_signal(&connection->recv_cond);
    pthread_join(connection->send_thread, NULL);
    pthread_join(connection->recv_thread, NULL);
    net_release(connection->net_connection_handle);

    /* emtpy recv queue and free allocated memory */
    while ((message = queue_extract(&connection->recv_queue)) != NULL) {
        free(message->buffer);
        free(message);
    }
    queue_release(&connection->recv_queue);

    /* emtpy send queue and free allocated memory */
    while ((message = queue_extract(&connection->send_queue)) != NULL) {
        free(message);
    }
    queue_release(&connection->send_queue);

    free(connection);

    return 1;

}

int rtp_recv_message(rtp_connection_t *connection, char **buffer, int *length) {

    message_t *message;

    if (connection->alive == 0)
        return -1;
    /* lock */
    pthread_mutex_lock(&connection->recv_mutex);
    while (queue_size(&connection->recv_queue) == 0 && connection->alive == 1) {
        pthread_cond_wait(&connection->recv_cond, &connection->recv_mutex);
    }

    if (connection->alive == 0) {
        pthread_mutex_unlock(&connection->recv_mutex);
        return -1;
    }

    /* extract */
    message = queue_extract(&connection->recv_queue);
    *buffer = message->buffer;
    *length = message->length;
    free(message);

    /* unlock */
    pthread_mutex_unlock(&connection->recv_mutex);

    return *length;
}

int rtp_send_message(rtp_connection_t *connection, char *buffer, int length) {

    message_t *message;

    if (connection->alive == 0)
        return -1;

    message = malloc(sizeof(message_t));
    if (message == NULL) {
        return -1;
    }
    message->buffer = malloc((size_t) length);
    message->length = length;

    if (message->buffer == NULL) {
        free(message);
        return -1;
    }

    memcpy(message->buffer, buffer, (size_t) length);

    /* lock */
    pthread_mutex_lock(&connection->send_mutex);

    /* add */
    queue_add(&(connection->send_queue), message);

    /* unlock */
    pthread_mutex_unlock(&connection->send_mutex);
    pthread_cond_signal(&connection->send_cond);
    return 1;

}
