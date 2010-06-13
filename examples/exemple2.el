
-* Example from HSCC04 *-
-* Hytech parametric analysis only terminate with a non-null inferior bound on the parameter (delta1>1/10) *-

-*--------------------------------------------------------------------
			Declarations
--------------------------------------------------------------------*-

define(alpha, 2) -- ou 1

var
	t, x, y ,z: clock;

-*--------------------------------------------------------------------
			Controller
--------------------------------------------------------------------*-

elastic automaton controleur

eventlabs : B;
internlabs : ;
orderlabs : A, C;

initially c1  & t=0 & z=0;

loc c1 :
	when True put A do {t' = 0} goto c2;
loc c2 : 
	when get B & t<=3/4  goto c3;
        when get B & t>=3/4  do{z'=0} goto c4;
loc c3 :
	when t>=3/4 do{z'=0} goto c4;
loc c4 :
        when True put C do{z'=0} goto c1;
end

-*--------------------------------------------------------------------
			Environment
--------------------------------------------------------------------*-

automaton environment

synclabs: A, 
	  B, 
	  C;
initially e1 & x=0 & y=0;

loc e1: while True wait {}
	when True sync A do {x' = 0} goto e2;
	when x>alpha goto Bad;
	when True sync C goto Bad;    -- receptiveness checking

loc e2: while y<=1 wait {}
	when True sync B goto e3;
	when x>alpha goto Bad;

	when True sync A goto Bad;    -- receptiveness checking
	when True sync C goto Bad;    -- receptiveness checking

loc e3: while True wait {}
	when x>=1/2  sync C do{y'=0}  goto e1;
	when x>alpha goto Bad;

	when True sync A goto Bad;    -- receptiveness checking
        when x<1/2 sync C goto Bad;   -- receptivenes checking
loc Bad: while True wait{}

end



bad  := loc[environment] = Bad ;

