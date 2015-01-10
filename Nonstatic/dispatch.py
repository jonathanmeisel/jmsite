import os
import render

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
	render.render('images.html')
if (path == 'index'):
	render.render('index.html')
if (path == 'about'):
	render.render('about.html')