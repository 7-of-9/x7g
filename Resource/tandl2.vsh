vs.1.0

dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3
dp3 r5, v1, c13
sge r5, -r5, -c11.x        ; make it 0 for negative and zero values (1 for others
mul oD0, r5, c9
mov oT0, v2
mul r5, r5, c9
dp3 r0, v1, c4
max r0, r0, c11.x        ; zero out negative values
mul r0, r0.x, r5
dp3 r1, v1, c11
max r1, r1, c11.x        ; zero out negative values
mov r1.w, c11.z
lit r2, r1
mad r1, r2.z, r5, r0
mul oD0, r1, c8






