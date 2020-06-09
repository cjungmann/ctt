#ifndef CUSSES_H
#define CUSSES_H

void ctt_clear(void);
void ctt_set_cursor(int row, int col);
void ctt_get_cursor(int *row, int *col);
void ctt_get_screen_size(int *rows, int *cols);

void ctt_cursor_right(int chars);

#endif
