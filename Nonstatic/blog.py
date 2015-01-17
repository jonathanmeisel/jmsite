#author Jonathan Meisel

class Blog:
	title = ''
	text = ''

	def __init__(self, filename):
		f = open(filename)
		self.title = f.readline()
		self.text = f.read().replace('\n', '<br>')


