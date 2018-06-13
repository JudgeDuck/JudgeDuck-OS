import html
import os
import subprocess
import threading
import time
import datetime


def list_dir(name):
	try:
		return os.listdir(name)
	except:
		return []


def parse_int(x, default = -1):
	try:
		return int(x, 10)
	except:
		return default

def parse_float(x, default = -1):
	try:
		return float(x)
	except:
		return default



def check_if_file_present(name):
	try:
		os.stat(name)
		return True
	except:
		return False

def read_file(name, fallback = ""):
	try:
		f = open(name, "r")
		res = f.read()
		f.close()
		return res
	except:
		return fallback

def write_file(name, content):
	try:
		f = open(name, "w")
		f.write(content)
		f.close()
		return True
	except:
		pass

def get_file_mtime(name, fallback = ""):
	try:
		return datetime.datetime.fromtimestamp(os.stat(name).st_mtime + 3600 * 8).strftime('%Y-%m-%d %H:%M:%S')
	except:
		return fallback

def get_current_time():
	return datetime.datetime.fromtimestamp(time.time() + 3600 * 8).strftime('%Y-%m-%d %H:%M:%S')
