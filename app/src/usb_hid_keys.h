#pragma once

#include <errno.h>
#include <error.h>
#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Modifier masks - used for the first byte in the HID report.
 * NOTE: The second byte in the report is reserved, 0x00
 */
#define KEY_MOD_LCTRL 0x01
#define KEY_MOD_LSHIFT 0x02
#define KEY_MOD_LALT 0x04
#define KEY_MOD_LMETA 0x08
#define KEY_MOD_RCTRL 0x10
#define KEY_MOD_RSHIFT 0x20
#define KEY_MOD_RALT 0x40
#define KEY_MOD_RMETA 0x80

static const unsigned char hid_keys_null[10] = { 0x00 };

static const unsigned char hid_keys[][10] = {
    { 0x01, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x84 }, // Keyboard a and A
    { 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x85 }, // Keyboard b and B
    { 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x00, 0x00, 0x00, 0x86 }, // Keyboard c and C
    { 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x87 }, // Keyboard d and D
    { 0x01, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x88 }, // Keyboard e and E
    { 0x01, 0x00, 0x00, 0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x89 }, // Keyboard f and F
    { 0x01, 0x00, 0x00, 0x00, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x8a }, // Keyboard g and G
    { 0x01, 0x00, 0x00, 0x00, 0x0b, 0x01, 0x00, 0x00, 0x00, 0x8b }, // Keyboard h and H
    { 0x01, 0x00, 0x00, 0x00, 0x0c, 0x01, 0x00, 0x00, 0x00, 0x8c }, // Keyboard i and I
    { 0x01, 0x00, 0x00, 0x00, 0x0d, 0x01, 0x00, 0x00, 0x00, 0x8d }, // Keyboard j and J
    { 0x01, 0x00, 0x00, 0x00, 0x0e, 0x01, 0x00, 0x00, 0x00, 0x8e }, // Keyboard k and K
    { 0x01, 0x00, 0x00, 0x00, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x8f }, // Keyboard l and L
    { 0x01, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, 0x90 }, // Keyboard m and M
    { 0x01, 0x00, 0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x00, 0x91 }, // Keyboard n and N
    { 0x01, 0x00, 0x00, 0x00, 0x12, 0x01, 0x00, 0x00, 0x00, 0x92 }, // Keyboard o and O
    { 0x01, 0x00, 0x00, 0x00, 0x13, 0x01, 0x00, 0x00, 0x00, 0x93 }, // Keyboard p and P
    { 0x01, 0x00, 0x00, 0x00, 0x14, 0x01, 0x00, 0x00, 0x00, 0x94 }, // Keyboard q and Q
    { 0x01, 0x00, 0x00, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x95 }, // Keyboard r and R
    { 0x01, 0x00, 0x00, 0x00, 0x16, 0x01, 0x00, 0x00, 0x00, 0x96 }, // Keyboard s and S
    { 0x01, 0x00, 0x00, 0x00, 0x17, 0x01, 0x00, 0x00, 0x00, 0x97 }, // Keyboard t and T
    { 0x01, 0x00, 0x00, 0x00, 0x18, 0x01, 0x00, 0x00, 0x00, 0x98 }, // Keyboard u and U
    { 0x01, 0x00, 0x00, 0x00, 0x19, 0x01, 0x00, 0x00, 0x00, 0x99 }, // Keyboard v and V
    { 0x01, 0x00, 0x00, 0x00, 0x1a, 0x01, 0x00, 0x00, 0x00, 0x9a }, // Keyboard w and W
    { 0x01, 0x00, 0x00, 0x00, 0x1b, 0x01, 0x00, 0x00, 0x00, 0x9b }, // Keyboard x and X
    { 0x01, 0x00, 0x00, 0x00, 0x1c, 0x01, 0x00, 0x00, 0x00, 0x9c }, // Keyboard y and Y
    { 0x01, 0x00, 0x00, 0x00, 0x1d, 0x01, 0x00, 0x00, 0x00, 0x9d }, // Keyboard z and Z
    { 0x01, 0x00, 0x00, 0x00, 0x1e, 0x01, 0x00, 0x00, 0x00, 0x9e }, // Keyboard 1 and !
    { 0x01, 0x00, 0x00, 0x00, 0x1f, 0x01, 0x00, 0x00, 0x00, 0x9f }, // Keyboard 2 and @
    { 0x01, 0x00, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0xa0 }, // Keyboard 3 and #
    { 0x01, 0x00, 0x00, 0x00, 0x21, 0x01, 0x00, 0x00, 0x00, 0xa1 }, // Keyboard 4 and $
    { 0x01, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x00, 0x00, 0xa2 }, // Keyboard 5 and %
    { 0x01, 0x00, 0x00, 0x00, 0x23, 0x01, 0x00, 0x00, 0x00, 0xa3 }, // Keyboard 6 and ^
    { 0x01, 0x00, 0x00, 0x00, 0x24, 0x01, 0x00, 0x00, 0x00, 0xa4 }, // Keyboard 7 and &
    { 0x01, 0x00, 0x00, 0x00, 0x25, 0x01, 0x00, 0x00, 0x00, 0xa5 }, // Keyboard 8 and *
    { 0x01, 0x00, 0x00, 0x00, 0x26, 0x01, 0x00, 0x00, 0x00, 0xa6 }, // Keyboard 9 and (
    { 0x01, 0x00, 0x00, 0x00, 0x27, 0x01, 0x00, 0x00, 0x00, 0xa7 }, // Keyboard 0 and )
    { 0x01, 0x00, 0x00, 0x00, 0x28, 0x01, 0x00, 0x00, 0x00, 0xa8 }, // Keyboard Return (ENTER)
    { 0x01, 0x00, 0x00, 0x00, 0x29, 0x01, 0x00, 0x00, 0x00, 0xa9 }, // Keyboard ESCAPE
    { 0x01, 0x00, 0x00, 0x00, 0x2a, 0x01, 0x00, 0x00, 0x00, 0xaa }, // Keyboard DELETE (Backspace)
    { 0x01, 0x00, 0x00, 0x00, 0x2b, 0x01, 0x00, 0x00, 0x00, 0xab }, // Keyboard Tab
    { 0x01, 0x00, 0x00, 0x00, 0x2c, 0x01, 0x00, 0x00, 0x00, 0xac }, // Keyboard Spacebar
    { 0x01, 0x00, 0x00, 0x00, 0x2d, 0x01, 0x00, 0x00, 0x00, 0xad }, // Keyboard - and _
    { 0x01, 0x00, 0x00, 0x00, 0x2e, 0x01, 0x00, 0x00, 0x00, 0xae }, // Keyboard = and +
    { 0x01, 0x00, 0x00, 0x00, 0x2f, 0x01, 0x00, 0x00, 0x00, 0xaf }, // Keyboard [ and {
    { 0x01, 0x00, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x00, 0xb0 }, // Keyboard ] and }
    { 0x01, 0x00, 0x00, 0x00, 0x31, 0x01, 0x00, 0x00, 0x00, 0xb1 }, // Keyboard \ and |
    { 0x01, 0x00, 0x00, 0x00, 0x32, 0x01, 0x00, 0x00, 0x00, 0xb2 }, // Keyboard Non-US # and ~
    { 0x01, 0x00, 0x00, 0x00, 0x33, 0x01, 0x00, 0x00, 0x00, 0xb3 }, // Keyboard ; and :
    { 0x01, 0x00, 0x00, 0x00, 0x34, 0x01, 0x00, 0x00, 0x00, 0xb4 }, // Keyboard ' and "
    { 0x01, 0x00, 0x00, 0x00, 0x35, 0x01, 0x00, 0x00, 0x00, 0xb5 }, // Keyboard ` and ~
    { 0x01, 0x00, 0x00, 0x00, 0x36, 0x01, 0x00, 0x00, 0x00, 0xb6 }, // Keyboard , and <
    { 0x01, 0x00, 0x00, 0x00, 0x37, 0x01, 0x00, 0x00, 0x00, 0xb7 }, // Keyboard . and >
    { 0x01, 0x00, 0x00, 0x00, 0x38, 0x01, 0x00, 0x00, 0x00, 0xb8 }, // Keyboard / and ?
    { 0x01, 0x00, 0x00, 0x00, 0x39, 0x01, 0x00, 0x00, 0x00, 0xb9 }, // Keyboard Caps Lock
    { 0x01, 0x00, 0x00, 0x00, 0x3a, 0x01, 0x00, 0x00, 0x00, 0xba }, // Keyboard F1
    { 0x01, 0x00, 0x00, 0x00, 0x3b, 0x01, 0x00, 0x00, 0x00, 0xbb }, // Keyboard F2
    { 0x01, 0x00, 0x00, 0x00, 0x3c, 0x01, 0x00, 0x00, 0x00, 0xbc }, // Keyboard F3
    { 0x01, 0x00, 0x00, 0x00, 0x3d, 0x01, 0x00, 0x00, 0x00, 0xbd }, // Keyboard F4
    { 0x01, 0x00, 0x00, 0x00, 0x3e, 0x01, 0x00, 0x00, 0x00, 0xbe }, // Keyboard F5
    { 0x01, 0x00, 0x00, 0x00, 0x3f, 0x01, 0x00, 0x00, 0x00, 0xbf }, // Keyboard F6
    { 0x01, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x00, 0xc0 }, // Keyboard F7
    { 0x01, 0x00, 0x00, 0x00, 0x41, 0x01, 0x00, 0x00, 0x00, 0xc1 }, // Keyboard F8
    { 0x01, 0x00, 0x00, 0x00, 0x42, 0x01, 0x00, 0x00, 0x00, 0xc2 }, // Keyboard F9
    { 0x01, 0x00, 0x00, 0x00, 0x43, 0x01, 0x00, 0x00, 0x00, 0xc3 }, // Keyboard F10
    { 0x01, 0x00, 0x00, 0x00, 0x44, 0x01, 0x00, 0x00, 0x00, 0xc4 }, // Keyboard F11
    { 0x01, 0x00, 0x00, 0x00, 0x45, 0x01, 0x00, 0x00, 0x00, 0xc5 }, // Keyboard F12
    { 0x01, 0x00, 0x00, 0x00, 0x46, 0x01, 0x00, 0x00, 0x00, 0xc6 }, // Keyboard Print Screen
    { 0x01, 0x00, 0x00, 0x00, 0x47, 0x01, 0x00, 0x00, 0x00, 0xc7 }, // Keyboard Scroll Lock
    { 0x01, 0x00, 0x00, 0x00, 0x48, 0x01, 0x00, 0x00, 0x00, 0xc8 }, // Keyboard Pause
    { 0x01, 0x00, 0x00, 0x00, 0x49, 0x01, 0x00, 0x00, 0x00, 0xc9 }, // Keyboard Insert
    { 0x01, 0x00, 0x00, 0x00, 0x4a, 0x01, 0x00, 0x00, 0x00, 0xca }, // Keyboard Home
    { 0x01, 0x00, 0x00, 0x00, 0x4b, 0x01, 0x00, 0x00, 0x00, 0xcb }, // Keyboard Page Up
    { 0x01, 0x00, 0x00, 0x00, 0x4c, 0x01, 0x00, 0x00, 0x00, 0xcc }, // Keyboard Delete Forward
    { 0x01, 0x00, 0x00, 0x00, 0x4d, 0x01, 0x00, 0x00, 0x00, 0xcd }, // Keyboard End
    { 0x01, 0x00, 0x00, 0x00, 0x4e, 0x01, 0x00, 0x00, 0x00, 0xce }, // Keyboard Page Down
    { 0x01, 0x00, 0x00, 0x00, 0x4f, 0x01, 0x00, 0x00, 0x00, 0xcf }, // Keyboard Right Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x50, 0x01, 0x00, 0x00, 0x00, 0xd0 }, // Keyboard Left Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x51, 0x01, 0x00, 0x00, 0x00, 0xd1 }, // Keyboard Down Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x52, 0x01, 0x00, 0x00, 0x00, 0xd2 }, // Keyboard Up Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x53, 0x01, 0x00, 0x00, 0x00, 0xd3 }, // Keyboard Num Lock and Clear
    { 0x01, 0x00, 0x00, 0x00, 0x54, 0x01, 0x00, 0x00, 0x00, 0xd4 }, // Keypad /
    { 0x01, 0x00, 0x00, 0x00, 0x55, 0x01, 0x00, 0x00, 0x00, 0xd5 }, // Keypad *
    { 0x01, 0x00, 0x00, 0x00, 0x56, 0x01, 0x00, 0x00, 0x00, 0xd6 }, // Keypad -
    { 0x01, 0x00, 0x00, 0x00, 0x57, 0x01, 0x00, 0x00, 0x00, 0xd7 }, // Keypad +
    { 0x01, 0x00, 0x00, 0x00, 0x58, 0x01, 0x00, 0x00, 0x00, 0xd8 }, // Keypad ENTER
    { 0x01, 0x00, 0x00, 0x00, 0x59, 0x01, 0x00, 0x00, 0x00, 0xd9 }, // Keypad 1 and End
    { 0x01, 0x00, 0x00, 0x00, 0x5a, 0x01, 0x00, 0x00, 0x00, 0xda }, // Keypad 2 and Down Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x5b, 0x01, 0x00, 0x00, 0x00, 0xdb }, // Keypad 3 and PageDn
    { 0x01, 0x00, 0x00, 0x00, 0x5c, 0x01, 0x00, 0x00, 0x00, 0xdc }, // Keypad 4 and Left Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x5d, 0x01, 0x00, 0x00, 0x00, 0xdd }, // Keypad 5
    { 0x01, 0x00, 0x00, 0x00, 0x5e, 0x01, 0x00, 0x00, 0x00, 0xde }, // Keypad 6 and Right Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x5f, 0x01, 0x00, 0x00, 0x00, 0xdf }, // Keypad 7 and Home
    { 0x01, 0x00, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0x00, 0xe0 }, // Keypad 8 and Up Arrow
    { 0x01, 0x00, 0x00, 0x00, 0x61, 0x01, 0x00, 0x00, 0x00, 0xe1 }, // Keypad 9 and Page Up
    { 0x01, 0x00, 0x00, 0x00, 0x62, 0x01, 0x00, 0x00, 0x00, 0xe2 }, // Keypad 0 and Insert
    { 0x01, 0x00, 0x00, 0x00, 0x63, 0x01, 0x00, 0x00, 0x00, 0xe3 }, // Keypad . and Delete
    { 0x01, 0x00, 0x00, 0x00, 0x64, 0x01, 0x00, 0x00, 0x00, 0xe4 }, // Keyboard Non-US \ and |
    { 0x01, 0x00, 0x00, 0x00, 0x65, 0x01, 0x00, 0x00, 0x00, 0xe5 }, // Keyboard Application
    { 0x01, 0x00, 0x00, 0x00, 0x66, 0x01, 0x00, 0x00, 0x00, 0xe6 }, // Keyboard Power
    { 0x01, 0x00, 0x00, 0x00, 0x67, 0x01, 0x00, 0x00, 0x00, 0xe7 }, // Keypad =
    { 0x01, 0x00, 0x00, 0x00, 0x68, 0x01, 0x00, 0x00, 0x00, 0xe8 }, // Keyboard F13
    { 0x01, 0x00, 0x00, 0x00, 0x69, 0x01, 0x00, 0x00, 0x00, 0xe9 }, // Keyboard F14
    { 0x01, 0x00, 0x00, 0x00, 0x6a, 0x01, 0x00, 0x00, 0x00, 0xea }, // Keyboard F15
    { 0x01, 0x00, 0x00, 0x00, 0x6b, 0x01, 0x00, 0x00, 0x00, 0xeb }, // Keyboard F16
    { 0x01, 0x00, 0x00, 0x00, 0x6c, 0x01, 0x00, 0x00, 0x00, 0xec }, // Keyboard F17
    { 0x01, 0x00, 0x00, 0x00, 0x6d, 0x01, 0x00, 0x00, 0x00, 0xed }, // Keyboard F18
    { 0x01, 0x00, 0x00, 0x00, 0x6e, 0x01, 0x00, 0x00, 0x00, 0xee }, // Keyboard F19
    { 0x01, 0x00, 0x00, 0x00, 0x6f, 0x01, 0x00, 0x00, 0x00, 0xef }, // Keyboard F20
    { 0x01, 0x00, 0x00, 0x00, 0x70, 0x01, 0x00, 0x00, 0x00, 0xf0 }, // Keyboard F21
    { 0x01, 0x00, 0x00, 0x00, 0x71, 0x01, 0x00, 0x00, 0x00, 0xf1 }, // Keyboard F22
    { 0x01, 0x00, 0x00, 0x00, 0x72, 0x01, 0x00, 0x00, 0x00, 0xf2 }, // Keyboard F23
    { 0x01, 0x00, 0x00, 0x00, 0x73, 0x01, 0x00, 0x00, 0x00, 0xf3 }, // Keyboard F24
    { 0x01, 0x00, 0x00, 0x00, 0x74, 0x01, 0x00, 0x00, 0x00, 0xf4 }, // Keyboard Execute
    { 0x01, 0x00, 0x00, 0x00, 0x75, 0x01, 0x00, 0x00, 0x00, 0xf5 }, // Keyboard Help
    { 0x01, 0x00, 0x00, 0x00, 0x76, 0x01, 0x00, 0x00, 0x00, 0xf6 }, // Keyboard Menu
    { 0x01, 0x00, 0x00, 0x00, 0x77, 0x01, 0x00, 0x00, 0x00, 0xf7 }, // Keyboard Select
    { 0x01, 0x00, 0x00, 0x00, 0x78, 0x01, 0x00, 0x00, 0x00, 0xf8 }, // Keyboard Stop
    { 0x01, 0x00, 0x00, 0x00, 0x79, 0x01, 0x00, 0x00, 0x00, 0xf9 }, // Keyboard Again
    { 0x01, 0x00, 0x00, 0x00, 0x7a, 0x01, 0x00, 0x00, 0x00, 0xfa }, // Keyboard Undo
    { 0x01, 0x00, 0x00, 0x00, 0x7b, 0x01, 0x00, 0x00, 0x00, 0xfb }, // Keyboard Cut
    { 0x01, 0x00, 0x00, 0x00, 0x7c, 0x01, 0x00, 0x00, 0x00, 0xfc }, // Keyboard Copy
    { 0x01, 0x00, 0x00, 0x00, 0x7d, 0x01, 0x00, 0x00, 0x00, 0xfd }, // Keyboard Paste
    { 0x01, 0x00, 0x00, 0x00, 0x7e, 0x01, 0x00, 0x00, 0x00, 0xfe }, // Keyboard Find
    { 0x01, 0x00, 0x00, 0x00, 0x7f, 0x01, 0x00, 0x00, 0x00, 0xff }, // Keyboard Mute
};

static const unsigned char hid_shift_keys[][10] = {
    { 0x01, 0x02, 0x00, 0x00, 0x04, 0x01, 0x02, 0x00, 0x00, 0x84 }, // Keyboard a and A
    { 0x01, 0x02, 0x00, 0x00, 0x05, 0x01, 0x02, 0x00, 0x00, 0x85 }, // Keyboard b and B
    { 0x01, 0x02, 0x00, 0x00, 0x06, 0x01, 0x02, 0x00, 0x00, 0x86 }, // Keyboard c and C
    { 0x01, 0x02, 0x00, 0x00, 0x07, 0x01, 0x02, 0x00, 0x00, 0x87 }, // Keyboard d and D
    { 0x01, 0x02, 0x00, 0x00, 0x08, 0x01, 0x02, 0x00, 0x00, 0x88 }, // Keyboard e and E
    { 0x01, 0x02, 0x00, 0x00, 0x09, 0x01, 0x02, 0x00, 0x00, 0x89 }, // Keyboard f and F
    { 0x01, 0x02, 0x00, 0x00, 0x0a, 0x01, 0x02, 0x00, 0x00, 0x8a }, // Keyboard g and G
    { 0x01, 0x02, 0x00, 0x00, 0x0b, 0x01, 0x02, 0x00, 0x00, 0x8b }, // Keyboard h and H
    { 0x01, 0x02, 0x00, 0x00, 0x0c, 0x01, 0x02, 0x00, 0x00, 0x8c }, // Keyboard i and I
    { 0x01, 0x02, 0x00, 0x00, 0x0d, 0x01, 0x02, 0x00, 0x00, 0x8d }, // Keyboard j and J
    { 0x01, 0x02, 0x00, 0x00, 0x0e, 0x01, 0x02, 0x00, 0x00, 0x8e }, // Keyboard k and K
    { 0x01, 0x02, 0x00, 0x00, 0x0f, 0x01, 0x02, 0x00, 0x00, 0x8f }, // Keyboard l and L
    { 0x01, 0x02, 0x00, 0x00, 0x10, 0x01, 0x02, 0x00, 0x00, 0x90 }, // Keyboard m and M
    { 0x01, 0x02, 0x00, 0x00, 0x11, 0x01, 0x02, 0x00, 0x00, 0x91 }, // Keyboard n and N
    { 0x01, 0x02, 0x00, 0x00, 0x12, 0x01, 0x02, 0x00, 0x00, 0x92 }, // Keyboard o and O
    { 0x01, 0x02, 0x00, 0x00, 0x13, 0x01, 0x02, 0x00, 0x00, 0x93 }, // Keyboard p and P
    { 0x01, 0x02, 0x00, 0x00, 0x14, 0x01, 0x02, 0x00, 0x00, 0x94 }, // Keyboard q and Q
    { 0x01, 0x02, 0x00, 0x00, 0x15, 0x01, 0x02, 0x00, 0x00, 0x95 }, // Keyboard r and R
    { 0x01, 0x02, 0x00, 0x00, 0x16, 0x01, 0x02, 0x00, 0x00, 0x96 }, // Keyboard s and S
    { 0x01, 0x02, 0x00, 0x00, 0x17, 0x01, 0x02, 0x00, 0x00, 0x97 }, // Keyboard t and T
    { 0x01, 0x02, 0x00, 0x00, 0x18, 0x01, 0x02, 0x00, 0x00, 0x98 }, // Keyboard u and U
    { 0x01, 0x02, 0x00, 0x00, 0x19, 0x01, 0x02, 0x00, 0x00, 0x99 }, // Keyboard v and V
    { 0x01, 0x02, 0x00, 0x00, 0x1a, 0x01, 0x02, 0x00, 0x00, 0x9a }, // Keyboard w and W
    { 0x01, 0x02, 0x00, 0x00, 0x1b, 0x01, 0x02, 0x00, 0x00, 0x9b }, // Keyboard x and X
    { 0x01, 0x02, 0x00, 0x00, 0x1c, 0x01, 0x02, 0x00, 0x00, 0x9c }, // Keyboard y and Y
    { 0x01, 0x02, 0x00, 0x00, 0x1d, 0x01, 0x02, 0x00, 0x00, 0x9d }, // Keyboard z and Z
    { 0x01, 0x02, 0x00, 0x00, 0x1e, 0x01, 0x02, 0x00, 0x00, 0x9e }, // Keyboard 1 and !
    { 0x01, 0x02, 0x00, 0x00, 0x1f, 0x01, 0x02, 0x00, 0x00, 0x9f }, // Keyboard 2 and @
    { 0x01, 0x02, 0x00, 0x00, 0x20, 0x01, 0x02, 0x00, 0x00, 0xa0 }, // Keyboard 3 and #
    { 0x01, 0x02, 0x00, 0x00, 0x21, 0x01, 0x02, 0x00, 0x00, 0xa1 }, // Keyboard 4 and $
    { 0x01, 0x02, 0x00, 0x00, 0x22, 0x01, 0x02, 0x00, 0x00, 0xa2 }, // Keyboard 5 and %
    { 0x01, 0x02, 0x00, 0x00, 0x23, 0x01, 0x02, 0x00, 0x00, 0xa3 }, // Keyboard 6 and ^
    { 0x01, 0x02, 0x00, 0x00, 0x24, 0x01, 0x02, 0x00, 0x00, 0xa4 }, // Keyboard 7 and &
    { 0x01, 0x02, 0x00, 0x00, 0x25, 0x01, 0x02, 0x00, 0x00, 0xa5 }, // Keyboard 8 and *
    { 0x01, 0x02, 0x00, 0x00, 0x26, 0x01, 0x02, 0x00, 0x00, 0xa6 }, // Keyboard 9 and (
    { 0x01, 0x02, 0x00, 0x00, 0x27, 0x01, 0x02, 0x00, 0x00, 0xa7 }, // Keyboard 0 and )
    { 0x01, 0x02, 0x00, 0x00, 0x28, 0x01, 0x02, 0x00, 0x00, 0xa8 }, // Keyboard Return (ENTER)
    { 0x01, 0x02, 0x00, 0x00, 0x29, 0x01, 0x02, 0x00, 0x00, 0xa9 }, // Keyboard ESCAPE
    { 0x01, 0x02, 0x00, 0x00, 0x2a, 0x01, 0x02, 0x00, 0x00, 0xaa }, // Keyboard DELETE (Backspace)
    { 0x01, 0x02, 0x00, 0x00, 0x2b, 0x01, 0x02, 0x00, 0x00, 0xab }, // Keyboard Tab
    { 0x01, 0x02, 0x00, 0x00, 0x2c, 0x01, 0x02, 0x00, 0x00, 0xac }, // Keyboard Spacebar
    { 0x01, 0x02, 0x00, 0x00, 0x2d, 0x01, 0x02, 0x00, 0x00, 0xad }, // Keyboard - and _
    { 0x01, 0x02, 0x00, 0x00, 0x2e, 0x01, 0x02, 0x00, 0x00, 0xae }, // Keyboard = and +
    { 0x01, 0x02, 0x00, 0x00, 0x2f, 0x01, 0x02, 0x00, 0x00, 0xaf }, // Keyboard [ and {
    { 0x01, 0x02, 0x00, 0x00, 0x30, 0x01, 0x02, 0x00, 0x00, 0xb0 }, // Keyboard ] and }
    { 0x01, 0x02, 0x00, 0x00, 0x31, 0x01, 0x02, 0x00, 0x00, 0xb1 }, // Keyboard \ and |
    { 0x01, 0x02, 0x00, 0x00, 0x32, 0x01, 0x02, 0x00, 0x00, 0xb2 }, // Keyboard Non-US # and ~
    { 0x01, 0x02, 0x00, 0x00, 0x33, 0x01, 0x02, 0x00, 0x00, 0xb3 }, // Keyboard ; and :
    { 0x01, 0x02, 0x00, 0x00, 0x34, 0x01, 0x02, 0x00, 0x00, 0xb4 }, // Keyboard ' and "
    { 0x01, 0x02, 0x00, 0x00, 0x35, 0x01, 0x02, 0x00, 0x00, 0xb5 }, // Keyboard ` and ~
    { 0x01, 0x02, 0x00, 0x00, 0x36, 0x01, 0x02, 0x00, 0x00, 0xb6 }, // Keyboard , and <
    { 0x01, 0x02, 0x00, 0x00, 0x37, 0x01, 0x02, 0x00, 0x00, 0xb7 }, // Keyboard . and >
    { 0x01, 0x02, 0x00, 0x00, 0x38, 0x01, 0x02, 0x00, 0x00, 0xb8 }, // Keyboard / and ?
    { 0x01, 0x02, 0x00, 0x00, 0x39, 0x01, 0x02, 0x00, 0x00, 0xb9 }, // Keyboard Caps Lock
    { 0x01, 0x02, 0x00, 0x00, 0x3a, 0x01, 0x02, 0x00, 0x00, 0xba }, // Keyboard F1
    { 0x01, 0x02, 0x00, 0x00, 0x3b, 0x01, 0x02, 0x00, 0x00, 0xbb }, // Keyboard F2
    { 0x01, 0x02, 0x00, 0x00, 0x3c, 0x01, 0x02, 0x00, 0x00, 0xbc }, // Keyboard F3
    { 0x01, 0x02, 0x00, 0x00, 0x3d, 0x01, 0x02, 0x00, 0x00, 0xbd }, // Keyboard F4
    { 0x01, 0x02, 0x00, 0x00, 0x3e, 0x01, 0x02, 0x00, 0x00, 0xbe }, // Keyboard F5
    { 0x01, 0x02, 0x00, 0x00, 0x3f, 0x01, 0x02, 0x00, 0x00, 0xbf }, // Keyboard F6;
    { 0x01, 0x02, 0x00, 0x00, 0x40, 0x01, 0x02, 0x00, 0x00, 0xc0 }, // Keyboard F7
    { 0x01, 0x02, 0x00, 0x00, 0x41, 0x01, 0x02, 0x00, 0x00, 0xc1 }, // Keyboard F8
    { 0x01, 0x02, 0x00, 0x00, 0x42, 0x01, 0x02, 0x00, 0x00, 0xc2 }, // Keyboard F9
    { 0x01, 0x02, 0x00, 0x00, 0x43, 0x01, 0x02, 0x00, 0x00, 0xc3 }, // Keyboard F10
    { 0x01, 0x02, 0x00, 0x00, 0x44, 0x01, 0x02, 0x00, 0x00, 0xc4 }, // Keyboard F11
    { 0x01, 0x02, 0x00, 0x00, 0x45, 0x01, 0x02, 0x00, 0x00, 0xc5 }, // Keyboard F12
    { 0x01, 0x02, 0x00, 0x00, 0x46, 0x01, 0x02, 0x00, 0x00, 0xc6 }, // Keyboard Print Screen
    { 0x01, 0x02, 0x00, 0x00, 0x47, 0x01, 0x02, 0x00, 0x00, 0xc7 }, // Keyboard Scroll Lock
    { 0x01, 0x02, 0x00, 0x00, 0x48, 0x01, 0x02, 0x00, 0x00, 0xc8 }, // Keyboard Pause
    { 0x01, 0x02, 0x00, 0x00, 0x49, 0x01, 0x02, 0x00, 0x00, 0xc9 }, // Keyboard Insert
    { 0x01, 0x02, 0x00, 0x00, 0x4a, 0x01, 0x02, 0x00, 0x00, 0xca }, // Keyboard Home
    { 0x01, 0x02, 0x00, 0x00, 0x4b, 0x01, 0x02, 0x00, 0x00, 0xcb }, // Keyboard Page Up
    { 0x01, 0x02, 0x00, 0x00, 0x4c, 0x01, 0x02, 0x00, 0x00, 0xcc }, // Keyboard Delete Forward
    { 0x01, 0x02, 0x00, 0x00, 0x4d, 0x01, 0x02, 0x00, 0x00, 0xcd }, // Keyboard End
    { 0x01, 0x02, 0x00, 0x00, 0x4e, 0x01, 0x02, 0x00, 0x00, 0xce }, // Keyboard Page Down
    { 0x01, 0x02, 0x00, 0x00, 0x4f, 0x01, 0x02, 0x00, 0x00, 0xcf }, // Keyboard Right Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x50, 0x01, 0x02, 0x00, 0x00, 0xd0 }, // Keyboard Left Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x51, 0x01, 0x02, 0x00, 0x00, 0xd1 }, // Keyboard Down Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x52, 0x01, 0x02, 0x00, 0x00, 0xd2 }, // Keyboard Up Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x53, 0x01, 0x02, 0x00, 0x00, 0xd3 }, // Keyboard Num Lock and Clear
    { 0x01, 0x02, 0x00, 0x00, 0x54, 0x01, 0x02, 0x00, 0x00, 0xd4 }, // Keypad /
    { 0x01, 0x02, 0x00, 0x00, 0x55, 0x01, 0x02, 0x00, 0x00, 0xd5 }, // Keypad *
    { 0x01, 0x02, 0x00, 0x00, 0x56, 0x01, 0x02, 0x00, 0x00, 0xd6 }, // Keypad -
    { 0x01, 0x02, 0x00, 0x00, 0x57, 0x01, 0x02, 0x00, 0x00, 0xd7 }, // Keypad +
    { 0x01, 0x02, 0x00, 0x00, 0x58, 0x01, 0x02, 0x00, 0x00, 0xd8 }, // Keypad ENTER
    { 0x01, 0x02, 0x00, 0x00, 0x59, 0x01, 0x02, 0x00, 0x00, 0xd9 }, // Keypad 1 and End
    { 0x01, 0x02, 0x00, 0x00, 0x5a, 0x01, 0x02, 0x00, 0x00, 0xda }, // Keypad 2 and Down Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x5b, 0x01, 0x02, 0x00, 0x00, 0xdb }, // Keypad 3 and PageDn
    { 0x01, 0x02, 0x00, 0x00, 0x5c, 0x01, 0x02, 0x00, 0x00, 0xdc }, // Keypad 4 and Left Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x5d, 0x01, 0x02, 0x00, 0x00, 0xdd }, // Keypad 5
    { 0x01, 0x02, 0x00, 0x00, 0x5e, 0x01, 0x02, 0x00, 0x00, 0xde }, // Keypad 6 and Right Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x5f, 0x01, 0x02, 0x00, 0x00, 0xdf }, // Keypad 7 and Home
    { 0x01, 0x02, 0x00, 0x00, 0x60, 0x01, 0x02, 0x00, 0x00, 0xe0 }, // Keypad 8 and Up Arrow
    { 0x01, 0x02, 0x00, 0x00, 0x61, 0x01, 0x02, 0x00, 0x00, 0xe1 }, // Keypad 9 and Page Up
    { 0x01, 0x02, 0x00, 0x00, 0x62, 0x01, 0x02, 0x00, 0x00, 0xe2 }, // Keypad 0 and Insert
    { 0x01, 0x02, 0x00, 0x00, 0x63, 0x01, 0x02, 0x00, 0x00, 0xe3 }, // Keypad . and Delete
    { 0x01, 0x02, 0x00, 0x00, 0x64, 0x01, 0x02, 0x00, 0x00, 0xe4 }, // Keyboard Non-US \ and |
    { 0x01, 0x02, 0x00, 0x00, 0x65, 0x01, 0x02, 0x00, 0x00, 0xe5 }, // Keyboard Application
    { 0x01, 0x02, 0x00, 0x00, 0x66, 0x01, 0x02, 0x00, 0x00, 0xe6 }, // Keyboard Power
    { 0x01, 0x02, 0x00, 0x00, 0x67, 0x01, 0x02, 0x00, 0x00, 0xe7 }, // Keypad =
    { 0x01, 0x02, 0x00, 0x00, 0x68, 0x01, 0x02, 0x00, 0x00, 0xe8 }, // Keyboard F13
    { 0x01, 0x02, 0x00, 0x00, 0x69, 0x01, 0x02, 0x00, 0x00, 0xe9 }, // Keyboard F14
    { 0x01, 0x02, 0x00, 0x00, 0x6a, 0x01, 0x02, 0x00, 0x00, 0xea }, // Keyboard F15
    { 0x01, 0x02, 0x00, 0x00, 0x6b, 0x01, 0x02, 0x00, 0x00, 0xeb }, // Keyboard F16
    { 0x01, 0x02, 0x00, 0x00, 0x6c, 0x01, 0x02, 0x00, 0x00, 0xec }, // Keyboard F17
    { 0x01, 0x02, 0x00, 0x00, 0x6d, 0x01, 0x02, 0x00, 0x00, 0xed }, // Keyboard F18
    { 0x01, 0x02, 0x00, 0x00, 0x6e, 0x01, 0x02, 0x00, 0x00, 0xee }, // Keyboard F19
    { 0x01, 0x02, 0x00, 0x00, 0x6f, 0x01, 0x02, 0x00, 0x00, 0xef }, // Keyboard F20
    { 0x01, 0x02, 0x00, 0x00, 0x70, 0x01, 0x02, 0x00, 0x00, 0xf0 }, // Keyboard F21
    { 0x01, 0x02, 0x00, 0x00, 0x71, 0x01, 0x02, 0x00, 0x00, 0xf1 }, // Keyboard F22
    { 0x01, 0x02, 0x00, 0x00, 0x72, 0x01, 0x02, 0x00, 0x00, 0xf2 }, // Keyboard F23
    { 0x01, 0x02, 0x00, 0x00, 0x73, 0x01, 0x02, 0x00, 0x00, 0xf3 }, // Keyboard F24
    { 0x01, 0x02, 0x00, 0x00, 0x74, 0x01, 0x02, 0x00, 0x00, 0xf4 }, // Keyboard Execute
    { 0x01, 0x02, 0x00, 0x00, 0x75, 0x01, 0x02, 0x00, 0x00, 0xf5 }, // Keyboard Help
    { 0x01, 0x02, 0x00, 0x00, 0x76, 0x01, 0x02, 0x00, 0x00, 0xf6 }, // Keyboard Menu
    { 0x01, 0x02, 0x00, 0x00, 0x77, 0x01, 0x02, 0x00, 0x00, 0xf7 }, // Keyboard Select
    { 0x01, 0x02, 0x00, 0x00, 0x78, 0x01, 0x02, 0x00, 0x00, 0xf8 }, // Keyboard Stop
    { 0x01, 0x02, 0x00, 0x00, 0x79, 0x01, 0x02, 0x00, 0x00, 0xf9 }, // Keyboard Again
    { 0x01, 0x02, 0x00, 0x00, 0x7a, 0x01, 0x02, 0x00, 0x00, 0xfa }, // Keyboard Undo
    { 0x01, 0x02, 0x00, 0x00, 0x7b, 0x01, 0x02, 0x00, 0x00, 0xfb }, // Keyboard Cut
    { 0x01, 0x02, 0x00, 0x00, 0x7c, 0x01, 0x02, 0x00, 0x00, 0xfc }, // Keyboard Copy
    { 0x01, 0x02, 0x00, 0x00, 0x7d, 0x01, 0x02, 0x00, 0x00, 0xfd }, // Keyboard Paste
    { 0x01, 0x02, 0x00, 0x00, 0x7e, 0x01, 0x02, 0x00, 0x00, 0xfe }, // Keyboard Find
    { 0x01, 0x02, 0x00, 0x00, 0x7f, 0x01, 0x02, 0x00, 0x00, 0xff }, // Keyboard Mute
};

inline static const int key_index(char key) {
    switch (key) {
    case 'a' ... 'z':
        return key - 'a';
    case '1' ... '9':
        return key - '1' + 26;
    case '0':
        return 35;
    case '\r':
    case '\n':
        return 36;
    case '\033':
        return 37;
    case '\b':
    case 127:
        return 38;
    case '\t':
        return 39;
    case ' ':
        return 40;
    case '-':
        return 41;
    case '=':
        return 42;
    case '[':
        return 43;
    case ']':
        return 44;
    case '\\':
        return 45;
    case ';':
        return 47;
    case '\'':
        return 48;
    case '`':
        return 49;
    case ',':
        return 50;
    case '.':
        return 51;
    case '/':
        return 52;
    default:
        return -1;
    }
}

inline static const unsigned char* key_press(char key) {
    if (key == 7)
        return hid_shift_keys[40]; // shift space for switching IME
    switch (key) {
    case 'a' ... 'z':
        return hid_keys[key - 'a'];
    case 'A' ... 'Z':
        return hid_shift_keys[key - 'A'];
    case '1' ... '9':
        return hid_keys[key - '1' + 26];
    case '!':
        return hid_shift_keys[26];
    case '@':
        return hid_shift_keys[27];
    case '#':
        return hid_shift_keys[28];
    case '$':
        return hid_shift_keys[29];
    case '%':
        return hid_shift_keys[30];
    case '^':
        return hid_shift_keys[31];
    case '&':
        return hid_shift_keys[32];
    case '*':
        return hid_shift_keys[33];
    case '(':
        return hid_shift_keys[34];
    case '0':
        return hid_keys[35];
    case ')':
        return hid_shift_keys[35];
    case '\r':
    case '\n':
        return hid_keys[36];
    case '\033':
        return hid_keys[37];
    case '\b':
    case 127:
        return hid_keys[38];
    case '\t':
        return hid_keys[39];
    case ' ':
        return hid_keys[40];
    case '-':
        return hid_keys[41];
    case '_':
        return hid_shift_keys[41];
    case '=':
        return hid_keys[42];
    case '+':
        return hid_shift_keys[42];
    case '[':
        return hid_keys[43];
    case '{':
        return hid_shift_keys[43];
    case ']':
        return hid_keys[44];
    case '}':
        return hid_shift_keys[44];
    case '\\':
        return hid_keys[45];
    case '|':
        return hid_shift_keys[45];
    case ';':
        return hid_keys[47];
    case ':':
        return hid_shift_keys[47];
    case '\'':
        return hid_keys[48];
    case '"':
        return hid_shift_keys[48];
    case '`':
        return hid_keys[49];
    case '~':
        return hid_shift_keys[49];
    case ',':
        return hid_keys[50];
    case '<':
        return hid_shift_keys[50];
    case '.':
        return hid_keys[51];
    case '>':
        return hid_shift_keys[51];
    case '/':
        return hid_keys[52];
    case '?':
        return hid_shift_keys[52];
    case 1:
        return hid_keys[70]; // home
    // case 4:
    //     return hid_keys[77]; // down
    // case 5:
    //     return hid_keys[78]; // up
    case 6:
        return hid_keys[75]; // right
    case 2:
        return hid_keys[76]; // left
    case 5:
        return hid_keys[73]; // end
    case 4:
        return hid_keys[72]; // delete
    case 12:
        return hid_keys[69]; // insert
    default:
        return hid_keys_null;
    }
}

static const unsigned char REPORT_DESC[] = {
    0x05, 0x01, 0x09, 0x06, // Usage Page (Generic Desktop),
    0xA1, 0x01, // Collection (Application)
    0x85, 0x01, // Report ID (1)
    0x05, 0x07, // Usage (Keyboard)
    0x19, 0xE0, // Usage Page (Key Codes);
    0x29, 0xE7, // Usage Minimum (224),
    0x15, 0x00, // Usage Maximum (231),
    0x25, 0x01, // Logical Minimum (0),
    0x75, 0x01, // Logical Maximum (1),
    0x95, 0x08, // Report Size (1),
    0x81, 0x02, // Report Count (8),
    0x95, 0x01, // Input (Data, Variable, Absolute),
    0x75, 0x08, // Report Count (1),
    0x81, 0x01, // Report Size (8),
    0x95, 0x05, // Input (Constant),
    0x75, 0x01, // Report Count (5),
    0x05, 0x08, // Report Size (1),
    0x19, 0x01, // Usage Page (Page# for LEDs),
    0x29, 0x05, // Usage Minimum (1),
    0x91, 0x02, // Usage Maximum (5),
    0x95, 0x01, // Output (Data, Variable, Absolute),
    0x75, 0x03, // Report Count (1),
    0x91, 0x01, // Report Size (3),
    0x95, 0x06, // Output (Constant),
    0x75, 0x08, // Report Count (6),
    0x15, 0x00, // Report Size (8),
    0x25, 0x65, // Logical Minimum (0),
    0x05, 0x07, // Logical Maximum(101),
    0x19, 0x00, // Usage Page (Key Codes),
    0x29, 0x65, // Usage Minimum (0),
    0x81, 0x00, // Usage Maximum (101), #Input (Data, Array),
    0xC0, // End Collection
    0x05, 0x0C, // Usage Page (Consumer)
    0x09, 0x01, // Usage (Consumer Control)
    0xA1, 0x01, // Collection (Application)
    0x85, 0x02, //   Report ID (2)
    0x05, 0x0C, //   Usage Page (Consumer)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x01, //   Logical Maximum (1)
    0x75, 0x01, //   Report Size (1)
    0x95, 0x08, //   Report Count (7)
    0x09, 0xB5, //   Usage (Scan Next Track)
    0x09, 0xB6, //   Usage (Scan Previous Track)
    0x09, 0xB7, //   Usage (Stop)
    0x09, 0xB8, //   Usage (Eject)
    0x09, 0xCD, //   Usage (Play/Pause)
    0x09, 0xE2, //   Usage (Mute)
    0x09, 0xE9, //   Usage (Volume Increment)
    0x09, 0xEA, //   Usage (Volume Decrement)
    0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0, // End Collection
};
#define REPORT_DESC_SIZE (sizeof(REPORT_DESC) / sizeof(REPORT_DESC[0]))
#define HID_EVENT_SIZE 5

// <https://source.android.com/devices/accessories/aoa2#hid-support>
#define AOA_REGISTER_HID 54
#define AOA_UNREGISTER_HID 55
#define AOA_SET_HID_REPORT_DESC 56
#define AOA_SEND_HID_EVENT 57

#define DEFAULT_TIMEOUT 1000

inline static void print_libusb_error(enum libusb_error errcode) {
    fprintf(stderr, "%s\n", libusb_strerror(errcode));
}

inline static libusb_device* find_device(uint16_t vid, uint16_t pid) {
    libusb_device** list;
    libusb_device* found = NULL;
    ssize_t cnt = libusb_get_device_list(NULL, &list);
    ssize_t i = 0;
    if (cnt < 0) {
        print_libusb_error((enum libusb_error)cnt);
        return NULL;
    }
    for (i = 0; i < cnt; ++i) {
        libusb_device* device = list[i];
        struct libusb_device_descriptor desc;
        libusb_get_device_descriptor(device, &desc);
        if (vid == desc.idVendor && pid == desc.idProduct) {
            libusb_ref_device(device);
            found = device;
            break;
        }
    }
    libusb_free_device_list(list, 1);
    return found;
}

inline static int register_hid(libusb_device_handle* handle, uint16_t descriptor_size) {
    const uint8_t requestType = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR;
    const uint8_t request = AOA_REGISTER_HID;
    // <https://source.android.com/devices/accessories/aoa2.html#hid-support>
    // value (arg0): accessory assigned ID for the HID device
    // index (arg1): total length of the HID report descriptor
    const uint16_t value = 0;
    const uint16_t index = descriptor_size;
    unsigned char* const buffer = NULL;
    const uint16_t length = 0;
    const unsigned int timeout = DEFAULT_TIMEOUT;
    int r = libusb_control_transfer(
        handle, requestType, request, value, index, buffer, length, timeout);
    if (r < 0) {
        print_libusb_error((enum libusb_error)r);
        return 1;
    }
    return 0;
}

inline static int send_hid_descriptor(libusb_device_handle* handle, const unsigned char* descriptor,
    uint16_t size, uint8_t max_packet_size_0) {
    const uint8_t requestType = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR;
    const uint8_t request = AOA_SET_HID_REPORT_DESC;
    // <https://source.android.com/devices/accessories/aoa2.html#hid-support>
    // value (arg0): accessory assigned ID for the HID device
    const uint16_t value = 0;
    // libusb_control_transfer expects non-const but should not modify it
    unsigned char* const buffer = (unsigned char*)descriptor;
    const unsigned int timeout = DEFAULT_TIMEOUT;
    /*
     * If the HID descriptor is longer than the endpoint zero max packet size,
     * the descriptor will be sent in multiple ACCESSORY_SET_HID_REPORT_DESC
     * commands. The data for the descriptor must be sent sequentially
     * if multiple packets are needed.
     *
     * <https://source.android.com/devices/accessories/aoa2.html#hid-support>
     */
    // index (arg1): offset of data (buffer) in descriptor
    uint16_t offset = 0;
    while (offset < size) {
        uint16_t packet_length = size - offset;
        if (packet_length > max_packet_size_0) {
            packet_length = max_packet_size_0;
        }
        int r = libusb_control_transfer(
            handle, requestType, request, value, offset, buffer + offset, packet_length, timeout);
        offset += packet_length;
        if (r < 0) {
            print_libusb_error((enum libusb_error)r);
            return 1;
        }
    }

    return 0;
}

inline static int send_hid_event(
    libusb_device_handle* handle, const unsigned char* event, uint16_t size) {
    const uint8_t requestType = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR;
    const uint8_t request = AOA_SEND_HID_EVENT;
    // <https://source.android.com/devices/accessories/aoa2.html#hid-support>
    // value (arg0): accessory assigned ID for the HID device
    // index (arg1): 0 (unused)
    const uint16_t value = 0;
    const uint16_t index = 0;
    // libusb_control_transfer expects non-const but should not modify it
    unsigned char* const buffer = (unsigned char*)event;
    const uint16_t length = size;
    const unsigned int timeout = DEFAULT_TIMEOUT;
    int r = libusb_control_transfer(
        handle, requestType, request, value, index, buffer, length, timeout);
    if (r < 0) {
        print_libusb_error((enum libusb_error)r);
        return 1;
    }
    return 0;
}
