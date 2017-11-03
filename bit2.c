/*
 *     bit2.c
 *     by Jia Wen Goh (jgoh01) and Winston Tan (wtan02), 09/17/2017
 *
 *     Bit2 is a 2D representation of a Bit that stores bits in rows & columns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <bit.h>
#include <bit2.h>
#include <assert.h>

/*
 * Bit2_T type that stores relevant information (i.e. dimensions and contents)
 */
#define T Bit2_T
struct T {
        int     width;
        int     height;
        Bit_T   array;
};

/*----------------------------------------------------------------*
 |                  Constructor/Destructor                        |
 *----------------------------------------------------------------*/
/*
 * [Name]:              Bit2_new
 * [Purpose]:           Creates Bit2 from given dimensions
 * [Parameters]:        2 int (dimensions)
 * [Returns]:           T (created Bit2)
 */
T Bit2_new(int width, int height)
{
        T bit2 = malloc(16);
        assert(bit2 != NULL);

        bit2->width  = width;
        bit2->height = height;
        bit2->array  = Bit_new(width * height);

        return bit2;
}

/*
 * [Name]:              Bit2_free
 * [Purpose]:           Frees allocated memory of Bit2
 * [Parameters]:        1 T* (pointer to the Bit2 to be freed)
 * [Returns]:           void
 */
void Bit2_free(T *bit2)
{
        Bit_T temp = (*bit2)->array;
        free(*bit2);
        Bit_free(&temp);
}

/*----------------------------------------------------------------*
 |                          Properties                            |
 *----------------------------------------------------------------*/
/*
 * [Name]:              Bit2_width
 * [Purpose]:           Returns the width of the given Bit2
 * [Parameters]:        1 T (the width of this Bit2 will be returned)
 * [Returns]:           int (width)
 */
int Bit2_width(T bit2)
{
        return bit2->width;
}

/*
 * [Name]:              Bit2_height
 * [Purpose]:           Returns the height of the given Bit2
 * [Parameters]:        1 T (the height of this Bit2 will be returned)
 * [Returns]:           int (height)
 */
int Bit2_height(T bit2)
{
        return bit2->height;
}

/*
 * [Name]:              Bit2_count
 * [Purpose]:           Returns the number of bits stored in the given Bit2
 * [Parameters]:        1 T (the # of bits of this Bit2 will be returned)
 * [Returns]:           int (# of bits)
 */
int Bit2_count(T bit2)
{
        return Bit_count(bit2->array);
}

/*
 * [Name]:              Bit2_get
 * [Purpose]:           Returns the element at the given col/row of the
 *                      given Bit2
 * [Parameters]:        1 T (referenced bit), 2 int (col/row index)
 * [Returns]:           int (bit at the given location)
 */
int Bit2_get(T bit2, int col, int row)
{
        int index = (row * bit2->width) + (col);
        return Bit_get(bit2->array, index);
}

/*
 * [Name]:              Bit2_put
 * [Purpose]:           Puts the given bit into the given col/row of the given
 *                      Bit2, returning the existing value
 * [Parameters]:        1 T (referenced bit), 3 int (col/row index, and bit to
 *                      be placed)
 * [Returns]:           int (previous bit at the given location)
 */
int Bit2_put(T bit2, int col, int row, int bit)
{
        int index = (row * bit2->width) + (col);
        int value = Bit_put(bit2->array, index, bit);
        return value;
}

/*----------------------------------------------------------------*
 |                          Mapping                               |
 *----------------------------------------------------------------*/
/*
 * [Name]:              Bit2_map_row_major
 * [Purpose]:           Calls the given apply function on each element in the
 *                      Bit2; the function will map row by row
 * [Parameters]:        1 T (referenced Bit2), apply function pointer,
 *                      1 void* (cl)
 * [Returns]:           void
 */
void Bit2_map_row_major(T bit2, void apply(int w_index, int h_index,
                        T bit2, int bit, void* cl), void* cl)
{
        for (int row = 0; row < bit2->height; row++) {
                for (int col = 0; col < bit2->width; col++) {
                        apply(col, row, bit2, Bit2_get(bit2, col, row),
                              cl);
                }
        }
}

/*
 * [Name]:              Bit2_map_col_major
 * [Purpose]:           Calls the given apply function on each element in the
 *                      Bit2; the function will map column by column
 * [Parameters]:        1 T (referenced Bit2), apply function pointer,
 *                      void* (cl)
 * [Returns]:           void
 */
void Bit2_map_col_major(T bit2, void apply(int w_index, int h_index,
                               T bit2, int bit, void* cl), void* cl)
{
        for (int col = 0; col < bit2->width; col++) {
                for (int row = 0; row < bit2->height; row++) {
                        apply(col, row, bit2, Bit2_get(bit2, col, row),
                        cl);
                }
        }
}