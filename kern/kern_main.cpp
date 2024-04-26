#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <algorithm>

#include <inc/syslog.hpp>
#include <inc/multiboot2_loader.hpp>
#include <inc/framebuffer.hpp>
#include <inc/acpi.hpp>
#include <inc/lapic.hpp>
#include <inc/memory.hpp>
#include <inc/pic.hpp>
#include <inc/trap.hpp>
#include <inc/logger.hpp>
#include <inc/timer.hpp>
#include <inc/pci.hpp>
#include <inc/dma.hpp>
#include <inc/network_driver.hpp>
#include <inc/elf.hpp>
#include <inc/x86_64.hpp>
#include <inc/duck_server.hpp>
#include <inc/scheduler.hpp>
#include <inc/judger.hpp>
#include <inc/abi.hpp>

static void print_hello() {
	printf("Hello world!\n");
	
	double e = 0, tmp = 1;
	for (int i = 1; i <= 20; i++) {
		e += tmp;
		tmp /= i;
	}
	printf("e = %.15lf\n", e);
	printf("pi = 2 * atan2(1, 0) = %.15lf\n", 2 * atan2(1, 0));
}

static void run_test(const char *elf_start, const char *elf_end) {
	int len = elf_end - elf_start;
	LDEBUG("start = %p, len = %d", elf_start, len);
	
	Logger::set_log_level(Logger::LL_INFO);
	ELF::App app;
	auto config = (ELF::AppConfig) {
		.memory_hard_limit = 100 << 20,
		.stdin_ptr = "100 987\n",
		.stdin_size = 8,
		.stdout_max_size = 10240,
		.stderr_max_size = 10240,
		.IB_ptr = NULL,
		.IB_size = 0,
		.OB_ptr = NULL,
		.OB_size = 0,
		.OB_need_clear = false,
	};
	assert(ELF::load(elf_start, len, config, app));
	auto res = ELF::run(app, 0);
	LINFO("time %.6lf ms, memory %d KiB (%.1lf MiB) (A: %d KB)",
		res.time_ns / 1e6, res.memory_kb, res.memory_kb / 1024.0,
		res.memory_kb_accessed);
	LINFO("tsc %lu, trap %u, retcode %d",
		res.time_tsc, res.trap_num, res.return_code);
	LINFO("stdout size %lu, stderr size %lu",
		res.stdout_size, res.stderr_size);
	LINFO(">>> stdout content (first 256 bytes) <<<");
	fwrite(res.stdout_ptr, 1, std::min(256ul, res.stdout_size), stdout);
	putchar('\n');
	LINFO(">>> stderr content (first 256 bytes) <<<");
	fwrite(res.stderr_ptr, 1, std::min(256ul, res.stderr_size), stderr);
	putchar('\n');
	LINFO("========================================");
}

static void run_tests() {
	LINFO("Running tests");
	
	extern const char _binary_hello_elf_start[];
	extern const char _binary_hello_elf_end[];
	run_test(_binary_hello_elf_start, _binary_hello_elf_end);
	
	extern const char _binary_hello32_elf_start[];
	extern const char _binary_hello32_elf_end[];
	run_test(_binary_hello32_elf_start, _binary_hello32_elf_end);
}

int main() {
	SysLog::init();
	
	print_hello();
	
	Logger::init();
	// TODO: disable debug logging for production
	
	Multiboot2_Loader::load();
	
	PIC::init();
	Timer::init();
	ACPI::init();
	LAPIC::init();
	
	Memory::init();
	
	Trap::init();
	
	PCI::init();
	FrameBuffer::init();
	DMA::init();
	NetworkDriver::init();
	DuckServer::init();
	Scheduler::init();
	Judger::init();
	
	run_tests();
	
	LINFO("Welcome to JudgeDuck-OS-64 !!!");
	LINFO("ABI Version %s", ABI::version_str);
	Logger::set_log_level(Logger::LL_WARN);
	
	DuckServer::run();
	// Should not return
	
	x86_64::reboot();
}
