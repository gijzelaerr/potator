/** M65C02: portable 65C02 emulator **************************/
/**                                                         **/
/**                         M65C02.c                        **/
/**                                                         **/
/** This file contains implementation for 65C02 CPU. Don't  **/
/** forget to provide Rd6502(), Wr6502(), Loop6502(), and   **/
/** possibly Op6502() functions to accomodate the emulated  **/
/** machine's architecture.                                 **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2002                 **/
/**               Alex Krasivsky  1996                      **/
/**               Steve Nickolas  2002                      **/
/**   Portions by Holger Picker   2002                      **/
/**   ADC and SBC instructions provided by Scott Hemphill   **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

/* This is M65C02 Version 1.4 of 2002.1220 -uso. */

#include "m6502.h"
#include "tables.h"
#include "../types.h"
#include <stdio.h>



/** Reset6502() **********************************************/
/** This function can be used to reset the registers before **/
/** starting execution with Run6502(). It sets registers to **/
/** their initial values.                                   **/
/*************************************************************/
void Reset6502(M6502 *R)
{
  R->A=R->X=R->Y=0x00;
  R->P=Z_FLAG|R_FLAG;
  R->S=0xFF;
  R->PC.B.l=Rd6502(0xFFFC);
  R->PC.B.h=Rd6502(0xFFFD);
  R->ICount=R->IPeriod;
  R->IRequest=INT_NONE;
  R->AfterCLI=0;
}


/* The following code was provided by Mr. Scott Hemphill. Thanks a lot again! */
void SBCInstruction(M6502 *R, register unsigned char val) {
  register unsigned int w;
  register unsigned int temp;

  if ((R->A ^ val) & 0x80)      {
    R->P |= V_FLAG;  }
  else {
    R->P &= ~V_FLAG; }

  if (R->P&D_FLAG) {            /* decimal subtraction */
    temp = 0xf + (R->A & 0xf) - (val & 0xf) + (R->P & C_FLAG);
    if (temp < 0x10) {
      w = 0;
      temp -= 6;
    } else {
      w = 0x10;
      temp -= 0x10;
    }
    w += 0xf0 + (R->A & 0xf0) - (val & 0xf0);
    if (w < 0x100) {
      R->P &= ~C_FLAG;
      if ((R->P&V_FLAG) && w < 0x80) R->P &= ~ V_FLAG;
      w -= 0x60;
    } else {
      R->P |= C_FLAG;
      if ((R->P&V_FLAG) && w >= 0x180) R->P &= ~V_FLAG;
    }
    w += temp;
  } else {                      /* standard binary subtraction */
    w = 0xff + R->A - val + (R->P&C_FLAG);
    if (w < 0x100) {
      R->P &= ~C_FLAG;
      if ((R->P & V_FLAG) && w < 0x80) R->P &= ~V_FLAG;
    } else {
      R->P |= C_FLAG;
      if ((R->P&V_FLAG) && w >= 0x180) R->P &= ~V_FLAG;
    }
  }
  R->A = (unsigned char)w;
  R->P = (R->P & ~(Z_FLAG | N_FLAG)) | (R->A >= 0x80 ? N_FLAG : 0) | (R->A == 0 ? Z_FLAG : 0);
} /* SBCinstruction */


/** Exec6502() ***********************************************/
/** This function will execute a single 6502 opcode. It     **/
/** will then return next PC, and current register values   **/
/** in R.                                                   **/
/*************************************************************/
word Exec6502(M6502 *R)
{
  register pair J,K;
  register byte I;

  I=Op6502(R->PC.W++);
  R->ICount-=Cycles[I];
  switch(I)
  {
#include "codes.h"
  }

  /* We are done */
  return(R->PC.W);
}

/** Int6502() ************************************************/
/** This function will generate interrupt of a given type.  **/
/** INT_NMI will cause a non-maskable interrupt. INT_IRQ    **/
/** will cause a normal interrupt, unless I_FLAG set in R.  **/
/*************************************************************/
void Int6502(M6502 *R,byte Type)
{
  register pair J;

  if((Type==INT_NMI)||((Type==INT_IRQ)&&!(R->P&I_FLAG)))
  {
    R->ICount-=7;
    M_PUSH(R->PC.B.h);
    M_PUSH(R->PC.B.l);
    M_PUSH(R->P&~B_FLAG);
    R->P&=~D_FLAG;
    if(Type==INT_NMI) J.W=0xFFFA; else { R->P|=I_FLAG;J.W=0xFFFE; }
    R->PC.B.l=Rd6502(J.W++);
    R->PC.B.h=Rd6502(J.W);
  }
}

/** Run6502() ************************************************/
/** This function will run 6502 code until Loop6502() call  **/
/** returns INT_QUIT. It will return the PC at which        **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
word Run6502(M6502 *R)
{
  register pair J,K;
  register byte I;

  for(;;)
  {
#ifdef DEBUG2
    /* Turn tracing on when reached trap address */
    if(R->PC.W==R->Trap) R->Trace=1;
    /* Call single-step debugger, exit if requested */
    if(R->Trace)
      if(!Debug6502(R)) return(R->PC.W);
#endif

    I=Op6502(R->PC.W++);
    R->ICount-=Cycles[I];
    switch(I)
    {
#include "codes.h"
    }

    /* If cycle counter expired... */
    if(R->ICount<=0)
    {
      /* If we have come after CLI, get INT_? from IRequest */
      /* Otherwise, get it from the loop handler            */
      if(R->AfterCLI)
      {
        I=R->IRequest;            /* Get pending interrupt     */
        R->ICount+=R->IBackup-1;  /* Restore the ICount        */
        R->AfterCLI=0;            /* Done with AfterCLI state  */
      }
      else
      {
        I=Loop6502(R);            /* Call the periodic handler */
        R->ICount=R->IPeriod;     /* Reset the cycle counter   */
      }

      if(I==INT_QUIT) return(R->PC.W); /* Exit if INT_QUIT     */
      if(I) Int6502(R,I);              /* Interrupt if needed  */
    }
  }

  /* Execution stopped */
  return(R->PC.W);
}
