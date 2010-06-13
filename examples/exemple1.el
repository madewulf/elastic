-* simplified example from HSCC04 *-
-* Hytech parametric analysis only terminate with a non-null inferior bound on the parameter (delta1>1/10) *-

-*--------------------------------------------------------------------
			Declarations
--------------------------------------------------------------------*-

define(alpha, 2) -- ou 1

var
	w, x, y : clock;


elastic automaton controller

eventlabs : B;
internlabs : ;
orderlabs : A, C;

initially c1  & w=0;

loc c1 :
	when w>=1 put A do {w' = 0} goto c2;
loc c2 : 
	when get B & True  goto c3;
loc c3 :
	when True put C goto c1;
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
	when x>=alpha goto Bad;
	when True sync C goto Bad;    -- receptiveness checking

loc e2: while y<=1 wait {}
	when y>=1 sync B goto e3;
	when x>=alpha goto Bad;

	when True sync A goto Bad;    -- receptiveness checking
	when True sync C goto Bad;    -- receptiveness checking

loc e3: while True wait {}
	when True sync C do{y'=0}  goto e1;
	when x>=alpha goto Bad;

	when True sync A goto Bad;    -- receptiveness checking

loc Bad: while True wait{}

end


init := param[controller]=1/5 ;

bad  := loc[environment] = Bad ;
