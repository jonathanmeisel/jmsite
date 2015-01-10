import sys

def render(filename):
	f = open(filename, 'r')

	for line in f:
		if line.startswith('###'):
			f2 = line.rstrip()
			f2 = f2.strip('###')
			render(f2)
		else:
			print line,


