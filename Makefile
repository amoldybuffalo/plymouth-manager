all: cli gui

cli:
	gcc cli-client/main.c common/utils.c -o target/pm

gui:
	gcc -o target/pm-gui graphical-client/main.c common/utils.c common/map.c graphical-client/themes.c `pkg-config --cflags --libs gtk4`

run:
	target/pm-gui

run-cli:
	sudo target/pm

verify:
	sudo plymouthd ; sudo plymouth --show-splash ; sleep 5 ; sudo killall plymouthd