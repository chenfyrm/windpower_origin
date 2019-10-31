;***********************************************************************; 
;*                                                                     *;
;* File Name     : qinv1.asm                                           *;
;*                                                                     *;
;* Originator    : Advanced Embeeded Control (AEC)                     *;
;*                 Texas Instruments Inc.                              *;
;*                                                                     *;
;* Description   : This file contain source code to find the reciprocal*; 
;*                 of fixed point number, The output is a 32 bit number*;    
;*                                                                     *;
;* Date          : 21/1/2002                                           *;
;***********************************************************************; 
;* Routine Name  : Generic Function                                    *;
;* Routine Type  : C Callable                                          *;
;*                                                                     *;
;* Input format:  Signed number in Q(x) format (x<16)                  *;
;* Output format: Signed number in Q(31-x) format (32 bit result)      *;
;*                                                                     *;
;* Example:                                                            *;
;* Input format:  Signed Q15 format  (Fractional Number)               *;
;* Output format: Signed Q16  format                                   *;
;***********************************************************************; 

; Module definition for external referance
            .def    _qinv1

_qinv1:
            SETC    OVM,SXM         
            MOV     ACC,AL

            CLRC    TC          ;TC = 0
            ABSTC   ACC         ;abs(x) store sign ,TC = TC^Sign
            
            MOVL    XT,ACC      ; XT=x            

            ZAPA                ; P=0, ACC=0

            MOV     PH,#0x8000  ; P=1 in Q31

            RPT     #31
            || SUBCUL ACC,XT        
            
            MOVL    ACC,P       ; ACC=Quotient
            ABS     ACC         ; Saturate 0x10000000 to 0x7FFFFFFF
            NEGTC   ACC
      	    CLRC    OVM
            LRETR           
            
            
                                    
            
            
                
            
            
            
            
                  

