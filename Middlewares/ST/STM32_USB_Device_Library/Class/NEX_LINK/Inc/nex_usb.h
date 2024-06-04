/*

The MIT License (MIT)

Copyright (c) 2016 Hubert Denkmair

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#pragma once

#define u32 uint32_t
#define u8 uint8_t

#define GSUSB_ENDPOINT_IN          0x81
#define GSUSB_ENDPOINT_OUT         0x02

enum nex_usb_breq {
        NEX_BREQ_HOST_FORMAT = 0,
        NEX_TIMESTAMP_SET,
        NEX_TIMESTAMP_GET,
        NEX_COMMAND_LEN,
};

enum nex_link_mode {
	/* reset a channel. turns it off */
	NEX_LINK_MODE_RESET = 0,
	/* starts a channel */
	NEX_LINK_MODE_START
};

enum nex_link_state {
	NEX_LINK_STATE_ERROR_ACTIVE = 0,
	NEX_LINK_STATE_ERROR_WARNING,
	NEX_LINK_STATE_ERROR_PASSIVE,
	NEX_LINK_STATE_BUS_OFF,
	NEX_LINK_STATE_STOPPED,
	NEX_LINK_STATE_SLEEPING
};
