# pok3r-dec
Vortex Pok3r firmware decoder

Inside Pok3r firmware updaters there is a block starting from 0x001a3800, that is 
basically firmware data. To extract it one can use 

```dd if=POK3R_V113.exe bs=1 skip=$((0x001a3800)) of=data```

This simple C program can decrypt it.

Algorithm is fairy simple:
- Read block of 10 bytes
- Swap bytes:  1st with 5th and 6th with 10th
- Swap odd bytes with even bytes
- For each byte : sub 7, rol 4
- For each byte : if >= 0x90 add 1 to low nibble (modulo 16 i.e 0xAF becomes 0xA0, not 0xB0)

compile is trivial, but tested only on Linux : `gcc dec.c -o dec`

Note. There two pieces of data in block mentioned, these must be decoded separately.

# References
- https://geekhack.org/index.php?topic=72262.0 - forum thread discussing Pok3r firmware hacking, used it and links to materials (big thanks to Spritesmod)
- http://www.holtek.com.tw/productdetail?p_p_auth=yxkUpU1U&p_p_id=productvgpageportlet_WAR_holtekProdportlet&p_p_lifecycle=0&p_p_state=maximized&p_p_mode=view&_productvgpageportlet_WAR_holtekProdportlet_virtualGroupId=180
- http://radare.org/r/ - musthave tool for re


# RE progress


Found protection code that forbits reading flash (v1.1.3):

```
[0x00000000]> s 0x4350
[0x00004350]> pd 12
            0x00004350      bde8f09f       pop.w {r4, r5, r6, r7, r8, sb, sl, fp, ip, pc}
            0x00004354      a4f52051       sub.w r1, r4, 0x2800
            0x00004358      b1f5806f       cmp.w r1, 0x400
        ,=< 0x0000435c      08d3           blo 0x4370                  ; allow read
        |   0x0000435e      b4f5784f       cmp.w r4, 0xf800
       ,==< 0x00004362      05d2           bhs 0x4370
       ||   0x00004364      104a           ldr r2, [pc, 0x40]          ; [0x43a8:4]=0x1fef0850
       ||   0x00004366      a4f57841       sub.w r1, r4, 0xf800
       ||   0x0000436a      9142           cmp r1, r2
      ,===< 0x0000436c      00d3           blo 0x4370
      |||   0x0000436e      0020           movs r0, 0 ; <========== zero out data, replace me with a NOP !
      ```-> 0x00004370      4bf82500       str.w r0, [fp, r5, lsl 2]
[0x00004350]>

```

Same protection for v1.1.5


```
[0x000045a4]> pd 12
            0x000045a4      a4f52051       sub.w r1, r4, 0x2800
            0x000045a8      b1f5806f       cmp.w r1, 0x400
        ,=< 0x000045ac      08d3           blo 0x45c0
        |   0x000045ae      b4f5784f       cmp.w r4, 0xf800
       ,==< 0x000045b2      05d2           bhs 0x45c0
       ||   0x000045b4      104a           ldr r2, [pc, 0x40]          ; [0x45f8:4]=0x1fef0850
       ||   0x000045b6      a4f57841       sub.w r1, r4, 0xf800
       ||   0x000045ba      9142           cmp r1, r2
      ,===< 0x000045bc      00d3           blo 0x45c0
      |||   0x000045be      0020           movs r0, 0
      ```-> 0x000045c0      4bf82500       str.w r0, [fp, r5, lsl 2]
            0x000045c4      6d1c           adds r5, r5, 1
[0x000045a4]>

```

havent tried other firmware versions yet, but looks like `sub.w r1, r4, 0x2800`
is a good starting point to find it.
