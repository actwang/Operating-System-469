default:
	cd makeprocs; make
	cd injectCO; make
	cd injectS2; make
	cd react1; make
	cd react2; make
	cd react3; make

clean:
	cd makeprocs; make clean
	cd injectCO; make clean
	cd injectS2; make clean
	cd react1; make clean
	cd react2; make clean
	cd react3; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 1 4; ee469_fixterminal