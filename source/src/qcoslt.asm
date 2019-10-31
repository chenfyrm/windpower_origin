;***********************************************************************; 
;*                                                                     *;
;* File Name     : qcoslt.asm                                          *;
;*                                                                     *;
;* Originator    : Advanced Embeeded Control (AEC)                     *;
;*                 Texas Instruments Inc.                              *;
;*                                                                     *;
;* Description   : This file contain source code for fixed point COS   *;
;*                function implemented using 256-point look-up table   *;
;*                                                                     *;
;* Date          : 21/1/2002                                           *;
;***********************************************************************;


; Module definition for external referance
                .def    _qcoslt   
                .ref    SINTAB_360
                              
_qcoslt:     
            SETC    SXM,OVM         
            MOVL    XAR5,#SINTAB_360
                            
            ADD     AL,#4000h      ; ACC= 90deg, COS(t)=SIN(90+t)

; Obtain the SIN of the angle "X=alpha" using Look-up Table and Linear Interpolation method
            MOVB    XAR0,#0     
            MOV     T,#0            
            MOVB    AR0,AL.MSB      ; AR0=indice (alpha/2^8)
            MOVB    T,AL.LSB        ; T=(X-X1) in Q8 format

                       
            MOV     ACC,*+XAR5[AR0] ; ACC=Y1=*(SINTAB_360 + indice)
            ADDB    XAR0,#1
            MOV     PL,*+XAR5[AR0]  ; PL=Y2
            SUB     PL,AL           ; PL=Y2-Y1 in Q15 
            MPY     P,T,PL          ; P=Y2-Y1 in Q23
            LSL     ACC,8           ; ACC=Y1 in Q23
            ADDL    ACC,P           ; Y=Y1+(Y2-Y1)*(X-X1)
            MOVH    AL,ACC<<8       ; AL=Y in Q15 format
            CLRC    OVM
            LRETR
            
            

