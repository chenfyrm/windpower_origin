;***********************************************************************;
;*                                                                     *;
;* File Name     : qatan.asm                                           *;
;*                                                                     *;
;* Originator    : Advanced Embeeded Control (AEC)                     *;
;*                 Texas Instruments Inc.                              *;
;*                                                                     *;
;* Description   : This file contain source code for fixed point ATAN  *;
;*                function implemented using Tylor series              *;
;*                                                                     *;
;* Date          : 21/1/2002                                           *;
;***********************************************************************;
;* Tylor Series Approximation of ATAN:                                 *;
;* Algorithm     :                                                     *;
;* atan(x): where 'x' is in 16.16 format                               *;
;*       = 0.318253*x + 0.003314*x^2 - 0.130908*x^3 + 0.068542*x^4     *;
;*          - 0.009159*x^5; if x<1                                     *;
;*       = 0.5-atan(1/x); if x>=1                                      *;
;***********************************************************************;
        
        .def      _qatan

a1      .set    0x5179      ; in Q16 for  0.318253 
a2      .set    0x6C97      ; in Q23 for  0.003314 
a3      .set    0xBCFA      ; in Q17 for -0.130908
a4      .set    0x462F      ; in Q18 for  0.068542 
a5      .set    -2401       ; in Q18 for -0.009159

_qatan:     SETC    SXM,OVM
            MOV     AR4,AH          ; AR4=HI(X)
            ABS     ACC             ; x=abs('X') 
            LSR64   ACC:P,#1
            MOV     AR5,AL          ; AR5=x
            
            CSB     ACC             ; Count sign bits, T=E+1
            LSL64   ACC:P,T         ; Remove upto 15 extra sign bits
                                    ; T=nrof(sign bits)
            MOV     PH,AH
            TCLR    T,#4            ; T=1, if x<1, clear bit 4
            
; 1/|X| is obtained by 16 bit division, though the X input was in 16.16 format,         
; The strategy adopted here uses the 16 significant bits obtained through removing 
; upto 15 sign bits for division. 

            MOV     ACC,#1<<14      ; ACC=#1 in Q14
            LSLL    ACC,T           ; ACC=#1 in 17-E.15+E format

            RPT     #15
            || SUBCU ACC,PH         ; (17-E.15+E)/(16-E.E)= 1.15 format
            
            MOV     ACC,AL<<16 
            ABS     ACC             ; AH=1/x
            MOV     AR5,AH,NTC      ; AR5=x or 1/x in Q15 format
            MOV     TL,#0
            MOV     T,AR5           ; T=x in Q15 format
            

; Comment Next 2-instructions, if this works
            MPY     ACC,@T,#a5      ; ACC=x*a5 in Q33 

; Store x*(a1+x*(a2+x*(a3+(a4+x*a5)))) in Q15
;           MOV     AR5,#a5
;           MPY     ACC,T,AR5

            ADD     ACC,#a4<<15     ; ACC=a4+x*a5 in Q33
 
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a4+x*a5) in Q32
            ADD     ACC, #a3<<15    ; ACC=a3+x*(a4+x*a5) in Q32
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a3+(a4+x*a5)) in Q31
            ADD     ACC, #a2<<8     ; ACC=a2+x*(a3+(a4+x*a5)) in Q31
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a2+x*(a3+(a4+x*a5))) in Q30
            ADD     ACC, #a1<<14    ; ACC=a1+x*(a2+x*(a3+(a4+x*a5))) in Q30 
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a1+x*(a2+x*(a3+(a4+x*a5)))) in Q29
            
            SFR     ACC,#14         ; ACC=atan(x) in Q15 format
            MOVL    P,ACC           ; P=arctan(x)
            NEG     ACC             ; ACC=atan(x)
            ADD     ACC,#4000h      ; ACC=0.5-atan(1/x) in Q15 format           
            MOVL    P,ACC,NTC       
            MOVL    ACC,P           ; ACC=0.5-atan(x), if X>1
                                    ; ACC=atan(x), if X<1
            TBIT    AR4,#15         ; TC=sign(X)
            NEGTC   ACC
            CLRC    OVM
            LRETR           
            
