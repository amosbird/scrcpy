#include "control_event.h"

#include <SDL2/SDL_stdinc.h>
#include <string.h>

#include "buffer_util.h"
#include "lock_util.h"
#include "log.h"

static void write_position(Uint8 *buf, const struct position *position) {
    buffer_write16be(&buf[0], position->point.x);
    buffer_write16be(&buf[2], position->point.y);
    buffer_write16be(&buf[4], position->screen_size.width);
    buffer_write16be(&buf[6], position->screen_size.height);
}

int control_event_serialize(const struct control_event *event, unsigned char *buf) {
    buf[0] = event->type;
    switch (event->type) {
        case CONTROL_EVENT_TYPE_KEYCODE:
            buf[1] = event->keycode_event.action;
            buffer_write32be(&buf[2], event->keycode_event.keycode);
            buffer_write32be(&buf[6], event->keycode_event.metastate);
            return 10;
        case CONTROL_EVENT_TYPE_TEXT: {
            // write length (1 byte) + date (non nul-terminated)
            size_t len = strlen(event->text_event.text);
            if (len > TEXT_MAX_LENGTH) {
                // injecting a text takes time, so limit the text length
                len = TEXT_MAX_LENGTH;
            }
            buffer_write16be(&buf[1], (Uint16) len);
            memcpy(&buf[3], event->text_event.text, len);
            return 3 + len;
        }
        case CONTROL_EVENT_TYPE_MOUSE:
            buf[1] = event->mouse_event.action;
            buffer_write32be(&buf[2], event->mouse_event.buttons);
            write_position(&buf[6], &event->mouse_event.position);
            return 14;
        case CONTROL_EVENT_TYPE_SCROLL:
            write_position(&buf[1], &event->scroll_event.position);
            buffer_write32be(&buf[9], (Uint32) event->scroll_event.hscroll);
            buffer_write32be(&buf[13], (Uint32) event->scroll_event.vscroll);
            return 17;
        case CONTROL_EVENT_TYPE_COMMAND:
            buf[1] = event->command_event.action;
            return 2;
        default:
            LOGW("Unknown event type: %u", (unsigned) event->type);
            return 0;
    }
}

void control_event_destroy(struct control_event *event) {
    if (event->type == CONTROL_EVENT_TYPE_TEXT) {
        SDL_free(event->text_event.text);
    }
    if (event->type == CONTROL_EVENT_TYPE_SWIPE) {
        SDL_free(event->swipe_event.events);
    }
}

SDL_bool control_event_queue_is_empty(const struct control_event_queue *queue) {
    return queue->head == queue->tail;
}

SDL_bool control_event_queue_is_full(const struct control_event_queue *queue) {
    return (queue->head + 1) % CONTROL_EVENT_QUEUE_SIZE == queue->tail;
}

SDL_bool control_event_queue_init(struct control_event_queue *queue) {
    queue->head = 0;
    queue->tail = 0;
    // the current implementation may not fail
    return SDL_TRUE;
}

void control_event_queue_destroy(struct control_event_queue *queue) {
    int i = queue->tail;
    while (i != queue->head) {
        control_event_destroy(&queue->data[i]);
        i = (i + 1) % CONTROL_EVENT_QUEUE_SIZE;
    }
}

SDL_bool control_event_queue_push(struct control_event_queue *queue, const struct control_event *event) {
    if (control_event_queue_is_full(queue)) {
        return SDL_FALSE;
    }
    queue->data[queue->head] = *event;
    queue->head = (queue->head + 1) % CONTROL_EVENT_QUEUE_SIZE;
    return SDL_TRUE;
}

SDL_bool control_event_queue_take(struct control_event_queue *queue, struct control_event *event) {
    if (control_event_queue_is_empty(queue)) {
        return SDL_FALSE;
    }
    *event = queue->data[queue->tail];
    queue->tail = (queue->tail + 1) % CONTROL_EVENT_QUEUE_SIZE;
    return SDL_TRUE;
}
