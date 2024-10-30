# Lukas Sarkauskas, 22415904 - Operating Systems Project 1
# ca216/myshell
# I Lukas Sarkauskas declare that I have followed DCU's Academic integrity policy

myshell: myshell.c commands.c utility.c utility.h commands.h
	@mkdir -p ../bin
	@if [ ../bin/myshell -nt myshell.c -a ../bin/myshell -nt commands.c -a ../bin/myshell -nt utility.c ]; then \
		echo "myshell is up to date."; \
	else \
		gcc -Wall myshell.c commands.c utility.c -o ../bin/myshell && echo "myshell has been built."; \
	fi