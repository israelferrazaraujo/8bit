1 screen 1:clear 400:defint a-z
2 f=32:width f
5 r$="":print "INIT:";:g=2*f-5:for x=1 to g:m$=chr$(1)+chr$(93.5+rnd(1)):r$=r$+m$:print m$;:next
10 cls:c=f/2:r=f/2-7:w=15:d=0:s=6144+(32-f)/2
20 l=0:for z=0 to 1 step 0:x=rnd(1)*10
30 if x<4 then r=r-1:if r<1 then r=1
40 if x>5 then r=r+1:if r+w>f-3 then r=f-w-3  
50 n=rnd(1)*(g-f):n=2*n+1:print mid$(r$,n,2*r);spc(w);mid$(r$,n,2*(f-r-w-1))
60 d=d+1:l=l+1:if l>24 then l=0:w=w-1:if w<3 then w=3
70 if d<25 then goto 20
80 A=stick(0):if A=7 then c=c-1 else if A=3 then c=c+1
90 p=vpeek(s+c):if p<>32 then 200
100 vpoke s+c,42:next
200 print spc(f/2-3);"CRASH!":if d>h then h=d
205 print "SCORE:";d,"HIGH:";h:print "PRESS SPACE TO TRY AGAIN":print "OR ESC TO END"
220 a$=inkey$:if a$=chr$(27) then end else if a$<>chr$(32) goto 220
230 goto 10

