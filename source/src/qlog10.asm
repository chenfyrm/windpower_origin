;***********************************************************************; 
;*                                                                     *;
;* File Name     : qlog10.asm                                          *;
;*                                                                     *;
;* Originator    : Advanced Embeeded Control (AEC)                     *;
;*                 Texas Instruments Inc.                              *;
;*                                                                     *;
;* Description   : This file contain source code for fixed point LOG10 *;
;*                function implemented using Tylor series              *;
;*                                                                     *;
;* Date          : 21/1/2002                                           *;
;***********************************************************************;
;* Tylor Series Approximation of LOG10:                                *;
;* Algorithm     :                                                     *;
;* The following equation approximates the sqrt(x), where x->[1.0,2.0] *;
;* 2*log10(x)= 0.8678284*(x-1)-0.4255677*(x-1)^2+0.2481384*(x-1)^3     *;
;*           -0.1155701*(x-1)^4+0.0272522(x-1)*x^5                     *;
;***********************************************************************;
  
; Module definition for external referance
            .def    _qlog10   
            
       
a1      .set    06f15h          ; 0.8678284 scaled by 2^15 
a2      .set    0930eh          ; -0.4255677 scaled by 2^16
a3      .set    07f0ch          ; 0.2481384 scaled by 2^17
a4      .set    089a8h          ; -0.1155701 scaled by 2^18 
a5      .set    00df4h          ; 0.0272552 scaled by 2^17 
log2    .set    009a2h          ; 2*log10(2) scaled by 2^12 

_qlog10:                            ; ACC=X in Q16 format
            SETC    SXM             ; Set the sign ext. mode
            MPY     P,T,#0          ; P=0 
            LSR64   ACC:P,#1        ; X=X/2

            CSB     ACC             ; Count sign bits, T=E
            
            LSL64   ACC:P,T         ; ACC=x=X/(2^(E-14) in  Q30 format
            TCLR    AH,#14          ; ACC=(x-1), clear bit 30 for x=x-1                     
    
            LSL     ACC,1           ; ACC=x=(x-1) in Q31    
            MOV     AR4,T           ; AR4=E 
            MOVL    XT,ACC          ; T=x in Q31
        
; Comment Next 2-instructions, if this works
            MPY     ACC,T,#a5   
        
;           MOV     AR5,#a5         ; AR5=a5    
;           MPY     ACC,T,AR5       ; ACC=a5*x in Q32 
            
            ADD     ACC,#a4<<14     ; ACC=a4+x*a5 in Q32
 
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a4+x*a5) in Q31
            ADD     ACC, #a3<<14    ; ACC=a3+x*(a4+x*a5) in Q31
            
            SUBB    XAR4,#14        ; AR4=E-14 
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a3+(a4+x*a5)) in Q30
            ADD     ACC, #a2<<14    ; ACC=a2+x*(a3+(a4+x*a5)) in Q30
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a2+x*(a3+(a4+x*a5))) in Q29
            ADD     ACC, #a1<<14    ; ACC=a1+x*(a2+x*(a3+(a4+x*a5))) in Q29 
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a1+x*(a2+x*(a3+(a4+x*a5)))) in Q28
                                    ;    = 2*log10(x) in Q28 
            MOV     ACC,@AH         ; ACC= 2*log10(x) in Q12
            
            MOV     T,AR4           ; T=E-14 in Q0
            MPY     P,T,#log2       ; P=(E-14)*(2*log10(2)) in 20.12 format
          
            SUBL    ACC,P           ; ACC=-(E-14)*(2*log10(2)) in 20.12 format
 
            ADD     ACC,#log2       
            SFR     ACC,1           ; convert 2*log10(X) to log10(X)                    
            LRETR       

   
   
   
   
   