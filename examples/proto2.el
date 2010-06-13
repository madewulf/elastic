-* The philips audio control protocol with 2 senders *-
-* Only usable with UPPAAL *-
define(sFreq,1)
define(bound,8) 
define(boundminusone,7)
define(Bbound,9)
define(Bboundminusone,8)
define(delta,1/8)
var Ax,z ,y, Bx
    : clock;
    Acount, Asending, Astarting, Ac, Aleng, Ap, AdoubleZero, Acol,
    Bcount, Bstarting, Bp, BdoubleZero,
    i,Bi,
    m,r,
    upi
    : discrete ;

automaton wire 
synclabs : Aup, Adown, AtestVolt, AVoltYes, AVoltNo, Bup, Bdown, BtestVolt, BVoltYes, BVoltNo, up; 

initially isDown & upi=0 ;

loc isDown : while True wait{}
	when True sync Aup do{upi'=1}  goto isUp0;
	when True sync Bup do{upi'=1} goto isUp0;
	when True sync AtestVolt goto isDown2;
	when True sync BtestVolt goto isDown3;

loc isDown2 : while True wait{}
	when asap sync AVoltNo goto isDown;

loc isDown3 : while True wait{}
	when asap sync BVoltNo goto isDown;

loc isUp0 : while True wait{}
	when asap sync up  goto isUp;

loc isUp : while True wait{}
	when upi=1 sync Adown do{upi'=0} goto isDown;
	when upi>1 sync Adown do{upi'=upi-1} goto isUp;
	when True sync Aup do{upi'=upi+1} goto isUp;

	when upi=1 sync Bdown do{upi'=0} goto isDown;
	when upi>1 sync Bdown do{upi'=upi-1} goto isUp;
	when True sync Bup do{upi'=upi+1} goto isUp;
	
	when True sync AtestVolt goto isUp2;
	when True sync BtestVolt goto isUp3;

loc isUp2 : while True wait{}
	when asap sync AVoltYes goto isUp;

loc isUp3 : while True wait{}
	when asap sync BVoltYes goto isUp;
end


elastic automaton Asender 
eventlabs : AVoltYes, AVoltNo;
internlabs : ;
orderlabs :  Aup, Adown, AtestVolt;
 
initially Aidle & Ax=0  & Asending=0 & Astarting=0 & Acol=0 & Ac=0 & Aleng=0 & AdoubleZero=0 & Acount=0 & Ap=0; 

loc Aidle : 
	when Ax>=sFreq put AtestVolt do{Ax'=0} goto Aidle2 ;

loc Aidle2 : 
	when get  AVoltYes & True do{Ax'=0, Acount'=0} goto Aidle;
	when get  AVoltNo & Acount<= boundminusone do{Ax'=0, Acount'=Acount+1} goto Aidle;
	when get  AVoltNo & Acount = bound goto Aidle3;

loc Aidle3 : 
	when True put Aup do{Ax'=0, Aleng'=1, Ac'=1, Ap'=1, Acount'=0,Astarting'=1,Asending'=1}  goto AoneSent; 

loc AoneSent : 
       when Ax>=2  & Ax <=2 & i=1  put Adown do{Ax'=0, Astarting'=0} goto AwaitingOne ;
       when Ax>=4  & Ax <=4 & i=0 & Astarting=0 put Adown do{ Ax'=0, Ap'=1-Ap, Aleng'=Aleng+1, Ac'=2 Ac,AdoubleZero'=0} goto AzeroSent; 
       when Ax>=2  & Ax <=2 & Ap=1 & Astarting=0 put Adown do{Asending'=0, Ax'=0, Ap'=0} goto Aidle;	

loc AwaitingOne : 
	when Ax>=sFreq & Acount<=0 put AtestVolt do{Ax'=0} goto AwaitingOne2;
	when Ax>=sFreq & Acount=1 put Aup do{Acount'=0,Ax'=0, Ap'=1-Ap, Aleng'=Aleng+1, Ac'=2 Ac+1} goto AoneSent;

loc AwaitingOne2 : 
	when get AVoltYes & True do{Acount'=0,Asending'=0,Ax'=0,Acol'=1} goto Aidle;
        when get AVoltNo & True do{Acount'=Acount+1, Ax'=0} goto AwaitingOne;

loc AzeroSent : 
	when Ax>=sFreq put AtestVolt do{Ax'=0} goto AzeroSent2;

loc AzeroSent2 : 
	when get AVoltYes & True do{Acount'=0,Asending'=0,Ax'=0,Acol'=1} goto Aidle; 
	when get AVoltNo & True do{Acount'=0, Ax'=0} goto AzeroSent3;

loc AzeroSent3 : 
	when Ax>=sFreq & i=0 put Aup do{Ax'=0} goto AwaitingZero;
	when Ax>=sFreq & Ax<=sFreq & i=1 do{Ax'=0} goto AzeroSent4;
 	when Ax>=sFreq & Ap=1 do{Asending'=0, Ax'=0, Ap'=0} goto Aidle;
	when Ax>=sFreq & AdoubleZero=1 do{Asending'=0, Ax'=0, Ap'=0 } goto Aidle;

loc AzeroSent4 : 
	when Ax>=sFreq put  AtestVolt do{Ax'=0} goto  AzeroSent5 ;

loc AzeroSent5 : 
	when get AVoltYes & True do{Asending'=0,Ax'=0,Acol'=0} goto Aidle; 
	when get AVoltNo & True do{Ax'=0} goto AzeroSent6;

loc AzeroSent6 : 
	when Ax>=sFreq & Ax<=sFreq put Aup do{ Ax'=0, Ap'=1-Ap, Aleng'=Aleng+1,Ac'=2 Ac+1} goto AoneSent; 

loc AwaitingZero : 
	when Ax>=2  & Ax<=2 put Adown do{Ax'=0,Ap'=1-Ap,Aleng'=Aleng+1,Ac'=2 Ac,AdoubleZero'=1} goto AzeroSent;

end

elastic automaton Bsender 
eventlabs : BVoltYes, BVoltNo;
internlabs : ;
orderlabs :  Bup, Bdown, BtestVolt; 

initially Bidle & Bx=0   & Bstarting=0 & Bp=0 & Bcount=0 & BdoubleZero=0; 

loc Bidle : 
	when Bx>=sFreq put BtestVolt do{Bx'=0} goto Bidle2 ;

loc Bidle2 : 
	when get BVoltYes & True do{Bx'=0, Bcount'=0} goto Bidle;
	when  get  BVoltNo & Bcount<= Bboundminusone do{Bx'=0, Bcount'=Bcount+1} goto Bidle;
	when  get  BVoltNo & Bcount = Bbound goto Bidle3;

loc Bidle3 : 
	when get Bup & True do{Bx'=0,  Bp'=1, Bcount'=0,Bstarting'=1}  goto BoneSent; 

loc BoneSent : 
       when Bx>=2  & Bi=1 & Bstarting=0 put Bdown do{Bx'=0} goto BwaitingOne ;
       when Bx>=4   & Bi=0 put Bdown do{ Bx'=0, Bp'=1-Bp, BdoubleZero'=0, Bstarting'=0} goto BzeroSent; 
       when Bx>=2  & Bp=1 & Bstarting=0 put Bdown do{Bx'=0, Bp'=0} goto Bidle;	

loc BwaitingOne : 
	when Bx>=sFreq & Bcount<=0 put BtestVolt do{Bx'=0} goto BwaitingOne2;
	when Bx>=sFreq & Bcount=1 put Bup do{Bcount'=0,Bx'=0, Bp'=1-Bp} goto BoneSent;

loc BwaitingOne2 :
	when get BVoltYes & True do{Bcount'=0,Bx'=0} goto Bidle;
        when get BVoltNo & True do{Bcount'=Bcount+1, Bx'=0} goto BwaitingOne;

loc BzeroSent : 
	when Bx>=sFreq put BtestVolt do{Bx'=0} goto BzeroSent2;

loc BzeroSent2 : 
	when get BVoltYes & True do{Bcount'=0,Bx'=0} goto Bidle; 
	when get BVoltNo & True do{Bcount'=0, Bx'=0} goto BzeroSent3;

loc BzeroSent3 : 
	when Bx>=sFreq  & Bi=0 put Bup do{Bx'=0} goto BwaitingZero;
	when Bx>=sFreq  & Bi=1 do{Bx'=0} goto BzeroSent4;
 	when Bx>=sFreq  & Bp=1 do{Bx'=0, Bp'=0 } goto Bidle;
	when Bx>=sFreq  & BdoubleZero=1 do{Bx'=0, Bp'=0 } goto Bidle;

loc BzeroSent4 : 
	when Bx>=sFreq put BtestVolt do{Bx'=0} goto  BzeroSent5 ;

loc BzeroSent5 : 
	when get BVoltYes & True do{Bx'=0} goto Bidle; 
	when get BVoltNo & True do{Bx'=0} goto BzeroSent6;

loc BzeroSent6 : 
	when Bx>=sFreq  put Bup do{ Bx'=0, Bp'=1-Bp} goto BoneSent; 


loc BwaitingZero : 
	when Bx>=2  put Bdown do{Bx'=0,Bp'=1-Bp,BdoubleZero'=1} goto BzeroSent;

end

elastic automaton receiver 

eventlabs : up;
internlabs :  finalZero, endMsg; 
orderlabs : ;

initially idle2  & y=0 & m = 0  & r = 0 ;

loc idle2 : 
	when get up & True do{y' = 0  , m' = 1  , r' = 1 } goto last_is_1;

loc last_is_1 : 
	when get up & y >= 3  & y <= 5  do{y' = 0  , m' = 1 - m  , r' = 1 } goto last_is_1;
	when get up & y >= 5  & y <= 7  do{y' = 0  , m' = 1 - m  , r' = 0 } goto last_is_0;
	when get up & y >= 7 do{y' = 0  , r' = 2 } goto last_is_1;
	when y >= 9  & m = 1 put endMsg do{y' = 0 } goto idle2;
	when y >= 9  & m = 0 put finalZero do{y' = 0  , r' = 0  , m' = 1 - m } goto idle2;

loc last_is_0 : 
	when get up & y >= 3  & y <= 5 do{y' = 0  , m' = 1 - m  , r' = 0 } goto last_is_0;
	when get up & y >= 5  do{y' = 0  , r' = 2 } goto last_is_1;
	when y >= 7 put finalZero do{y' = 0  , r' = 0 } goto idle2;
end

automaton checkOutput 
synclabs : getup, finalZero, endMsg; 

initially checkIdle  & z = 0  & Aleng = 0  & Ac = 0  & AdoubleZero = 0  ;

loc checkIdle : while True wait{}
	when Asending=1 sync getup do{z' = 0 } goto treating;
	when Asending=0 sync getup do{z'=0,Ac'=0,  Aleng'=0} goto checkIdle; 

	when True sync finalZero do{z'=0, Aleng'=0, Ac'=0} goto checkIdle ;
	when True sync endMsg do{z'=0, Aleng'=0, Ac'=0} goto checkIdle ;

loc check : while True wait{} 
	    
	when True sync getup do{z'=0} goto treating;

        when Aleng=1 & Ac=0  sync finalZero do {Aleng'=0} goto checkIdle;

        when Aleng=0 sync finalZero do {Aleng'=0, Ac'=0} goto cerror;
        when Aleng>1 sync finalZero do {Aleng'=0, Ac'=0} goto cerror;
	when Ac>0 sync finalZero do{Aleng'=0, Ac'=0} goto cerror;

	when Aleng = 0 sync endMsg  do{Aleng'=0, Ac'=0} goto checkIdle;
        when Aleng >0 sync endMsg do{Aleng'=0, Ac'=0} goto cerror;

loc treating : while z <= 0 wait{}
	when Aleng <=0  do{z'=0} goto cerror;
	when r = 0  & Aleng = 1  & Ac = 1 do{z' = 0 } goto cerror;
	when r = 0  & Aleng = 1  & Ac = 0 do{Aleng' = Aleng - 1  , z' = 0 } goto check;
	when r = 0  & Aleng = 2  & Ac > 1 do{z' = 0 } goto cerror;
	when r = 0  & Aleng = 2  & Ac <= 1 do{Aleng' = Aleng - 1  , z' = 0 } goto check;
	when r = 0  & Aleng = 3  & Ac > 3 do{z' = 0 } goto cerror;
	when r = 0  & Aleng = 3  & Ac <= 3 do{Aleng' = Aleng - 1  , z' = 0 } goto check;
	when r = 1  & Aleng = 1  & Ac = 0 do{z' = 0 } goto cerror;
	when r = 1  & Aleng = 1  & Ac = 1 do{Aleng' = Aleng - 1  , Ac' = Ac - 1  , z' = 0 } goto check;
	when r = 1  & Aleng = 2  & Ac <= 1 do{z' = 0 } goto cerror;
	when r = 1  & Aleng = 2  & Ac > 1 do{Aleng' = Aleng - 1  , Ac' = Ac - 2  , z' = 0 } goto check;
	when r = 1  & Aleng = 3  & Ac <= 3 do{z' = 0 } goto cerror;
	when r = 1  & Aleng = 3  & Ac > 3 do{Aleng' = Aleng - 1  , Ac' = Ac - 4  , z' = 0 } goto check;
	when r = 2  & Aleng = 1 do{z' = 0 } goto cerror;
	when r = 2  & Aleng = 2  & Ac = 1 do{Aleng' = 0  , Ac' = 0  , z' = 0 } goto check;
	when r = 2  & Aleng = 2  & Ac = 0 do{z' = 0 } goto cerror;
	when r = 2  & Aleng = 2  & Ac > 2 do{z' = 0 } goto cerror;
	when r = 2  & Aleng = 3  & Ac = 3 do{Aleng' = 1  , Ac' = 1  , z' = 0 } goto check;
	when r = 2  & Aleng = 3  & Ac = 2 do{Aleng' = 1  , Ac' = 0  , z' = 0 } goto check;
	when r = 2  & Aleng = 3  & Ac > 3 do{z' = 0 } goto cerror;
	when r = 2  & Aleng = 3  & Ac < 2 do{z' = 0 } goto cerror;

loc cerror : while z<=6 wait{}
	when True sync getup do {Aleng'=0, Ac'=0} goto cerror;
	when True sync finalZero do{Aleng'=0,Ac'=0} goto cerror;
	when True sync endMsg do{Aleng'=0, Ac'=0} goto cerror; 
 
	when z>=6 & Acol=0 do{z'=0} goto cerror2;
	when z>=6 & Acol=1 do{Acol'=0, Aleng'=0, Ac'=0,z'=0} goto checkIdle;

loc cerror2 : while z<=0 wait{}
	when True goto cerror2;
end	

automaton random 
synclabs : ; 

initially ran & i = 0  & Bi = 0 ;

loc ran : while True wait{}
	when True do{i' = 1 } goto ran;
	when True do{i' = 0 } goto ran;
	when True do{Bi' = 1 } goto ran;
	when True do{Bi' = 0 } goto ran;
end

init := param[Asender]=delta & param[Bsender]=delta & param[receiver]=delta ;

bad  := loc[checkOutput] = cerror2 ;

view[up]=getup; 