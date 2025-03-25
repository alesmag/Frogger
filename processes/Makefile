frogger.out: utils.o car.o frog.o gameArea.o inits.o log.o menus.o projectiles.o table.o main.o
	gcc utils.o car.o frog.o gameArea.o inits.o log.o menus.o projectiles.o table.o main.o -o frogger.out -lncurses

utils.o: utils/utils.c utils/utils.h
	gcc -c utils/utils.c

car.o: car.c utils/utils.h
	gcc -c car.c

frog.o: frog.c utils/utils.h
	gcc -c frog.c

gameArea.o: gameArea.c utils/utils.h
	gcc -c gameArea.c

inits.o: inits.c utils/utils.h
	gcc -c inits.c

log.o: log.c utils/utils.h
	gcc -c log.c

menus.o: menus.c utils/utils.h
	gcc -c menus.c

projectiles.o: projectiles.c utils/utils.h
	gcc -c projectiles.c

table.o: table.c utils/utils.h
	gcc -c table.c

main.o: main.c utils/utils.h
	gcc -c main.c

clean:
	rm -f *.o
	rm -f frogger.out
	rm -f AUDIO/dump.txt