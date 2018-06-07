#include <QCoreApplication>
// #include <QTcpSocket>
#include <QUdpSocket>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

QUdpSocket sock;
QTextStream ts;
QDataStream ds;

#define printf(x...) fprintf(stderr, x)

void sendFile(const char *sfn, const char *tfn)
{
	printf("send %s -> %s\n", sfn, tfn);
	string fn = string("f") + tfn;
	ts << fn.c_str();
	ts.flush();
	sock.waitForReadyRead();
	qDebug() << ts.readAll();
	char buf[512];
	FILE *fin = fopen(sfn, "rb");
	int r;
	while((r = fread(buf + 1, 1, sizeof(buf) - 2, fin)) > 0)
	{
		buf[0] = 'z';
		buf[r + 1] = 0;
		ds.writeRawData(buf, r + 1);
		sock.flush();
		sock.waitForReadyRead();
		qDebug() << ts.readAll();
	}
	ts << "e";
	ts.flush();
	sock.waitForReadyRead();
	qDebug() << ts.readAll();
	fclose(fin);
	printf("send ok\n");
}
void runCmd(string cmd)
{
	printf("runCmd %s\n", cmd.c_str());
	cmd = "c" + cmd;
	ts << cmd.c_str();
	ts.flush();
	sock.waitForReadyRead();
	qDebug() << ts.readAll();
	printf("runCmd ok\n");
}
QString fileContent(string fn)
{
	printf("fileContent %s\n", fn.c_str());
	fn = "o" + fn;
	ts << fn.c_str();
	ts.flush();
	sock.waitForReadyRead();
	printf("fileContent ok\n");
	return ts.readAll();
}

size_t fileSize(const char *fn)
{
	struct stat st;
	if(stat(fn, &st))
		return 0;
	return st.st_size;
}

// fn = ***.c
QString judgeFile(const char *fn)
{
	size_t sz = fileSize(fn);
	if(sz <= 0) return "source too small";
	if(sz >= 10000) return "source too large";
	string cmd = string("rm -f /home/yjp/OS2018spring-projects-g04/obj/user/judging && cp ") + fn + " /home/yjp/OS2018spring-projects-g04/user/judging.c && make -C /home/yjp/OS2018spring-projects-g04/ obj/user/judging > /dev/null";
	int r = system(cmd.c_str());
	if(r) return "compile error";
	
	sz = fileSize("/home/yjp/OS2018spring-projects-g04/obj/user/judging");
	if(sz <= 0) return "binary too small";
	if(sz >= 131072) return "binary too large";
	
	qDebug() << sock.bind("10.0.2.2", 8008);
	sock.connectToHost("10.0.2.15", 80);
	sock.waitForConnected();
	ts.setDevice(&sock);
	ds.setDevice(&sock);
	
	sendFile("/home/yjp/OS2018spring-projects-g04/obj/user/judging", "judging");
	runCmd("arbiter judging 5000 65536 1 10000 > arbiter.out");
	return fileContent("arbiter.out");
}

int main(int argc, char **argv)
{
	QCoreApplication a(argc, argv);

	QTextStream qout(stdout);
	
	qout << judgeFile(argv[1]);
	
	return 0;
}

