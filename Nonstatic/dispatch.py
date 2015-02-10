import os
import render
import re
from os import listdir

def error():
	print render.render('error.html', {'title': 'Not Found'})
	exit(1)

class post:
	title = ''
	body = ''
	mid = ''

def handleDir(path, staticfiles):
	if '..' in path:
		error()
	
	class fileInfo:
		name = ''
		path = ''
		reg = False
		size = 0

	files = []

	for f in listdir(staticfiles + path):
		fullpath = staticfiles + path + f
		if f.startswith('.'):
			continue

		fi = fileInfo()
		fi.name = f
		fi.path = f

		if (os.path.isfile(fullpath)):
			fi.reg = True
			fi.size = os.path.getsize(fullpath)
		else:
			fi.path = fi.path + '/'
		files.append(fi)

	print render.render('dir.html', {'title':path, 'files':files})

def handleBlog(params):
	path = params['name']
	path = 'blog/' + path

	if not os.path.isfile(path):
		error()

	posts = []
	f = open(path, 'r').read().replace('\n', '<br>').split('//split<br>')
	for line in f:
		splitpost = line.split('<br>', 1)
		title = splitpost[0]
		body = splitpost[1]
		body = body + '<br>'

		p = post()
		p.body = body
		p.title = title
		p.mid = re.sub(r'\W+', '', title)
		posts.append(p)

	print render.render('blog.html', {'title':'Blog', 'posts':posts})

class ImagePage:

	def __init__(self):
		self.title = ''
		self.key = ''
		self.images = []

def getImages():
	imagefile = open('images.txt', 'r')
	imagepages = imagefile.read().split('\n#\n')

	ips = []
	keyedips = {}

	for page in imagepages:
		rows = page.split('\n')
		
		if len(rows) < 2:
			continue
		
		firstrow = rows[0].split(' ', 1)
		if len(firstrow) < 2:
			continue

		ip = ImagePage()
		ip.key = firstrow[0]
		ip.title = firstrow[1]

		for i in range(1, len(rows)):
			r = rows[i].split(' ')
			ip.images.append(rows[i].split(' '))

		ips.append(ip)
		keyedips[ip.key] = ip

	return ips, keyedips


def handleImages(params, ips, keyedips):
	name = ''
	# Either it's the parameter, or default
	if params.has_key('gallery'):
		name = params['gallery']

	if not keyedips.has_key(name):
		name = ips[0].key

	print render.render('images.html', {'title':'images', 'gallery':keyedips[name], 'images':ips})

path = os.environ['QUERY_PATH']

# remove leading slashes
path = path.lstrip('/')
paramlist = os.environ['QUERY_PARAMS']	
paramlist = paramlist.split('&')

params = {}
for setting in paramlist:
	settinglist = setting.split('=')
	if (len(settinglist) != 2):
		continue
	params[settinglist[0]] = settinglist[1]

staticfiles = '../Static/'

if (path == 'images'):
	ips, keyedips = getImages()
	handleImages(params, ips, keyedips)
elif (path == 'index'):
	print render.render('index.html', {'title':"Jonathan Meisel's Website"})
elif (path == 'about'):
	print render.render('about.html', {'title':'About'})
elif (path == 'blog'):
	handleBlog(params)
elif (path.endswith('/') and os.path.isdir(staticfiles + path)):
	handleDir(path, staticfiles)
else:
	error()

