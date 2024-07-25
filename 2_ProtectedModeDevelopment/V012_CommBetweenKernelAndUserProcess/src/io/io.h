#ifndef IO_H
#define IO_H

#define PIC_MASTER_ADD  0x20
#define PIC_SLAVE_ADD   0xA0

#define PIC_TIMER_INT    (PIC_MASTER_ADD + 0)
#define PIC_KEYBRD_INT   (PIC_MASTER_ADD + 1)
#define PIC_CASC_INT     (PIC_MASTER_ADD + 2)
#define PIC_COM2_INT     (PIC_MASTER_ADD + 3)
#define PIC_COM1_INT     (PIC_MASTER_ADD + 4)
#define PIC_LPT2_INT     (PIC_MASTER_ADD + 5)
#define PIC_FLPDISK_INT  (PIC_MASTER_ADD + 6)
#define PIC_LPT1_INT     (PIC_MASTER_ADD + 7)

#define PIC_RTCLK_INT    (PIC_SLAVE_ADD + 0)
#define PIC_PERIPH1_INT  (PIC_SLAVE_ADD + 1)
#define PIC_PERIPH2_INT  (PIC_SLAVE_ADD + 2)
#define PIC_PERIPH3_INT  (PIC_SLAVE_ADD + 3)
#define PIC_MOUSEPS_INT  (PIC_SLAVE_ADD + 4)
#define PIC_COPROC_INT   (PIC_SLAVE_ADD + 5)
#define PIC_ATAPRIM_INT  (PIC_SLAVE_ADD + 6)
#define PIC_ATASEC_INT   (PIC_SLAVE_ADD + 7)

unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);

#endif