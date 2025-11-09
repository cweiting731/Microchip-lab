List p=18f4520
    ;	init
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	MOVLB 0x1 ; BSR = 1
	; init sequence A
	LFSR 0, 0x200
	MOVLW 0x0D ; testcast 1 = 0x01, testcast 2 = 0x05
	MOVWF POSTINC0
	MOVLW 0x27 ; testcast 1 = 0x03, testcast 2 = 0x1A
	MOVWF POSTINC0
	MOVLW 0x4B ; testcast 1 = 0x05, testcast 2 = 0x2C
	MOVWF POSTINC0
	MOVLW 0x7E ; testcast 1 = 0x07, testcast 2 = 0x40
	MOVWF POSTINC0
	MOVLW 0xB2 ; testcast 1 = 0x09, testcast 2 = 0x7E
	MOVWF POSTINC0
	MOVLW 0xE8 ; testcast 1 = 0x0B, testcast 2 = 0xA0
	MOVWF POSTINC0
	; init sequence B
	LFSR 0, 0x210
	MOVLW 0x05 ; testcast 1 = 0x02, testcast 2 = 0x06
	MOVWF POSTINC0
	MOVLW 0x39 ; testcast 1 = 0x04, testcast 2 = 0x20
	MOVWF POSTINC0
	MOVLW 0x6C ; testcast 1 = 0x06, testcast 2 = 0x33
	MOVWF POSTINC0
	MOVLW 0xA1 ; testcast 1 = 0x08, testcast 2 = 0x80
	MOVWF POSTINC0
	MOVLW 0xF3 ; testcast 1 = 0x0A, testcast 2 = 0xF0
	MOVWF POSTINC0
	
	LFSR 0, 0x200 ; index of A (Ai)
	LFSR 1, 0x210 ; index of B (Bi)
	LFSR 2, 0x220 ; index of C (Ci)
	 ; [0xF0] = length of A ;; have to change
	MOVLW 0x06
	MOVWF 0xF0
	; [0xF1] = length of B ;; have to change
	MOVLW 0x05
	MOVWF 0xF1
	
    loop:
	; A finished -> take all B rest
	MOVF 0xF0, W
	BZ take_all_B_rest
	; B finished -> take all A rest
	MOVF 0xF1, W
	BZ take_all_A_rest
	
	; compare A[Ai], B[Bi]
	MOVF INDF1, W ; W = B[Bi]
	CPFSGT INDF0 ; skip if f > W ; skip if A[Ai] > B[Bi]
	    BRA takeA ; A[Ai] <= B[Bi]
	    BRA takeB ; A[Ai] > B[Bi]
	
	takeA:
	    MOVFF POSTINC0, POSTINC2 ; C[Ci++] = A[Ai++]
	    DECF 0xF0 ; Al--
	    BRA loop
	    
	takeB:
	    MOVFF POSTINC1, POSTINC2 ; C[Ci++] = B[Bi++]
	    DECF 0xF1 ; Bl--
	    BRA loop
	
	take_all_A_rest:
	    ; check
	    MOVF 0xF0, W
	    BZ done
	    
	    copyA_loop:
		MOVFF POSTINC0, POSTINC2 ; C[Ci++] = A[Ai++]
		DECFSZ  0xF0 ; skip if --Ai == 0
		BRA copyA_loop
		BRA done
	
	take_all_B_rest:
	    ; check
	    MOVF 0xF1, W
	    BZ done
	    
	    copyB_loop:
		MOVFF POSTINC1, POSTINC2 ; C[Ci++] = B[Bi++]
		DECFSZ  0xF1 ; skip if --Bi == 0
		BRA copyB_loop
		BRA done
    
    done:
	NOP
	
	END


