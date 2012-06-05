
----
----Run this on a max file which has only objects that are candidates of
---- being instanced
---(ie: remove all other objects)
---Also, the objects need to be Editable meshes.
rollout test "test" (
	
	button geom "geom"

	
	--Given object o1 can be represented as an instance of o2
	--guess the transform that achieves that
	--makes a new object which is an instance of o2
	--as a clone of o1
	--delete o1
	function repl o1 o2 = 
	(		    			
		m = SgpuInstanceMaker.GetTransform o1 o2
		error = SgpuInstanceMaker.GetError  o1 o2
		o3 = instance o2
		--o3name = o2.name					    
		m2 = o2.transform
		o3.transform = m * m2
		o3.objectoffsetpos =o2.objectoffsetpos
		o3.objectoffsetrot =o2.objectoffsetrot
		o3.objectoffsetscale =o2.objectoffsetscale
		
		format "\t\tmatrix %\n"  m
		format "\to1 matrix %\n" o1.transform
		format "\to1 offset pos %\n" o1.objectoffsetpos		
		format "\to1 offset quat %\n" o1.objectoffsetrot				
		format "\to21 offset scale %\n" o1.objectoffsetscale
		format "\to3 matrix %\n" o3.transform
		format "\t\terror %\n"  error
		format "\t\treplacing % with %\n" o3.name o2.name	
		delete o1
		
	)

	function instanceMaker =
	(
		bag1 = for o in objects collect o
		--for i = 1 to bag1.count do (
		bagofbags = #()	
		--first form a bag of bags.
		--Each bag in the bag of bags contain objects that are instancable
		--Every object in the bag can be represented as an instance of the
		--first one
		while bag1.count > 0 do 
		(
			o1 = bag1[1]
			deleteItem bag1 1
			bag2 = #()
			append bag2 o1
			bag3 = #()
			for o2 in bag1 do
			(
				bCanInstance = SgpuInstanceMaker.CanExpressAsInstances o2 o1 
				if bCanInstance then (
					    format "appending bag3 o2%\n" o2
						append bag3 o2						
				)								
			)
			for o2 in bag3 do
			(
				idx = findItem bag1 o2
				if ( idx <= 0) then continue
				deleteItem bag1 idx
				append bag2 o2
				format "appending bag2 o2%\n" o2
			)
			--format "bagofbags %\n" bag2
			append bagofbags bag2
		)
		--do the actual instancing
		for b in bagofbags do 
		(
			if b.count > 1 then 
			(
				o1 = b[1]
				deleteItem b 1
				while b.count > 0 do
				(
						o2 = b[1]		
						repl o2 o1
					    deleteItem b 1	
				)				
			)
		)
	)
	
	function replaceInstance o2 o1 = (
		m = SgpuInstanceMaker.GetTransform o2 o1
		m2 = o1.transform
		o3 = instance o1
		o3.transform = m * m2		
	)
	
	on geom  pressed do
	( 
		instanceMaker()
	)

)



try 
(
  destroyDialog test
)
catch()

createDialog test