##
# Project Title
#
# @file
# @version 0.1
CFLAGS=-Wall -g

output: pipe.c
	gcc pipe.c line.c -g -o pipe
clean:
	rm -f pipe

# end
