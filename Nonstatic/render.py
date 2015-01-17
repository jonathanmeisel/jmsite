"""
@summary: Has all the necessary functions needed to render a page to HTML. 
@author: Abhishek Shrivastava <i.abhi27 [at] gmail [dot] com>
"""
import re
import sys
from yaptu  import copier

def render(page_name, value_dict):
    """
    Pass the name of the controller's template file and the dict of values to substitute.
    Returns ready-to-be-pushed HTML.
    
    """
    page_tpl = open(page_name,'r').read()
    render_tpl = open('template.html','r').read()
    
    page_html = templatify(page_tpl, value_dict)   	  
    
    return templatify(render_tpl, {'page_html' : page_html, 'title':value_dict['title']})

def templatify(tpl, values):
    """
    Custom wrapper around the YAPTU template engine. Does the usual job.
    
    """
    class DecoyFile:
    
        def __init__(self):
            self.data = []
            
        def write(self, line):
            self.data.append(line)
        
        def read(self):
            return "".join(self.data)
    
    rex=re.compile('%([^@]+)s')
    rbe=re.compile('\+')
    ren=re.compile('-')
    rco=re.compile('= ')
    decoy = DecoyFile()    
    cop = copier(rex, values, rbe, ren, rco, ouf=decoy)
    lines_block = [line+'\n' for line in tpl.split('\n')]
    cop.copy(lines_block)
    return decoy.read()

