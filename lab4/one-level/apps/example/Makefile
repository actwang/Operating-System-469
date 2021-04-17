default:
	cd makeprocs; make
	cd Q2_1; make
	cd Q2_2; make
	cd Q2_3; make
	cd Q2_4; make
	cd Q2_5; make
	cd Q2_6; make

clean:
	cd makeprocs; make clean
	cd Q2_1; make clean
	cd Q2_2; make clean
	cd Q2_3; make clean
	cd Q2_4; make clean
	cd Q2_5; make clean
	cd Q2_6; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 6; ee469_fixterminal

debug: 
	cd ../../bin; dlxsim -x os.dlx.obj -a -D -p -u makeprocs.dlx.obj 6;ee469_fixterminal