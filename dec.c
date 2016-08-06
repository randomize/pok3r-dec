

#include <unistd.h>
#include <stdio.h>

#define SIZE_B 5

unsigned char mapdec(unsigned char p)
{
    if (p >= 0x90)
        return (p & 0xf0) | ((p + 1) & 0x0f );
    else
        return p;
}

void decode(unsigned char* p, int len)
{
    for (int i = 0; i < len; i++) {

        unsigned char c = p[i];
        int x = c;
        x = x + 256 - 7;
        c = x; 

        c = (c << 4) | (c >> 4);
        p[i] = mapdec(c);

    }
}

void swap(char* a, char* b)
{
    char c = *a;
    *a = *b;
    *b = c;
}


int main(int argc, const char *argv[])
{
    unsigned char ch[ SIZE_B*2 ];

    /* read(0, ) */
    /* int t = read(0, ch, 2); */
    /* decode(ch, t); */
    /* write(1, ch, t); */

    for (;;)
    {

        int t = read(0, ch, SIZE_B*2);

        if (t == SIZE_B*2)
        {
            swap(ch, ch + SIZE_B - 1);
            swap(ch + SIZE_B, ch + SIZE_B*2 - 1);
        } else if (t >= SIZE_B) {
            swap(ch, ch + SIZE_B - 1);
        }

        for (int i = 0; i < t - 1; i+=2) {
            swap(ch + i, ch + i + 1);
        }
            

        decode(ch, t);


        write(1, ch, t);

        if (t != SIZE_B*2)
            break;

    }
    return 0;
}
