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
; EXPECTED INPUT
; c0 - c3 	: word x view x proj. transposed
; c4 - c6	: word inverse transposed
; c20		: main directional light direction
; c21		: main directional light colour
; c22		: ambient light colour
; c30		: material colour

; transformation
dp4 oPos.x, v0, c0	; xfrm position
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

; tex coords
mov oT0, v2			; output tex coords

; vertex lighting
dp3 r0.x, v1, c4		; xfrm normal
dp3 r0.y, v1, c5
dp3 r0.z, v1, c6

dp3 r0.w, r0, r0		; normalize normal
rsq r0.w, r0.w
mul r0, r0, r0.w

dp3 r1, r0, c20		; N dot L
mul r1, r1, c21		; modulate light colour
mul r1, r1, c30		; modulate material colour
mul r1, r1, c22		; modulate ambient light colour
mov r1.w, c30		; use material alpha
mov oD0, r1			; output final diffuse colour



;add r2, r1, r2		; get total diffuse colour

