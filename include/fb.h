#ifndef __FB_H__
#define __FB_H__

extern unsigned long fb_start;

void fb_init();
void draw_pixel(int x, int y, unsigned char attr);
void draw_rect(int x1, int y1, int x2, int y2, unsigned char attr, int fill);
void draw_line(int x1, int y1, int x2, int y2, unsigned char attr);
void draw_char(unsigned char ch, int x, int y, unsigned char attr);
void draw_str(int x, int y, char *s, unsigned char attr);

#endif // !__FB_H__