default:
	cd fork_apps; make
	cd makeprocs; make

clean:
	cd fork_apps; make clean
	cd makeprocs; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 5; ee469_fixterminal
