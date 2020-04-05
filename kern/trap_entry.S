// All 256 trap handlers

.text
.align 8
.globl trap_return
trap_return:
	movq %rdi, %rsp
	
	fxrstor (%rsp)
	addq $512, %rsp
	
	// tf_fsbase
	popq %rax
	wrfsbase %rax
	
	popq %rcx
	popq %rbx
	popq %rbp
	popq %rsi
	popq %rdi
	popq %r8
	popq %r9
	popq %r10
	popq %r11
	popq %r12
	popq %r13
	popq %r14
	popq %r15
	
	// skip tsc
	popq %rax
	
	popq %rax
	popq %rdx
	
	addq $16, %rsp
	iretq

.align 8
__trap_entry:
	pushq %rdx
	pushq %rax
	
	// tsc
	rdtsc
	shl $32, %rdx
	orq %rdx, %rax
	pushq %rax
	mfence
	
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %r11
	pushq %r10
	pushq %r9
	pushq %r8
	pushq %rdi
	pushq %rsi
	pushq %rbp
	pushq %rbx
	pushq %rcx
	
	// tf_fsbase
	rdfsbase %rax
	pushq %rax
	
	subq $512, %rsp
	fxsave (%rsp)
	
	movq %rsp, %rdi
	call trap_handler

.text
.align 8
.globl __syscall_entry
__syscall_entry:
	// We clobber all registers this time
	movq %rax, %r15  // save rax [syscall number]
	rdtsc
	shl $32, %rdx
	orq %rdx, %rax
	mfence
	
	movq __syscall_stack, %rsp
	xorq %r11, %r11
	pushq %r11  // fake tf_ss
	pushq %r11  // fake tf_rsp
	pushq %r11  // fake tf_rflags
	pushq $3    // fake userspace tf_cs
	pushq %r11  // fake tf_rip
	pushq %r11  // fake tf_errorcode
	pushq $255  // fake tf_num
	pushq %r11  // fake rdx
	pushq %r15  // saved rax [syscall number]
	pushq %rax  // tsc
	movq %rdi, -72(%rsp)  // rdi [first argument]
	subq $112, %rsp  // skip other regs and fsbase
	
	subq $512, %rsp
	fxsave (%rsp)
	
	movq %rsp, %rdi
	call trap_handler

.text
.align 8
.globl syscall_return_record_tsc
syscall_return_record_tsc:
	movq %rdi, %rsp
	
	fxrstor __init_fx_state
	addq $512, %rsp
	
	// tf_fsbase
	popq %rax
	wrfsbase %rax
	
	popq %rcx
	popq %rbx
	popq %rbp
	popq %rsi
	popq %rdi
	popq %r8
	popq %r9
	popq %r10
	popq %r11
	popq %r12
	popq %r13
	popq %r14
	popq %r15
	
	// clear all caches
	wbinvd
	movq %cr3, %rax
	movq %rax, %cr3
	mfence
	
	// rewrite tsc
	rdtsc
	shl $32, %rdx
	orq %rdx, %rax
	movq %rax, (%rsp)
	popq %rax
	
	popq %rax
	popq %rdx
	
	movq 16(%rsp), %rcx  // tf_rip
	movq 32(%rsp), %r11  // tf_rflags
	movq 40(%rsp), %rsp  // tf_rsp
	
	sysretq

#define TRAP_NOEC(name, id) \
	.align 8; \
	name: \
		pushq $0; \
		pushq $(id); \
		jmp __trap_entry

#define TRAP_EC(name, id) \
	.align 8; \
	name: \
		pushq $(id); \
		jmp __trap_entry

TRAP_NOEC(__trap_0, 0)
TRAP_NOEC(__trap_1, 1)
TRAP_NOEC(__trap_2, 2)
TRAP_NOEC(__trap_3, 3)
TRAP_NOEC(__trap_4, 4)
TRAP_NOEC(__trap_5, 5)
TRAP_NOEC(__trap_6, 6)
TRAP_NOEC(__trap_7, 7)
TRAP_EC(__trap_8, 8)
TRAP_NOEC(__trap_9, 9)
TRAP_EC(__trap_10, 10)
TRAP_EC(__trap_11, 11)
TRAP_EC(__trap_12, 12)
TRAP_EC(__trap_13, 13)
TRAP_EC(__trap_14, 14)
TRAP_EC(__trap_15, 15)
TRAP_NOEC(__trap_16, 16)
TRAP_NOEC(__trap_17, 17)
TRAP_NOEC(__trap_18, 18)
TRAP_NOEC(__trap_19, 19)
TRAP_NOEC(__trap_20, 20)
TRAP_NOEC(__trap_21, 21)
TRAP_NOEC(__trap_22, 22)
TRAP_NOEC(__trap_23, 23)
TRAP_NOEC(__trap_24, 24)
TRAP_NOEC(__trap_25, 25)
TRAP_NOEC(__trap_26, 26)
TRAP_NOEC(__trap_27, 27)
TRAP_NOEC(__trap_28, 28)
TRAP_NOEC(__trap_29, 29)
TRAP_NOEC(__trap_30, 30)
TRAP_NOEC(__trap_31, 31)
TRAP_NOEC(__trap_32, 32)
TRAP_NOEC(__trap_33, 33)
TRAP_NOEC(__trap_34, 34)
TRAP_NOEC(__trap_35, 35)
TRAP_NOEC(__trap_36, 36)
TRAP_NOEC(__trap_37, 37)
TRAP_NOEC(__trap_38, 38)
TRAP_NOEC(__trap_39, 39)
TRAP_NOEC(__trap_40, 40)
TRAP_NOEC(__trap_41, 41)
TRAP_NOEC(__trap_42, 42)
TRAP_NOEC(__trap_43, 43)
TRAP_NOEC(__trap_44, 44)
TRAP_NOEC(__trap_45, 45)
TRAP_NOEC(__trap_46, 46)
TRAP_NOEC(__trap_47, 47)
TRAP_NOEC(__trap_48, 48)
TRAP_NOEC(__trap_49, 49)
TRAP_NOEC(__trap_50, 50)
TRAP_NOEC(__trap_51, 51)
TRAP_NOEC(__trap_52, 52)
TRAP_NOEC(__trap_53, 53)
TRAP_NOEC(__trap_54, 54)
TRAP_NOEC(__trap_55, 55)
TRAP_NOEC(__trap_56, 56)
TRAP_NOEC(__trap_57, 57)
TRAP_NOEC(__trap_58, 58)
TRAP_NOEC(__trap_59, 59)
TRAP_NOEC(__trap_60, 60)
TRAP_NOEC(__trap_61, 61)
TRAP_NOEC(__trap_62, 62)
TRAP_NOEC(__trap_63, 63)
TRAP_NOEC(__trap_64, 64)
TRAP_NOEC(__trap_65, 65)
TRAP_NOEC(__trap_66, 66)
TRAP_NOEC(__trap_67, 67)
TRAP_NOEC(__trap_68, 68)
TRAP_NOEC(__trap_69, 69)
TRAP_NOEC(__trap_70, 70)
TRAP_NOEC(__trap_71, 71)
TRAP_NOEC(__trap_72, 72)
TRAP_NOEC(__trap_73, 73)
TRAP_NOEC(__trap_74, 74)
TRAP_NOEC(__trap_75, 75)
TRAP_NOEC(__trap_76, 76)
TRAP_NOEC(__trap_77, 77)
TRAP_NOEC(__trap_78, 78)
TRAP_NOEC(__trap_79, 79)
TRAP_NOEC(__trap_80, 80)
TRAP_NOEC(__trap_81, 81)
TRAP_NOEC(__trap_82, 82)
TRAP_NOEC(__trap_83, 83)
TRAP_NOEC(__trap_84, 84)
TRAP_NOEC(__trap_85, 85)
TRAP_NOEC(__trap_86, 86)
TRAP_NOEC(__trap_87, 87)
TRAP_NOEC(__trap_88, 88)
TRAP_NOEC(__trap_89, 89)
TRAP_NOEC(__trap_90, 90)
TRAP_NOEC(__trap_91, 91)
TRAP_NOEC(__trap_92, 92)
TRAP_NOEC(__trap_93, 93)
TRAP_NOEC(__trap_94, 94)
TRAP_NOEC(__trap_95, 95)
TRAP_NOEC(__trap_96, 96)
TRAP_NOEC(__trap_97, 97)
TRAP_NOEC(__trap_98, 98)
TRAP_NOEC(__trap_99, 99)
TRAP_NOEC(__trap_100, 100)
TRAP_NOEC(__trap_101, 101)
TRAP_NOEC(__trap_102, 102)
TRAP_NOEC(__trap_103, 103)
TRAP_NOEC(__trap_104, 104)
TRAP_NOEC(__trap_105, 105)
TRAP_NOEC(__trap_106, 106)
TRAP_NOEC(__trap_107, 107)
TRAP_NOEC(__trap_108, 108)
TRAP_NOEC(__trap_109, 109)
TRAP_NOEC(__trap_110, 110)
TRAP_NOEC(__trap_111, 111)
TRAP_NOEC(__trap_112, 112)
TRAP_NOEC(__trap_113, 113)
TRAP_NOEC(__trap_114, 114)
TRAP_NOEC(__trap_115, 115)
TRAP_NOEC(__trap_116, 116)
TRAP_NOEC(__trap_117, 117)
TRAP_NOEC(__trap_118, 118)
TRAP_NOEC(__trap_119, 119)
TRAP_NOEC(__trap_120, 120)
TRAP_NOEC(__trap_121, 121)
TRAP_NOEC(__trap_122, 122)
TRAP_NOEC(__trap_123, 123)
TRAP_NOEC(__trap_124, 124)
TRAP_NOEC(__trap_125, 125)
TRAP_NOEC(__trap_126, 126)
TRAP_NOEC(__trap_127, 127)
TRAP_NOEC(__trap_128, 128)
TRAP_NOEC(__trap_129, 129)
TRAP_NOEC(__trap_130, 130)
TRAP_NOEC(__trap_131, 131)
TRAP_NOEC(__trap_132, 132)
TRAP_NOEC(__trap_133, 133)
TRAP_NOEC(__trap_134, 134)
TRAP_NOEC(__trap_135, 135)
TRAP_NOEC(__trap_136, 136)
TRAP_NOEC(__trap_137, 137)
TRAP_NOEC(__trap_138, 138)
TRAP_NOEC(__trap_139, 139)
TRAP_NOEC(__trap_140, 140)
TRAP_NOEC(__trap_141, 141)
TRAP_NOEC(__trap_142, 142)
TRAP_NOEC(__trap_143, 143)
TRAP_NOEC(__trap_144, 144)
TRAP_NOEC(__trap_145, 145)
TRAP_NOEC(__trap_146, 146)
TRAP_NOEC(__trap_147, 147)
TRAP_NOEC(__trap_148, 148)
TRAP_NOEC(__trap_149, 149)
TRAP_NOEC(__trap_150, 150)
TRAP_NOEC(__trap_151, 151)
TRAP_NOEC(__trap_152, 152)
TRAP_NOEC(__trap_153, 153)
TRAP_NOEC(__trap_154, 154)
TRAP_NOEC(__trap_155, 155)
TRAP_NOEC(__trap_156, 156)
TRAP_NOEC(__trap_157, 157)
TRAP_NOEC(__trap_158, 158)
TRAP_NOEC(__trap_159, 159)
TRAP_NOEC(__trap_160, 160)
TRAP_NOEC(__trap_161, 161)
TRAP_NOEC(__trap_162, 162)
TRAP_NOEC(__trap_163, 163)
TRAP_NOEC(__trap_164, 164)
TRAP_NOEC(__trap_165, 165)
TRAP_NOEC(__trap_166, 166)
TRAP_NOEC(__trap_167, 167)
TRAP_NOEC(__trap_168, 168)
TRAP_NOEC(__trap_169, 169)
TRAP_NOEC(__trap_170, 170)
TRAP_NOEC(__trap_171, 171)
TRAP_NOEC(__trap_172, 172)
TRAP_NOEC(__trap_173, 173)
TRAP_NOEC(__trap_174, 174)
TRAP_NOEC(__trap_175, 175)
TRAP_NOEC(__trap_176, 176)
TRAP_NOEC(__trap_177, 177)
TRAP_NOEC(__trap_178, 178)
TRAP_NOEC(__trap_179, 179)
TRAP_NOEC(__trap_180, 180)
TRAP_NOEC(__trap_181, 181)
TRAP_NOEC(__trap_182, 182)
TRAP_NOEC(__trap_183, 183)
TRAP_NOEC(__trap_184, 184)
TRAP_NOEC(__trap_185, 185)
TRAP_NOEC(__trap_186, 186)
TRAP_NOEC(__trap_187, 187)
TRAP_NOEC(__trap_188, 188)
TRAP_NOEC(__trap_189, 189)
TRAP_NOEC(__trap_190, 190)
TRAP_NOEC(__trap_191, 191)
TRAP_NOEC(__trap_192, 192)
TRAP_NOEC(__trap_193, 193)
TRAP_NOEC(__trap_194, 194)
TRAP_NOEC(__trap_195, 195)
TRAP_NOEC(__trap_196, 196)
TRAP_NOEC(__trap_197, 197)
TRAP_NOEC(__trap_198, 198)
TRAP_NOEC(__trap_199, 199)
TRAP_NOEC(__trap_200, 200)
TRAP_NOEC(__trap_201, 201)
TRAP_NOEC(__trap_202, 202)
TRAP_NOEC(__trap_203, 203)
TRAP_NOEC(__trap_204, 204)
TRAP_NOEC(__trap_205, 205)
TRAP_NOEC(__trap_206, 206)
TRAP_NOEC(__trap_207, 207)
TRAP_NOEC(__trap_208, 208)
TRAP_NOEC(__trap_209, 209)
TRAP_NOEC(__trap_210, 210)
TRAP_NOEC(__trap_211, 211)
TRAP_NOEC(__trap_212, 212)
TRAP_NOEC(__trap_213, 213)
TRAP_NOEC(__trap_214, 214)
TRAP_NOEC(__trap_215, 215)
TRAP_NOEC(__trap_216, 216)
TRAP_NOEC(__trap_217, 217)
TRAP_NOEC(__trap_218, 218)
TRAP_NOEC(__trap_219, 219)
TRAP_NOEC(__trap_220, 220)
TRAP_NOEC(__trap_221, 221)
TRAP_NOEC(__trap_222, 222)
TRAP_NOEC(__trap_223, 223)
TRAP_NOEC(__trap_224, 224)
TRAP_NOEC(__trap_225, 225)
TRAP_NOEC(__trap_226, 226)
TRAP_NOEC(__trap_227, 227)
TRAP_NOEC(__trap_228, 228)
TRAP_NOEC(__trap_229, 229)
TRAP_NOEC(__trap_230, 230)
TRAP_NOEC(__trap_231, 231)
TRAP_NOEC(__trap_232, 232)
TRAP_NOEC(__trap_233, 233)
TRAP_NOEC(__trap_234, 234)
TRAP_NOEC(__trap_235, 235)
TRAP_NOEC(__trap_236, 236)
TRAP_NOEC(__trap_237, 237)
TRAP_NOEC(__trap_238, 238)
TRAP_NOEC(__trap_239, 239)
TRAP_NOEC(__trap_240, 240)
TRAP_NOEC(__trap_241, 241)
TRAP_NOEC(__trap_242, 242)
TRAP_NOEC(__trap_243, 243)
TRAP_NOEC(__trap_244, 244)
TRAP_NOEC(__trap_245, 245)
TRAP_NOEC(__trap_246, 246)
TRAP_NOEC(__trap_247, 247)
TRAP_NOEC(__trap_248, 248)
TRAP_NOEC(__trap_249, 249)
TRAP_NOEC(__trap_250, 250)
TRAP_NOEC(__trap_251, 251)
TRAP_NOEC(__trap_252, 252)
TRAP_NOEC(__trap_253, 253)
TRAP_NOEC(__trap_254, 254)
TRAP_NOEC(__trap_255, 255)


.section .rodata
.globl __traps
.type __traps, @object
.size __traps, 2048
.align 16
__traps:
.quad __trap_0
.quad __trap_1
.quad __trap_2
.quad __trap_3
.quad __trap_4
.quad __trap_5
.quad __trap_6
.quad __trap_7
.quad __trap_8
.quad __trap_9
.quad __trap_10
.quad __trap_11
.quad __trap_12
.quad __trap_13
.quad __trap_14
.quad __trap_15
.quad __trap_16
.quad __trap_17
.quad __trap_18
.quad __trap_19
.quad __trap_20
.quad __trap_21
.quad __trap_22
.quad __trap_23
.quad __trap_24
.quad __trap_25
.quad __trap_26
.quad __trap_27
.quad __trap_28
.quad __trap_29
.quad __trap_30
.quad __trap_31
.quad __trap_32
.quad __trap_33
.quad __trap_34
.quad __trap_35
.quad __trap_36
.quad __trap_37
.quad __trap_38
.quad __trap_39
.quad __trap_40
.quad __trap_41
.quad __trap_42
.quad __trap_43
.quad __trap_44
.quad __trap_45
.quad __trap_46
.quad __trap_47
.quad __trap_48
.quad __trap_49
.quad __trap_50
.quad __trap_51
.quad __trap_52
.quad __trap_53
.quad __trap_54
.quad __trap_55
.quad __trap_56
.quad __trap_57
.quad __trap_58
.quad __trap_59
.quad __trap_60
.quad __trap_61
.quad __trap_62
.quad __trap_63
.quad __trap_64
.quad __trap_65
.quad __trap_66
.quad __trap_67
.quad __trap_68
.quad __trap_69
.quad __trap_70
.quad __trap_71
.quad __trap_72
.quad __trap_73
.quad __trap_74
.quad __trap_75
.quad __trap_76
.quad __trap_77
.quad __trap_78
.quad __trap_79
.quad __trap_80
.quad __trap_81
.quad __trap_82
.quad __trap_83
.quad __trap_84
.quad __trap_85
.quad __trap_86
.quad __trap_87
.quad __trap_88
.quad __trap_89
.quad __trap_90
.quad __trap_91
.quad __trap_92
.quad __trap_93
.quad __trap_94
.quad __trap_95
.quad __trap_96
.quad __trap_97
.quad __trap_98
.quad __trap_99
.quad __trap_100
.quad __trap_101
.quad __trap_102
.quad __trap_103
.quad __trap_104
.quad __trap_105
.quad __trap_106
.quad __trap_107
.quad __trap_108
.quad __trap_109
.quad __trap_110
.quad __trap_111
.quad __trap_112
.quad __trap_113
.quad __trap_114
.quad __trap_115
.quad __trap_116
.quad __trap_117
.quad __trap_118
.quad __trap_119
.quad __trap_120
.quad __trap_121
.quad __trap_122
.quad __trap_123
.quad __trap_124
.quad __trap_125
.quad __trap_126
.quad __trap_127
.quad __trap_128
.quad __trap_129
.quad __trap_130
.quad __trap_131
.quad __trap_132
.quad __trap_133
.quad __trap_134
.quad __trap_135
.quad __trap_136
.quad __trap_137
.quad __trap_138
.quad __trap_139
.quad __trap_140
.quad __trap_141
.quad __trap_142
.quad __trap_143
.quad __trap_144
.quad __trap_145
.quad __trap_146
.quad __trap_147
.quad __trap_148
.quad __trap_149
.quad __trap_150
.quad __trap_151
.quad __trap_152
.quad __trap_153
.quad __trap_154
.quad __trap_155
.quad __trap_156
.quad __trap_157
.quad __trap_158
.quad __trap_159
.quad __trap_160
.quad __trap_161
.quad __trap_162
.quad __trap_163
.quad __trap_164
.quad __trap_165
.quad __trap_166
.quad __trap_167
.quad __trap_168
.quad __trap_169
.quad __trap_170
.quad __trap_171
.quad __trap_172
.quad __trap_173
.quad __trap_174
.quad __trap_175
.quad __trap_176
.quad __trap_177
.quad __trap_178
.quad __trap_179
.quad __trap_180
.quad __trap_181
.quad __trap_182
.quad __trap_183
.quad __trap_184
.quad __trap_185
.quad __trap_186
.quad __trap_187
.quad __trap_188
.quad __trap_189
.quad __trap_190
.quad __trap_191
.quad __trap_192
.quad __trap_193
.quad __trap_194
.quad __trap_195
.quad __trap_196
.quad __trap_197
.quad __trap_198
.quad __trap_199
.quad __trap_200
.quad __trap_201
.quad __trap_202
.quad __trap_203
.quad __trap_204
.quad __trap_205
.quad __trap_206
.quad __trap_207
.quad __trap_208
.quad __trap_209
.quad __trap_210
.quad __trap_211
.quad __trap_212
.quad __trap_213
.quad __trap_214
.quad __trap_215
.quad __trap_216
.quad __trap_217
.quad __trap_218
.quad __trap_219
.quad __trap_220
.quad __trap_221
.quad __trap_222
.quad __trap_223
.quad __trap_224
.quad __trap_225
.quad __trap_226
.quad __trap_227
.quad __trap_228
.quad __trap_229
.quad __trap_230
.quad __trap_231
.quad __trap_232
.quad __trap_233
.quad __trap_234
.quad __trap_235
.quad __trap_236
.quad __trap_237
.quad __trap_238
.quad __trap_239
.quad __trap_240
.quad __trap_241
.quad __trap_242
.quad __trap_243
.quad __trap_244
.quad __trap_245
.quad __trap_246
.quad __trap_247
.quad __trap_248
.quad __trap_249
.quad __trap_250
.quad __trap_251
.quad __trap_252
.quad __trap_253
.quad __trap_254
.quad __trap_255
