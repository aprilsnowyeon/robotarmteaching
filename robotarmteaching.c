#include <18f25k22.h>
#include <ctype.h>

#fuses INTRC_IO, NOPLLEN, NOWDT, NOMCLR

#use delay(clock = 8000000)

#byte TRISA = 0xf92
#byte PORTA = 0xf80
#byte TRISB = 0xf93
#byte PORTB = 0xf81
#byte TRISC = 0xf94
#byte PORTC = 0xf82
#byte INTCON1 = 0Xff2
#byte INTCON2 = 0xff1
#byte INTCON3 = 0xff0

#bit motor0 = PORTB.2
#bit motor1 = PORTB.3
#bit motor2 = PORTC.3
#bit motor3 = PORTC.5

//PORTBinterrupt
#byte IOCB = 0xf62
#bit aa = PORTB.4
#bit bb = PORTB.5
#bit cc = PORTB.6
#bit dd = PORTB.7

//RB
#bit RBIE = INTCON1.3
#bit IOCB7 = IOCB.7
#bit IOCB6 = IOCB.6
#bit IOCB5 = IOCB.5
#bit IOCB4 = IOCB.4

//int0 int1
#bit INT0IE = INTCON1.4
#bit INT1IE = INTCON3.3
#bit INTEDG0 = INTCON2.6
#bit INTEDG1 = INTCON2.5
//232
#byte TXREG1 = 0xfad
#byte TXSTA1 = 0xfac
#byte RCSTA1 = 0xfab
#byte RCREG1 = 0xfae
#byte SPBRG1 = 0xfaf
#byte BAUDCON1 = 0xfb8
#byte PIE1 = 0xf9d

#bit TX9 = TXSTA1.6
#bit TXEN = TXSTA1.5
#bit SYNC = TXSTA1.4
#bit BRGH = TXSTA1.2
#bit BRG16 = BAUDCON1.3
#bit TRMT = TXSTA1.1
#bit SPEN = RCSTA1.7
#bit RX9 = RCSTA1.6
#bit CREN = RCSTA1.4

#bit RC1IE = PIE1.5
#bit GIE = INTCON1.7
#bit GIEL = INTCON1.6
//tmr0
#byte T0CON = 0xfd5
#byte TMR0L = 0xfd6

#bit TMR0IE = INTCON1.5
#bit TMR0ON = T0CON.7
#bit T08BIT = T0CON.6
#bit T0CS = T0CON.5
#bit TOPS0 = T0CON.0
#bit TOPS1 = T0CON.1
#bit TOPS2 = T0CON.2
#bit PSA = T0CON.3
//tmr1
#byte T1CON = 0xfcd
#byte T1GCON = 0xfcc
#byte TMR1H = 0xfce
#byte TMR1L = 0xfcf

#bit TMR1CS0 = T1CON.6
#bit TMR1CS1 = T1CON.7
#bit T1CKPS0 = T1CON.5
#bit T1CKPS1 = T1CON.6
#bit T1SYNC = T1CON.2
#bit TMR1GE = T1GCON.7
#bit TMR1IE = PIE1.0
#bit TMR1ON = T1CON.0
//CLCD
#bit RS = PORTC.0
#bit RW = PORTC.1
#bit E = PORTC.2

#define BIT8_LINE2_DOT58 0x38
#define DISPON_CUROFF_BLKOFF 0x0c
#define INC_NOSHIFT 0x06

#define DISPCLEAR 0x01
#define E_BIT 0x04

#define btnup (aa==0 && bb==1 && cc==1 && dd==1)
#define btndown (aa==1 && bb==0 && cc==1 && dd==1)
#define btnnext (aa==0 && bb==0 && cc==1 && dd==1)
#define btnback (aa==1 && bb==1 && cc==0 && dd==1)
#define btnok (aa==1 && bb==1 && cc==1 && dd==0)
#define btncancel (aa==1 && bb==1 && cc==0 && dd==0)
#define btnnormal (aa==1 && bb==1 && cc==1 && dd==1)

void init_rb()
{
    RBIE = 1;
    IOCB7 = 1;
    IOCB6 = 1;
    IOCB5 = 1;
    IOCB4 = 1;
}

void init_port()
{
    TX9 = 0;
    TXEN = 1;
    SYNC = 0;
    BRGH = 0;
    BRG16 = 0;
    TRMT = 1;
    SPEN = 1;
    RX9 = 0;
    CREN = 1;
    RC1IE = 0;
}

void interrupt()
{
    PORTA = 0x00;
    TRISA = 0x00;
    PORTB = 0x00;
    TRISB = 0xF3;
    PORTC = 0x00;
    TRISC = 0x80;
    INT0IE = 1;
    INT1IE = 1;
    INTEDG0 = 0;
    INTEDG1 = 0;
    GIEL = 1;
    GIE = 1;
}

void tmr0bit()
{
    INTEDG0 = 0;
    TMR0IE = 1;
    T08BIT = 1;
    T0CS = 0;
    TOPS0 = 0;
    TOPS1 = 0;
    TOPS2 = 0;
    PSA = 0;
    TMR0ON = 0;
}

void tmr1bit()
{
    TMR1CS0 = 0; //Fosc/4
    TMR1CS1 = 0;
    T1CKPS0 = 0; //1:1
    T1CKPS1 = 0;
    T1SYNC = 1;
    TMR1GE = 0;
    TMR1IE = 1;
    TMR1ON = 0;
}

void tx_data(char txr)
{
    while (TRMT == 0);
    TXREG1 = txr;
}

void tx_array(char* str)
{
    while (*str != 0x00)
    {
        tx_data(*str++);
    }
}

void CLCD_cmd(char cmd)
{
    RS = 0;
    RW = 0;
    delay_us(1);
    E = 1;
    PORTA = cmd;
    E = 0;
    delay_ms(1);
}

void CLCD_data(char data)
{
    RS = 1;
    delay_us(1);
    E = 1;
    PORTA = data;
    E = 0;
    delay_ms(1);
}

void CLCD_array(char* str)
{
    while (*str != 0x00)
    {
        CLCD_data(*str++);
    }
}

int mode = 0, tmr0mode = 0, ui0mode = 0, receivemode = 0, protocalnum = 0, totalmotionlong = 0, playcancelshort = 0, teachcancelshort = 0;
unsigned int16 tmr0count = 0, motortarget[4][50], motornow[4], Pprotocal[22], tmr1count = 0;
int  motionstep = 0;
int tmr1updown, tmr1okcancel, tmr1mode; //tmr1mode = 0 is updown group 1 is okcancel group

int selectT[] = "<< Teaching >>", selectP[] = "<< Playing >>", mode0uitext[] = "SelectMode :";
int waitingtext1[] = "Loading", waitingtext2[] = "SlotData", selectmotion[] = "SelectSlot : ", slotname[] = "Name : ", Aslotrdanum = 0, Aslotdata[55], selectmotionnum = 1, Aslot[] = "ASlotData", SlotData[] = "SlotData";
int mode2uitext1[] = "step ", mode2uitext2[] = " motor ", mode2uitext3[] = "Angle ", mode2uitext4[] = " SPD ", mode2uitext5[] = "Teaching Cancel";
int selectmotornum = 1, ui2angle[4], ui2step[2];
int16 motorangletext; //ui2 angle text bug fix... but why???????

int16 mode4motion, mode4complete = 0;
int mode4inttotext[4], mode4uitext1[] = "Data Sending...", mode4uitext2[] = "    ...Complete!";
int mode6uitext[] = "Recive data : ";
int mode7uitext1[] = "Data Roaded", mode7uitext2[] = "OK:S/P  Cancel:R", mode7uitext3[] = "Playing Cancel";

void motorset()
{
    tmr0mode = 0;
    TMR0ON = 1;
}

void mode0ui()
{
    mode = 0;
    for (int ii = 0; ii < 4; ii++)
    {
        motortarget[ii][0] = 500;
        for (int jj = 1; jj < 50; jj++)
            motortarget[ii][jj] = 0;
    }
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(mode0uitext);
    CLCD_cmd(0xc0);
    if (ui0mode == 0)
        CLCD_array(selectT);
    if (ui0mode == 1)
        CLCD_array(selectP);
}

void mode1ui()
{
    mode = 1;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(selectmotion);
    CLCD_data(selectmotionnum + 0x30);
    CLCD_data(' ');
    CLCD_data(Aslotdata[11 * selectmotionnum - 1]);
    CLCD_cmd(0xc0);
    CLCD_array(slotname);
    for (int jj = 1; jj < 10; jj++)
    {
        CLCD_data(Aslotdata[jj + (11 * (selectmotionnum - 1))]);
    }

    motionstep = 0;
}

void mode2ui()
{
    mode = 2;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(mode2uitext1);
    ui2step[0] = motionstep / 10 + 0x30;
    ui2step[1] = motionstep % 10 + 0x30;
    CLCD_data(ui2step[0]);
    CLCD_data(ui2step[1]);
    CLCD_array(mode2uitext2);
    CLCD_data(selectmotornum + 0x30);

    CLCD_cmd(0xc0);
    CLCD_array(mode2uitext3);
    motorangletext = motortarget[selectmotornum - 1][motionstep];
    ui2angle[0] = motorangletext / 1000 + 0x30;
    motorangletext %= 1000;
    ui2angle[1] = motorangletext / 100 + 0x30;
    motorangletext %= 100;
    ui2angle[2] = motorangletext / 10 + 0x30;
    ui2angle[3] = motorangletext % 10 + 0x30;
    CLCD_data(ui2angle[0]);
    CLCD_data(ui2angle[1]);
    CLCD_data(ui2angle[2]);
    CLCD_data(ui2angle[3]);

    motor0 = 1;
    delay_us(motortarget[0][motionstep]);
    motor0 = 0;
    delay_us(20000 - motortarget[0][motionstep]);
    motor1 = 1;
    delay_us(motortarget[1][motionstep]);
    motor1 = 0;
    delay_us(20000 - motortarget[1][motionstep]);
    motor2 = 1;
    delay_us(motortarget[2][motionstep]);
    motor2 = 0;
    delay_us(20000 - motortarget[2][motionstep]);
    motor3 = 1;
    delay_us(motortarget[3][motionstep]);
    motor3 = 0;
    delay_us(20000 - motortarget[3][motionstep]);
}

void mode3ui()
{
    mode = 3;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(waitingtext1);
    CLCD_cmd(0xc0);
    CLCD_array(waitingtext2);
    RC1IE = 1;
    tx_array(Aslot);
}

void mode4ui()
{
    mode = 4;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(mode4uitext1);
    for (int ii = 0; motortarget[0][ii] != 0; ii++)
    {
        if (ii == 0)
        {
            tx_data(selectmotionnum + 0x30);
            tx_data('T');
        }
        else
        {
            tx_data('N');
        }

        for (int jj = 0; jj < 4; jj++)
        {
            if (jj == 0)
                tx_data('a');
            if (jj == 1)
                tx_data('b');
            if (jj == 2)
                tx_data('c');
            if (jj == 3)
                tx_data('d');

            mode4motion = motortarget[jj][ii];
            mode4inttotext[0] = mode4motion / 1000 + 0x30;
            mode4motion %= 1000;
            mode4inttotext[1] = mode4motion / 100 + 0x30;
            mode4motion %= 100;
            mode4inttotext[2] = mode4motion / 10 + 0x30;
            mode4inttotext[3] = mode4motion % 10 + 0x30;
            tx_data(mode4inttotext[0]);
            tx_data(mode4inttotext[1]);
            tx_data(mode4inttotext[2]);
            tx_data(mode4inttotext[3]);
        }
        if (motortarget[0][ii + 1] == 0)
        {
            tx_data('S');
            for (int ii = 0; ii < 4; ii++)
                motornow[ii] = motortarget[ii][motionstep];
            tmr0mode = 0;
            motionstep = 0;
            RBIE = 0;
            INT0IE = 0;
            INT1IE = 0;
            TMR0ON = 1;
        }
        else
            tx_data('N');
    }
}

void mode5ui()
{
    mode = 5;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(selectmotion);
    CLCD_data(selectmotionnum + 0x30);
    CLCD_data(' ');
    CLCD_data(Aslotdata[11 * selectmotionnum - 1]);
    CLCD_cmd(0xc0);
    CLCD_array(slotname);
    for (int jj = 1; jj < 10; jj++)
    {
        CLCD_data(Aslotdata[jj + (11 * (selectmotionnum - 1))]);
    }

    motionstep = 0;
}

void mode6ui()
{
    mode = 6;
    tmr0mode = 1;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(mode6uitext);
    CLCD_data(selectmotionnum + 0x30);
    totalmotionlong = 0;
}

void mode7ui()
{
    mode = 7;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(mode7uitext1);
    CLCD_cmd(0xc0);
    CLCD_array(mode7uitext2);
}

void mode8ui()
{
    mode = 8;
    CLCD_cmd(0x01);
    CLCD_cmd(0x80);
    CLCD_array(waitingtext1);
    CLCD_cmd(0xc0);
    CLCD_array(waitingtext2);
    RC1IE = 1;
    tx_array(Aslot);
}

void main()
{
    delay_ms(500);

    init_rb();
    init_port();
    interrupt();
    tmr0bit();
    tmr1bit();

    SPBRG1 = 12;

    TMR0L = 56;
    TMR1H = 177;
    TMR1L = 224;

    delay_ms(50);

    CLCD_cmd(BIT8_LINE2_DOT58);
    CLCD_cmd(DISPON_CUROFF_BLKOFF);
    CLCD_cmd(INC_NOSHIFT);
    CLCD_cmd(DISPCLEAR);
    delay_ms(2);

    mode0ui();

    while (1)
    {
    }
}

#INT_RB
void int_rb0123()
{
    delay_ms(15);
    if (btnup)
    {
        if (mode == 2)
        {
            motortarget[selectmotornum - 1][motionstep] += 10;
            if (motortarget[selectmotornum - 1][motionstep] > 2500)
            {
                motortarget[selectmotornum - 1][motionstep] = 2500;
            }
            mode2ui();

            tmr1mode = 0;
            tmr1updown = 0;
            TMR1ON = 1;
        }
    }
    if (btndown)
    {
        if (mode == 2)
        {
            motortarget[selectmotornum - 1][motionstep] -= 10;
            if (motortarget[selectmotornum - 1][motionstep] < 500)
            {
                motortarget[selectmotornum - 1][motionstep] = 500;
            }
            mode2ui();

            tmr1mode = 0;
            tmr1updown = 1;
            TMR1ON = 1;
        }
    }
    if (btnnext)
    {
        if (mode == 0)
        {
            ui0mode++;
            if (ui0mode > 1)
                ui0mode = 0;
            mode0ui();
        }
        if (mode == 1)
        {
            selectmotionnum++;
            if (selectmotionnum > 5)
                selectmotionnum = 1;
            mode1ui();
        }

        if (mode == 2)
        {
            selectmotornum++;
            if (selectmotornum > 4)
                selectmotornum = 4;
            mode2ui();
        }
        if (mode == 5)
        {
            selectmotionnum++;
            if (selectmotionnum > 5)
                selectmotionnum = 1;
            mode5ui();
        }
    }
    if (btnback)
    {
        if (mode == 0)
        {
            ui0mode++;
            if (ui0mode > 1)
                ui0mode = 0;
            mode0ui();
        }
        if (mode == 1)
        {
            selectmotionnum--;
            if (selectmotionnum < 1)
                selectmotionnum = 5;
            mode1ui();
        }
        if (mode == 2)
        {
            selectmotornum--;
            if (selectmotornum < 1)
                selectmotornum = 1;
            mode2ui();
        }
        if (mode == 5)
        {
            selectmotionnum--;
            if (selectmotionnum < 1)
                selectmotionnum = 5;
            mode5ui();
        }
    }
    if (btnok)
    {
        if (mode == 0)
        {
            if (ui0mode == 0)
            {
                mode3ui();
            }
            if (ui0mode == 1)
                mode8ui();
        }
        else if (mode == 1)
        {
            mode2ui();
        }
        else if (mode == 2)
        {
            tmr1mode = 1;
            tmr1okcancel = 0;
            TMR1ON = 1;
        }
        else if (mode == 5)
        {
            mode6ui();
            RC1IE = 1;
            tx_data(selectmotionnum + 0x30);
            tx_array(SlotData);
        }
        else if (mode == 7)
        {
            tmr0mode = 1;
            if (TMR0ON == 0)
                TMR0ON = 1;
            else if (TMR0ON == 1)
                TMR0ON = 0;
        }
    }
    if (btncancel)
    {
        if (mode != 2 && mode != 7)
        {
            mode = 0;
            mode0ui();
        }
        else if (mode == 2)
        {
            tmr1mode = 1;
            tmr1okcancel = 1;
            TMR1ON = 1;
        }
        else if (mode == 7)
        {
            TMR0ON = 0;
            tmr1mode = 1;
            tmr1okcancel = 1;
            TMR1ON = 1;
        }
    }
}

#INT_EXT
void ext0()
{
    delay_ms(15);
    if (mode == 2)
    {
        motortarget[1][motionstep] += 20;
        motortarget[2][motionstep] += 20;
        mode2ui();
    }
}

#INT_EXT1
void ext1()
{
    delay_ms(15);
    if (mode == 2)
    {
        motortarget[1][motionstep] -= 20;
        motortarget[2][motionstep] -= 20;
        mode2ui();
    }
}

#INT_RDA
void rda()
{
    if (mode == 3)
    {
        Aslotdata[Aslotrdanum] = RCREG1;
        Aslotrdanum++;
        if (Aslotrdanum == 55)
        {
            RC1IE = 0;
            Aslotrdanum = 0;
            mode1ui();
        }
    }
    if (mode == 8)
    {
        Aslotdata[Aslotrdanum] = RCREG1;
        Aslotrdanum++;
        if (Aslotrdanum == 55)
        {
            RC1IE = 0;
            Aslotrdanum = 0;
            mode5ui();
        }
    }
    if (mode == 6)
    {
        Pprotocal[protocalnum] = RCREG1;
        protocalnum++;
        if (receivemode == 0)
        {
            if (Pprotocal[0] != 'P')
            {
                protocalnum = 0;
            }
            else if (protocalnum == 22 && Pprotocal[0] == 'P')
            {
                motortarget[0][totalmotionlong] = (Pprotocal[2] - 0x30) * 1000;
                motortarget[0][totalmotionlong] += (Pprotocal[3] - 0x30) * 100;
                motortarget[0][totalmotionlong] += (Pprotocal[4] - 0x30) * 10;
                motortarget[0][totalmotionlong] += Pprotocal[5] - 0x30;
                motortarget[1][totalmotionlong] = (Pprotocal[7] - 0x30) * 1000;
                motortarget[1][totalmotionlong] += (Pprotocal[8] - 0x30) * 100;
                motortarget[1][totalmotionlong] += (Pprotocal[9] - 0x30) * 10;
                motortarget[1][totalmotionlong] += Pprotocal[10] - 0x30;
                motortarget[2][totalmotionlong] = (Pprotocal[12] - 0x30) * 1000;
                motortarget[2][totalmotionlong] += (Pprotocal[13] - 0x30) * 100;
                motortarget[2][totalmotionlong] += (Pprotocal[14] - 0x30) * 10;
                motortarget[2][totalmotionlong] += Pprotocal[15] - 0x30;
                motortarget[3][totalmotionlong] = (Pprotocal[17] - 0x30) * 1000;
                motortarget[3][totalmotionlong] += (Pprotocal[18] - 0x30) * 100;
                motortarget[3][totalmotionlong] += (Pprotocal[19] - 0x30) * 10;
                motortarget[3][totalmotionlong] += Pprotocal[20] - 0x30;
                if (Pprotocal[21] == 'N')
                {
                    totalmotionlong++;
                    receivemode = 1;
                    CLCD_cmd(0x80);
                    CLCD_data('a');
                }
                else if (Pprotocal[21] == 'S')
                {
                    mode7ui();
                    RC1IE = 0;
                    CLCD_cmd(0x80);
                    CLCD_data('b');
                }
                protocalnum = 0;
            }
        }
        else if (receivemode == 1)
        {
            if (Pprotocal[0] != 'N')
            {
                protocalnum = 0;
            }
            else if (protocalnum == 22 && Pprotocal[0] == 'N')
            {
                motortarget[0][totalmotionlong] = (Pprotocal[2] - 0x30) * 1000;
                motortarget[0][totalmotionlong] += (Pprotocal[3] - 0x30) * 100;
                motortarget[0][totalmotionlong] += (Pprotocal[4] - 0x30) * 10;
                motortarget[0][totalmotionlong] += Pprotocal[5] - 0x30;
                motortarget[1][totalmotionlong] = (Pprotocal[7] - 0x30) * 1000;
                motortarget[1][totalmotionlong] += (Pprotocal[8] - 0x30) * 100;
                motortarget[1][totalmotionlong] += (Pprotocal[9] - 0x30) * 10;
                motortarget[1][totalmotionlong] += Pprotocal[10] - 0x30;
                motortarget[2][totalmotionlong] = (Pprotocal[12] - 0x30) * 1000;
                motortarget[2][totalmotionlong] += (Pprotocal[13] - 0x30) * 100;
                motortarget[2][totalmotionlong] += (Pprotocal[14] - 0x30) * 10;
                motortarget[2][totalmotionlong] += Pprotocal[15] - 0x30;
                motortarget[3][totalmotionlong] = (Pprotocal[17] - 0x30) * 1000;
                motortarget[3][totalmotionlong] += (Pprotocal[18] - 0x30) * 100;
                motortarget[3][totalmotionlong] += (Pprotocal[19] - 0x30) * 10;
                motortarget[3][totalmotionlong] += Pprotocal[20] - 0x30;
                if (Pprotocal[21] == 'N')
                {
                    totalmotionlong++;
                    CLCD_cmd(0x80);
                    CLCD_data('c');
                }
                else if (Pprotocal[21] == 'S')
                {
                    mode7ui();
                    RC1IE = 0;
                    receivemode = 0;
                    CLCD_cmd(0x80);
                    CLCD_data('d');
                }
                protocalnum = 0;
            }
        }
    }
}

#INT_TIMER0
void tmr0()
{
    TMR0L = 156; //0.1ms
    tmr0count++;
    if (tmr0mode == 0)
    {
        if (tmr0count == 300)
        {
            tmr0count = 0;
            if (mode == 4)
            {
                mode4complete++;
                if (mode4complete % 6 <= 3)
                {
                    CLCD_cmd(0xc0);
                    CLCD_array(mode4uitext2);
                }
                else if (mode4complete % 6 > 3)
                {
                    CLCD_cmd(0x01);
                    CLCD_cmd(0x80);
                    CLCD_array(mode4uitext1);
                }
                if (mode4complete > 6)
                    mode4complete = 0;
            }
            if (teachcancelshort == 1)
            {
                for (int ii = 0; ii < 4; ii++)
                {
                    if (motornow[ii] < motortarget[ii][motionstep])
                    {
                        if (motortarget[ii][motionstep] - motornow[ii] < 30)
                            motornow[ii] = motortarget[ii][motionstep];
                        else
                            motornow[ii] += 30;
                    }
                    else if (motornow[ii] > motortarget[ii][motionstep])
                    {
                        if (motornow[ii] - motortarget[ii][motionstep] < 30)
                            motornow[ii] = motortarget[ii][motionstep];
                        else
                            motornow[ii] -= 30;
                    }
                }
            }
            else
            {
                for (int ii = 0; ii < 4; ii++)
                {
                    if (motornow[ii] > 500)
                    {
                        motornow[ii] -= 30;
                        if (motornow[ii] < 500)
                            motornow[ii] = 500;
                    }
                }
            }

            motor0 = 1;
            delay_us(motornow[0]);
            motor0 = 0;
            delay_us(20000 - motornow[0]);
            motor1 = 1;
            delay_us(motornow[1]);
            motor1 = 0;
            delay_us(20000 - motornow[1]);
            motor2 = 1;
            delay_us(motornow[2]);
            motor2 = 0;
            delay_us(20000 - motornow[2]);
            motor3 = 1;
            delay_us(motornow[3]);
            motor3 = 0;
            delay_us(20000 - motornow[3]);

            if (teachcancelshort == 1 && motornow[0] == motortarget[0][motionstep] && motornow[1] == motortarget[1][motionstep] && motornow[2] == motortarget[2][motionstep] && motornow[3] == motortarget[3][motionstep])
            {
                TMR0ON = 0;
                TMR0L = 156;
                RBIE = 1;
                INT0IE = 1;
                INT1IE = 1;
                teachcancelshort = 0;
                mode2ui();
            }

            if (teachcancelshort != 1 && motornow[0] == 500 && motornow[1] == 500 && motornow[2] == 500 && motornow[3] == 500)
            {
                TMR0ON = 0;
                TMR0L = 156;
                RBIE = 1;
                INT0IE = 1;
                INT1IE = 1;
                mode0ui();
            }
        }
    }
    if (tmr0mode == 1)
    {
        if (mode == 7 && tmr0count == 100)
        {
            tmr0count = 0;
            for (int ii = 0; ii < 4; ii++)
            {
                if (motornow[ii] < motortarget[ii][motionstep])
                {
                    if (motortarget[ii][motionstep] - motornow[ii] < 20)
                        motornow[ii] = motortarget[ii][motionstep];
                    else
                        motornow[ii] += 20;
                }
                else if (motornow[ii] > motortarget[ii][motionstep])
                {
                    if (motornow[ii] - motortarget[ii][motionstep] < 20)
                        motornow[ii] = motortarget[ii][motionstep];
                    else
                        motornow[ii] -= 20;
                }
            }

            motor0 = 1;
            delay_us(motornow[0]);
            motor0 = 0;
            delay_us(20000 - motornow[0]);
            motor1 = 1;
            delay_us(motornow[1]);
            motor1 = 0;
            delay_us(20000 - motornow[1]);
            motor2 = 1;
            delay_us(motornow[2]);
            motor2 = 0;
            delay_us(20000 - motornow[2]);
            motor3 = 1;
            delay_us(motornow[3]);
            motor3 = 0;
            delay_us(20000 - motornow[3]);

            if (motornow[0] == motortarget[0][motionstep] && motornow[1] == motortarget[1][motionstep] && motornow[2] == motortarget[2][motionstep] && motornow[3] == motortarget[3][motionstep])
            {
                if (playcancelshort == 1)
                {
                    TMR0ON = 0;
                    playcancelshort = 0;
                }
                else
                {
                    motionstep++;
                    if (motionstep > totalmotionlong)
                        motionstep = 0;
                }
            }
        }
    }
}

#INT_TIMER1
void tmr1()
{
    TMR1H = 177;
    TMR1L = 224; //0.01s
    tmr1count++;

    if (tmr1mode == 0)//tmr1mode is check updown / okcancel 
    {
        if (tmr1count % 5 == 0)
        {
            tmr1count = 0;
            if (btnup && tmr1updown == 0)
            {
                motortarget[selectmotornum - 1][motionstep] += 20;
                if (motortarget[selectmotornum - 1][motionstep] > 2500)
                {
                    motortarget[selectmotornum - 1][motionstep] = 2500;
                }
                mode2ui();
            }
            else if (btndown && tmr1updown == 1)
            {
                motortarget[selectmotornum - 1][motionstep] -= 20;
                if (motortarget[selectmotornum - 1][motionstep] < 500)
                {
                    motortarget[selectmotornum - 1][motionstep] = 500;
                }
                mode2ui();
            }
        }
    }
    if (tmr1mode == 1)
    {
        if (tmr1count < 100)
        {
            if (btnnormal && tmr1okcancel == 0)
            {
                TMR1ON = 0;
                tmr1count = 0;
                TMR1H = 177;
                TMR1L = 224;

                if (mode == 2)
                {
                    if (motionstep < 50)
                        motionstep += 1;

                    for (int ii = 0; ii < 4; ii++)
                    {
                        motortarget[ii][motionstep] = motortarget[ii][motionstep - 1];
                        motortarget[ii][motionstep + 1] = 0;
                    }
                    mode2ui();
                }
            }
            else if (btnnormal && tmr1okcancel == 1)
            {
                TMR1ON = 0;
                tmr1count = 0;
                TMR1H = 177;
                TMR1L = 224;

                if (mode == 2)
                {
                    if (motionstep > 0)
                    {
                        for (int ii = 0; ii < 4; ii++)
                        {
                            motornow[ii] = motortarget[ii][motionstep];
                            motortarget[ii][motionstep] = 0;
                        }
                        motionstep -= 1;

                        tmr0mode = 0;
                        RBIE = 0;
                        INT0IE = 0;
                        INT1IE = 0;
                        teachcancelshort = 1;

                        TMR0ON = 1;
                    }
                }
                if (mode == 7)
                {
                    tmr0mode = 1;
                    playcancelshort = 1;
                    motionstep = 0;
                    TMR0ON = 1;
                }
            }
        }
        if (tmr1count >= 100)
        {
            if (btnok && tmr1okcancel == 0)
            {
                TMR1ON = 0;
                TMR1H = 177;
                TMR1L = 224;
                tmr1count = 0;
                if (mode == 2)
                    mode4ui();
            }
            if (btncancel && tmr1okcancel == 1)
            {
                TMR1ON = 0;
                TMR1H = 177;
                TMR1L = 224;
                tmr1count = 0;

                tmr0mode = 0;
                RBIE = 0;
                INT0IE = 0;
                INT1IE = 0;



                CLCD_cmd(0x01);
                CLCD_cmd(0x80);
                if (mode == 2)
                {
                    CLCD_array(mode2uitext5);
                    for (int ii = 0; ii < 4; ii++)
                        motornow[ii] = motortarget[ii][motionstep];
                }
                if (mode == 7)
                {
                    CLCD_array(mode7uitext3);
                    motionstep = 0;
                }

                TMR0ON = 1;

            }
        }
    }
    if (btnnormal)  //tmr1end
    {
        TMR1ON = 0;
        tmr1count = 0;
    }
}