#include "input_manager.h"

#include <SDL2/SDL_assert.h>
#include "convert.h"
#include "lock_util.h"
#include "log.h"

static Uint32 timestamp = 0;


// Convert window coordinates (as provided by SDL_GetMouseState() to renderer coordinates (as provided in SDL mouse events)
//
// See my question:
// <https://stackoverflow.com/questions/49111054/how-to-get-mouse-position-on-mouse-wheel-event>
static void convert_to_renderer_coordinates(SDL_Renderer *renderer, int *x, int *y) {
    SDL_Rect viewport;
    float scale_x, scale_y;
    SDL_RenderGetViewport(renderer, &viewport);
    SDL_RenderGetScale(renderer, &scale_x, &scale_y);
    *x = (int) (*x / scale_x) - viewport.x;
    *y = (int) (*y / scale_y) - viewport.y;
}

static void get_mouse_point(struct screen *screen, int *x, int *y) {
    SDL_GetMouseState(x, y);
    convert_to_renderer_coordinates(screen->renderer, x, y);
}

static const int ACTION_DOWN = 1;
static const int ACTION_UP = 1 << 1;

static void send_keycode(struct controller *controller, enum android_keycode keycode, int actions, const char *name) {
    // send DOWN event
    struct control_event control_event;
    control_event.type = CONTROL_EVENT_TYPE_KEYCODE;
    control_event.keycode_event.keycode = keycode;
    control_event.keycode_event.metastate = 0;

    if (actions & ACTION_DOWN) {
        control_event.keycode_event.action = AKEY_EVENT_ACTION_DOWN;
        if (!controller_push_event(controller, &control_event)) {
            LOGW("Cannot send %s (DOWN)", name);
            return;
        }
    }

    if (actions & ACTION_UP) {
        control_event.keycode_event.action = AKEY_EVENT_ACTION_UP;
        if (!controller_push_event(controller, &control_event)) {
            LOGW("Cannot send %s (UP)", name);
        }
    }
}

static inline void action_paste(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_PASTE, actions, "PASTE");
}

static inline void action_call(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_HEADSETHOOK, actions, "HEADSETHOOK");
}

static inline void action_pp(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_MEDIA_PLAY_PAUSE, actions, "PLAYPAUSE");
}

static inline void action_play(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_MEDIA_PLAY, actions, "PLAY");
}

static inline void action_stop(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_MEDIA_STOP, actions, "STOP");
}

static inline void action_next(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_MEDIA_NEXT, actions, "NEXT");
}

static inline void action_prev(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_MEDIA_PREVIOUS, actions, "PREV");
}

static inline void action_home(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_HOME, actions, "HOME");
}

static inline void action_back(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_BACK, actions, "BACK");
}

static inline void action_app_switch(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_APP_SWITCH, actions, "APP_SWITCH");
}

static inline void action_unlock(struct controller *controller, int actions) {
    system("unlockphone");
}

static inline void action_power(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_POWER, actions, "POWER");
}

static inline void action_volume_up(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_VOLUME_UP, actions, "VOLUME_UP");
}

static inline void action_volume_down(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_VOLUME_DOWN, actions, "VOLUME_DOWN");
}

static inline void action_menu(struct controller *controller, int actions) {
    send_keycode(controller, AKEYCODE_MENU, actions, "MENU");
}

// turn the screen on if it was off, press BACK otherwise
static void press_back_or_turn_screen_on(struct controller *controller) {
    struct control_event control_event;
    control_event.type = CONTROL_EVENT_TYPE_COMMAND;
    control_event.command_event.action = CONTROL_EVENT_COMMAND_BACK_OR_SCREEN_ON;

    if (!controller_push_event(controller, &control_event)) {
        LOGW("Cannot turn screen on");
    }
}

static void switch_fps_counter_state(struct frames *frames) {
    mutex_lock(frames->mutex);
    if (frames->fps_counter.started) {
        LOGI("FPS counter stopped");
        fps_counter_stop(&frames->fps_counter);
    } else {
        LOGI("FPS counter started");
        fps_counter_start(&frames->fps_counter);
    }
    mutex_unlock(frames->mutex);
}

static void clipboard_paste(struct controller *controller) {
    char *text = SDL_GetClipboardText();
    if (!text) {
        LOGW("Cannot get clipboard text: %s", SDL_GetError());
        return;
    }
    if (!*text) {
        // empty text
        SDL_free(text);
        return;
    }

    struct control_event control_event;
    control_event.type = CONTROL_EVENT_TYPE_TEXT;
    control_event.text_event.text = text;
    if (!controller_push_event(controller, &control_event)) {
        SDL_free(text);
        LOGW("Cannot send clipboard paste event");
    }
}

void input_manager_enable_modifiers(Uint32 time)
{
    timestamp = time;
}

void input_manager_process_text_input(struct input_manager *input_manager,
                                      const SDL_TextInputEvent *event) {
    char c = event->text[0];
    if (c == 'h' || c == 'j' || c == 'k' || c == 'l')
        return;
    const unsigned char * data;
    data = key_press(c);
    if (data == hid_keys_null)
        return;
    if (send_hid_event(input_manager->handle, data, HID_EVENT_SIZE))
        goto log_err;
    if (send_hid_event(input_manager->handle, data + 5, HID_EVENT_SIZE))
        goto log_err;
    return;
log_err:
    LOGW("Cannot send usb key event");
}

static void swipe(int dir, struct input_manager *input_manager) {
    struct size screen_size = input_manager->screen->frame_size;
    static const int steps = 10;
    struct control_event * tos = SDL_malloc((steps + 2) * sizeof(struct control_event));

    Uint16 xs[steps + 1], ys[steps + 1];
    if (dir == 0) {
        for (int i = 0; i <= steps; ++i) {
            xs[i] = screen_size.width * 7 / 8 - screen_size.width * i / steps / 8;
            ys[i] = screen_size.height / 2;
        }
    } else if (dir == 1) {
        for (int i = 0; i <= steps; ++i) {
            xs[i] = screen_size.width / 8 + screen_size.width * i / steps * 7 / 8;
            ys[i] = screen_size.height / 2;
        }
    } else if (dir == 2) {
        for (int i = 0; i <= steps; ++i) {
            xs[i] = screen_size.width / 2;
            ys[i] = screen_size.height * 3 / 4 - screen_size.height * i / steps / 2;
        }
    } else if (dir == 3) {
        for (int i = 0; i <= steps; ++i) {
            xs[i] = screen_size.width / 2;
            ys[i] = screen_size.height / 4 + screen_size.height * i / steps / 2;
        }
    }

    struct control_event to;
    to.type = CONTROL_EVENT_TYPE_SWIPE;
    to.swipe_event.events = tos;
    to.swipe_event.num = steps + 2;
    to.swipe_event.time = 50000;

    tos[0].type = CONTROL_EVENT_TYPE_MOUSE;
    tos[0].mouse_event.action = AMOTION_EVENT_ACTION_DOWN;
    tos[0].mouse_event.buttons = AMOTION_EVENT_BUTTON_PRIMARY;
    tos[0].mouse_event.position.screen_size = screen_size;
    tos[0].mouse_event.position.point.x = xs[0];
    tos[0].mouse_event.position.point.y = ys[0];

    for (int i = 1; i <= steps; ++i) {
        tos[i].type = CONTROL_EVENT_TYPE_MOUSE;
        tos[i].mouse_event.action = AMOTION_EVENT_ACTION_MOVE;
        tos[i].mouse_event.buttons = AMOTION_EVENT_BUTTON_PRIMARY;
        tos[i].mouse_event.position.screen_size = screen_size;
        tos[i].mouse_event.position.point.x = xs[i];
        tos[i].mouse_event.position.point.y = ys[i];
    }

    tos[steps + 1].type = CONTROL_EVENT_TYPE_MOUSE;
    tos[steps + 1].mouse_event.action = AMOTION_EVENT_ACTION_UP;
    tos[steps + 1].mouse_event.buttons = AMOTION_EVENT_BUTTON_PRIMARY;
    tos[steps + 1].mouse_event.position.screen_size = screen_size;
    tos[steps + 1].mouse_event.position.point.x = xs[steps];
    tos[steps + 1].mouse_event.position.point.y = ys[steps];

    if (!controller_push_event(input_manager->controller, &to)) {
        LOGW("Cannot send mouse motion event");
    }
}

void input_manager_process_key(struct input_manager *input_manager,
                               SDL_KeyboardEvent *event) {

    SDL_bool ctrl = event->keysym.mod & (KMOD_LCTRL | KMOD_RCTRL);
    SDL_bool alt = event->keysym.mod & (KMOD_LALT | KMOD_RALT);
    SDL_bool meta = event->keysym.mod & (KMOD_LGUI | KMOD_RGUI);
    SDL_bool shift = event->keysym.mod & KMOD_SHIFT;

    if (shift && event->keysym.sym == SDLK_INSERT)
    {
        if (!ctrl && !alt && !event->repeat && event->type == SDL_KEYDOWN) {
            /* clipboard_paste(input_manager->controller); */
            server_paste(input_manager->server);
            action_paste(input_manager->controller, ACTION_DOWN);
        }
        return;
    }

    if (event->timestamp < timestamp + 10)
    {
        return;
    }

    if (ctrl && event->keysym.sym == SDLK_v)
    {
        if (!alt && !event->repeat && event->type == SDL_KEYDOWN) {
            server_paste(input_manager->server);
            action_paste(input_manager->controller, ACTION_DOWN);
        }
        return;
    }

    if (ctrl && event->keysym.sym == SDLK_c)
    {
        if (!alt && !event->repeat && event->type == SDL_KEYDOWN) {
            system("androidclip");
        }
        return;
    }

    SDL_Keycode keycode = event->keysym.sym;
    if (alt)
    {
        switch (keycode) {
            case SDLK_h:
                if (!ctrl && !event->repeat && event->type == SDL_KEYDOWN) {
                    swipe(1, input_manager);
                }
                return;
            case SDLK_l:
                if (!ctrl && !event->repeat && event->type == SDL_KEYDOWN) {
                    swipe(0, input_manager);
                }
                return;
            case SDLK_j:
                if (!ctrl && !event->repeat && event->type == SDL_KEYDOWN) {
                    swipe(2, input_manager);
                }
                return;
            case SDLK_k:
                if (!ctrl && !event->repeat && event->type == SDL_KEYDOWN) {
                    swipe(3, input_manager);
                }
                return;
        }
    }

    int i = -1;
    int action = event->type == SDL_KEYDOWN ? ACTION_DOWN : ACTION_UP;
    SDL_bool repeat = event->repeat;
    const unsigned char * data;
    switch (keycode)
    {
        case SDLK_VOLUMEDOWN:
            {
                static unsigned char xx[4] = {0x02, 0x80, 0x02, 0x00};
                if (send_hid_event(input_manager->handle, xx, 2))
                    goto log_err;
                if (send_hid_event(input_manager->handle, xx + 2, 2))
                    goto log_err;
            }
            return;
        case SDLK_VOLUMEUP:
            {
                static unsigned char xx[4] = {0x02, 0x40, 0x02, 0x00};
                if (send_hid_event(input_manager->handle, xx, 2))
                    goto log_err;
                if (send_hid_event(input_manager->handle, xx + 2, 2))
                    goto log_err;
            }
            return;
        case SDLK_MUTE:
            {
                static unsigned char xx[4] = {0x02, 0x20, 0x02, 0x00};
                if (send_hid_event(input_manager->handle, xx, 2))
                    goto log_err;
                if (send_hid_event(input_manager->handle, xx + 2, 2))
                    goto log_err;
            }
            return;
        case SDLK_AUDIOPLAY:
            {
                static unsigned char xx[4] = {0x02, 0x10, 0x02, 0x00};
                if (send_hid_event(input_manager->handle, xx, 2))
                    goto log_err;
                if (send_hid_event(input_manager->handle, xx + 2, 2))
                    goto log_err;
            }
            return;
        case SDLK_AUDIOPREV:
            {
                static unsigned char xx[4] = {0x02, 0x02, 0x02, 0x00};
                if (send_hid_event(input_manager->handle, xx, 2))
                    goto log_err;
                if (send_hid_event(input_manager->handle, xx + 2, 2))
                    goto log_err;
            }
            return;
        case SDLK_AUDIONEXT:
            {
                static unsigned char xx[4] = {0x02, 0x01, 0x02, 0x00};
                if (send_hid_event(input_manager->handle, xx, 2))
                    goto log_err;
                if (send_hid_event(input_manager->handle, xx + 2, 2))
                    goto log_err;
            }
            return;
        case SDLK_ESCAPE:
            action_back(input_manager->controller, action);
            /* press_back_or_turn_screen_on(input_manager->controller); */
            return;
        case SDLK_RETURN:
        case SDLK_BACKSPACE:
        case SDLK_TAB:
            data = key_press(keycode);
            goto out2;
    }

    if (event->keysym.scancode == 53)
    {
        data = hid_shift_keys[40]; // shift-space
        goto out2;
    }

    // capture all Ctrl events
    if (ctrl | alt) {
        SDL_bool shift = event->keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT);
        if (shift) {
            // currently, there is no shortcut involving SHIFT
            return;
        }
        switch (keycode) {
            case SDLK_h:
                if (ctrl && !alt && !repeat) {
                    action_home(input_manager->controller, action);
                }
                return;
            case SDLK_BACKSPACE:
                if (ctrl && !alt && !repeat) {
                    action_back(input_manager->controller, action);
                }
                return;
            case SDLK_s:
                if (ctrl && !alt && !repeat) {
                    action_app_switch(input_manager->controller, action);
                }
                return;
            case SDLK_m:
                if (ctrl && !alt && !repeat) {
                    action_menu(input_manager->controller, action);
                }
                return;
            case SDLK_l:
                if (ctrl && !alt && !repeat) {
                    action_unlock(input_manager->controller, action);
                }
                return;
            case SDLK_p:
                if (ctrl && !alt && !repeat) {
                    action_power(input_manager->controller, action);
                }
                return;
            case SDLK_DOWN:
                if (ctrl && !alt) {
                    // forward repeated events
                    action_volume_down(input_manager->controller, action);
                }
                return;
            case SDLK_UP:
                if (ctrl && !alt) {
                    // forward repeated events
                    action_volume_up(input_manager->controller, action);
                }
                return;
            case SDLK_x:
                if (ctrl && !alt && !repeat && event->type == SDL_KEYDOWN) {
                    screen_resize_to_fit(input_manager->screen);
                }
                return;
            case SDLK_g:
                if (ctrl && !alt && !repeat && event->type == SDL_KEYDOWN) {
                    screen_resize_to_pixel_perfect(input_manager->screen);
                }
                return;
            case SDLK_i:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 39; // tab
                    goto out;
                }
            case SDLK_d:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 72; // delete
                    goto out;
                }
            case SDLK_a:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 70; // home
                    goto out;
                }
            case SDLK_e:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 73; // end
                    goto out;
                }
            case SDLK_b:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 76; // left
                    goto out;
                }
            case SDLK_f:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 75; // right
                    goto out;
                }
            case SDLK_j:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 77; // down
                    goto out;
                }
            case SDLK_k:
                if (ctrl && !(alt && event->type == SDL_KEYDOWN)) {
                    i = 78; // up
                    goto out;
                }
        }
        return;
    }
    else if (keycode == SDLK_h || keycode == SDLK_j || keycode == SDLK_k || keycode == SDLK_l)
    {

    }
    else
        return;

out:
    ;
    if (i != -1)
        data = hid_keys[i];
    else
    {
        i = key_index(event->keysym.sym);
    }
    if (i == -1)
    {
        data = key_press(event->keysym.sym);
    }
    else
    {
        if (shift)
            data = hid_shift_keys[i];
        else
            data = hid_keys[i];
    }
    /* printf("event->keysym.sym = %d\n", event->keysym.sym); */
    /* printf("data = "); */
    /* for (auto i = 0ul; i < 8; ++i) { */
    /*     printf("%d ", data[i]); */
    /* } */
    /* printf("\n"); */
    if (data == hid_keys_null)
        return;
out2:
    if (event->type == SDL_KEYDOWN)
    {
        if (send_hid_event(input_manager->handle, data, HID_EVENT_SIZE))
            goto log_err;
    }
    else
    {
        if (send_hid_event(input_manager->handle, data + 5, HID_EVENT_SIZE))
            goto log_err;
    }
    return;
log_err:
    LOGW("Cannot send usb key event");
}

void input_manager_process_mouse_motion(struct input_manager *input_manager,
                                        const SDL_MouseMotionEvent *event) {
    if (!event->state) {
        // do not send motion events when no button is pressed
        return;
    }
    struct control_event control_event;
    if (mouse_motion_from_sdl_to_android(event, input_manager->screen->frame_size, &control_event)) {
        if (!controller_push_event(input_manager->controller, &control_event)) {
            LOGW("Cannot send mouse motion event");
        }
    }
}

static SDL_bool is_outside_device_screen(struct input_manager *input_manager,
                                         int x, int y)
{
    return x < 0 || x >= input_manager->screen->frame_size.width ||
           y < 0 || y >= input_manager->screen->frame_size.height;
}

void input_manager_process_mouse_button(struct input_manager *input_manager,
                                        const SDL_MouseButtonEvent *event) {
    SDL_bool outside_device_screen = is_outside_device_screen(input_manager,
                                                              event->x,
                                                              event->y);
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button == SDL_BUTTON_RIGHT) {
            press_back_or_turn_screen_on(input_manager->controller);
            return;
        }
        if (event->button == SDL_BUTTON_MIDDLE) {
            action_home(input_manager->controller, ACTION_DOWN | ACTION_UP);
            return;
        }
        // double-click on black borders resize to fit the device screen
        if (event->button == SDL_BUTTON_LEFT && event->clicks == 2
                && outside_device_screen) {
            screen_resize_to_fit(input_manager->screen);
            return;
        }
        // otherwise, send the click event to the device
    }

    if (outside_device_screen) {
        // ignore
        return;
    }

    struct control_event control_event;
    if (mouse_button_from_sdl_to_android(event, input_manager->screen->frame_size, &control_event)) {
        if (!controller_push_event(input_manager->controller, &control_event)) {
            LOGW("Cannot send mouse button event");
        }
    }
}

void input_manager_process_mouse_wheel(struct input_manager *input_manager,
                                       const SDL_MouseWheelEvent *event) {
    int x;
    int y;
    get_mouse_point(input_manager->screen, &x, &y);
    if (is_outside_device_screen(input_manager, x, y)) {
        // ignore
        return;
    }

    SDL_assert_release(x >= 0 && x < 0x10000 && y >= 0 && y < 0x10000);

    struct position position = {
        .screen_size = input_manager->screen->frame_size,
        .point = {
            .x = (Uint16) x,
            .y = (Uint16) y,
        },
    };
    struct control_event control_event;
    if (mouse_wheel_from_sdl_to_android(event, position, &control_event)) {
        if (!controller_push_event(input_manager->controller, &control_event)) {
            LOGW("Cannot send mouse wheel event");
        }
    }
}
