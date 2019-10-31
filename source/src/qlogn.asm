;***********************************************************************; 
;*                                                                     *;
;* File Name     : qlogn.asm                                           *;
;*                                                                     *;
;* Originator    : Advanced Embeeded Control (AEC)                     *;
;*                 Texas Instruments Inc.                              *;
;*                                                                     *;
;* Description   : This file contain source code for fixed point LOGN  *;
;*                function implemented using Tylor series              *;
;*                                                                     *;
;* Date          : 21/1/2002                                           *;
;***********************************************************************;
;* Tylor Series Approximation of LOGN:                                 *;
;* Algorithm     :                                                     *;
;* The following equation approximates the sqrt(x), where x->[1.0,2.0] *;                  
;* logn(x)= 0.9991150*(x-1)-0.4899597*(x-1)^2+0.2856751*(x-1)^3        *;
;*           -0.1330566*(x-1)^4+0.03137207(x-1)*x^5                    *;
;***********************************************************************; 
  
    
; Module definition for external referance
            .def    _qlogn   
                
a1      .set    07fe3h          ; 0.9991150  scaled by 2^15
a2      .set    08292h          ; -0.4899597 scaled by 2^16 
a3      .set    04922h          ; 0.2856751 scaled by 2^16
a4      .set    0bbe0h          ; -0.1330566 scaled by 2^17
a5      .set    00808h          ; 0.03137207 scaled by 2^16 for Immediate MPY

logn2       .set    00b17h          ; logn(2)=0.693147 scaled by 2^12 for immediate
 
_qlogn:                             ; ACC=X in Q16 format
            SETC    SXM             ; Set the sign ext. mode
            MPY     P,T,#0          ; P=0 
            LSR64   ACC:P,#1        ; X=X/2

            CSB     ACC             ; Count sign bits, T=E
            
            LSL64   ACC:P,T         ; ACC=x=X/(2^(E-14) in  Q30 format
            TCLR    AH,#14          ; ACC=(x-1), clear bit 30 for x=x-1                     
    
            LSL     ACC,1           ; ACC=x=(x-1) in Q31    
            MOV     AR4,T           ; AR4=E 
            MOVL    XT,ACC          ; XT=x in Q31
        
; Comment Next 2-instructions, if this works
            MPY     ACC,T,#a5               
            
;           MOV     AR5,#a5         ; AR5=a5    
;           MPY     ACC,T,AR5       ; ACC=a5*x in Q31             
            
            ADD     ACC,#a4<<14     ; ACC=a4+x*a5 in Q31
 
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a4+x*a5) in Q30
            ADD     ACC, #a3<<14    ; ACC=a3+x*(a4+x*a5) in Q30
            
            SUBB    XAR4,#14        ; AR4=E-14 
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a3+(a4+x*a5)) in Q29
            ADD     ACC, #a2<<13    ; ACC=a2+x*(a3+(a4+x*a5)) in Q29
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a2+x*(a3+(a4+x*a5))) in Q28
            ADD     ACC, #a1<<13    ; ACC=a1+x*(a2+x*(a3+(a4+x*a5))) in Q28 
            
            QMPYL   ACC,XT,@ACC     ; ACC=x*(a1+x*(a2+x*(a3+(a4+x*a5)))) in Q27
                                    ;    = 2*ln(x) in Q27
            
            SFR     ACC,#15         ; ACC= 2*ln(x) in Q12
            
            MOV     T,AR4           ; T=E-14 in Q0
            MPY     P,T,#logn2      ; P=(E-14)*(2*ln(2)) in 20.12 format
          
            SUBL    ACC,P           ; ACC=-(E-14)*(2*ln(2)) in 20.12 format
 
            ADD     ACC,#(logn2+2)          
            SFR     ACC,1           ; ACC=ln(X) in Q11                  
            LRETR       

   
   
   
   
   