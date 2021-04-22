default:
	cd test_heap; make

clean:
	cd test_heap; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u test_heap.dlx.obj; ee469_fixterminal
	
debug: 
	cd ../../bin; dlxsim -x os.dlx.obj -a -D -p -u test_heap.dlx.obj;ee469_fixterminal