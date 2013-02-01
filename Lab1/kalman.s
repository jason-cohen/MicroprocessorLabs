; kalman.s

; comments, purpose of code, inputs, outputs, errors.

	AREA FILTERCODE, CODE ; Define area to be code
	EXPORT kalman ; Declare symbol to be exported

kalman
	MOV R5, R1
	MOV R6, #3
	BX R14
	
	END
	