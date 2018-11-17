#define _DEFAULT_SOURCE
#include "scrcpy.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libavformat/avformat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <pthread.h>
#include <SDL2/SDL.h>

#include "command.h"
#include "common.h"
#include "controller.h"
#include "decoder.h"
#include "device.h"
#include "events.h"
#include "file_handler.h"
#include "frames.h"
#include "fps_counter.h"
#include "input_manager.h"
#include "log.h"
#include "lock_util.h"
#include "net.h"
#include "recorder.h"
#include "screen.h"
#include "server.h"
#include "tiny_xpm.h"

volatile int quited = 0;

static struct server server = SERVER_INITIALIZER;
static struct screen screen = SCREEN_INITIALIZER;
static struct frames frames;
static struct decoder decoder;
static struct controller controller;
static struct file_handler file_handler;
static struct recorder recorder;

static struct input_manager input_manager = {
    .controller = &controller,
    .frames = &frames,
    .screen = &screen,
    .server = &server,
};

#if defined(__APPLE__) || defined(__WINDOWS__)
# define CONTINUOUS_RESIZING_WORKAROUND
#endif

#ifdef CONTINUOUS_RESIZING_WORKAROUND
// On Windows and MacOS, resizing blocks the event loop, so resizing events are
// not triggered. As a workaround, handle them in an event handler.
//
// <https://bugzilla.libsdl.org/show_bug.cgi?id=2077>
// <https://stackoverflow.com/a/40693139/1987178>
static int event_watcher(void *data, SDL_Event *event) {
    if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
        // called from another thread, not very safe, but it's a workaround!
        screen_render(&screen);
    }
    return 0;
}
#endif

static SDL_bool is_apk(const char *file) {
    const char *ext = strrchr(file, '.');
    return ext && !strcmp(ext, ".apk");
}

// turn the screen on if it was off, press BACK otherwise
static void encoder_control(struct controller *controller, int suspend) {
    struct control_event control_event;
    control_event.type = CONTROL_EVENT_TYPE_COMMAND;
    control_event.command_event.action = suspend ? CONTROL_EVENT_SUSPEND_ENCODER : CONTROL_EVENT_RESUME_ENCODER;

    if (!controller_push_event(controller, &control_event)) {
        LOGW("Cannot control encoder");
    }
}

static SDL_bool event_loop(void) {
#ifdef CONTINUOUS_RESIZING_WORKAROUND
    SDL_AddEventWatch(event_watcher, NULL);
#endif
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        switch (event.type) {
            case EVENT_DECODER_STOPPED:
                LOGD("Video decoder stopped");
                return SDL_FALSE;
            case SDL_QUIT:
                LOGD("User requested to quit");
                quited = 1;
                return SDL_TRUE;
            case EVENT_NEW_FRAME:
                if (!screen.has_frame) {
                    screen.has_frame = SDL_TRUE;
                    // this is the very first frame, show the window
                    screen_show_window(&screen);
                }
                if (!screen_update_frame(&screen, &frames)) {
                    return SDL_FALSE;
                }
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        input_manager_enable_modifiers(event.window.timestamp);
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        screen_render(&screen);
                        break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        encoder_control(input_manager.controller, 1);
                        break;
                    case SDL_WINDOWEVENT_SHOWN:
                        encoder_control(input_manager.controller, 0);
                        break;
                }
                break;
            case SDL_TEXTINPUT:
                input_manager_process_text_input(&input_manager, &event.text);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                input_manager_process_key(&input_manager, &event.key);
                break;
            case SDL_MOUSEMOTION:
                input_manager_process_mouse_motion(&input_manager, &event.motion);
                break;
            case SDL_MOUSEWHEEL:
                input_manager_process_mouse_wheel(&input_manager, &event.wheel);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                input_manager_process_mouse_button(&input_manager, &event.button);
                break;
            case SDL_DROPFILE: {
                file_handler_action_t action;
                if (is_apk(event.drop.file)) {
                    action = ACTION_INSTALL_APK;
                } else {
                    action = ACTION_PUSH_FILE;
                }
                file_handler_request(&file_handler, action, event.drop.file);
                break;
            }
        }
    }
    return SDL_FALSE;
}

static process_t set_show_touches_enabled(const char *serial, SDL_bool enabled) {
    const char *value = enabled ? "1" : "0";
    const char *const adb_cmd[] = {
        "shell", "settings", "put", "system", "show_touches", value
    };
    return adb_execute(serial, adb_cmd, ARRAY_LEN(adb_cmd));
}

static void wait_show_touches(process_t process) {
    // reap the process, ignore the result
    process_check_success(process, "show_touches");
}

#define SOCK_PATH "/tmp/scrcpy.socket"

static void handle(char c) {
    SDL_Event sdlevent;
    sdlevent.type = SDL_KEYUP;
    switch (c) {
        case 'z':
            encoder_control(input_manager.controller, 0);
            break;
        case 'm':
            sdlevent.key.keysym.sym = SDLK_MUTE;
            SDL_PushEvent(&sdlevent);
            break;
        case 'u':
            sdlevent.key.keysym.sym = SDLK_VOLUMEUP;
            SDL_PushEvent(&sdlevent);
            break;
        case 'd':
            sdlevent.key.keysym.sym = SDLK_VOLUMEDOWN;
            SDL_PushEvent(&sdlevent);
            break;
        case 's':
            sdlevent.key.keysym.sym = SDLK_AUDIOPLAY;
            SDL_PushEvent(&sdlevent);
            break;
        case 'p':
            sdlevent.key.keysym.sym = SDLK_AUDIOPREV;
            SDL_PushEvent(&sdlevent);
            break;
        case 'n':
            sdlevent.key.keysym.sym = SDLK_AUDIONEXT;
            SDL_PushEvent(&sdlevent);
            break;
        case 'c':
            // TODO
            sdlevent.key.keysym.sym = SDLK_5;
            SDL_PushEvent(&sdlevent);
            break;
    }
}

int s;
static void* amos_handler(void* arg) {
    int s2;
    unsigned int t, len;
    struct sockaddr_un local, remote;
    char str[100];

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr*)&local, len) == -1) {
        perror("bind failed");
        exit(1);
    }

    if (listen(s, 5) == -1) {
        perror("listen failed");
        exit(1);
    }

    for (;;) {
        t = sizeof(remote);
        if ((s2 = accept(s, (struct sockaddr*)&remote, &t)) == -1) {
            perror("accept failed");
            break;
        }
        int n = recv(s2, str, 100, 0);
        if (n > 0)
            handle(str[0]);
        close(s2);
    }

    pthread_exit(NULL);
}


SDL_bool scrcpy(const struct scrcpy_options *options) {
    SDL_bool send_frame_meta = !!options->record_filename;
    if (!server_start(&server, options->serial, options->port,
                      options->max_size, options->bit_rate, options->crop,
                      send_frame_meta)) {
        return SDL_FALSE;
    }

    process_t proc_show_touches = PROCESS_NONE;
    SDL_bool show_touches_waited;
    if (options->show_touches) {
        LOGI("Enable show_touches");
        proc_show_touches = set_show_touches_enabled(options->serial, SDL_TRUE);
        show_touches_waited = SDL_FALSE;
    }

    SDL_bool ret = SDL_TRUE;

    if (!sdl_init_and_configure()) {
        ret = SDL_FALSE;
        goto finally_destroy_server;
    }

    socket_t device_socket = server_connect_to(&server);
    if (device_socket == INVALID_SOCKET) {
        server_stop(&server);
        ret = SDL_FALSE;
        goto finally_destroy_server;
    }

    char device_name[DEVICE_NAME_FIELD_LENGTH];
    struct size frame_size;

    // screenrecord does not send frames when the screen content does not change
    // therefore, we transmit the screen size before the video stream, to be able
    // to init the window immediately
    if (!device_read_info(device_socket, device_name, &frame_size)) {
        server_stop(&server);
        ret = SDL_FALSE;
        goto finally_destroy_server;
    }

    if (!frames_init(&frames)) {
        server_stop(&server);
        ret = SDL_FALSE;
        goto finally_destroy_server;
    }

    if (!file_handler_init(&file_handler, server.serial)) {
        ret = SDL_FALSE;
        server_stop(&server);
        goto finally_destroy_frames;
    }

    struct recorder *rec = NULL;
    if (options->record_filename) {
        if (!recorder_init(&recorder, options->record_filename, frame_size)) {
            ret = SDL_FALSE;
            server_stop(&server);
            goto finally_destroy_file_handler;
        }
        rec = &recorder;
    }

    decoder_init(&decoder, &frames, &screen, device_socket, rec);

    // now we consumed the header values, the socket receives the video stream
    // start the decoder
    if (!decoder_start(&decoder)) {
        ret = SDL_FALSE;
        server_stop(&server);
        goto finally_destroy_recorder;
    }

    if (!controller_init(&controller, device_socket)) {
        ret = SDL_FALSE;
        goto finally_stop_decoder;
    }

    if (!controller_start(&controller)) {
        ret = SDL_FALSE;
        goto finally_destroy_controller;
    }

    if (!screen_init_rendering(&screen, device_name, frame_size)) {
        ret = SDL_FALSE;
        goto finally_stop_and_join_controller;
    }

    pthread_t handler;
    {
        if (pthread_create(&handler, NULL, amos_handler, NULL)) {
            perror("pthread_create");
            exit(1);
        }
    }

    if (options->show_touches) {
        wait_show_touches(proc_show_touches);
        show_touches_waited = SDL_TRUE;
    }

    if (options->fullscreen) {
        screen_switch_fullscreen(&screen);
    }

    libusb_init(NULL);
    libusb_device* device = find_device(options->vid, options->pid);
    if (!device) {
        fprintf(stderr, "Device %04x:%04x not found\n", options->vid, options->pid);
        exit(1);
    }
    printf("Device %04x:%04x found. Opening...\n", options->vid, options->pid);

    int r = libusb_open(device, &input_manager.handle);
    if (r) {
        print_libusb_error(r);
        libusb_unref_device(device);
        exit(1);
    }

    printf("Registering HID...\n");
    if (register_hid(input_manager.handle, REPORT_DESC_SIZE))
    {
        printf("Registering HID failed\n");
        exit(1);
    }

    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(device, &desc);
    int max_packet_size_0 = desc.bMaxPacketSize0;

    printf("Sending HID descriptor...\n");
    if (send_hid_descriptor(input_manager.handle, REPORT_DESC, REPORT_DESC_SIZE, max_packet_size_0))
    {
        printf("Sending HID descriptor failed\n");
        exit(1);
    }

    // an event sent too early after the HID descriptor may fail
    usleep(1000000);

    ret = event_loop();
    LOGD("quit...");

    screen_destroy(&screen);

    shutdown(s, SHUT_RD);
    pthread_join(handler, NULL);

finally_stop_and_join_controller:
    controller_stop(&controller);
    controller_join(&controller);
finally_destroy_controller:
    controller_destroy(&controller);
finally_stop_decoder:
    decoder_stop(&decoder);
    // stop the server before decoder_join() to wake up the decoder
    server_stop(&server);
    decoder_join(&decoder);
finally_destroy_file_handler:
    file_handler_stop(&file_handler);
    file_handler_join(&file_handler);
    file_handler_destroy(&file_handler);
finally_destroy_recorder:
    if (options->record_filename) {
        recorder_destroy(&recorder);
    }
finally_destroy_frames:
    frames_destroy(&frames);
finally_destroy_server:
    if (options->show_touches) {
        if (!show_touches_waited) {
            // wait the process which enabled "show touches"
            wait_show_touches(proc_show_touches);
        }
        LOGI("Disable show_touches");
        proc_show_touches = set_show_touches_enabled(options->serial, SDL_FALSE);
        wait_show_touches(proc_show_touches);
    }

    server_destroy(&server);

    return ret;
}
