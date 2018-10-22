#ifndef _GPIO_040_H_
#define _GPIO_040_H_
void Hard_Init();

void Hard_displayreset();
void Hard_Gpiocreate();

void Hard_capturereset();
int Hard_getccdstatus();
int Hard_getfirstatus();


#endif