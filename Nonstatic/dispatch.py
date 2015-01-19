import os
import render

def error():
	print render.render('error.html', {'title': 'Not Found'})
	exit(1)

class post:
	title = ''
	body = ''
	mid = ''

def handleBlog(params):
	path = params['name']
	path = 'blog/' + path

	if not os.path.isfile(path):
		error()

	posts = []
	f = open(path, 'r').read().replace('\n', '<br>').split('{{title}}')
	for line in f:
		splitpost = line.split('<br>', 1)
		title = splitpost[0]
		body = splitpost[1]
		body = body + '<br>'

		p = post()
		p.body = body
		p.title = title
		p.mid = '_'.join(title.split())
		posts.append(p)

	print render.render('blog.html', {'title':'Blog', 'posts':posts})

def handleImages():
	relevant_path = '../Static/imagepage'
	included_extenstions = ['jpg']
	images = [fn for fn in os.listdir(relevant_path) if any([fn.endswith(ext) for ext in included_extenstions])];
	print render.render('images.html', {'title':'images', 'imageslist':images})

path = os.environ['QUERY_PATH']

# remove leading slashes
path = path.strip('/')
paramlist = os.environ['QUERY_PARAMS']	
paramlist = paramlist.split('&')

params = {}
for setting in paramlist:
	settinglist = setting.split('=')
	if (len(settinglist) != 2):
		continue
	params[settinglist[0]] = settinglist[1]

if (path == 'images'):
	#print render.render('images.html', {'title':'Images'})
	handleImages()
elif (path == 'index'):
	print render.render('index.html', {'title':"Jonathan Meisel's Website"})
elif (path == 'about'):
	print render.render('about.html', {'title':'About'})
elif (path == 'blog'):
	handleBlog(params)
else:
	error()

