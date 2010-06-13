-- The Elastic specification of the whole system for the audio control protocol of FM05
-- Hytech parametric analysis only terminates with stronger constraints than delta1 + delta2 <1/2 *-


var 

x,y,z  : clock;
i, c,leng, m,p,r,doublezero
: discrete;
automaton random 
synclabs : ;
initially ran & i=0 ;

loc ran : while True wait{}
          when True do {i'=1} goto ran;
          when True do {i'=0} goto ran;
end


elastic automaton sender 

eventlabs : ;
internlabs: ; 
orderlabs :  up, down;

initially idle & x=0 & p=0 & i=0 & leng=0 & c=0 & doublezero=0;

loc idle : 
	when x>=12 put up do{x'=0,p'=1,  c'=1, leng'=1} goto oneSent;

loc oneSent :
        when x>=2 & x<=2 & i=1 put  down do{x'=0} goto waitingOne ;
	when x>=4 & x<= 4 & i=0 put  down do{ x'=0, p'=1-p, leng'=leng+1,c'=2 c,i'>=0,i'<=1,doublezero'=0} goto zeroSent; 
	when x>=2 & x<=2 & p=1  put down do{x'=0,p'=0} goto idle;
     
loc waitingOne : 
	when x>=2 & x<=2 put up do{x'=0, p'=1-p, leng'=leng+1, c'=2 c +1,i'>=0,i'<=1} goto oneSent;   

loc zeroSent : 
	when x>=2 & x<=2  & i=0 put  up do {x'=0} goto waitingZero;
	when x>=4 & x <= 4  & i=1 put up do{ x'=0, p'=1-p, leng'=leng+1, c'=2 c + 1,i'>=0,i'<=1} goto oneSent; 
 	when  x>=2 & x<=2 & p=1  do{x'=0,p'=0} goto idle;
 	when  x>=2 & x<=2 & doublezero=1 do{x'=0, p'=0} goto idle;


loc waitingZero : 
	when x>=2 & x<=2 put  down do{x'=0, p'=1-p,  leng'=leng+1,c'=2 c,i'>=0,i'<=1,doublezero'=1} goto zeroSent;
end

elastic automaton receiver 

eventlabs : up;
internlabs : finalZero; 
orderlabs : ; 
initially idle2 & y=0 & m=0 & r=0   ;

loc idle2: 
        when get up & True  do {y'=0,m'=1,r'=1} goto last_is_1;
loc last_is_1 : 
        when  get up & 3<=y & y<=5  do {y'=0,m'=1-m,r'=1} goto last_is_1; 
        when  get up & 5<=y & y<=7  do {y'=0,m'=1-m,r'=0} goto last_is_0;
        when  get up & 7<=y         do {y'=0,r'=2} goto last_is_1;
        when  y>=9 & m=1 do {y'=0} goto idle2; 
        when  y>=9 & m=0 put finalZero do{y'=0,r'=0,m'=1-m}   goto idle2;
loc last_is_0 : 
        when get up & 3<=y & y<=5  do {y'=0, m'=1-m, r'=0} goto last_is_0;
        when get up & 5<=y         do {y'=0, r'=2} goto last_is_1; 
        when y>=7 put finalZero    do {y'=0, r'=0} goto idle2;
        
end

automaton checkOutput

synclabs : getup , finalZero;

initially check & z=0 & leng=0 & c=0 & doublezero=0 ;

loc check : while True wait{}
	when True sync getup do {z'=0} goto treating;
	when True sync finalZero do {z'=0} goto treating;
	when leng>3 goto cerror;
	when leng<0 goto cerror;


loc treating : while z<=0 wait{}
        when r=0 & leng=1 & c=1 do{z'=0} goto cerror;
        when r=0 & leng=1 & c=0 do{leng'=leng-1,z'=0} goto check;
	when r=0 & leng=2 & c>1 do{z'=0}  goto cerror;
        when r=0 & leng=2 & c<=1 do{leng'=leng-1,z'=0} goto check;
        when r=0 & leng=3 & c>3 do{z'=0} goto cerror;
	when r=0 & leng=3 & c<=3 do{leng'=leng-1,z'=0} goto check;

	when r=1 & leng=1 & c=0 do{z'=0} goto cerror;
	when r=1 & leng=1 & c=1 do{leng'=leng-1,c'=c-1,z'=0} goto check; 
	when r=1 & leng=2 & c<=1 do{z'=0} goto cerror;
        when r=1 & leng=2 & c>1 do{leng'=leng-1,c'=c-2,z'=0} goto check;
        when r=1 & leng=3 & c<=3 do{z'=0} goto cerror;
	when r=1 & leng=3 & c>3 do{leng'=leng-1,c'=c-4,z'=0} goto check;


	when r=2 & leng=1 do{z'=0} goto cerror;
	when r=2 & leng=2 & c=1 do{leng'=0, c'=0,z'=0} goto check;
	when r=2 & leng=2 & c=0 do{z'=0} goto cerror;
	when r=2 & leng=2 & c>2 do{z'=0} goto cerror;
	when r=2 & leng=3 & c=3 do {leng'=1, c'=1,z'=0} goto check;
	when r=2 & leng=3 & c=2 do {leng'=1,c'=0,z'=0} goto check;
	when r=2 & leng=3 & c>3 do{z'=0} goto cerror;
        when r=2 & leng=3 & c<2 do{z'=0} goto cerror;

loc cerror : while True wait{} 

end 

init := param[sender]= 1/5 & param[receiver]=1/5;

bad  := loc[checkOutput] = cerror ;

view[up]=getup; 