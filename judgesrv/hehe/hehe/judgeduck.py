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
from django.http import HttpResponse, HttpResponseRedirect
from django.template import loader, Context
from django.views.decorators.csrf import csrf_exempt
from django.shortcuts import render_to_response
from threading import *
from django.conf import settings
from django.conf.urls.static import static
from django.shortcuts import redirect
from django.contrib.staticfiles import views as static_views
import html
import os
import subprocess
import threading
import time
import datetime
import markdown2

from . import jd_htmldocs as htmldocs
from . import database as db
from . import utils


@csrf_exempt
def index_view(req):
	res = HttpResponse(content_type="text/html")
	res.write(htmldocs.index_htmldoc % render_problems())
	return res

def render_problems():
	ret = ""
	plist = db.get_problem_list()
	for pid in plist:
		pinfo = db.get_problem_info(pid)
		if pinfo == None: continue
		name = html.escape(pinfo["name"])
		description = "%s %d ms %d KB" % (html.escape(pinfo["description"]), pinfo["time_limit"], pinfo["memory_limit"])
		ret += htmldocs.index_problem_list_problem % (pid, name, description, pid)
	if ret == "":
		ret = "咕咕咕，好像没有题目呀"
	return ret

@csrf_exempt
def problem_view(req):
	res = HttpResponse(content_type="text/html")
	pid = req.GET.get("pid", "")
	pinfo = db.get_problem_info(pid)
	if pinfo == None:
		res.write(htmldocs.problem_page_htmldoc % "咕咕咕，找不到题目")
	else:
		res.write(htmldocs.problem_page_htmldoc % render_problem(pinfo))
	return res

def render_problem(pinfo):
	ret = "<h2> %s </h2>" % html.escape(pinfo["name"])
	ret += "时间限制： %s ms <br />" % pinfo["time_limit"]
	ret += "空间限制： %s KB <br />" % pinfo["memory_limit"]
	ret += "<br />"
	ret += markdown2.markdown(pinfo["statement"])
	ret += "<br />"
	ret += htmldocs.problem_page_submit_htmldoc % (pinfo["pid"], html.escape(pinfo["sample_code"]))
	return ret

def reload_view(req):
	db.reload()
	return redirect(".")
#

@csrf_exempt
def submit_view(req):
	res = HttpResponse(content_type="text/html")
	ret = db.submit(req.GET.get("pid", None), req.POST.get("name", "咕咕咕"), req.POST.get("code", "咕咕咕"))
	if ret == None:
		res.write("<meta http-equiv=Content-Type content='text/html;charset=utf-8'><script>alert('提交失败'); window.location='.'</script>")
		return res
	else:
		return HttpResponseRedirect("detail?sid=%s" % ret)

def detail_view(req):
	res = HttpResponse(content_type="text/html")
	sid = utils.parse_int(req.GET.get('sid', -1))
	sub = db.get_submission(sid)
	if sub == None:
		res.write("Invalid submission id")
		return res
	res_content = utils.read_file(db.path_result + "%d.txt" % sid)
	code_content = utils.read_file(db.path_code + "%d.txt" % sid)
	pid = sub["pid"]
	pinfo = db.get_problem_info(pid)
	problem_title = pinfo["name"] if pinfo != None else "不存在的题目"
	player_name = sub["name"]
	res.write(htmldocs.detail_htmldoc %
		(sid, pid, html.escape(problem_title), html.escape(player_name), html.escape(res_content), html.escape(code_content)))
	return res

def board_view(req):
	res = HttpResponse(content_type="text/html")
	pid = req.GET.get("pid", "")
	pinfo = db.get_problem_info(pid)
	if pinfo == None:
		res.write("Invalid problem id")
		return res
	res.write(htmldocs.board_htmldoc % (pinfo["name"], render_board(pid)))
	return res

def render_board(pid):
	res = ""
	board = db.get_board(pid)
	if len(board) == 0:
		res = "还没人AC呢"
	else:
		res = "<table border='1'>"
		res += "<tr><th>Rank</th><th>Time (ms)</th><th>Run ID</th><th>昵称</th><th>提交时间</th><th>评测时间</th></tr>"
		row_id = 0
		for row in board:
			row_id += 1
			res += "<tr>"
			res += "<td>%d</td>" % row_id
			res += "<td>%s</td>" % row[0]
			res += "<td><a target='_blank' href='detail?sid=%s'>%s</a></td>" % (row[1], row[1])
			res += "<td>%s</td>" % html.escape(row[2])
			res += "<td>%s</td>" % html.escape(row[3])
			res += "<td>%s</td>" % html.escape(row[4])
			res += "</tr>"
		res += "</table>"
	return res












def do_judge(sid):
	sub = db.get_submission(sid)
	if sub == None:
		print("咕咕咕咕咕咕，好像没有 %s 这条提交记录啊" % sid)
		return
	pid = sub["pid"]
	pinfo = db.get_problem_info(pid)
	if pinfo == None:
		print("咕咕咕咕咕，在测提交记录 %s 的函数，发现好像没有 %s 这道题啊" % (sid, pid))
		return
	code = utils.read_file(db.path_code + "%s.txt" % sid)
	fcode = open("contestant.c", "w")
	fcode.write(code)
	fcode.close()
	finput = open("input.txt", "w")
	input_content = utils.read_file(db.path_problems + "%s/input.txt" % pid)
	finput.write(input_content)
	finput.close()
	flib = open("tasklib.c", "w")
	flib.write(utils.read_file(db.path_problems + "%s/tasklib.c" % pid))
	flib.close()
	for filename in pinfo["files"]:
		f = open(filename, "w")
		f.write(utils.read_file(db.path_problems + ("%s/" % pid) + filename))
		f.close()
	TL = "%s" % pinfo["time_limit"]
	ML = "%s" % pinfo["memory_limit"]
	print("run tl = %s ml = %s" % (TL, ML))
	try:
		cp = subprocess.run(["../judgesrv", TL, ML], stdout=subprocess.PIPE, timeout=20)
		result = str(cp.stdout, "utf-8")
		if result == "":
			result = "咕咕咕，评测机好像炸了"
	except:
		result = "咕咕咕，评测失败"
	print("result = %s" % result)
	fres = open(db.path_temp + "judge_res.txt", "w")
	fres.write(result)
	fres.close()
	os.rename(db.path_temp + "judge_res.txt", db.path_result + "%d.txt" % sid)
	db.update_submission(sid)

def judge_server_thread_func():
	while True:
		time.sleep(1)
		files = os.listdir(db.path_pending)
		min_id = -1
		for filename in files:
			if filename[-4:] == ".txt":
				id = utils.parse_int(filename[:-4], -1)
				if id != -1 and (min_id == -1 or id < min_id):
					min_id = id
		if min_id == -1:
			continue
		print("[new_oj] judging id = %d" % min_id)
		try:
			do_judge(min_id)
		except:
			print("[new_oj] judge failed !!!!!!!")
		print("[new_oj] judge done, id = %d" % min_id)
		try:
			os.remove(db.path_pending + "%d.txt" % min_id)
		except:
			pass




class myThread(threading.Thread):
	def __init__(self, name, func):
		threading.Thread.__init__(self)
		self.name = name
		self.func = func
	def run(self):
		self.func()

judge_server_thread = myThread("judgesrv", judge_server_thread_func)
# judge_server_thread.start()






urlpatterns = [
	url("^$", index_view),
	url("^problem$", problem_view),
	url("^reload$", reload_view),
	url("^submit$", submit_view),
	url("^detail$", detail_view),
	url("^board$", board_view),
	url(r'^libs/(?P<path>.*)$', 'django.views.static.serve', {'document_root': './static/libs'}),
	url(r'^css/(?P<path>.*)$', 'django.views.static.serve', {'document_root': './static/css'}),
	#url("^detail$", detail_view),
	#url(""),
    #url('^$', index),
	#url('^detail$', detail),
	#url('^board$', board_view),
    # url(r'^admin/', include(admin.site.urls)),
]
# + static('/static/', document_root='./static')
