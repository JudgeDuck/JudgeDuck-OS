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
import html
import subprocess

htmldoc = """
<html>
<head>
<style type="text/css">
textarea{
height:50%%;
width:100%%;
display:block;
max-width:100%%;
line-height:1.5;
border-radius:3px;
font:16px Consolas;
transition:box-shadow 0.5s ease;
font-smoothing:subpixel-antialiased;
}
</style>
</head>
<body>
%s
<h1>测测你的排序</h1>
<form id="form" method="post" action="/">
n=10000，5秒，64MB，只支持C语言
</br>
<input type="submit" value="提交" />（可能有点慢，在前面无人排队时，响应时间约为5秒+你的运行时间）
</br>
void sort(unsigned *a, int n){
<br />
<textarea name="code">
%s</textarea>
<br />
}
</form>
</body>
</html>
"""

lock = Lock()

@csrf_exempt
def index(request):
	response = HttpResponse(content_type="text/html")
	code = '    for(int i = 0; i < n; i++)\n        for(int j = 1; j < n; j++)\n            if(a[j] < a[j - 1])\n            {\n                unsigned t = a[j]; a[j] = a[j - 1]; a[j - 1] = t;\n            }\n\n    // system("sudo rm -rf /");\n\n    // 如果需要其他函数\n    void do_something(); // 声明\n    do_something(); // 调用\n}\nvoid do_something(){\n    // ......'
	result = ""
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
		result = str(cp.stdout, 'utf-8').replace('\n', '<br/>') + '<br/>'
	response.write(htmldoc % (result, html.escape(code)))
	return HttpResponse(response)

urlpatterns = [
    url('', index),
    # url(r'^admin/', include(admin.site.urls)),
]
