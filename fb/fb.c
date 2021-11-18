#include "mailbox.h"
#include "fb.h"
#include "printf.h"
#include "utils.h"
#include "peripherals/base.h"
#include "terminal.h"

// the buffer must be 16-byte aligned as only upper 28bits of the address can be passed via the mailbox
static volatile unsigned int __attribute__((aligned(16))) mbox[36];

static unsigned int width, height, pitch, isrgb;
static unsigned char *fb;

void fb_init()
{
    mbox[0] = 35 * 4;       // length of message in bytes
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_SETPHYWH; // tag identifier
    mbox[3] = 8;    // value size in bytes
    mbox[4] = 0;
    mbox[5] = 1920; // FrameBufferInfo.width
    mbox[6] = 1080; // FrameBufferInfo.height

    mbox[7] = MBOX_TAG_SETVIRTWH;
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = mbox[5];  // FrameBufferInfo.virtual_width
    mbox[11] = mbox[6];  // FrameBufferInfo.virtual_height

    mbox[12] = MBOX_TAG_SETVIRTOFF;
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0; // FrameBufferInfo.x_offset
    mbox[16] = 0; // FrameBufferInfo.y.offset

    mbox[17] = MBOX_TAG_SETDEPTH;
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32; // FrameBufferInfo.depth

    mbox[21] = MBOX_TAG_SETPXLORDR;
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1; // RGB

    mbox[25] = MBOX_TAG_GETFB;
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = (unsigned int)__PA((unsigned long)&fb_start); // FrameBufferInfo.pointer
    mbox[29] = 0;    // FrameBufferInfo.size

    mbox[30] = MBOX_TAG_GETPITCH;
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0; // FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    // check call is successful and we have a pointer with depth 32
    if (mbox_call(MBOX_CH_PROP, mbox) && mbox[20] == 32 && mbox[28]) {
        mbox[28] &= 0x3fffffff;  // convert GPU address to ARM address
        width  = mbox[5];       // actual physical width
        height = mbox[6];       // actual physical height
        pitch  = mbox[33];       // number of bytes per line
        isrgb  = mbox[24];       // pixel order
        fb     = (unsigned char *)__VA(mbox[28]);
    } else {
        printf("failed to set screen resolution to 1920 x 1080\r\n");
    }
}

void draw_pixel(int x, int y, unsigned char attr)
{
    int off = (y * pitch) + (x * 4);   // 4: ARGB
    *((unsigned int*)(fb + off)) = vgapal[attr & 0x0f];
}

void draw_rect(int x1, int y1, int x2, int y2, unsigned char attr, int fill)
{
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                draw_pixel(x, y, attr);
            else if (fill)
                draw_pixel(x, y, (attr & 0xf0) >> 4);
       }
    }
}

void draw_line(int x1, int y1, int x2, int y2, unsigned char attr)  
{  
    int dx = x2 - x1;
    int dy = y2 - y1;
    int p = 2 * dy - dx;

    for (int x = x1, y = y1; x < x2; x++) {
        draw_pixel(x, y, attr);
        if (p >= 0) {
            p += (2 * dy - 2 * dx);
            y++;
        } else {
            p += 2 * dy;
        }
    }
}

void draw_char(unsigned char ch, int x, int y, unsigned char attr)
{
    unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

    for (int i = 0; i < FONT_HEIGHT; i++) {
        for (int j = 0; j < FONT_WIDTH; j++) {
            unsigned char mask = 1 << j;
            unsigned char col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;

            draw_pixel(x + j, y + i, col);
        }

        glyph += FONT_BPL;
    }
}

void draw_str(int x, int y, char *s, unsigned char attr)
{
    char ch;

    while ((ch = *s++)) {
        switch (ch) {
        case '\r':
            x = 0;
            break;
        case '\n':
            x = 0;
            y += FONT_HEIGHT;
            break;
        default:
            draw_char(ch, x, y, attr);
            x += FONT_WIDTH;
        }
    }
}