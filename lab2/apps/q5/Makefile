default:
	cd makeprocs; make
	cd injectH2O; make
	cd injectSO4; make
	cd react1; make
	cd react2; make
	cd react3; make

clean:
	cd makeprocs; make clean
	cd injectH2O; make clean
	cd injectSO4; make clean
	cd react1; make clean
	cd react2; make clean
	cd react3; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 3; ee469_fixterminal
