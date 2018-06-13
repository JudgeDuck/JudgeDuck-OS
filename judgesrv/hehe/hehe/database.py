#encoding=utf-8

# This file is for data store

path_prefix = "data/"
path_temp = path_prefix + "temp/"
path_problems = path_prefix + "problems/"
path_code = path_prefix + "code/"
path_metadata = path_prefix + "metadata/"
path_result = path_prefix + "result/"
path_pending = path_prefix + "pending/"
path_pending_rejudge = path_prefix + "pending_rejudge/"


from threading import *
lock = Lock()

from . import utils
import os

problems = None
submissions = None
last_sub_id = 0


def get_problem_list():
	lock.acquire()
	ret = []
	for pid in problems:
		ret.append(pid)
	lock.release()
	return sorted(ret)

def get_problem_info(pid):
	lock.acquire()
	ret = problems.get(pid, None)
	lock.release()
	return ret

#

# return sid
def submit(pid, name, code):
	lock.acquire()
	pinfo = problems.get(pid, None)
	if pinfo == None:
		lock.release()
		return None
	name = name.strip().split("\n")[0].split("\r")[0]
	global last_sub_id
	sid = last_sub_id
	print("sid %s, pid %s, name %s" % (sid, pid, name))
	try:
		code_to_write = code
		utils.write_file(path_temp + "code.txt", code_to_write)
		utils.write_file(path_temp + "code_copy.txt", code_to_write)
		meta = name + "\n"
		meta += utils.get_current_time() + "\n"
		meta += pid + "\n"
		utils.write_file(path_temp + "meta.txt", meta)
		os.rename(path_temp + "meta.txt", path_metadata + "%d.txt" % sid)
		os.rename(path_temp + "code.txt", path_code + "%d.txt" % sid)
		os.rename(path_temp + "code_copy.txt", path_pending + "%d.txt" % sid)
		update_submission(sid)
		last_sub_id += 1
		lock.release()
		return sid
	except:
		lock.release()
		return None

def get_submission(sid):
	lock.acquire()
	ret = submissions.get(sid, None)
	lock.release()
	return ret


def get_board(pid):
	lock.acquire()
	pinfo = problems.get(pid, None)
	if pinfo == None:
		lock.release()
		return []
	board_map = {}
	for i in submissions:
		sub = submissions[i]
		if sub["pid"] != pid:
			continue
		if sub["status"] != "Accepted":
			continue
		player_name = sub["name"]
		time_ms = sub["time"]
		if not (player_name in board_map) or time_ms < board_map[player_name][0]:
			board_map[player_name] = [time_ms, sub["sid"], player_name, sub["submit_time"], sub["judge_time"]]
	board = []
	for i in board_map:
		board.append(board_map[i])
	board = sorted(board)[:100]
	lock.release()
	return board





def init():
	lock.acquire()
	init_problems()
	init_submissions()
	lock.release()

def reload():
	init()

def init_problems():
	global problems
	problems = {}
	li = utils.list_dir(path_problems)
	for pid in li:
		add_problem(pid)

def add_problem(pid):
	prob = {
		"pid": pid,
		"name": "不知道",
		"description": "不存在的",
		"time_limit": 1000,
		"memory_limit": 4,
		"files": [],
		"statement": "",
	}
	path = path_problems + pid + "/"
	conf_content = utils.read_file(path + "config.txt").split("\n")
	keys = [
		"name",
		"description",
		"time_limit",
		"memory_limit",
		"files",
	]
	for s in conf_content:
		match_k = None
		for k in keys:
			if s[:len(k)] == k:
				match_k = k
				break
		if match_k != None:
			val = s[len(k) + 1:]
			if match_k == "time_limit" or match_k == "memory_limit":
				val = int(val)
				if val <= 0:
					val = 1
			if match_k == "files":
				val = val.split(" ")
			prob[match_k] = val
	prob["statement"] = utils.read_file(path + "statement.md", "咕咕咕")
	prob["sample_code"] = utils.read_file(path + "sample.c", "咕咕咕")
	problems[pid] = prob

#

def init_submissions():
	global submissions
	submissions = {}
	n_subs = len(utils.list_dir(path_code))
	global last_sub_id
	last_sub_id = n_subs
	for i in range(n_subs):
		update_submission(i)

def update_submission(sid):
	name = path_result + "%d.txt" % sid
	res_str = utils.read_file(name)
	res_arr = res_str.split("\n")
	ok1 = False
	ok2 = False
	time_ms = None
	time_ms_prefix = "time_ms = "
	time_ms_prefix_len = len(time_ms_prefix)
	#code_content = read_file("code/%d.txt" % id)
	#code_first_row = code_content.split("\n")[0]
	
	metadata_content = utils.read_file(path_metadata + "%d.txt" % sid).split("\n")
	player_name = "咕咕咕"
	submit_time = "不存在的"
	pid = ""
	try:
		player_name = metadata_content[0]
		submit_time = metadata_content[1]
		pid = metadata_content[2]
	except:
		return
	
	#if code_first_row[:len(name_magic)] == name_magic:
	#	player_name = code_first_row[len(name_magic):]
	
	sub = {
		"sid": sid,
		"pid": pid,
		"status": "Pending",
		"time": 1e100,
		"name": player_name,
		"submit_time": submit_time,
		"judge_time": utils.get_file_mtime(path_result + "%d.txt" % sid, "")
	}
	
	if res_str != "":
		sub["status"] = "Judge Failed"
	
	for s in res_arr:
		if s == "Correct Answer!":
			ok1 = True
		if s[:len("verdict = ")] == "verdict = ":
			sub["status"] = s[len("verdict = "):]
		if s == "verdict = Run Finished":
			ok2 = True
		if s[:time_ms_prefix_len] == time_ms_prefix:
			time_ms = utils.parse_float(s[time_ms_prefix_len:])
			sub["time"] = time_ms
	if ok1 and ok2 and time_ms != None:
		if time_ms > 0 and time_ms < 100 * 1000:
			sub["status"] = "Accepted"
	if ok2 and (not ok1):
		sub["status"] = "Wrong Answer"
	global submissions
	submissions[sid] = sub











init()
