List p=18f4520
    ;	init
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	; 30x store testcast used
	; 31x store testcast
	; 32x store palidrome answer
; == init data ==
	; Testcase 1: [0x63, 0x2B, 0xAA, 0xFD, 0xAA, 0xB2, 0x36, 0xDF] 
	; Testcase 2: [0x63, 0x2B, 0xAA, 0xFD, 0xAB, 0xB2, 0x36, 0xDF]
	; Testcase 3: [0x63, 0x63, 0xAA, 0xFD, 0xAB, 0xFF, 0xFF, 0xFF]
	; Testcast 4: [0x63, 0x36, 0xAA, 0xFD, 0xAA, 0xFF, 0xFF, 0xDF]
	; Testcast 5: [0x00, 0x01, 0x10, 0x00, 0x10, 0x01, 0x02, 0x20]
	; Testcast 6: [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
	LFSR 0, 0x310
	MOVLW 0xC4
	MOVWF POSTINC0
	MOVLW 0xBB
	MOVWF POSTINC0
	MOVLW 0xBB 
	MOVWF POSTINC0
	MOVLW 0x00
	MOVWF POSTINC0
	MOVLW 0x4C 
	MOVWF POSTINC0
	MOVLW 0x8B
	MOVWF POSTINC0
	MOVLW 0xBB
	MOVWF POSTINC0
	MOVLW 0x00
	MOVWF POSTINC0
	MOVLB 0x3 ; BSR = 3
	; testcast done
	LFSR 0, 0x360
	MOVLW 0x03 ; FSRH [0x360] store FSR0's High value
	MOVWF POSTINC0
	MOVLW 0x20 ; head index [0x361] store FSR0's Low value ; head of ans
	MOVWF POSTINC0
	MOVLW 0x27 ; end index [0x362] store FSR0's Low value ; end of ans
	MOVWF POSTINC0
	MOVLW 0x04 ; count [0x363] 
	MOVWF POSTINC0
; == init data done ==
    mainLoop:
	; find min in testcast which isn't used
	; the min will be store in [0x340], and mark it used
	CALL findMin ; FSR2 point to it
	
	; FSR0 -> head
	MOVFF 0x360, FSR0H
	MOVFF 0x361, FSR0L
	; W = head = min
	MOVFF 0x340, INDF0
	MOVF INDF0, W
	
	; copy head of ans and rotate 4 times
	; FSR0 -> head down [0x32x] -> [0x33x]
	BSF FSR0L, d'4'
	MOVWF INDF0 ; [0x33x] = head
	; e.g. [0x33x] = (0xAB -> 0xBA)
	RLNCF INDF0
	RLNCF INDF0
	RLNCF INDF0
	RLNCF INDF0
	
	; W will store the palidrom value after function
	CALL findSame ; [0x370] = 1 find same ; [0x370] = 0 not find same 
	BTFSS 0x70, d'0', 1 ; skip if [0x370] = 1
	BRA noPalidrome ; [0x370] = 0 ; skip
	
	; FSR0 -> end
	MOVFF 0x360, FSR0H
	MOVFF 0x362, FSR0L
	; end = W
	MOVWF INDF0
	
	INCF 0x61, f, 1 ; [0x361]++
	DECF 0x62, f, 1 ; [0x362]--
    
	DECFSZ 0x63, F, 1 ; skip if --count == 0
	BRA mainLoop
	BRA mainDone
	
    noPalidrome:
	LFSR 0, 0x320
	MOVLW 0xFF
	MOVWF POSTINC0
	MOVWF POSTINC0
	MOVWF POSTINC0
	MOVWF POSTINC0
	MOVWF POSTINC0
	MOVWF POSTINC0
	MOVWF POSTINC0
	MOVWF POSTINC0
	
    mainDone:
	BRA done
; =============================================================================
; function
    findMin:
    ; [0x40] = findMin answer
    ; [0x41] = final selected index
	MOVLW 0xFF
	MOVWF 0x40 ; [0x40] = findMin answer
	MOVLW d'8'
	MOVWF 0x50 ; [0x50] = count = 8
	LFSR 1, 0x300 ; FSR1 -> current
	
	findMinLoop:
	    ; 0 for not used ; 1 for used
	    MOVF INDF1, W
	    BNZ findMin_skip ; check used ; skip if used
	    
	    ; FSR1 -> value
	    MOVLW 0x10
	    ADDWF FSR1L 
	    
	    ; skip if f > W
	    MOVF 0x40, W, 1
	    CPFSGT INDF1 
	    BRA findMin_take ; <= ; take
	    BRA findMin_skip ; current value > current min ; don't take
	    
	    findMin_take:
		MOVFF FSR1L, FSR2L
		MOVFF FSR1H, FSR2H ; FSR2 -> current value
		MOVFF INDF1, 0x340 ; [0x40] = current min
		
	    findMin_skip:
		; repoint to [0x30x]
		INCF FSR1L
		BCF FSR1L, d'4'
	     
		DECFSZ 0x50, F, 1 ; skip if --count == 0
		BRA findMinLoop
		BRA findMin_done
	    
	findMin_done:
	    BCF FSR2L, d'4' ; repoint to [0x30x]
	    BSF INDF2, d'0' ; set used
	    RETURN
	    
; =============================================================================
    ; [0x370] store the ans of this function, find same = 1, not find same = 0, default = 0
    ; should make sure target must be store in FSR0 before function start
    findSame:
	; function init
	CLRF 0x70, 1 ; [0x370] = 0
	MOVLW 0x08
	MOVWF 0x71, 1 ; [0x701] = count
	LFSR 1, 0x300 ; FSR1 -> head
	
	findSame_loop: 
	    MOVF INDF1, W
	    BNZ findSame_skip ; check used ; skip if used
	    
	    ; FSR1 -> value
	    BSF FSR1L, d'4'
	    
	    MOVF INDF0, W
	    CPFSEQ INDF1 ; skip if f = W
	    BRA findSame_skip ; f != W
	    
	    ; f == W, then finish
	    BSF 0x70, d'0', 1 ; set true
	    ; set the same value to used
	    BCF FSR1L, d'4'
	    BSF INDF1, d'0'
	    BRA findSame_done
	    
	    findSame_skip:
		; repoint to [0x30x]
		INCF FSR1L
		BCF FSR1L, d'4'

		DECFSZ 0x71, F, 1 ; skip if --count == 0
		BRA findSame_loop
		BRA findSame_done
	    
	findSame_done:
	    RETURN
    done: 
	NOP
	END