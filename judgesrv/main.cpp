#include <QCoreApplication>
// #include <QTcpSocket>
#include <QUdpSocket>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

QUdpSocket sock;
QTextStream ts;
QDataStream ds;

#define printf(x...) fprintf(stderr, x)

size_t fileSize(const char *fn)
{
	struct stat st;
	if(stat(fn, &st))
		return 0;
	return st.st_size;
}

#include <unistd.h>

void sendFile(const char *sfn, const char *tfn)
{
	char buf[512];
	printf("send %s -> %s\n", sfn, tfn);
	string fn = string("f") + tfn;
	qDebug() << ts.readAll();
	ts << fn.c_str();
	ts.flush();
		// ts.readAll();
	while(1)
	{
		while(sock.waitForReadyRead(1000))
		{
			int len = sock.readDatagram(buf, sizeof(buf));
			buf[len] = 0;
			if(buf[0] == 'f')
			{
				printf("sync ok\n"), fflush(stdout);
				goto next;
			}
			else printf("sync fail %s\n", buf);
		}
	}
next:;
	int sz = fileSize(sfn);
	char *fileAll = new char[sz];
	FILE *fin = fopen(sfn, "rb");
	fread(fileAll, 1, sz, fin);
	fclose(fin);
	printf("filesize %d\n", (int) sz), fflush(stdout);
	
	typedef pair<int, int> pii; // (off, len)
	set<pii> todo;
	for(int i = 0; i < sz; i += 500)
		todo.insert(pii(i, min(sz - i, 500)));
	// printf("total todo %d\n", (int) todo.size()), fflush(stdout);
	
	while(todo.size())
	{
		printf("remaining %d\n", (int) todo.size()), fflush(stdout);
		vector<pii> todo_copy(todo.begin(), todo.end());
		int online = 0;
		for(pii p: todo_copy)
		{
			if(online >= 20) break;
			++online;
			printf("send %d | %d\n", p.first, online), fflush(stdout);
			memcpy(buf + 4, &p.first, 4);
			memcpy(buf + 8, fileAll + p.first, p.second);
			buf[0] = 'g';
			buf[1] = 'g';
			buf[2] = 'g';
			buf[3] = 'g';
			ds.writeRawData(buf, p.second + 8);
			sock.flush();
			// usleep(10000);
			while(sock.waitForReadyRead(online >= 20 ? 1000 : 1))
			{
				int len = sock.readDatagram(buf, sizeof(buf));
				buf[len] = 0;
				if(buf[0] != 'a')
				{
					printf("ack wrong %s\n", buf), fflush(stdout);
					continue;
				}
				int off; memcpy(&off, buf + 4, 4);
				--online;
				printf("ack %d | %d\n", off, online), fflush(stdout);
				auto it = todo.lower_bound(pii(off, 0));
				if(it != todo.end() && it->first == off) todo.erase(it);
				if(!todo.size()) break;
			}
		}
		if(todo.size()) while(sock.waitForReadyRead(1000))
		{
			int len = sock.readDatagram(buf, sizeof(buf));
			buf[len] = 0;
			if(buf[0] != 'a')
			{
				printf("ack wrong %s\n", buf), fflush(stdout);
				continue;
			}
			int off; memcpy(&off, buf + 4, 4);
			printf("ack %d\n", off), fflush(stdout);
			auto it = todo.lower_bound(pii(off, 0));
			if(it != todo.end() && it->first == off) todo.erase(it);
			if(!todo.size()) break;
		}
	}
	delete[] fileAll;
	printf("send ok, waiting sync\n"), fflush(stdout);
	while(1)
	{
		ts << "e";
		ts.flush();
		ts.readAll();
		while(sock.waitForReadyRead(1000))
		{
			int len = sock.readDatagram(buf, sizeof(buf));
			buf[len] = 0;
			if(buf[0] == 'o')
			{
				printf("sync ok\n"), fflush(stdout);
				return;
			}
			else printf("sync fail %s\n", buf);
		}
	}
}
void runCmd(string cmd)
{
	printf("runCmd %s\n", cmd.c_str());
	cmd = "c" + cmd;
	ts << cmd.c_str();
	ts.flush();
	sock.waitForReadyRead();
	ts.readAll();
	printf("runCmd ok\n");
}
QString fileContent(string fn)
{
	printf("fileContent %s\n", fn.c_str());
	fn = "o" + fn;
	ts << fn.c_str();
	ts.flush();
	sock.waitForReadyRead();
	QString ret = ts.readAll();
	printf("fileContent ok:\n%s", ret.toStdString().c_str());
	return ret;
}
QString localFileContent(QString fn)
{
	QFile file(fn);
	file.open(QIODevice::ReadOnly);
	QTextStream ts(&file);
	return ts.readAll();
}

QTextStream qout(stdout);
long long time_ns; int mem_kb;

// fn = ***.c
QString judgeFile(string language)
{
	//size_t sz = fileSize("contestant.c");
	//if(sz <= 0) return "source too small";
	//if(sz >= 10000) return "source too large";
	size_t sz = 0;
	
	string GCC = "ulimit -v 524288 && gcc -DJD_OLD_MEMORY_LIMIT -pipe  -O2  -MD -fno-omit-frame-pointer -static -Wall -Wno-format -Wno-unused -gstabs -m32 -fno-tree-ch -fno-stack-protector -gstabs -c -o ";
	
	string GXX = "ulimit -v 524288 && g++ -DJD_OLD_MEMORY_LIMIT -pipe  -O2  -MD -fno-omit-frame-pointer -static -Wall -Wno-format -Wno-unused -gstabs -m32 -fno-tree-ch -fno-stack-protector -fno-exceptions -fno-unwind-tables -fno-rtti -fno-threadsafe-statics -gstabs -c -o ";
	
	string GXX11 = "ulimit -v 524288 && g++ -DJD_OLD_MEMORY_LIMIT -pipe  -O2 -std=c++11 -MD -fno-omit-frame-pointer -static -Wall -Wno-format -Wno-unused -gstabs -m32 -fno-tree-ch -fno-stack-protector -fno-exceptions -fno-unwind-tables -fno-rtti -fno-threadsafe-statics -gstabs -c -o ";
	
	string GXX_TASKLIB = "ulimit -v 524288 && g++ -DJD_OLD_MEMORY_LIMIT -pipe  -O2  -MD -fno-omit-frame-pointer -static -Wall -Wno-format -Wno-unused -gstabs -m32 -fno-tree-ch -fno-stack-protector -fno-exceptions -fno-unwind-tables -fno-rtti -fno-threadsafe-statics  -I../libtaskduck_include -DJOS_USER -gstabs -c -o ";
	
	string G = GCC;
	string contestant_filename = "contestant.c";
	if (language == "C++") {
		G = GXX;
		contestant_filename = "contestant.cpp";
	} else if (language == "C++11") {
		G = GXX11;
		contestant_filename = "contestant.cpp";
	}
	
	string tasklib_option = "";
	if (language == "C") {
		tasklib_option = "-DTASKLIB_USE_C";
	}
	
	qout << "compiling...\n";
	qout.flush();
	
	if(system((G + "contestant.o " + contestant_filename + " > gcc_contestant.log 2>&1").c_str()))
		return "contestant compile error\n" + localFileContent("gcc_contestant.log").left(40);
	if(system((GXX_TASKLIB + "tasklib.o tasklib.cpp " + tasklib_option + " > gcc_tasklib.log 2>&1").c_str()))
		return "tasklib compile error\n" + localFileContent("gcc_tasklib.log").left(40);
	if(system("ld -o judging -T ../judgeduck.ld -m elf_i386 -nostdlib contestant.o libopenlibm.a tasklib.o ../libtaskduck/libtaskduck.a ../libjudgeduck/libjudgeduck.a ../libstdduck/libstdduck.a /usr/lib/gcc/i686-linux-gnu/5/libgcc.a > ld.log 2>&1"))
		return "link error\n" + localFileContent("ld.log").left(1024);
	system("size judging > size.out");
	FILE *fin = fopen("size.out", "r");
	unsigned data = 1 << 30, bss = 1 << 30;
	fscanf(fin, "%*s%*s%*s%*s%*s%*s%*s%d%d", &data, &bss);
	if(data + bss > (1280u << 20))
		return "too large global variables";
	fclose(fin);
	
	qout << "compile success!\n";
	qout.flush();
	
	sz = fileSize("judging");
	if(sz <= 0) return "binary too small";
	if(sz >= 262144) return "binary too large";
	
	qDebug() << sock.bind(8008);
	sock.connectToHost("172.29.8.241", 8000);
	qDebug() << sock.waitForConnected();
	ts.setDevice(&sock);
	ds.setDevice(&sock);
	
	sendFile("input.txt", "input.txt");
	sendFile("answer.txt", "answer.txt");
	
	sendFile("judging", "judging");
	runCmd("arbiter judging " + to_string(time_ns) + " " + to_string(mem_kb) + " > arbiter.out");
	return fileContent("arbiter.out");
}

/*
precondition:
	contestant.c
	tasklib.c
	(any associated headers)
	input.txt
*/
int main(int argc, char **argv)
{
	QCoreApplication a(argc, argv);
	
	if(argc != 1 + 3)
	{
		qout << "usage: judgesrv [time_ns] [mem_kb] [lang]";
		qout.flush();
		return 1;
	}
	time_ns = atoll(argv[1]);
	mem_kb = atoi(argv[2]);
	string language = string(argv[3]);
	
	qout << judgeFile(language);
	qout.flush();
	
	return 0;
}

