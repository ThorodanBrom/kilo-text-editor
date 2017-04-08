#1st line -> what we want to build:what is required to build it
#indent with tabs
#2nd line -> command used to build  kilo 
# Wall,Wextra,pendantic -> warnings
kilo:kilo.c
	$(CC) kilo.c -o kilo -Wall -Wextra -pedantic -std=c99

