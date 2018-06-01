#encoding=utf-8
"""hehe URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/1.8/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  url(r'^$', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  url(r'^$', Home.as_view(), name='home')
Including another URLconf
    1. Add an import:  from blog import urls as blog_urls
    2. Add a URL to urlpatterns:  url(r'^blog/', include(blog_urls))
"""
from django.conf.urls import include, url
from django.contrib import admin
from django.http import HttpResponse
from django.template import loader, Context
from django.views.decorators.csrf import csrf_exempt
from django.shortcuts import render_to_response
from threading import *
import subprocess

htmldoc = """
<html>
<head>
</head>
<body>
<h1>测测你的排序有多快</h1>
<form id="form" method="post" action="/">
void sort(unsigned *a, int n){
<br />
<textarea name="code">
%s
</textarea>
<br />
}
</br>
n=10000，5秒，64MB
</br>
<input type="submit" value="提交" />
</form>
</body>
</html>
"""

lock = Lock()

@csrf_exempt
def index(request):
	response = HttpResponse(content_type="text/html")
	code = ""
	if 'code' in request.POST:
		code = request.POST['code']
		print(code)
		lock.acquire()
		fcode = open("judging.c", "w")
		fcode.write('#include "sort.lib"\n')
		fcode.write('void sort(unsigned *a, int n) {\n')
		fcode.write(code)
		fcode.write('\n}\n')
		fcode.close()
		cp = subprocess.run(["../judgesrv", "judging.c"], stdout=subprocess.PIPE)
		lock.release()
		response.write(str(cp.stdout, 'utf-8').replace('\n', '<br/>'))
		response.write('<br/>');
	response.write(htmldoc % code)
	return HttpResponse(response)

urlpatterns = [
    url('', index),
    # url(r'^admin/', include(admin.site.urls)),
]
