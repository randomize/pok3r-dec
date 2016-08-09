/*
Decoder for Vortex Pok3r firmware. Firmware comes as windows PE,
containing block of data encrypted with some bit shifting and byte swapping,
this simple c program decrypts it.

Copyright © 2016 Eugene Mihailenco <mihailencoe@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <unistd.h>

#define SIZE_B 5


// Each byte decreased by 7 and cyclic shifted by 4bits, then
// for things greater or less then 0x90 we add 1 to low nibble
void decode(unsigned char* p, int len)
{
    for (int i = 0; i < len; i++)
    {
        unsigned char c = p[i];

        if (c >= 0x90)
            c = (c & 0xf0) | ((c + 15) & 0x0f);
        c = (c << 4) | (c >> 4);
        c += 7;

        p[i] = c;
    }
}

void swap(unsigned char* a, unsigned char* b)
{
    unsigned char c = *a; *a = *b; *b = c;
}


int main(int argc, const char* argv[])
{
    // Read block of 10 a time
    unsigned char ch[SIZE_B * 2];

    for (;;)
    {

        int t = read(0, ch, SIZE_B * 2);

        // Swap each even with odd bytes
        for (int i = 0; i < t - 1; i += 2)
        {
            swap(ch + i, ch + i + 1);
        }

        // Swapping 1st and 5th (and 6th and 10th if any) bytes
        if (t == SIZE_B * 2)
        {
            swap(ch, ch + SIZE_B - 1);
            swap(ch + SIZE_B, ch + SIZE_B * 2 - 1);
        }
        else if (t >= SIZE_B)
        {
            swap(ch, ch + SIZE_B - 1);
        }


        // Do bitshifting decryption
        decode(ch, t);

        write(1, ch, t);

        if (t != SIZE_B * 2)
            break;
    }
    return 0;
}
