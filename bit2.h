/*
 *     bit2.h
 *     by Jia Wen Goh (jgoh01) and Winston Tan (wtan02), 09/17/2017
 *
 *     Bit2 is a 2D representation of a Bit that stores bits in rows & columns.
 */

#ifndef BIT2_H_INCLUDED
#define BIT2_H_INCLUDED

#include <stdio.h>

#define T Bit2_T
typedef struct T *T;

/*
 *               Bit2.h
 *
 *   The 2D Bit (Bit2) is a 2D representation of a Bit that can store bits in
 *   rows & columns.
 *
 *   ---= Functions =---
 *      [Constructor/Destructor]
 *      - Bit2_new  - Creates Bit2 based on given dimensions
 *      - Bit2_free - Frees allocated memory of Bit2
 *
 *      [Properties]
 *      - Bit2_width  - Returns the width of the given Bit2
 *      - Bit2_height - Returns the height of the given Bit2
 *      - Bit2_size   - Returns the size of the stored contents
 *                      of the given Bit2
 *      - Bit2_get    - Returns the element at the given row/col
 *                      of the given Bit2
 *      - Bit2_put    - Puts the given bit into the given row/col of the
 *                      given Bit2, returning the existing value
 *
 *      [Mapping]
 *      - Bit2_map_row_major
 *      - Bit2_map_col_major
 *                     - These functions calls the given apply function for
 *                       each element in the Bit2. The function maps column
 *                       by column for Bit2_map_row_major, and row by row
 *                       for Bit2_map_col_major.
 *
 *   Errors:
 *     A Hanson exception must be raised if memory cannot be allocated for
 *     Bit2.
 */

extern T    Bit2_new  (int width, int height);
extern void Bit2_free (T *bit2);

extern int Bit2_width (T bit2);
extern int Bit2_height(T bit2);
extern int Bit2_count (T bit2);
extern int Bit2_get   (T bit2, int col, int row);
extern int Bit2_put   (T bit2, int col, int row, int bit);

extern void Bit2_map_row_major(T bit2, void apply(int w_index, int h_index,
                               T bit2, int bit, void* cl), void* cl);
extern void Bit2_map_col_major(T bit2, void apply(int w_index, int h_index,
                               T bit2, int bit, void* cl), void* cl);

#undef T
#endif
