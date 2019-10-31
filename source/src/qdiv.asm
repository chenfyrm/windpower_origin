;***********************************************************************; 
;*                                                                     *;
;* File Name     : qdiv.asm                                            *;
;*                                                                     *;
;* Originator    : Advanced Embeeded Control (AEC)                     *;
;*                 Texas Instruments Inc.                              *;
;*                                                                     *;
;* Description   : This file contain source code to divide two 16-bit  *;
;*                 fixed point numbers                                 *;
;***********************************************************************;
;* Routine Name  : Generic Function                                    *;
;* Routine Type  : C Callable                                          *;
;*                                                                     *;
;* Description   :                                                     *;
;* long qdiv(int x, int y)                                             *;
;* Input format:  "x" in Q(x) format, "y" in Q(y) format               *;
;* Output format: signed Q(16+Q(x)-Q(y)) format                        *;
;***********************************************************************;

; Module definition for external referance
             .def   _qdiv
            
_qdiv:      SETC    OVM,SXM         
            MOV     T,AH
            
            MOV     ACC,AL<<16  
            CLRC    TC
            ABSTC   ACC         ; ACC=|x| in Q(16+x) format
            MOVL    P,ACC       ; P=|x| in Q(16+x)
            
            MOV     ACC,T       ; ACC=y
            ABSTC   ACC         ; ACC=|y|
            MOVL    XT,ACC      ; XT=|y|
            MOVB    ACC,#0  
            
            RPT     #31
            || SUBCUL ACC,XT        
            
            MOVL    ACC,P       ; ACC=Quotient
            NEGTC   ACC
	    CLRC    OVM
            LRETR           
            
            
                                    
            
            
                
            
            
            
            
                  