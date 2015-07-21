; Includes the macros definitions
.nolist
.include "m328pdef.inc"
.list

.def    WR1 = r16               ; The working registers
.def    WR2 = r17
.def    WR3 = r18

.def    Arg1 = r19              ; Argument registers
.def    Arg2 = r20

.def    T1 = r13                ; The 24 bits value used to
.def    T2 = r14                ; hold the number of milliseconds
.def    T3 = r15                ; Since the µC's initialization

.equ    pin_fr = PD5            ; The motors pins
.equ    pin_br = PD3
.equ    pin_bl = PD6
.equ    pin_fl = PB3

.equ    min_spd = 110           ; The min/max value of the counters. Used for
.equ    max_spd = 245           ; calibration purpose only
.equ    zero_spd = 125          ; The min/max value used for min/max speed
.equ    full_spd = 225

.equ    tim_fr = OCR0B          ; The motors timers
.equ    tim_br = OCR2B
.equ    tim_bl = OCR0A
.equ    tim_fl = OCR2A

.equ    BaudRate = 38400        ; The baudrate in baud
.equ    dt = 10                 ; The length of a loop in ms

;===================;
;   DATA SEGMENT    ;
;===================;

.dseg
.org    SRAM_START

;===================;
;  PROGRAM SEGMENT  ;
;===================;

.cseg
; Interrupt vectors
.org    0x0000
    RJMP    Start               ; The reset vector, the "main" program

.org    OC1Aaddr
    JMP     Timer1Aisr          ; The int vector called every ms by Timer 1 A

; Move the "normal" section: not the int vectors one
.org INT_VECTORS_SIZE   

; Data stored in the progmem

HelloWorld:
    .DB "hello, world", 10, 0

SetupFinished:
    .DB "Setup done", 10, 0

DataWaiting:
    .DB "Data waiting", 10, 0

;-------------------;
;   Initialization  ;
;-------------------;
Start:
    ; Stack Pointer setup
    LDI     WR1, HIGH(RAMEND)   ; Setup the Stack Pointer
    OUT     SPH, WR1            ; at the top of the ram
    LDI     WR1, LOW(RAMEND)    ; because it goes downward
    OUT     SPL, WR1

    ; USART setup (8 even 2)
    LDI     WR1, (1<<RXEN0)+(1<<TXEN0)
    LDI     WR2, (1<<UPM01)+(1<<USBS0)+(1<<UCSZ00)+(1<<UCSZ01)

    STS     UCSR0B, WR1         ; OCR0B: reception and transmition
    STS     UCSR0C, WR2         ; OCR0C: Even parity, 2 stop bits

    LDI     WR1, HIGH(1000000 / BaudRate - 1)
    LDI     WR2, LOW(1000000 / BaudRate - 1)

    STS     UBRR0H, WR1         ; Set the baud rate, with the formula
    STS     UBRR0L, WR2         ; UBRR = F_CPU / 16 / Baudrate - 1 (F_CPU = 16 MHz)

    ; Timer1A (milliseconds) setup
    CLR     T1                  ; 0 ms spends since startup
    CLR     T2
    CLR     T3

    LDI     WR1, (1 << WGM12) + (1 << CS10)
    STS     TCCR1B, WR1         ; TCCR1A/1C must be 0, its initial value 
                                ; Set TCCR1B in order to have:
                                ; CTC Mode, TOP = OCR1A, no prescaling
    LDI     WR1, (1 << COM1A0)
    STS     TCCR1A, WR1

    LDI     WR1, HIGH(15999)    ; Must wait 16000 clock cycles (so 1 kHz)
    STS     OCR1AH, WR1         ; From 0 to 16000 -> 16001 cc
    LDI     WR1, LOW(15999)     ; So load OCR1A with 15999
    STS     OCR1AL, WR1

    LDI     WR1, 1 << OCIE1A    ; Enable Output Compare A match interrupt
    STS     TIMSK1, WR1

    ; Timer0 and Timer2 settings (motors)
    LDI     WR1, (1<<pin_fr)+(1<<pin_br)+(1<<pin_bl)
    LDI     WR2, (1<<pin_fl)
    OUT     DDRD, WR1           ; Set the motors pin dir as output
    OUT     DDRB, WR2

    LDI     WR1, (1<<WGM00)+(1<<COM0A1)+(1<<COM0B1)
    LDI     WR2, (1<<CS01)+(1<<CS00)
    LDI     WR3, (1<<CS22)

    STS     TCCR0A, WR1         ; PWM, phase correct, top = 0xFF
    STS     TCCR2A, WR1         
    STS     TCCR0B, WR2         ; F_pwm = F_CPU / 64 ~= 488 Hz
    STS     TCCR2B, WR3

    LDI     WR1, 20
    STS     tim_fr, WR1
    STS     tim_br, WR1
    STS     tim_bl, WR1
    STS     tim_fl, WR1

    ; I/O settings
    SER     WR1
    OUT     DDRD, WR1
    OUT     PORTD, WR1
    OUT     DDRB, WR1
    OUT     PORTB, WR1

    ; Enable interrupts
    SEI

    ; Setup finished
    LDI     ZH, HIGH(SetupFinished*2)
    LDI     ZL, LOW(SetupFinished*2)
    RCALL   UARTSendString

;------------------;
;   Main  Loop     ;
;------------------;
Loop:
    CLI                         ; Reset the counter
    CLR     T1
    CLR     T2
    CLR     T3
    SEI

    LDS     WR1, UCSR0A
    SBRC    WR1, RXC0           ; Skip next if RXC0 = 0
    RJMP    ReceivedData

BackToReality:
    LDI     WR1, BYTE1(dt)      ; Load the period
    LDI     WR2, BYTE2(dt)
    LDI     WR3, BYTE3(dt)

Wait:                           ; Wait until a period is passed

    CLI                         ; Compare T1 (elapsed time in ms) with
    CP      T1, WR1             ; WR[1|2|3] (the loop date)
    CPC     T2, WR2
    CPC     T3, WR3
    SEI

    BRLO    Wait                ; If T1 >= dt, then the delay is passed

    RJMP    Loop

ReceivedData:
    LDS     WR1, UDR0

    LDI     ZH, HIGH(HelloWorld*2)
    LDI     ZL, LOW(HelloWorld*2)
    RCALL   UARTSendString

    RJMP    BackToReality

;-------------------;
;     Ms counter    ;
;-------------------;
Timer1Aisr:
    PUSH    WR1                 ; Must save the working register and the
    IN      WR1, SREG           ; Status register on the stack before
    PUSH    WR1                 ; using them

    INC     T1                  ; Increment the counter, and continue
     BRNE    Timer1AisrEnd      ; if an overflow appends.
    INC     T2                  ; Else, just go to the RETI instruction
     BRNE    Timer1AisrEnd
    INC     T3
    
Timer1AisrEnd:                  ; Now, T1 = T1 + 1, retrieve the value of
    POP     WR1                 ; WR1 and SREG from the stack
    OUT     SREG, WR1
    POP     WR1

    RETI

;------------------;
;    UART Send     ;
; Arg1: character  ;
;------------------;
UARTSend:
    PUSH    WR1                     ; Save WR1 on the stack

    LDS     WR1, UCSR0A             ; Load the Usart status reg A on WR1
    BST     WR1, UDRE0              ; move Usart data reg empty bit in T bit of SREG
    BRTC    PC - 3                  ; Loop until data reg empty (return 3 lines upward)

    STS     UDR0, Arg1              ; Send the current byte

    POP     WR1

    RET

;---------------------------;
;      UART send string     ;
; Z reg must contain the    ;
; address of the array to   ;
; send through serial line  ;
;---------------------------;
UARTSendString:
    PUSH    WR1             ; Save working regs on the stack
    PUSH    WR2

UARTSendStringReadArray:    ; Loop which reads each character of the array
    LPM     WR1, Z+         ; Load each char, and move the pointer to the next
    TST     WR1             ; Test if end of array reached
    BREQ    UARTSendStringEnd

    LDS     WR2, UCSR0A     ; Load Usart status reg 1 on WR2
    BST     WR2, UDRE0      ; Move UDRE0 bit in SREG T bit
    BRTC    PC - 3          ; Loop until UDRE0 = 1, ie buffer free

    STS     UDR0, WR1       ; Send the current character

    RJMP    UARTSendStringReadArray

UARTSendStringEnd:

    POP     WR2
    POP     WR1

    RET
