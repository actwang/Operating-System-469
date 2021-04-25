default:
	cd fork_apps; make

clean:
	cd fork_apps; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u fork_apps.dlx.obj; ee469_fixterminal
	
debug: 
	cd ../../bin; dlxsim -x os.dlx.obj -a -D -p -u fork_apps.dlx.obj; ee469_fixterminal