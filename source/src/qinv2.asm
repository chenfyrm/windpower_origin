;***********************************************************************; 
;*                                                                     *;
;* File Name     : qinv2.asm                                           *;
;*                                                                     *;
;* Originator    : Advanced Embeeded Control (AEC)                     *;
;*                 Texas Instruments Inc.                              *;
;*                                                                     *;
;* Description   : This file contain source code to find the reciprocal*; 
;*                 of fixed point number, The output is a 16bit number *;    
;*                                                                     *;
;* Date          : 21/1/2002                                           *;
;***********************************************************************; 
;* Routine Name  : Generic Function                                    *;
;* Routine Type  : C Callable                                          *;
;*                                                                     *;
;* Input format:  Signed number in Q(x) format (x<16)                  *;
;* Output format: Signed number in Q(15-x) format (16 bit result)      *;
;*                                                                     *;
;* Example:                                                            *;
;* Input format:  Signed Q0 format  (Fractional Number)                *;
;* Output format: Signed Q15  format                                   *;
;***********************************************************************; 

; Module definition for external referance
             .def   _qinv2
            
_qinv2:     SETC    OVM,SXM         
            MOV     ACC,AL

            CLRC    TC          ;TC = 0
            ABSTC   ACC         ;abs(x) store sign ,TC = TC^Sign
            
            MOV     T,AL        ; T=x            

            MOV     ACC,#1<<15  ; ACC=1 in Q15 format

            RPT     #15
            || SUBCU ACC,T      
            
            MOV     ACC,AL<<16      ; ACC=Quotient
            
            ABS     ACC         ; Saturate 0x10000000 to 0x7FFFFFFF
            NEGTC   ACC
            MOV     AL,AH
            CLRC    OVM
            LRETR           
            
            
                                    
            
            
                
            
            
            
            
                  