#include "ring.h"
#include <stdlib.h>
#include <search.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

// Write your ring buffer functions here

struct RingBuffer{
    size_t at; // The index of the next byte to read
    size_t size; // The total number of unread bytes in the ring buffer
    size_t capacity; // total number of bytes that can be stored in this ring buffer
    char * buffer; // The actual data storage. Must be created with Calloc using capacity as num of elements to create
};

// Memory Management Functions
RingBuffer *rb_new(size_t capacity) {
    // create rb object
    RingBuffer *rb = malloc(sizeof *rb);
    if(!rb){
        printf("Failed to allocate the ring buff struct\n");
        return NULL;
    }
    rb->at = 0;
    rb->size = 0;
    rb->capacity = capacity;

    // allocates the buffer to the number of bytes of capacity with sizeof the buffer
    rb->buffer = calloc(capacity, sizeof(*rb->buffer));
    if(!rb->buffer){
        printf("Bad memory allocation\n");
        free(rb);
        return NULL;
    }
    return rb;
}

void rb_free(RingBuffer *rb) {
    // free memory
    if(!rb){
        return;
    }
    free(rb->buffer);
    free(rb);
}

// Ring Buffer Values
size_t rb_at(const RingBuffer *rb){
    // return next byte to be read.
    // if we just want the index of the next byte to read and not the value
    return rb->at;
}

size_t rb_size(const RingBuffer *rb){
    // return readable memory. This can be done by the total size allocated minus the actual number of bytes stored.
    return rb->size;
}

size_t rb_capacity(const RingBuffer *rb) {
    // return number of bytes that the ring buffer can store
    // size_t is always positive
    return rb->capacity;
}

// Individual item functions
bool rb_push (RingBuffer *rb, char data) {
    if (rb->size == rb->capacity){
        printf("No room in the buffer to push data to.\n");
        return false;
    }
    size_t index = (rb->at + rb->size) % rb->capacity;
    // modulus will wrap the buffer like a circle and get us an index in
    rb->buffer[index] = data;
    rb->size++;
    return true;
}

bool rb_pop(RingBuffer *rb, char *data) {
    if (rb->size == 0){
        return false;
    }

    char value = rb->buffer[rb->at];

    if(data != NULL){
        *data = value;
    }

   rb->at = (rb->at + 1) % rb->capacity;
   rb->size--;
   return true;
}

char rb_peek(const RingBuffer *rb) {
    // returns not the index of the next byte but the value at the point in the buffer
    if (rb->size == 0){
        printf("nothing in the buffer\n");
        return -1;
    }
    return rb->buffer[rb->at];
}

void rb_ignore(RingBuffer *rb, size_t num) {
    if(num == 0){
        return;
    }

    if(num >= rb->size){
        rb->at = (rb->at + rb->size) % rb->capacity;
        rb->size = 0;
        return;
    }
    rb->at = (rb->at + num) % rb->capacity;
    rb->size = rb->size - num;
}

// Buffer Functions
size_t rb_read(RingBuffer *rb, char *buf, size_t max_bytes) {
    // using memcopy to read,
    size_t index = rb->at;
    size_t bytes, bytes_read;
    if (max_bytes < rb->size) {
        bytes = max_bytes;
    } else{
        bytes = rb->size;
    }

    if(bytes < rb->capacity - index){
        bytes_read = bytes;
    } else {
        bytes_read = rb->capacity - index;
    }


    memcpy(buf, rb->buffer + index, bytes_read);

    size_t bytes_read2 = bytes - bytes_read;
    if(bytes_read2 > 0){
        memcpy(buf + bytes_read, rb->buffer, bytes_read2);
    }

    rb->size -= bytes;
    rb->at = (rb->at + bytes) % rb->capacity;
    return bytes;
}

size_t rb_write(RingBuffer *rb, const char *buf, size_t bytes) {
    size_t free_space = rb->capacity - rb->size;

    size_t write_bytes;
    if (bytes < free_space) {
        write_bytes = bytes;
    } else {
        write_bytes = free_space;
    }

    size_t index = (rb->at + rb->size) % rb->capacity;

    size_t first_chunk;
    if (write_bytes < rb->capacity - index) {
        first_chunk = write_bytes;
    } else {
        first_chunk = rb->capacity - index;
    }

    memcpy(rb->buffer + index, buf, first_chunk);

    size_t second_chunk = write_bytes - first_chunk;
    if (second_chunk > 0) {
        memcpy(rb->buffer, buf + first_chunk, second_chunk);
    }

    rb->size += write_bytes;
    return write_bytes;
}


void rb_clear(RingBuffer *rb){
    // keep the buffer contents the same, but set the size and capacity to represent no elements
    rb->at = 0;
    rb->size = 0;
}
