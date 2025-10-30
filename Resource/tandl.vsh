vs.1.0
;
; INPUT REGISTERS
; v0 - v15  : Vertex Data Registers
; ro - r11  : Temp. Registers
; c1 - c95  : Constants Registers
; a0	      : Address Register
;
; OUTPUT REGISTERS
; oD0		: Diffuse Color [vector4]
; oD1		: Specular Color [vector4]
; oFog	: Fog Register [float]
; oPos	: Position Register [vector4] MUST BE WRITTEN! - homogenous clipping space.
; oPts	: Point Size Register [float]
; oT0 - oT7 : Tex. Coord Registers [vector4]
;

;
; EXPECTED INPUTS
; c0 - c3 	: word x view x proj. transposed
; c4 - c6	: word inverse transposed
; c7		: light direction
; c8		: light colour
; c9		: material colour
; c10		: ambient colour
;
; c11		: halfway vector (between eye and light) in model space
; c11		: specular power constants (0,10,50,100)
; c13		: points to the eye in model space
;

; TRANSFORM
dp4 oPos.x, v0, c0	; xfrm position
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

mov oT0, v2			; output tex coords

; LIGHTING, AMBIENT + MATERIAL
dp3 r0.x, v1, c4		; xfrm normal
dp3 r0.y, v1, c5
dp3 r0.z, v1, c6
dp3 r0.w, r0, r0		; normalize normal
rsq r0.w, r0.w
mul r0, r0, r0.w
dp3 r1, r0, c7		; I dot N
max r1, r1, c11.x
;mul r1, r1, c8		; modulate light colour

mov r9, c10			; modulate ambient with material colour
mul r2, r1, c9		
mul r9, r9, r2

mov r9.w, c9		; use alpha from material

; LIGHTING, SPECULAR
dp3 r2, v1, c11		; dp of normal and half vec.
max r2, r2, c11.x		; zero out -ve values
mov r2.w, c11.z		; raise to the specular index!
lit r3, r2
mul r3, r3, c9		; modulate material colour
add r3, r3, r9		; add previous diffuse colour
mul oD0, r3, c9		; modulate with material colour again
