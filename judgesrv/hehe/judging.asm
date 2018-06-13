
judging:     file format elf32-i386


Disassembly of section .text:

00800020 <_start>:
  800020:	81 fc 00 e0 bf ee    	cmp    $0xeebfe000,%esp
  800026:	75 04                	jne    80002c <args_exist>
  800028:	6a 00                	push   $0x0
  80002a:	6a 00                	push   $0x0

0080002c <args_exist>:
  80002c:	e8 dd 0c 00 00       	call   800d0e <libmain>
  800031:	eb fe                	jmp    800031 <args_exist+0x5>
  800033:	66 90                	xchg   %ax,%ax
  800035:	66 90                	xchg   %ax,%ax
  800037:	66 90                	xchg   %ax,%ax
  800039:	66 90                	xchg   %ax,%ax
  80003b:	66 90                	xchg   %ax,%ax
  80003d:	66 90                	xchg   %ax,%ax
  80003f:	90                   	nop

00800040 <powmod>:
  800040:	55                   	push   %ebp
  800041:	89 e5                	mov    %esp,%ebp
  800043:	57                   	push   %edi
  800044:	56                   	push   %esi
  800045:	53                   	push   %ebx
  800046:	83 ec 1c             	sub    $0x1c,%esp
  800049:	c7 45 e4 01 00 00 00 	movl   $0x1,-0x1c(%ebp)
  800050:	8b 45 08             	mov    0x8(%ebp),%eax
  800053:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  800056:	eb 49                	jmp    8000a1 <powmod+0x61>
  800058:	90                   	nop
  800059:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  800060:	89 c7                	mov    %eax,%edi
  800062:	89 c6                	mov    %eax,%esi
  800064:	c1 ff 1f             	sar    $0x1f,%edi
  800067:	f6 c3 01             	test   $0x1,%bl
  80006a:	74 17                	je     800083 <powmod+0x43>
  80006c:	f7 6d e4             	imull  -0x1c(%ebp)
  80006f:	6a 00                	push   $0x0
  800071:	68 01 00 80 3b       	push   $0x3b800001
  800076:	52                   	push   %edx
  800077:	50                   	push   %eax
  800078:	e8 33 30 00 00       	call   8030b0 <__moddi3>
  80007d:	83 c4 10             	add    $0x10,%esp
  800080:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  800083:	0f af fe             	imul   %esi,%edi
  800086:	89 f0                	mov    %esi,%eax
  800088:	6a 00                	push   $0x0
  80008a:	f7 e6                	mul    %esi
  80008c:	68 01 00 80 3b       	push   $0x3b800001
  800091:	d1 fb                	sar    %ebx
  800093:	01 ff                	add    %edi,%edi
  800095:	01 fa                	add    %edi,%edx
  800097:	52                   	push   %edx
  800098:	50                   	push   %eax
  800099:	e8 12 30 00 00       	call   8030b0 <__moddi3>
  80009e:	83 c4 10             	add    $0x10,%esp
  8000a1:	85 db                	test   %ebx,%ebx
  8000a3:	75 bb                	jne    800060 <powmod+0x20>
  8000a5:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  8000a8:	8d 65 f4             	lea    -0xc(%ebp),%esp
  8000ab:	5b                   	pop    %ebx
  8000ac:	5e                   	pop    %esi
  8000ad:	5f                   	pop    %edi
  8000ae:	5d                   	pop    %ebp
  8000af:	c3                   	ret    

008000b0 <ntt>:
  8000b0:	55                   	push   %ebp
  8000b1:	31 c0                	xor    %eax,%eax
  8000b3:	31 d2                	xor    %edx,%edx
  8000b5:	89 e5                	mov    %esp,%ebp
  8000b7:	57                   	push   %edi
  8000b8:	56                   	push   %esi
  8000b9:	53                   	push   %ebx
  8000ba:	83 ec 3c             	sub    $0x3c,%esp
  8000bd:	8b 75 0c             	mov    0xc(%ebp),%esi
  8000c0:	8b 4d 08             	mov    0x8(%ebp),%ecx
  8000c3:	d1 fe                	sar    %esi
  8000c5:	3b 55 0c             	cmp    0xc(%ebp),%edx
  8000c8:	7d 34                	jge    8000fe <ntt+0x4e>
  8000ca:	39 c2                	cmp    %eax,%edx
  8000cc:	7d 13                	jge    8000e1 <ntt+0x31>
  8000ce:	8b 3c 91             	mov    (%ecx,%edx,4),%edi
  8000d1:	8d 1c 81             	lea    (%ecx,%eax,4),%ebx
  8000d4:	89 7d e0             	mov    %edi,-0x20(%ebp)
  8000d7:	8b 3b                	mov    (%ebx),%edi
  8000d9:	89 3c 91             	mov    %edi,(%ecx,%edx,4)
  8000dc:	8b 7d e0             	mov    -0x20(%ebp),%edi
  8000df:	89 3b                	mov    %edi,(%ebx)
  8000e1:	89 f3                	mov    %esi,%ebx
  8000e3:	eb 0b                	jmp    8000f0 <ntt+0x40>
  8000e5:	8d 76 00             	lea    0x0(%esi),%esi
  8000e8:	85 d8                	test   %ebx,%eax
  8000ea:	74 08                	je     8000f4 <ntt+0x44>
  8000ec:	31 d8                	xor    %ebx,%eax
  8000ee:	d1 fb                	sar    %ebx
  8000f0:	85 db                	test   %ebx,%ebx
  8000f2:	75 f4                	jne    8000e8 <ntt+0x38>
  8000f4:	31 d8                	xor    %ebx,%eax
  8000f6:	83 c2 01             	add    $0x1,%edx
  8000f9:	3b 55 0c             	cmp    0xc(%ebp),%edx
  8000fc:	7c cc                	jl     8000ca <ntt+0x1a>
  8000fe:	c7 45 c0 01 00 00 00 	movl   $0x1,-0x40(%ebp)
  800105:	8b 45 c0             	mov    -0x40(%ebp),%eax
  800108:	3b 45 0c             	cmp    0xc(%ebp),%eax
  80010b:	0f 8d 80 01 00 00    	jge    800291 <ntt+0x1e1>
  800111:	8b 45 c0             	mov    -0x40(%ebp),%eax
  800114:	c7 45 c8 01 00 00 00 	movl   $0x1,-0x38(%ebp)
  80011b:	01 c0                	add    %eax,%eax
  80011d:	89 c7                	mov    %eax,%edi
  80011f:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  800122:	b8 00 00 80 3b       	mov    $0x3b800000,%eax
  800127:	99                   	cltd   
  800128:	f7 ff                	idiv   %edi
  80012a:	89 c3                	mov    %eax,%ebx
  80012c:	b8 03 00 00 00       	mov    $0x3,%eax
  800131:	eb 34                	jmp    800167 <ntt+0xb7>
  800133:	90                   	nop
  800134:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  800138:	89 c7                	mov    %eax,%edi
  80013a:	89 45 e0             	mov    %eax,-0x20(%ebp)
  80013d:	c1 ff 1f             	sar    $0x1f,%edi
  800140:	89 7d e4             	mov    %edi,-0x1c(%ebp)
  800143:	8b 55 e4             	mov    -0x1c(%ebp),%edx
  800146:	8b 45 e0             	mov    -0x20(%ebp),%eax
  800149:	d1 fb                	sar    %ebx
  80014b:	6a 00                	push   $0x0
  80014d:	68 01 00 80 3b       	push   $0x3b800001
  800152:	89 d7                	mov    %edx,%edi
  800154:	0f af f8             	imul   %eax,%edi
  800157:	f7 e0                	mul    %eax
  800159:	01 ff                	add    %edi,%edi
  80015b:	01 fa                	add    %edi,%edx
  80015d:	52                   	push   %edx
  80015e:	50                   	push   %eax
  80015f:	e8 4c 2f 00 00       	call   8030b0 <__moddi3>
  800164:	83 c4 10             	add    $0x10,%esp
  800167:	85 db                	test   %ebx,%ebx
  800169:	74 29                	je     800194 <ntt+0xe4>
  80016b:	f6 c3 01             	test   $0x1,%bl
  80016e:	74 c8                	je     800138 <ntt+0x88>
  800170:	89 c7                	mov    %eax,%edi
  800172:	89 45 e0             	mov    %eax,-0x20(%ebp)
  800175:	6a 00                	push   $0x0
  800177:	f7 6d c8             	imull  -0x38(%ebp)
  80017a:	68 01 00 80 3b       	push   $0x3b800001
  80017f:	c1 ff 1f             	sar    $0x1f,%edi
  800182:	89 7d e4             	mov    %edi,-0x1c(%ebp)
  800185:	52                   	push   %edx
  800186:	50                   	push   %eax
  800187:	e8 24 2f 00 00       	call   8030b0 <__moddi3>
  80018c:	83 c4 10             	add    $0x10,%esp
  80018f:	89 45 c8             	mov    %eax,-0x38(%ebp)
  800192:	eb af                	jmp    800143 <ntt+0x93>
  800194:	83 7d 10 ff          	cmpl   $0xffffffff,0x10(%ebp)
  800198:	0f 84 01 01 00 00    	je     80029f <ntt+0x1ef>
  80019e:	8b 7d c0             	mov    -0x40(%ebp),%edi
  8001a1:	8b 45 08             	mov    0x8(%ebp),%eax
  8001a4:	c7 45 cc 00 00 00 00 	movl   $0x0,-0x34(%ebp)
  8001ab:	8b 75 cc             	mov    -0x34(%ebp),%esi
  8001ae:	c7 45 d8 01 00 00 00 	movl   $0x1,-0x28(%ebp)
  8001b5:	8d 04 b8             	lea    (%eax,%edi,4),%eax
  8001b8:	8b 7d 08             	mov    0x8(%ebp),%edi
  8001bb:	89 45 c4             	mov    %eax,-0x3c(%ebp)
  8001be:	8b 45 d4             	mov    -0x2c(%ebp),%eax
  8001c1:	c1 e0 02             	shl    $0x2,%eax
  8001c4:	39 75 c0             	cmp    %esi,-0x40(%ebp)
  8001c7:	89 45 d0             	mov    %eax,-0x30(%ebp)
  8001ca:	0f 8e af 00 00 00    	jle    80027f <ntt+0x1cf>
  8001d0:	8b 45 c4             	mov    -0x3c(%ebp),%eax
  8001d3:	8b 75 cc             	mov    -0x34(%ebp),%esi
  8001d6:	89 45 e0             	mov    %eax,-0x20(%ebp)
  8001d9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  8001e0:	8b 4d e0             	mov    -0x20(%ebp),%ecx
  8001e3:	8b 45 d8             	mov    -0x28(%ebp),%eax
  8001e6:	f7 29                	imull  (%ecx)
  8001e8:	6a 00                	push   $0x0
  8001ea:	68 01 00 80 3b       	push   $0x3b800001
  8001ef:	52                   	push   %edx
  8001f0:	50                   	push   %eax
  8001f1:	e8 ba 2e 00 00       	call   8030b0 <__moddi3>
  8001f6:	8b 0c b7             	mov    (%edi,%esi,4),%ecx
  8001f9:	89 c3                	mov    %eax,%ebx
  8001fb:	83 c4 10             	add    $0x10,%esp
  8001fe:	29 c1                	sub    %eax,%ecx
  800200:	b8 11 c8 35 11       	mov    $0x1135c811,%eax
  800205:	81 c1 01 00 80 3b    	add    $0x3b800001,%ecx
  80020b:	f7 e9                	imul   %ecx
  80020d:	89 c8                	mov    %ecx,%eax
  80020f:	c1 f8 1f             	sar    $0x1f,%eax
  800212:	c1 fa 1a             	sar    $0x1a,%edx
  800215:	29 c2                	sub    %eax,%edx
  800217:	8b 45 e0             	mov    -0x20(%ebp),%eax
  80021a:	69 d2 01 00 80 3b    	imul   $0x3b800001,%edx,%edx
  800220:	29 d1                	sub    %edx,%ecx
  800222:	89 08                	mov    %ecx,(%eax)
  800224:	03 1c b7             	add    (%edi,%esi,4),%ebx
  800227:	b8 11 c8 35 11       	mov    $0x1135c811,%eax
  80022c:	f7 eb                	imul   %ebx
  80022e:	89 d9                	mov    %ebx,%ecx
  800230:	c1 f9 1f             	sar    $0x1f,%ecx
  800233:	c1 fa 1a             	sar    $0x1a,%edx
  800236:	29 ca                	sub    %ecx,%edx
  800238:	69 d2 01 00 80 3b    	imul   $0x3b800001,%edx,%edx
  80023e:	29 d3                	sub    %edx,%ebx
  800240:	89 1c b7             	mov    %ebx,(%edi,%esi,4)
  800243:	03 75 d4             	add    -0x2c(%ebp),%esi
  800246:	8b 5d d0             	mov    -0x30(%ebp),%ebx
  800249:	01 5d e0             	add    %ebx,-0x20(%ebp)
  80024c:	39 75 0c             	cmp    %esi,0xc(%ebp)
  80024f:	7f 8f                	jg     8001e0 <ntt+0x130>
  800251:	8b 45 d8             	mov    -0x28(%ebp),%eax
  800254:	6a 00                	push   $0x0
  800256:	f7 6d c8             	imull  -0x38(%ebp)
  800259:	68 01 00 80 3b       	push   $0x3b800001
  80025e:	83 45 cc 01          	addl   $0x1,-0x34(%ebp)
  800262:	52                   	push   %edx
  800263:	50                   	push   %eax
  800264:	e8 47 2e 00 00       	call   8030b0 <__moddi3>
  800269:	83 c4 10             	add    $0x10,%esp
  80026c:	83 45 c4 04          	addl   $0x4,-0x3c(%ebp)
  800270:	8b 75 cc             	mov    -0x34(%ebp),%esi
  800273:	39 75 c0             	cmp    %esi,-0x40(%ebp)
  800276:	89 45 d8             	mov    %eax,-0x28(%ebp)
  800279:	0f 8f 51 ff ff ff    	jg     8001d0 <ntt+0x120>
  80027f:	8b 45 d4             	mov    -0x2c(%ebp),%eax
  800282:	89 45 c0             	mov    %eax,-0x40(%ebp)
  800285:	8b 45 c0             	mov    -0x40(%ebp),%eax
  800288:	3b 45 0c             	cmp    0xc(%ebp),%eax
  80028b:	0f 8c 80 fe ff ff    	jl     800111 <ntt+0x61>
  800291:	83 7d 10 ff          	cmpl   $0xffffffff,0x10(%ebp)
  800295:	74 6f                	je     800306 <ntt+0x256>
  800297:	8d 65 f4             	lea    -0xc(%ebp),%esp
  80029a:	5b                   	pop    %ebx
  80029b:	5e                   	pop    %esi
  80029c:	5f                   	pop    %edi
  80029d:	5d                   	pop    %ebp
  80029e:	c3                   	ret    
  80029f:	8b 45 c8             	mov    -0x38(%ebp),%eax
  8002a2:	c7 45 e0 1e 00 00 00 	movl   $0x1e,-0x20(%ebp)
  8002a9:	bb ff ff 7f 3b       	mov    $0x3b7fffff,%ebx
  8002ae:	c7 45 c8 01 00 00 00 	movl   $0x1,-0x38(%ebp)
  8002b5:	8d 76 00             	lea    0x0(%esi),%esi
  8002b8:	89 c7                	mov    %eax,%edi
  8002ba:	89 c6                	mov    %eax,%esi
  8002bc:	c1 ff 1f             	sar    $0x1f,%edi
  8002bf:	f6 c3 01             	test   $0x1,%bl
  8002c2:	74 17                	je     8002db <ntt+0x22b>
  8002c4:	f7 6d c8             	imull  -0x38(%ebp)
  8002c7:	6a 00                	push   $0x0
  8002c9:	68 01 00 80 3b       	push   $0x3b800001
  8002ce:	52                   	push   %edx
  8002cf:	50                   	push   %eax
  8002d0:	e8 db 2d 00 00       	call   8030b0 <__moddi3>
  8002d5:	83 c4 10             	add    $0x10,%esp
  8002d8:	89 45 c8             	mov    %eax,-0x38(%ebp)
  8002db:	89 f9                	mov    %edi,%ecx
  8002dd:	89 f0                	mov    %esi,%eax
  8002df:	6a 00                	push   $0x0
  8002e1:	0f af ce             	imul   %esi,%ecx
  8002e4:	68 01 00 80 3b       	push   $0x3b800001
  8002e9:	d1 fb                	sar    %ebx
  8002eb:	f7 e6                	mul    %esi
  8002ed:	01 c9                	add    %ecx,%ecx
  8002ef:	01 ca                	add    %ecx,%edx
  8002f1:	52                   	push   %edx
  8002f2:	50                   	push   %eax
  8002f3:	e8 b8 2d 00 00       	call   8030b0 <__moddi3>
  8002f8:	83 c4 10             	add    $0x10,%esp
  8002fb:	83 6d e0 01          	subl   $0x1,-0x20(%ebp)
  8002ff:	75 b7                	jne    8002b8 <ntt+0x208>
  800301:	e9 98 fe ff ff       	jmp    80019e <ntt+0xee>
  800306:	8b 45 0c             	mov    0xc(%ebp),%eax
  800309:	be 1e 00 00 00       	mov    $0x1e,%esi
  80030e:	c7 45 e0 01 00 00 00 	movl   $0x1,-0x20(%ebp)
  800315:	bb ff ff 7f 3b       	mov    $0x3b7fffff,%ebx
  80031a:	eb 38                	jmp    800354 <ntt+0x2a4>
  80031c:	89 c1                	mov    %eax,%ecx
  80031e:	89 45 d8             	mov    %eax,-0x28(%ebp)
  800321:	c1 f9 1f             	sar    $0x1f,%ecx
  800324:	89 4d dc             	mov    %ecx,-0x24(%ebp)
  800327:	8b 55 dc             	mov    -0x24(%ebp),%edx
  80032a:	8b 45 d8             	mov    -0x28(%ebp),%eax
  80032d:	d1 fb                	sar    %ebx
  80032f:	6a 00                	push   $0x0
  800331:	68 01 00 80 3b       	push   $0x3b800001
  800336:	89 d7                	mov    %edx,%edi
  800338:	0f af f8             	imul   %eax,%edi
  80033b:	f7 e0                	mul    %eax
  80033d:	01 ff                	add    %edi,%edi
  80033f:	01 fa                	add    %edi,%edx
  800341:	89 f7                	mov    %esi,%edi
  800343:	52                   	push   %edx
  800344:	50                   	push   %eax
  800345:	e8 66 2d 00 00       	call   8030b0 <__moddi3>
  80034a:	83 c4 10             	add    $0x10,%esp
  80034d:	83 ef 01             	sub    $0x1,%edi
  800350:	89 fe                	mov    %edi,%esi
  800352:	74 29                	je     80037d <ntt+0x2cd>
  800354:	f6 c3 01             	test   $0x1,%bl
  800357:	74 c3                	je     80031c <ntt+0x26c>
  800359:	89 c1                	mov    %eax,%ecx
  80035b:	89 45 d8             	mov    %eax,-0x28(%ebp)
  80035e:	6a 00                	push   $0x0
  800360:	f7 6d e0             	imull  -0x20(%ebp)
  800363:	68 01 00 80 3b       	push   $0x3b800001
  800368:	c1 f9 1f             	sar    $0x1f,%ecx
  80036b:	89 4d dc             	mov    %ecx,-0x24(%ebp)
  80036e:	52                   	push   %edx
  80036f:	50                   	push   %eax
  800370:	e8 3b 2d 00 00       	call   8030b0 <__moddi3>
  800375:	83 c4 10             	add    $0x10,%esp
  800378:	89 45 e0             	mov    %eax,-0x20(%ebp)
  80037b:	eb aa                	jmp    800327 <ntt+0x277>
  80037d:	31 db                	xor    %ebx,%ebx
  80037f:	8b 75 08             	mov    0x8(%ebp),%esi
  800382:	8b 7d 0c             	mov    0xc(%ebp),%edi
  800385:	eb 26                	jmp    8003ad <ntt+0x2fd>
  800387:	89 f6                	mov    %esi,%esi
  800389:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  800390:	8b 45 e0             	mov    -0x20(%ebp),%eax
  800393:	f7 2c 9e             	imull  (%esi,%ebx,4)
  800396:	6a 00                	push   $0x0
  800398:	68 01 00 80 3b       	push   $0x3b800001
  80039d:	52                   	push   %edx
  80039e:	50                   	push   %eax
  80039f:	e8 0c 2d 00 00       	call   8030b0 <__moddi3>
  8003a4:	83 c4 10             	add    $0x10,%esp
  8003a7:	89 04 9e             	mov    %eax,(%esi,%ebx,4)
  8003aa:	83 c3 01             	add    $0x1,%ebx
  8003ad:	39 fb                	cmp    %edi,%ebx
  8003af:	7c df                	jl     800390 <ntt+0x2e0>
  8003b1:	8d 65 f4             	lea    -0xc(%ebp),%esp
  8003b4:	5b                   	pop    %ebx
  8003b5:	5e                   	pop    %esi
  8003b6:	5f                   	pop    %edi
  8003b7:	5d                   	pop    %ebp
  8003b8:	c3                   	ret    
  8003b9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

008003c0 <mul>:
  8003c0:	55                   	push   %ebp
  8003c1:	89 e5                	mov    %esp,%ebp
  8003c3:	57                   	push   %edi
  8003c4:	56                   	push   %esi
  8003c5:	53                   	push   %ebx
  8003c6:	bb 01 00 00 00       	mov    $0x1,%ebx
  8003cb:	83 ec 1c             	sub    $0x1c,%esp
  8003ce:	8b 55 0c             	mov    0xc(%ebp),%edx
  8003d1:	8b 45 14             	mov    0x14(%ebp),%eax
  8003d4:	8b 7d 08             	mov    0x8(%ebp),%edi
  8003d7:	8b 4d 10             	mov    0x10(%ebp),%ecx
  8003da:	8b 75 18             	mov    0x18(%ebp),%esi
  8003dd:	8d 44 02 ff          	lea    -0x1(%edx,%eax,1),%eax
  8003e1:	eb 07                	jmp    8003ea <mul+0x2a>
  8003e3:	90                   	nop
  8003e4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  8003e8:	01 db                	add    %ebx,%ebx
  8003ea:	39 c3                	cmp    %eax,%ebx
  8003ec:	7c fa                	jl     8003e8 <mul+0x28>
  8003ee:	31 c0                	xor    %eax,%eax
  8003f0:	eb 1f                	jmp    800411 <mul+0x51>
  8003f2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  8003f8:	c7 04 85 20 6a 62 12 	movl   $0x0,0x12626a20(,%eax,4)
  8003ff:	00 00 00 00 
  800403:	c7 04 85 00 10 00 10 	movl   $0x0,0x10001000(,%eax,4)
  80040a:	00 00 00 00 
  80040e:	83 c0 01             	add    $0x1,%eax
  800411:	39 c3                	cmp    %eax,%ebx
  800413:	7f e3                	jg     8003f8 <mul+0x38>
  800415:	31 c0                	xor    %eax,%eax
  800417:	89 4d 10             	mov    %ecx,0x10(%ebp)
  80041a:	eb 11                	jmp    80042d <mul+0x6d>
  80041c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  800420:	8b 0c 87             	mov    (%edi,%eax,4),%ecx
  800423:	89 0c 85 00 10 00 10 	mov    %ecx,0x10001000(,%eax,4)
  80042a:	83 c0 01             	add    $0x1,%eax
  80042d:	39 d0                	cmp    %edx,%eax
  80042f:	7c ef                	jl     800420 <mul+0x60>
  800431:	8b 4d 10             	mov    0x10(%ebp),%ecx
  800434:	31 c0                	xor    %eax,%eax
  800436:	8b 7d 14             	mov    0x14(%ebp),%edi
  800439:	eb 12                	jmp    80044d <mul+0x8d>
  80043b:	90                   	nop
  80043c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  800440:	8b 14 81             	mov    (%ecx,%eax,4),%edx
  800443:	89 14 85 20 6a 62 12 	mov    %edx,0x12626a20(,%eax,4)
  80044a:	83 c0 01             	add    $0x1,%eax
  80044d:	39 f8                	cmp    %edi,%eax
  80044f:	7c ef                	jl     800440 <mul+0x80>
  800451:	83 ec 04             	sub    $0x4,%esp
  800454:	31 ff                	xor    %edi,%edi
  800456:	6a 01                	push   $0x1
  800458:	53                   	push   %ebx
  800459:	68 00 10 00 10       	push   $0x10001000
  80045e:	e8 4d fc ff ff       	call   8000b0 <ntt>
  800463:	83 c4 0c             	add    $0xc,%esp
  800466:	6a 01                	push   $0x1
  800468:	53                   	push   %ebx
  800469:	68 20 6a 62 12       	push   $0x12626a20
  80046e:	e8 3d fc ff ff       	call   8000b0 <ntt>
  800473:	83 c4 10             	add    $0x10,%esp
  800476:	eb 34                	jmp    8004ac <mul+0xec>
  800478:	90                   	nop
  800479:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  800480:	8b 04 bd 20 6a 62 12 	mov    0x12626a20(,%edi,4),%eax
  800487:	6a 00                	push   $0x0
  800489:	68 01 00 80 3b       	push   $0x3b800001
  80048e:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  800491:	f7 2c bd 00 10 00 10 	imull  0x10001000(,%edi,4)
  800498:	52                   	push   %edx
  800499:	50                   	push   %eax
  80049a:	e8 11 2c 00 00       	call   8030b0 <__moddi3>
  80049f:	83 c4 10             	add    $0x10,%esp
  8004a2:	89 04 bd 00 10 00 10 	mov    %eax,0x10001000(,%edi,4)
  8004a9:	83 c7 01             	add    $0x1,%edi
  8004ac:	39 fb                	cmp    %edi,%ebx
  8004ae:	7f d0                	jg     800480 <mul+0xc0>
  8004b0:	83 ec 04             	sub    $0x4,%esp
  8004b3:	6a ff                	push   $0xffffffff
  8004b5:	53                   	push   %ebx
  8004b6:	68 00 10 00 10       	push   $0x10001000
  8004bb:	e8 f0 fb ff ff       	call   8000b0 <ntt>
  8004c0:	83 c4 10             	add    $0x10,%esp
  8004c3:	31 c0                	xor    %eax,%eax
  8004c5:	eb 16                	jmp    8004dd <mul+0x11d>
  8004c7:	89 f6                	mov    %esi,%esi
  8004c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  8004d0:	8b 14 85 00 10 00 10 	mov    0x10001000(,%eax,4),%edx
  8004d7:	89 14 86             	mov    %edx,(%esi,%eax,4)
  8004da:	83 c0 01             	add    $0x1,%eax
  8004dd:	39 c3                	cmp    %eax,%ebx
  8004df:	7f ef                	jg     8004d0 <mul+0x110>
  8004e1:	8d 65 f4             	lea    -0xc(%ebp),%esp
  8004e4:	5b                   	pop    %ebx
  8004e5:	5e                   	pop    %esi
  8004e6:	5f                   	pop    %edi
  8004e7:	5d                   	pop    %ebp
  8004e8:	c3                   	ret    
  8004e9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

008004f0 <poly_multiply>:
  8004f0:	55                   	push   %ebp
  8004f1:	89 e5                	mov    %esp,%ebp
  8004f3:	5d                   	pop    %ebp
  8004f4:	e9 c7 fe ff ff       	jmp    8003c0 <mul>
  8004f9:	66 90                	xchg   %ax,%ax
  8004fb:	66 90                	xchg   %ax,%ax
  8004fd:	66 90                	xchg   %ax,%ax
  8004ff:	90                   	nop

00800500 <fft>:
  800500:	55                   	push   %ebp
  800501:	89 e5                	mov    %esp,%ebp
  800503:	57                   	push   %edi
  800504:	56                   	push   %esi
  800505:	53                   	push   %ebx
  800506:	89 d6                	mov    %edx,%esi
  800508:	89 cf                	mov    %ecx,%edi
  80050a:	83 ec 3c             	sub    $0x3c,%esp
  80050d:	89 55 cc             	mov    %edx,-0x34(%ebp)
  800510:	89 4d e0             	mov    %ecx,-0x20(%ebp)
  800513:	31 d2                	xor    %edx,%edx
  800515:	eb 0f                	jmp    800526 <fft+0x26>
  800517:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  80051a:	8b 1c 90             	mov    (%eax,%edx,4),%ebx
  80051d:	8b 0c 91             	mov    (%ecx,%edx,4),%ecx
  800520:	83 c2 01             	add    $0x1,%edx
  800523:	89 1c 8f             	mov    %ebx,(%edi,%ecx,4)
  800526:	39 f2                	cmp    %esi,%edx
  800528:	7c ed                	jl     800517 <fft+0x17>
  80052a:	31 c9                	xor    %ecx,%ecx
  80052c:	ba 01 00 00 00       	mov    $0x1,%edx
  800531:	8b 5d cc             	mov    -0x34(%ebp),%ebx
  800534:	eb 03                	jmp    800539 <fft+0x39>
  800536:	83 c1 01             	add    $0x1,%ecx
  800539:	89 d0                	mov    %edx,%eax
  80053b:	d3 e0                	shl    %cl,%eax
  80053d:	39 c3                	cmp    %eax,%ebx
  80053f:	7f f5                	jg     800536 <fft+0x36>
  800541:	89 c8                	mov    %ecx,%eax
  800543:	89 4d bc             	mov    %ecx,-0x44(%ebp)
  800546:	c7 45 c0 00 00 00 00 	movl   $0x0,-0x40(%ebp)
  80054d:	83 e8 01             	sub    $0x1,%eax
  800550:	8b 75 c0             	mov    -0x40(%ebp),%esi
  800553:	39 75 bc             	cmp    %esi,-0x44(%ebp)
  800556:	89 45 b8             	mov    %eax,-0x48(%ebp)
  800559:	0f 84 28 01 00 00    	je     800687 <fft+0x187>
  80055f:	8b 75 c0             	mov    -0x40(%ebp),%esi
  800562:	8b 4d b8             	mov    -0x48(%ebp),%ecx
  800565:	b8 04 00 00 00       	mov    $0x4,%eax
  80056a:	8b 7d 08             	mov    0x8(%ebp),%edi
  80056d:	c7 45 d0 00 00 00 00 	movl   $0x0,-0x30(%ebp)
  800574:	29 f1                	sub    %esi,%ecx
  800576:	d3 e0                	shl    %cl,%eax
  800578:	89 f1                	mov    %esi,%ecx
  80057a:	8b 04 07             	mov    (%edi,%eax,1),%eax
  80057d:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  800580:	b8 01 00 00 00       	mov    $0x1,%eax
  800585:	d3 e0                	shl    %cl,%eax
  800587:	8d 34 00             	lea    (%eax,%eax,1),%esi
  80058a:	89 45 c8             	mov    %eax,-0x38(%ebp)
  80058d:	89 75 c4             	mov    %esi,-0x3c(%ebp)
  800590:	8b 75 e0             	mov    -0x20(%ebp),%esi
  800593:	8d 04 86             	lea    (%esi,%eax,4),%eax
  800596:	8b 75 cc             	mov    -0x34(%ebp),%esi
  800599:	89 45 dc             	mov    %eax,-0x24(%ebp)
  80059c:	8b 45 d0             	mov    -0x30(%ebp),%eax
  80059f:	39 f0                	cmp    %esi,%eax
  8005a1:	0f 8d d0 00 00 00    	jge    800677 <fft+0x177>
  8005a7:	89 f6                	mov    %esi,%esi
  8005a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  8005b0:	8b 7d c8             	mov    -0x38(%ebp),%edi
  8005b3:	89 c6                	mov    %eax,%esi
  8005b5:	c7 45 e4 01 00 00 00 	movl   $0x1,-0x1c(%ebp)
  8005bc:	01 c7                	add    %eax,%edi
  8005be:	89 7d d8             	mov    %edi,-0x28(%ebp)
  8005c1:	e9 94 00 00 00       	jmp    80065a <fft+0x15a>
  8005c6:	8d 76 00             	lea    0x0(%esi),%esi
  8005c9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  8005d0:	8b 45 e0             	mov    -0x20(%ebp),%eax
  8005d3:	8b 7d dc             	mov    -0x24(%ebp),%edi
  8005d6:	8b 1c b0             	mov    (%eax,%esi,4),%ebx
  8005d9:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  8005dc:	f7 24 b7             	mull   (%edi,%esi,4)
  8005df:	6a 00                	push   $0x0
  8005e1:	68 01 00 80 3b       	push   $0x3b800001
  8005e6:	52                   	push   %edx
  8005e7:	50                   	push   %eax
  8005e8:	e8 b3 2d 00 00       	call   8033a0 <__umoddi3>
  8005ed:	8d 0c 03             	lea    (%ebx,%eax,1),%ecx
  8005f0:	89 c7                	mov    %eax,%edi
  8005f2:	b8 0f 81 5c 13       	mov    $0x135c810f,%eax
  8005f7:	81 c3 01 00 80 3b    	add    $0x3b800001,%ebx
  8005fd:	83 c4 10             	add    $0x10,%esp
  800600:	f7 e1                	mul    %ecx
  800602:	89 c8                	mov    %ecx,%eax
  800604:	29 fb                	sub    %edi,%ebx
  800606:	8b 7d dc             	mov    -0x24(%ebp),%edi
  800609:	29 d0                	sub    %edx,%eax
  80060b:	d1 e8                	shr    %eax
  80060d:	01 c2                	add    %eax,%edx
  80060f:	8b 45 e0             	mov    -0x20(%ebp),%eax
  800612:	c1 ea 1d             	shr    $0x1d,%edx
  800615:	69 d2 01 00 80 3b    	imul   $0x3b800001,%edx,%edx
  80061b:	29 d1                	sub    %edx,%ecx
  80061d:	89 0c b0             	mov    %ecx,(%eax,%esi,4)
  800620:	b8 0f 81 5c 13       	mov    $0x135c810f,%eax
  800625:	f7 e3                	mul    %ebx
  800627:	89 d8                	mov    %ebx,%eax
  800629:	29 d0                	sub    %edx,%eax
  80062b:	d1 e8                	shr    %eax
  80062d:	01 c2                	add    %eax,%edx
  80062f:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  800632:	c1 ea 1d             	shr    $0x1d,%edx
  800635:	69 d2 01 00 80 3b    	imul   $0x3b800001,%edx,%edx
  80063b:	29 d3                	sub    %edx,%ebx
  80063d:	f7 65 d4             	mull   -0x2c(%ebp)
  800640:	89 1c b7             	mov    %ebx,(%edi,%esi,4)
  800643:	6a 00                	push   $0x0
  800645:	83 c6 01             	add    $0x1,%esi
  800648:	68 01 00 80 3b       	push   $0x3b800001
  80064d:	52                   	push   %edx
  80064e:	50                   	push   %eax
  80064f:	e8 4c 2d 00 00       	call   8033a0 <__umoddi3>
  800654:	83 c4 10             	add    $0x10,%esp
  800657:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  80065a:	3b 75 d8             	cmp    -0x28(%ebp),%esi
  80065d:	0f 8c 6d ff ff ff    	jl     8005d0 <fft+0xd0>
  800663:	8b 75 c4             	mov    -0x3c(%ebp),%esi
  800666:	01 75 d0             	add    %esi,-0x30(%ebp)
  800669:	8b 45 d0             	mov    -0x30(%ebp),%eax
  80066c:	8b 75 cc             	mov    -0x34(%ebp),%esi
  80066f:	39 f0                	cmp    %esi,%eax
  800671:	0f 8c 39 ff ff ff    	jl     8005b0 <fft+0xb0>
  800677:	83 45 c0 01          	addl   $0x1,-0x40(%ebp)
  80067b:	8b 75 c0             	mov    -0x40(%ebp),%esi
  80067e:	39 75 bc             	cmp    %esi,-0x44(%ebp)
  800681:	0f 85 d8 fe ff ff    	jne    80055f <fft+0x5f>
  800687:	8d 65 f4             	lea    -0xc(%ebp),%esp
  80068a:	5b                   	pop    %ebx
  80068b:	5e                   	pop    %esi
  80068c:	5f                   	pop    %edi
  80068d:	5d                   	pop    %ebp
  80068e:	c3                   	ret    
  80068f:	90                   	nop

00800690 <std>:
  800690:	55                   	push   %ebp
  800691:	89 e5                	mov    %esp,%ebp
  800693:	57                   	push   %edi
  800694:	56                   	push   %esi
  800695:	53                   	push   %ebx
  800696:	89 c3                	mov    %eax,%ebx
  800698:	8d 7a 01             	lea    0x1(%edx),%edi
  80069b:	81 ec 30 00 00 07    	sub    $0x7000030,%esp
  8006a1:	8b 45 08             	mov    0x8(%ebp),%eax
  8006a4:	89 8d e4 ff ff f8    	mov    %ecx,-0x700001c(%ebp)
  8006aa:	68 00 00 00 01       	push   $0x1000000
  8006af:	6a 00                	push   $0x0
  8006b1:	8d 70 01             	lea    0x1(%eax),%esi
  8006b4:	8d 85 e8 ff ff f8    	lea    -0x7000018(%ebp),%eax
  8006ba:	50                   	push   %eax
  8006bb:	e8 80 0e 00 00       	call   801540 <memset>
  8006c0:	8d 85 e8 ff ff f9    	lea    -0x6000018(%ebp),%eax
  8006c6:	83 c4 0c             	add    $0xc,%esp
  8006c9:	68 00 00 00 01       	push   $0x1000000
  8006ce:	6a 00                	push   $0x0
  8006d0:	50                   	push   %eax
  8006d1:	e8 6a 0e 00 00       	call   801540 <memset>
  8006d6:	8d 85 e8 ff ff fa    	lea    -0x5000018(%ebp),%eax
  8006dc:	83 c4 0c             	add    $0xc,%esp
  8006df:	68 00 00 00 01       	push   $0x1000000
  8006e4:	6a 00                	push   $0x0
  8006e6:	50                   	push   %eax
  8006e7:	e8 54 0e 00 00       	call   801540 <memset>
  8006ec:	83 c4 10             	add    $0x10,%esp
  8006ef:	31 c0                	xor    %eax,%eax
  8006f1:	8b 8d e4 ff ff f8    	mov    -0x700001c(%ebp),%ecx
  8006f7:	eb 14                	jmp    80070d <std+0x7d>
  8006f9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  800700:	8b 14 83             	mov    (%ebx,%eax,4),%edx
  800703:	89 94 85 e8 ff ff f8 	mov    %edx,-0x7000018(%ebp,%eax,4)
  80070a:	83 c0 01             	add    $0x1,%eax
  80070d:	39 f8                	cmp    %edi,%eax
  80070f:	7c ef                	jl     800700 <std+0x70>
  800711:	31 c0                	xor    %eax,%eax
  800713:	eb 10                	jmp    800725 <std+0x95>
  800715:	8d 76 00             	lea    0x0(%esi),%esi
  800718:	8b 14 81             	mov    (%ecx,%eax,4),%edx
  80071b:	89 94 85 e8 ff ff f9 	mov    %edx,-0x6000018(%ebp,%eax,4)
  800722:	83 c0 01             	add    $0x1,%eax
  800725:	39 f0                	cmp    %esi,%eax
  800727:	7c ef                	jl     800718 <std+0x88>
  800729:	8d 85 e8 ff ff fb    	lea    -0x4000018(%ebp),%eax
  80072f:	83 ec 04             	sub    $0x4,%esp
  800732:	8d 9d e8 ff ff fd    	lea    -0x2000018(%ebp),%ebx
  800738:	68 00 00 00 01       	push   $0x1000000
  80073d:	6a 00                	push   $0x0
  80073f:	50                   	push   %eax
  800740:	e8 fb 0d 00 00       	call   801540 <memset>
  800745:	8d 85 e8 ff ff fc    	lea    -0x3000018(%ebp),%eax
  80074b:	83 c4 0c             	add    $0xc,%esp
  80074e:	68 00 00 00 01       	push   $0x1000000
  800753:	6a 00                	push   $0x0
  800755:	50                   	push   %eax
  800756:	e8 e5 0d 00 00       	call   801540 <memset>
  80075b:	83 c4 0c             	add    $0xc,%esp
  80075e:	68 00 00 00 01       	push   $0x1000000
  800763:	6a 00                	push   $0x0
  800765:	53                   	push   %ebx
  800766:	e8 d5 0d 00 00       	call   801540 <memset>
  80076b:	8d 85 e8 ff ff fe    	lea    -0x1000018(%ebp),%eax
  800771:	83 c4 0c             	add    $0xc,%esp
  800774:	68 00 00 00 01       	push   $0x1000000
  800779:	6a 00                	push   $0x0
  80077b:	50                   	push   %eax
  80077c:	e8 bf 0d 00 00       	call   801540 <memset>
  800781:	8d 04 37             	lea    (%edi,%esi,1),%eax
  800784:	83 c4 10             	add    $0x10,%esp
  800787:	31 c9                	xor    %ecx,%ecx
  800789:	89 c2                	mov    %eax,%edx
  80078b:	89 85 d4 ff ff f8    	mov    %eax,-0x700002c(%ebp)
  800791:	b8 01 00 00 00       	mov    $0x1,%eax
  800796:	eb 0d                	jmp    8007a5 <std+0x115>
  800798:	90                   	nop
  800799:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  8007a0:	01 c0                	add    %eax,%eax
  8007a2:	83 c1 01             	add    $0x1,%ecx
  8007a5:	39 d0                	cmp    %edx,%eax
  8007a7:	7c f7                	jl     8007a0 <std+0x110>
  8007a9:	89 85 e0 ff ff f8    	mov    %eax,-0x7000020(%ebp)
  8007af:	c7 85 e8 ff ff fe 00 	movl   $0x0,-0x1000018(%ebp)
  8007b6:	00 00 00 
  8007b9:	be 01 00 00 00       	mov    $0x1,%esi
  8007be:	83 e9 01             	sub    $0x1,%ecx
  8007c1:	89 c7                	mov    %eax,%edi
  8007c3:	eb 23                	jmp    8007e8 <std+0x158>
  8007c5:	8d 76 00             	lea    0x0(%esi),%esi
  8007c8:	89 f0                	mov    %esi,%eax
  8007ca:	d1 f8                	sar    %eax
  8007cc:	8b 94 85 e8 ff ff fe 	mov    -0x1000018(%ebp,%eax,4),%edx
  8007d3:	89 f0                	mov    %esi,%eax
  8007d5:	83 e0 01             	and    $0x1,%eax
  8007d8:	d3 e0                	shl    %cl,%eax
  8007da:	d1 fa                	sar    %edx
  8007dc:	09 c2                	or     %eax,%edx
  8007de:	89 94 b5 e8 ff ff fe 	mov    %edx,-0x1000018(%ebp,%esi,4)
  8007e5:	83 c6 01             	add    $0x1,%esi
  8007e8:	39 f7                	cmp    %esi,%edi
  8007ea:	7f dc                	jg     8007c8 <std+0x138>
  8007ec:	8b bd e0 ff ff f8    	mov    -0x7000020(%ebp),%edi
  8007f2:	b8 00 00 80 3b       	mov    $0x3b800000,%eax
  8007f7:	31 d2                	xor    %edx,%edx
  8007f9:	c7 85 dc ff ff f8 01 	movl   $0x1,-0x7000024(%ebp)
  800800:	00 00 00 
  800803:	f7 f7                	div    %edi
  800805:	89 bd d8 ff ff f8    	mov    %edi,-0x7000028(%ebp)
  80080b:	89 85 e4 ff ff f8    	mov    %eax,-0x700001c(%ebp)
  800811:	b8 03 00 00 00       	mov    $0x3,%eax
  800816:	8b 8d e4 ff ff f8    	mov    -0x700001c(%ebp),%ecx
  80081c:	eb 30                	jmp    80084e <std+0x1be>
  80081e:	66 90                	xchg   %ax,%ax
  800820:	89 c6                	mov    %eax,%esi
  800822:	31 ff                	xor    %edi,%edi
  800824:	0f af fe             	imul   %esi,%edi
  800827:	89 f0                	mov    %esi,%eax
  800829:	6a 00                	push   $0x0
  80082b:	f7 e6                	mul    %esi
  80082d:	68 01 00 80 3b       	push   $0x3b800001
  800832:	89 8d e4 ff ff f8    	mov    %ecx,-0x700001c(%ebp)
  800838:	01 ff                	add    %edi,%edi
  80083a:	01 fa                	add    %edi,%edx
  80083c:	52                   	push   %edx
  80083d:	50                   	push   %eax
  80083e:	e8 5d 2b 00 00       	call   8033a0 <__umoddi3>
  800843:	8b 8d e4 ff ff f8    	mov    -0x700001c(%ebp),%ecx
  800849:	83 c4 10             	add    $0x10,%esp
  80084c:	d1 e9                	shr    %ecx
  80084e:	85 c9                	test   %ecx,%ecx
  800850:	74 34                	je     800886 <std+0x1f6>
  800852:	f6 c1 01             	test   $0x1,%cl
  800855:	74 c9                	je     800820 <std+0x190>
  800857:	89 c6                	mov    %eax,%esi
  800859:	6a 00                	push   $0x0
  80085b:	68 01 00 80 3b       	push   $0x3b800001
  800860:	f7 a5 dc ff ff f8    	mull   -0x7000024(%ebp)
  800866:	89 8d e4 ff ff f8    	mov    %ecx,-0x700001c(%ebp)
  80086c:	31 ff                	xor    %edi,%edi
  80086e:	52                   	push   %edx
  80086f:	50                   	push   %eax
  800870:	e8 2b 2b 00 00       	call   8033a0 <__umoddi3>
  800875:	83 c4 10             	add    $0x10,%esp
  800878:	89 85 dc ff ff f8    	mov    %eax,-0x7000024(%ebp)
  80087e:	8b 8d e4 ff ff f8    	mov    -0x700001c(%ebp),%ecx
  800884:	eb 9e                	jmp    800824 <std+0x194>
  800886:	c7 85 e8 ff ff fd 01 	movl   $0x1,-0x2000018(%ebp)
  80088d:	00 00 00 
  800890:	be 01 00 00 00       	mov    $0x1,%esi
  800895:	8b bd dc ff ff f8    	mov    -0x7000024(%ebp),%edi
  80089b:	eb 2a                	jmp    8008c7 <std+0x237>
  80089d:	8d 76 00             	lea    0x0(%esi),%esi
  8008a0:	8b 44 b3 fc          	mov    -0x4(%ebx,%esi,4),%eax
  8008a4:	6a 00                	push   $0x0
  8008a6:	68 01 00 80 3b       	push   $0x3b800001
  8008ab:	89 c1                	mov    %eax,%ecx
  8008ad:	c1 f9 1f             	sar    $0x1f,%ecx
  8008b0:	f7 e7                	mul    %edi
  8008b2:	0f af cf             	imul   %edi,%ecx
  8008b5:	01 ca                	add    %ecx,%edx
  8008b7:	52                   	push   %edx
  8008b8:	50                   	push   %eax
  8008b9:	e8 e2 2a 00 00       	call   8033a0 <__umoddi3>
  8008be:	83 c4 10             	add    $0x10,%esp
  8008c1:	89 04 b3             	mov    %eax,(%ebx,%esi,4)
  8008c4:	83 c6 01             	add    $0x1,%esi
  8008c7:	39 b5 e0 ff ff f8    	cmp    %esi,-0x7000020(%ebp)
  8008cd:	7f d1                	jg     8008a0 <std+0x210>
  8008cf:	8b bd e0 ff ff f8    	mov    -0x7000020(%ebp),%edi
  8008d5:	8d 85 e8 ff ff fe    	lea    -0x1000018(%ebp),%eax
  8008db:	83 ec 08             	sub    $0x8,%esp
  8008de:	8d 8d e8 ff ff fb    	lea    -0x4000018(%ebp),%ecx
  8008e4:	31 f6                	xor    %esi,%esi
  8008e6:	50                   	push   %eax
  8008e7:	8d 85 e8 ff ff f8    	lea    -0x7000018(%ebp),%eax
  8008ed:	53                   	push   %ebx
  8008ee:	89 fa                	mov    %edi,%edx
  8008f0:	e8 0b fc ff ff       	call   800500 <fft>
  8008f5:	58                   	pop    %eax
  8008f6:	8d 85 e8 ff ff fe    	lea    -0x1000018(%ebp),%eax
  8008fc:	8d 8d e8 ff ff fc    	lea    -0x3000018(%ebp),%ecx
  800902:	5a                   	pop    %edx
  800903:	50                   	push   %eax
  800904:	8d 85 e8 ff ff f9    	lea    -0x6000018(%ebp),%eax
  80090a:	53                   	push   %ebx
  80090b:	89 fa                	mov    %edi,%edx
  80090d:	e8 ee fb ff ff       	call   800500 <fft>
  800912:	83 c4 10             	add    $0x10,%esp
  800915:	eb 38                	jmp    80094f <std+0x2bf>
  800917:	89 f6                	mov    %esi,%esi
  800919:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  800920:	8b 84 b5 e8 ff ff fc 	mov    -0x3000018(%ebp,%esi,4),%eax
  800927:	6a 00                	push   $0x0
  800929:	68 01 00 80 3b       	push   $0x3b800001
  80092e:	89 85 e4 ff ff f8    	mov    %eax,-0x700001c(%ebp)
  800934:	f7 a4 b5 e8 ff ff fb 	mull   -0x4000018(%ebp,%esi,4)
  80093b:	52                   	push   %edx
  80093c:	50                   	push   %eax
  80093d:	e8 5e 2a 00 00       	call   8033a0 <__umoddi3>
  800942:	83 c4 10             	add    $0x10,%esp
  800945:	89 84 b5 e8 ff ff fb 	mov    %eax,-0x4000018(%ebp,%esi,4)
  80094c:	83 c6 01             	add    $0x1,%esi
  80094f:	39 f7                	cmp    %esi,%edi
  800951:	7f cd                	jg     800920 <std+0x290>
  800953:	8b 95 e0 ff ff f8    	mov    -0x7000020(%ebp),%edx
  800959:	b8 01 00 00 00       	mov    $0x1,%eax
  80095e:	eb 12                	jmp    800972 <std+0x2e2>
  800960:	8b 0c 83             	mov    (%ebx,%eax,4),%ecx
  800963:	8b 34 93             	mov    (%ebx,%edx,4),%esi
  800966:	89 34 83             	mov    %esi,(%ebx,%eax,4)
  800969:	89 0c 93             	mov    %ecx,(%ebx,%edx,4)
  80096c:	83 c0 01             	add    $0x1,%eax
  80096f:	83 ea 01             	sub    $0x1,%edx
  800972:	39 d0                	cmp    %edx,%eax
  800974:	7c ea                	jl     800960 <std+0x2d0>
  800976:	8d 85 e8 ff ff fe    	lea    -0x1000018(%ebp),%eax
  80097c:	83 ec 08             	sub    $0x8,%esp
  80097f:	8b 95 e0 ff ff f8    	mov    -0x7000020(%ebp),%edx
  800985:	8d 8d e8 ff ff f8    	lea    -0x7000018(%ebp),%ecx
  80098b:	50                   	push   %eax
  80098c:	8d 85 e8 ff ff fb    	lea    -0x4000018(%ebp),%eax
  800992:	53                   	push   %ebx
  800993:	bb ff ff 7f 3b       	mov    $0x3b7fffff,%ebx
  800998:	e8 63 fb ff ff       	call   800500 <fft>
  80099d:	8b 85 d8 ff ff f8    	mov    -0x7000028(%ebp),%eax
  8009a3:	83 c4 10             	add    $0x10,%esp
  8009a6:	c7 85 e0 ff ff f8 1e 	movl   $0x1e,-0x7000020(%ebp)
  8009ad:	00 00 00 
  8009b0:	c7 85 e4 ff ff f8 01 	movl   $0x1,-0x700001c(%ebp)
  8009b7:	00 00 00 
  8009ba:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  8009c0:	31 ff                	xor    %edi,%edi
  8009c2:	f6 c3 01             	test   $0x1,%bl
  8009c5:	89 c6                	mov    %eax,%esi
  8009c7:	74 1d                	je     8009e6 <std+0x356>
  8009c9:	f7 a5 e4 ff ff f8    	mull   -0x700001c(%ebp)
  8009cf:	6a 00                	push   $0x0
  8009d1:	68 01 00 80 3b       	push   $0x3b800001
  8009d6:	52                   	push   %edx
  8009d7:	50                   	push   %eax
  8009d8:	e8 c3 29 00 00       	call   8033a0 <__umoddi3>
  8009dd:	83 c4 10             	add    $0x10,%esp
  8009e0:	89 85 e4 ff ff f8    	mov    %eax,-0x700001c(%ebp)
  8009e6:	89 f9                	mov    %edi,%ecx
  8009e8:	89 f0                	mov    %esi,%eax
  8009ea:	6a 00                	push   $0x0
  8009ec:	0f af ce             	imul   %esi,%ecx
  8009ef:	68 01 00 80 3b       	push   $0x3b800001
  8009f4:	d1 eb                	shr    %ebx
  8009f6:	f7 e6                	mul    %esi
  8009f8:	01 c9                	add    %ecx,%ecx
  8009fa:	01 ca                	add    %ecx,%edx
  8009fc:	52                   	push   %edx
  8009fd:	50                   	push   %eax
  8009fe:	e8 9d 29 00 00       	call   8033a0 <__umoddi3>
  800a03:	83 c4 10             	add    $0x10,%esp
  800a06:	83 ad e0 ff ff f8 01 	subl   $0x1,-0x7000020(%ebp)
  800a0d:	75 b1                	jne    8009c0 <std+0x330>
  800a0f:	8b 9d d4 ff ff f8    	mov    -0x700002c(%ebp),%ebx
  800a15:	31 f6                	xor    %esi,%esi
  800a17:	8b bd e4 ff ff f8    	mov    -0x700001c(%ebp),%edi
  800a1d:	83 eb 01             	sub    $0x1,%ebx
  800a20:	eb 2a                	jmp    800a4c <std+0x3bc>
  800a22:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  800a28:	89 f8                	mov    %edi,%eax
  800a2a:	6a 00                	push   $0x0
  800a2c:	68 01 00 80 3b       	push   $0x3b800001
  800a31:	f7 a4 b5 e8 ff ff f8 	mull   -0x7000018(%ebp,%esi,4)
  800a38:	52                   	push   %edx
  800a39:	50                   	push   %eax
  800a3a:	e8 61 29 00 00       	call   8033a0 <__umoddi3>
  800a3f:	83 c4 10             	add    $0x10,%esp
  800a42:	89 84 b5 e8 ff ff fa 	mov    %eax,-0x5000018(%ebp,%esi,4)
  800a49:	83 c6 01             	add    $0x1,%esi
  800a4c:	39 de                	cmp    %ebx,%esi
  800a4e:	7c d8                	jl     800a28 <std+0x398>
  800a50:	31 c0                	xor    %eax,%eax
  800a52:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  800a55:	eb 16                	jmp    800a6d <std+0x3dd>
  800a57:	89 f6                	mov    %esi,%esi
  800a59:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  800a60:	8b 94 85 e8 ff ff fa 	mov    -0x5000018(%ebp,%eax,4),%edx
  800a67:	89 14 81             	mov    %edx,(%ecx,%eax,4)
  800a6a:	83 c0 01             	add    $0x1,%eax
  800a6d:	39 d8                	cmp    %ebx,%eax
  800a6f:	7c ef                	jl     800a60 <std+0x3d0>
  800a71:	8d 65 f4             	lea    -0xc(%ebp),%esp
  800a74:	5b                   	pop    %ebx
  800a75:	5e                   	pop    %esi
  800a76:	5f                   	pop    %edi
  800a77:	5d                   	pop    %ebp
  800a78:	c3                   	ret    
  800a79:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00800a80 <poly_multiply_wrapper>:
  800a80:	55                   	push   %ebp
  800a81:	89 e5                	mov    %esp,%ebp
  800a83:	83 ec 14             	sub    $0x14,%esp
  800a86:	68 38 f2 65 1c       	push   $0x1c65f238
  800a8b:	ff 35 58 04 e0 1c    	pushl  0x1ce00458
  800a91:	68 1c e9 28 1c       	push   $0x1c28e91c
  800a96:	ff 35 54 04 e0 1c    	pushl  0x1ce00454
  800a9c:	68 00 e0 eb 1b       	push   $0x1bebe000
  800aa1:	e8 4a fa ff ff       	call   8004f0 <poly_multiply>
  800aa6:	83 c4 20             	add    $0x20,%esp
  800aa9:	c9                   	leave  
  800aaa:	e9 03 10 00 00       	jmp    801ab2 <sys_quit_judge>
  800aaf:	90                   	nop

00800ab0 <umain>:
  800ab0:	55                   	push   %ebp
  800ab1:	89 e5                	mov    %esp,%ebp
  800ab3:	57                   	push   %edi
  800ab4:	56                   	push   %esi
  800ab5:	53                   	push   %ebx
  800ab6:	81 ec 8c 00 00 00    	sub    $0x8c,%esp
  800abc:	83 7d 08 03          	cmpl   $0x3,0x8(%ebp)
  800ac0:	74 0e                	je     800ad0 <umain+0x20>
  800ac2:	8d 65 f4             	lea    -0xc(%ebp),%esp
  800ac5:	5b                   	pop    %ebx
  800ac6:	5e                   	pop    %esi
  800ac7:	5f                   	pop    %edi
  800ac8:	5d                   	pop    %ebp
  800ac9:	c3                   	ret    
  800aca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  800ad0:	8b 45 0c             	mov    0xc(%ebp),%eax
  800ad3:	31 db                	xor    %ebx,%ebx
  800ad5:	8b 50 04             	mov    0x4(%eax),%edx
  800ad8:	eb 10                	jmp    800aea <umain+0x3a>
  800ada:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  800ae0:	8d 0c 9b             	lea    (%ebx,%ebx,4),%ecx
  800ae3:	83 c2 01             	add    $0x1,%edx
  800ae6:	8d 5c 48 d0          	lea    -0x30(%eax,%ecx,2),%ebx
  800aea:	0f be 02             	movsbl (%edx),%eax
  800aed:	84 c0                	test   %al,%al
  800aef:	75 ef                	jne    800ae0 <umain+0x30>
  800af1:	8b 45 0c             	mov    0xc(%ebp),%eax
  800af4:	31 f6                	xor    %esi,%esi
  800af6:	8b 50 08             	mov    0x8(%eax),%edx
  800af9:	eb 0f                	jmp    800b0a <umain+0x5a>
  800afb:	90                   	nop
  800afc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  800b00:	8d 0c b6             	lea    (%esi,%esi,4),%ecx
  800b03:	83 c2 01             	add    $0x1,%edx
  800b06:	8d 74 48 d0          	lea    -0x30(%eax,%ecx,2),%esi
  800b0a:	0f be 02             	movsbl (%edx),%eax
  800b0d:	84 c0                	test   %al,%al
  800b0f:	75 ef                	jne    800b00 <umain+0x50>
  800b11:	81 fe 00 00 08 00    	cmp    $0x80000,%esi
  800b17:	7f a9                	jg     800ac2 <umain+0x12>
  800b19:	8d 45 84             	lea    -0x7c(%ebp),%eax
  800b1c:	83 ec 04             	sub    $0x4,%esp
  800b1f:	6a 64                	push   $0x64
  800b21:	6a 00                	push   $0x0
  800b23:	50                   	push   %eax
  800b24:	e8 17 0a 00 00       	call   801540 <memset>
  800b29:	a1 00 00 00 10       	mov    0x10000000,%eax
  800b2e:	89 5d 84             	mov    %ebx,-0x7c(%ebp)
  800b31:	83 c4 10             	add    $0x10,%esp
  800b34:	89 75 88             	mov    %esi,-0x78(%ebp)
  800b37:	c7 45 d8 01 00 00 00 	movl   $0x1,-0x28(%ebp)
  800b3e:	8d b8 00 d0 0b ff    	lea    -0xf43000(%eax),%edi
  800b44:	89 85 74 ff ff ff    	mov    %eax,-0x8c(%ebp)
  800b4a:	89 bd 70 ff ff ff    	mov    %edi,-0x90(%ebp)
  800b50:	89 fb                	mov    %edi,%ebx
  800b52:	eb 27                	jmp    800b7b <umain+0xcb>
  800b54:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  800b58:	83 ec 04             	sub    $0x4,%esp
  800b5b:	6a 07                	push   $0x7
  800b5d:	53                   	push   %ebx
  800b5e:	6a 00                	push   $0x0
  800b60:	e8 99 0c 00 00       	call   8017fe <sys_page_alloc>
  800b65:	83 c4 10             	add    $0x10,%esp
  800b68:	85 c0                	test   %eax,%eax
  800b6a:	0f 88 8c 01 00 00    	js     800cfc <umain+0x24c>
  800b70:	a1 00 00 00 10       	mov    0x10000000,%eax
  800b75:	81 c3 00 10 00 00    	add    $0x1000,%ebx
  800b7b:	39 d8                	cmp    %ebx,%eax
  800b7d:	77 d9                	ja     800b58 <umain+0xa8>
  800b7f:	8b 85 74 ff ff ff    	mov    -0x8c(%ebp),%eax
  800b85:	8b 9d 70 ff ff ff    	mov    -0x90(%ebp),%ebx
  800b8b:	b9 15 ec 65 01       	mov    $0x165ec15,%ecx
  800b90:	be cd cc cc cc       	mov    $0xcccccccd,%esi
  800b95:	8d b8 00 d9 48 ff    	lea    -0xb72700(%eax),%edi
  800b9b:	89 1d 00 00 00 10    	mov    %ebx,0x10000000
  800ba1:	c7 80 54 f4 ff ff 40 	movl   $0xf4240,-0xbac(%eax)
  800ba8:	42 0f 00 
  800bab:	c7 80 58 f4 ff ff 40 	movl   $0xf4240,-0xba8(%eax)
  800bb2:	42 0f 00 
  800bb5:	8d 76 00             	lea    0x0(%esi),%esi
  800bb8:	69 c9 6d 4e c6 41    	imul   $0x41c64e6d,%ecx,%ecx
  800bbe:	83 c3 04             	add    $0x4,%ebx
  800bc1:	81 c1 39 30 00 00    	add    $0x3039,%ecx
  800bc7:	89 c8                	mov    %ecx,%eax
  800bc9:	f7 e6                	mul    %esi
  800bcb:	c1 ea 03             	shr    $0x3,%edx
  800bce:	8d 04 92             	lea    (%edx,%edx,4),%eax
  800bd1:	89 ca                	mov    %ecx,%edx
  800bd3:	01 c0                	add    %eax,%eax
  800bd5:	29 c2                	sub    %eax,%edx
  800bd7:	89 53 fc             	mov    %edx,-0x4(%ebx)
  800bda:	39 fb                	cmp    %edi,%ebx
  800bdc:	75 da                	jne    800bb8 <umain+0x108>
  800bde:	8b bd 74 ff ff ff    	mov    -0x8c(%ebp),%edi
  800be4:	be cd cc cc cc       	mov    $0xcccccccd,%esi
  800be9:	89 f8                	mov    %edi,%eax
  800beb:	8d bf 1c e2 85 ff    	lea    -0x7a1de4(%edi),%edi
  800bf1:	2d e4 26 b7 00       	sub    $0xb726e4,%eax
  800bf6:	89 85 6c ff ff ff    	mov    %eax,-0x94(%ebp)
  800bfc:	89 c3                	mov    %eax,%ebx
  800bfe:	66 90                	xchg   %ax,%ax
  800c00:	69 c9 6d 4e c6 41    	imul   $0x41c64e6d,%ecx,%ecx
  800c06:	83 c3 04             	add    $0x4,%ebx
  800c09:	81 c1 39 30 00 00    	add    $0x3039,%ecx
  800c0f:	89 c8                	mov    %ecx,%eax
  800c11:	f7 e6                	mul    %esi
  800c13:	c1 ea 03             	shr    $0x3,%edx
  800c16:	8d 04 92             	lea    (%edx,%edx,4),%eax
  800c19:	89 ca                	mov    %ecx,%edx
  800c1b:	01 c0                	add    %eax,%eax
  800c1d:	29 c2                	sub    %eax,%edx
  800c1f:	89 53 fc             	mov    %edx,-0x4(%ebx)
  800c22:	39 fb                	cmp    %edi,%ebx
  800c24:	75 da                	jne    800c00 <umain+0x150>
  800c26:	8b b5 70 ff ff ff    	mov    -0x90(%ebp),%esi
  800c2c:	83 ec 04             	sub    $0x4,%esp
  800c2f:	bb 00 e0 eb 3b       	mov    $0x3bebe000,%ebx
  800c34:	68 5c 24 f4 00       	push   $0xf4245c
  800c39:	56                   	push   %esi
  800c3a:	68 00 e0 eb 1b       	push   $0x1bebe000
  800c3f:	e8 b1 09 00 00       	call   8015f5 <memcpy>
  800c44:	58                   	pop    %eax
  800c45:	8d 45 84             	lea    -0x7c(%ebp),%eax
  800c48:	5a                   	pop    %edx
  800c49:	50                   	push   %eax
  800c4a:	68 80 0a 80 00       	push   $0x800a80
  800c4f:	e8 fb 0d 00 00       	call   801a4f <sys_enter_judge>
  800c54:	87 e3                	xchg   %esp,%ebx
  800c56:	8b bd 74 ff ff ff    	mov    -0x8c(%ebp),%edi
  800c5c:	59                   	pop    %ecx
  800c5d:	58                   	pop    %eax
  800c5e:	8d 87 38 e2 85 ff    	lea    -0x7a1dc8(%edi),%eax
  800c64:	8b 97 54 f4 ff ff    	mov    -0xbac(%edi),%edx
  800c6a:	8b 8d 6c ff ff ff    	mov    -0x94(%ebp),%ecx
  800c70:	50                   	push   %eax
  800c71:	ff b7 58 f4 ff ff    	pushl  -0xba8(%edi)
  800c77:	89 f0                	mov    %esi,%eax
  800c79:	e8 12 fa ff ff       	call   800690 <std>
  800c7e:	87 e3                	xchg   %esp,%ebx
  800c80:	58                   	pop    %eax
  800c81:	5a                   	pop    %edx
  800c82:	68 02 03 00 00       	push   $0x302
  800c87:	68 1c 35 80 00       	push   $0x80351c
  800c8c:	e8 e7 15 00 00       	call   802278 <open>
  800c91:	8b 97 54 f4 ff ff    	mov    -0xbac(%edi),%edx
  800c97:	89 c3                	mov    %eax,%ebx
  800c99:	03 97 58 f4 ff ff    	add    -0xba8(%edi),%edx
  800c9f:	83 c4 10             	add    $0x10,%esp
  800ca2:	31 c0                	xor    %eax,%eax
  800ca4:	89 f9                	mov    %edi,%ecx
  800ca6:	eb 1b                	jmp    800cc3 <umain+0x213>
  800ca8:	90                   	nop
  800ca9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  800cb0:	8b bc 81 38 e2 85 ff 	mov    -0x7a1dc8(%ecx,%eax,4),%edi
  800cb7:	39 3c 85 38 f2 65 1c 	cmp    %edi,0x1c65f238(,%eax,4)
  800cbe:	75 25                	jne    800ce5 <umain+0x235>
  800cc0:	83 c0 01             	add    $0x1,%eax
  800cc3:	39 d0                	cmp    %edx,%eax
  800cc5:	7c e9                	jl     800cb0 <umain+0x200>
  800cc7:	83 ec 08             	sub    $0x8,%esp
  800cca:	68 3b 35 80 00       	push   $0x80353b
  800ccf:	53                   	push   %ebx
  800cd0:	e8 2a 17 00 00       	call   8023ff <fprintf>
  800cd5:	89 1c 24             	mov    %ebx,(%esp)
  800cd8:	e8 d4 0f 00 00       	call   801cb1 <close>
  800cdd:	83 c4 10             	add    $0x10,%esp
  800ce0:	e9 dd fd ff ff       	jmp    800ac2 <umain+0x12>
  800ce5:	83 ec 04             	sub    $0x4,%esp
  800ce8:	50                   	push   %eax
  800ce9:	68 27 35 80 00       	push   $0x803527
  800cee:	53                   	push   %ebx
  800cef:	e8 0b 17 00 00       	call   8023ff <fprintf>
  800cf4:	83 c4 10             	add    $0x10,%esp
  800cf7:	e9 c6 fd ff ff       	jmp    800ac2 <umain+0x12>
  800cfc:	53                   	push   %ebx
  800cfd:	68 00 35 80 00       	push   $0x803500
  800d02:	6a 20                	push   $0x20
  800d04:	68 12 35 80 00       	push   $0x803512
  800d09:	e8 8f 00 00 00       	call   800d9d <_panic>

00800d0e <libmain>:
  800d0e:	55                   	push   %ebp
  800d0f:	89 e5                	mov    %esp,%ebp
  800d11:	56                   	push   %esi
  800d12:	53                   	push   %ebx
  800d13:	8b 5d 08             	mov    0x8(%ebp),%ebx
  800d16:	8b 75 0c             	mov    0xc(%ebp),%esi
  800d19:	c7 05 00 e0 eb 3b 00 	movl   $0x0,0x3bebe000
  800d20:	00 00 00 
  800d23:	e8 98 0a 00 00       	call   8017c0 <sys_getenvid>
  800d28:	ba 00 00 00 00       	mov    $0x0,%edx
  800d2d:	69 ca 2c 01 00 00    	imul   $0x12c,%edx,%ecx
  800d33:	81 c1 00 00 c0 ee    	add    $0xeec00000,%ecx
  800d39:	8b 49 48             	mov    0x48(%ecx),%ecx
  800d3c:	39 c8                	cmp    %ecx,%eax
  800d3e:	75 14                	jne    800d54 <libmain+0x46>
  800d40:	69 d2 2c 01 00 00    	imul   $0x12c,%edx,%edx
  800d46:	81 c2 00 00 c0 ee    	add    $0xeec00000,%edx
  800d4c:	89 15 00 e0 eb 3b    	mov    %edx,0x3bebe000
  800d52:	eb 0b                	jmp    800d5f <libmain+0x51>
  800d54:	83 c2 01             	add    $0x1,%edx
  800d57:	81 fa 00 04 00 00    	cmp    $0x400,%edx
  800d5d:	75 ce                	jne    800d2d <libmain+0x1f>
  800d5f:	85 db                	test   %ebx,%ebx
  800d61:	7e 07                	jle    800d6a <libmain+0x5c>
  800d63:	8b 06                	mov    (%esi),%eax
  800d65:	a3 04 00 00 10       	mov    %eax,0x10000004
  800d6a:	83 ec 08             	sub    $0x8,%esp
  800d6d:	56                   	push   %esi
  800d6e:	53                   	push   %ebx
  800d6f:	e8 3c fd ff ff       	call   800ab0 <umain>
  800d74:	e8 0a 00 00 00       	call   800d83 <exit>
  800d79:	83 c4 10             	add    $0x10,%esp
  800d7c:	8d 65 f8             	lea    -0x8(%ebp),%esp
  800d7f:	5b                   	pop    %ebx
  800d80:	5e                   	pop    %esi
  800d81:	5d                   	pop    %ebp
  800d82:	c3                   	ret    

00800d83 <exit>:
  800d83:	55                   	push   %ebp
  800d84:	89 e5                	mov    %esp,%ebp
  800d86:	83 ec 08             	sub    $0x8,%esp
  800d89:	e8 4e 0f 00 00       	call   801cdc <close_all>
  800d8e:	83 ec 0c             	sub    $0xc,%esp
  800d91:	6a 00                	push   $0x0
  800d93:	e8 e7 09 00 00       	call   80177f <sys_env_destroy>
  800d98:	83 c4 10             	add    $0x10,%esp
  800d9b:	c9                   	leave  
  800d9c:	c3                   	ret    

00800d9d <_panic>:
  800d9d:	55                   	push   %ebp
  800d9e:	89 e5                	mov    %esp,%ebp
  800da0:	56                   	push   %esi
  800da1:	53                   	push   %ebx
  800da2:	8d 5d 14             	lea    0x14(%ebp),%ebx
  800da5:	8b 35 04 00 00 10    	mov    0x10000004,%esi
  800dab:	e8 10 0a 00 00       	call   8017c0 <sys_getenvid>
  800db0:	83 ec 0c             	sub    $0xc,%esp
  800db3:	ff 75 0c             	pushl  0xc(%ebp)
  800db6:	ff 75 08             	pushl  0x8(%ebp)
  800db9:	56                   	push   %esi
  800dba:	50                   	push   %eax
  800dbb:	68 58 35 80 00       	push   $0x803558
  800dc0:	e8 b1 00 00 00       	call   800e76 <cprintf>
  800dc5:	83 c4 18             	add    $0x18,%esp
  800dc8:	53                   	push   %ebx
  800dc9:	ff 75 10             	pushl  0x10(%ebp)
  800dcc:	e8 54 00 00 00       	call   800e25 <vcprintf>
  800dd1:	c7 04 24 4a 35 80 00 	movl   $0x80354a,(%esp)
  800dd8:	e8 99 00 00 00       	call   800e76 <cprintf>
  800ddd:	83 c4 10             	add    $0x10,%esp
  800de0:	cc                   	int3   
  800de1:	eb fd                	jmp    800de0 <_panic+0x43>

00800de3 <putch>:
  800de3:	55                   	push   %ebp
  800de4:	89 e5                	mov    %esp,%ebp
  800de6:	53                   	push   %ebx
  800de7:	83 ec 04             	sub    $0x4,%esp
  800dea:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  800ded:	8b 13                	mov    (%ebx),%edx
  800def:	8d 42 01             	lea    0x1(%edx),%eax
  800df2:	89 03                	mov    %eax,(%ebx)
  800df4:	8b 4d 08             	mov    0x8(%ebp),%ecx
  800df7:	88 4c 13 08          	mov    %cl,0x8(%ebx,%edx,1)
  800dfb:	3d ff 00 00 00       	cmp    $0xff,%eax
  800e00:	75 1a                	jne    800e1c <putch+0x39>
  800e02:	83 ec 08             	sub    $0x8,%esp
  800e05:	68 ff 00 00 00       	push   $0xff
  800e0a:	8d 43 08             	lea    0x8(%ebx),%eax
  800e0d:	50                   	push   %eax
  800e0e:	e8 2f 09 00 00       	call   801742 <sys_cputs>
  800e13:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  800e19:	83 c4 10             	add    $0x10,%esp
  800e1c:	83 43 04 01          	addl   $0x1,0x4(%ebx)
  800e20:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  800e23:	c9                   	leave  
  800e24:	c3                   	ret    

00800e25 <vcprintf>:
  800e25:	55                   	push   %ebp
  800e26:	89 e5                	mov    %esp,%ebp
  800e28:	81 ec 18 01 00 00    	sub    $0x118,%esp
  800e2e:	c7 85 f0 fe ff ff 00 	movl   $0x0,-0x110(%ebp)
  800e35:	00 00 00 
  800e38:	c7 85 f4 fe ff ff 00 	movl   $0x0,-0x10c(%ebp)
  800e3f:	00 00 00 
  800e42:	ff 75 0c             	pushl  0xc(%ebp)
  800e45:	ff 75 08             	pushl  0x8(%ebp)
  800e48:	8d 85 f0 fe ff ff    	lea    -0x110(%ebp),%eax
  800e4e:	50                   	push   %eax
  800e4f:	68 e3 0d 80 00       	push   $0x800de3
  800e54:	e8 54 01 00 00       	call   800fad <vprintfmt>
  800e59:	83 c4 08             	add    $0x8,%esp
  800e5c:	ff b5 f0 fe ff ff    	pushl  -0x110(%ebp)
  800e62:	8d 85 f8 fe ff ff    	lea    -0x108(%ebp),%eax
  800e68:	50                   	push   %eax
  800e69:	e8 d4 08 00 00       	call   801742 <sys_cputs>
  800e6e:	8b 85 f4 fe ff ff    	mov    -0x10c(%ebp),%eax
  800e74:	c9                   	leave  
  800e75:	c3                   	ret    

00800e76 <cprintf>:
  800e76:	55                   	push   %ebp
  800e77:	89 e5                	mov    %esp,%ebp
  800e79:	83 ec 10             	sub    $0x10,%esp
  800e7c:	8d 45 0c             	lea    0xc(%ebp),%eax
  800e7f:	50                   	push   %eax
  800e80:	ff 75 08             	pushl  0x8(%ebp)
  800e83:	e8 9d ff ff ff       	call   800e25 <vcprintf>
  800e88:	c9                   	leave  
  800e89:	c3                   	ret    

00800e8a <printnum>:
  800e8a:	55                   	push   %ebp
  800e8b:	89 e5                	mov    %esp,%ebp
  800e8d:	57                   	push   %edi
  800e8e:	56                   	push   %esi
  800e8f:	53                   	push   %ebx
  800e90:	83 ec 1c             	sub    $0x1c,%esp
  800e93:	89 c7                	mov    %eax,%edi
  800e95:	89 d6                	mov    %edx,%esi
  800e97:	8b 45 08             	mov    0x8(%ebp),%eax
  800e9a:	8b 55 0c             	mov    0xc(%ebp),%edx
  800e9d:	89 45 d8             	mov    %eax,-0x28(%ebp)
  800ea0:	89 55 dc             	mov    %edx,-0x24(%ebp)
  800ea3:	8b 4d 10             	mov    0x10(%ebp),%ecx
  800ea6:	bb 00 00 00 00       	mov    $0x0,%ebx
  800eab:	89 4d e0             	mov    %ecx,-0x20(%ebp)
  800eae:	89 5d e4             	mov    %ebx,-0x1c(%ebp)
  800eb1:	39 d3                	cmp    %edx,%ebx
  800eb3:	72 05                	jb     800eba <printnum+0x30>
  800eb5:	39 45 10             	cmp    %eax,0x10(%ebp)
  800eb8:	77 45                	ja     800eff <printnum+0x75>
  800eba:	83 ec 0c             	sub    $0xc,%esp
  800ebd:	ff 75 18             	pushl  0x18(%ebp)
  800ec0:	8b 45 14             	mov    0x14(%ebp),%eax
  800ec3:	8d 58 ff             	lea    -0x1(%eax),%ebx
  800ec6:	53                   	push   %ebx
  800ec7:	ff 75 10             	pushl  0x10(%ebp)
  800eca:	83 ec 08             	sub    $0x8,%esp
  800ecd:	ff 75 e4             	pushl  -0x1c(%ebp)
  800ed0:	ff 75 e0             	pushl  -0x20(%ebp)
  800ed3:	ff 75 dc             	pushl  -0x24(%ebp)
  800ed6:	ff 75 d8             	pushl  -0x28(%ebp)
  800ed9:	e8 92 23 00 00       	call   803270 <__udivdi3>
  800ede:	83 c4 18             	add    $0x18,%esp
  800ee1:	52                   	push   %edx
  800ee2:	50                   	push   %eax
  800ee3:	89 f2                	mov    %esi,%edx
  800ee5:	89 f8                	mov    %edi,%eax
  800ee7:	e8 9e ff ff ff       	call   800e8a <printnum>
  800eec:	83 c4 20             	add    $0x20,%esp
  800eef:	eb 18                	jmp    800f09 <printnum+0x7f>
  800ef1:	83 ec 08             	sub    $0x8,%esp
  800ef4:	56                   	push   %esi
  800ef5:	ff 75 18             	pushl  0x18(%ebp)
  800ef8:	ff d7                	call   *%edi
  800efa:	83 c4 10             	add    $0x10,%esp
  800efd:	eb 03                	jmp    800f02 <printnum+0x78>
  800eff:	8b 5d 14             	mov    0x14(%ebp),%ebx
  800f02:	83 eb 01             	sub    $0x1,%ebx
  800f05:	85 db                	test   %ebx,%ebx
  800f07:	7f e8                	jg     800ef1 <printnum+0x67>
  800f09:	83 ec 08             	sub    $0x8,%esp
  800f0c:	56                   	push   %esi
  800f0d:	83 ec 04             	sub    $0x4,%esp
  800f10:	ff 75 e4             	pushl  -0x1c(%ebp)
  800f13:	ff 75 e0             	pushl  -0x20(%ebp)
  800f16:	ff 75 dc             	pushl  -0x24(%ebp)
  800f19:	ff 75 d8             	pushl  -0x28(%ebp)
  800f1c:	e8 7f 24 00 00       	call   8033a0 <__umoddi3>
  800f21:	83 c4 14             	add    $0x14,%esp
  800f24:	0f be 80 7b 35 80 00 	movsbl 0x80357b(%eax),%eax
  800f2b:	50                   	push   %eax
  800f2c:	ff d7                	call   *%edi
  800f2e:	83 c4 10             	add    $0x10,%esp
  800f31:	8d 65 f4             	lea    -0xc(%ebp),%esp
  800f34:	5b                   	pop    %ebx
  800f35:	5e                   	pop    %esi
  800f36:	5f                   	pop    %edi
  800f37:	5d                   	pop    %ebp
  800f38:	c3                   	ret    

00800f39 <getuint>:
  800f39:	55                   	push   %ebp
  800f3a:	89 e5                	mov    %esp,%ebp
  800f3c:	83 fa 01             	cmp    $0x1,%edx
  800f3f:	7e 0e                	jle    800f4f <getuint+0x16>
  800f41:	8b 10                	mov    (%eax),%edx
  800f43:	8d 4a 08             	lea    0x8(%edx),%ecx
  800f46:	89 08                	mov    %ecx,(%eax)
  800f48:	8b 02                	mov    (%edx),%eax
  800f4a:	8b 52 04             	mov    0x4(%edx),%edx
  800f4d:	eb 22                	jmp    800f71 <getuint+0x38>
  800f4f:	85 d2                	test   %edx,%edx
  800f51:	74 10                	je     800f63 <getuint+0x2a>
  800f53:	8b 10                	mov    (%eax),%edx
  800f55:	8d 4a 04             	lea    0x4(%edx),%ecx
  800f58:	89 08                	mov    %ecx,(%eax)
  800f5a:	8b 02                	mov    (%edx),%eax
  800f5c:	ba 00 00 00 00       	mov    $0x0,%edx
  800f61:	eb 0e                	jmp    800f71 <getuint+0x38>
  800f63:	8b 10                	mov    (%eax),%edx
  800f65:	8d 4a 04             	lea    0x4(%edx),%ecx
  800f68:	89 08                	mov    %ecx,(%eax)
  800f6a:	8b 02                	mov    (%edx),%eax
  800f6c:	ba 00 00 00 00       	mov    $0x0,%edx
  800f71:	5d                   	pop    %ebp
  800f72:	c3                   	ret    

00800f73 <sprintputch>:
  800f73:	55                   	push   %ebp
  800f74:	89 e5                	mov    %esp,%ebp
  800f76:	8b 45 0c             	mov    0xc(%ebp),%eax
  800f79:	83 40 08 01          	addl   $0x1,0x8(%eax)
  800f7d:	8b 10                	mov    (%eax),%edx
  800f7f:	3b 50 04             	cmp    0x4(%eax),%edx
  800f82:	73 0a                	jae    800f8e <sprintputch+0x1b>
  800f84:	8d 4a 01             	lea    0x1(%edx),%ecx
  800f87:	89 08                	mov    %ecx,(%eax)
  800f89:	8b 45 08             	mov    0x8(%ebp),%eax
  800f8c:	88 02                	mov    %al,(%edx)
  800f8e:	5d                   	pop    %ebp
  800f8f:	c3                   	ret    

00800f90 <printfmt>:
  800f90:	55                   	push   %ebp
  800f91:	89 e5                	mov    %esp,%ebp
  800f93:	83 ec 08             	sub    $0x8,%esp
  800f96:	8d 45 14             	lea    0x14(%ebp),%eax
  800f99:	50                   	push   %eax
  800f9a:	ff 75 10             	pushl  0x10(%ebp)
  800f9d:	ff 75 0c             	pushl  0xc(%ebp)
  800fa0:	ff 75 08             	pushl  0x8(%ebp)
  800fa3:	e8 05 00 00 00       	call   800fad <vprintfmt>
  800fa8:	83 c4 10             	add    $0x10,%esp
  800fab:	c9                   	leave  
  800fac:	c3                   	ret    

00800fad <vprintfmt>:
  800fad:	55                   	push   %ebp
  800fae:	89 e5                	mov    %esp,%ebp
  800fb0:	57                   	push   %edi
  800fb1:	56                   	push   %esi
  800fb2:	53                   	push   %ebx
  800fb3:	83 ec 2c             	sub    $0x2c,%esp
  800fb6:	8b 75 08             	mov    0x8(%ebp),%esi
  800fb9:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  800fbc:	8b 7d 10             	mov    0x10(%ebp),%edi
  800fbf:	eb 12                	jmp    800fd3 <vprintfmt+0x26>
  800fc1:	85 c0                	test   %eax,%eax
  800fc3:	0f 84 89 03 00 00    	je     801352 <vprintfmt+0x3a5>
  800fc9:	83 ec 08             	sub    $0x8,%esp
  800fcc:	53                   	push   %ebx
  800fcd:	50                   	push   %eax
  800fce:	ff d6                	call   *%esi
  800fd0:	83 c4 10             	add    $0x10,%esp
  800fd3:	83 c7 01             	add    $0x1,%edi
  800fd6:	0f b6 47 ff          	movzbl -0x1(%edi),%eax
  800fda:	83 f8 25             	cmp    $0x25,%eax
  800fdd:	75 e2                	jne    800fc1 <vprintfmt+0x14>
  800fdf:	c6 45 d4 20          	movb   $0x20,-0x2c(%ebp)
  800fe3:	c7 45 d8 00 00 00 00 	movl   $0x0,-0x28(%ebp)
  800fea:	c7 45 d0 ff ff ff ff 	movl   $0xffffffff,-0x30(%ebp)
  800ff1:	c7 45 e0 ff ff ff ff 	movl   $0xffffffff,-0x20(%ebp)
  800ff8:	ba 00 00 00 00       	mov    $0x0,%edx
  800ffd:	eb 07                	jmp    801006 <vprintfmt+0x59>
  800fff:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  801002:	c6 45 d4 2d          	movb   $0x2d,-0x2c(%ebp)
  801006:	8d 47 01             	lea    0x1(%edi),%eax
  801009:	89 45 e4             	mov    %eax,-0x1c(%ebp)
  80100c:	0f b6 07             	movzbl (%edi),%eax
  80100f:	0f b6 c8             	movzbl %al,%ecx
  801012:	83 e8 23             	sub    $0x23,%eax
  801015:	3c 55                	cmp    $0x55,%al
  801017:	0f 87 1a 03 00 00    	ja     801337 <vprintfmt+0x38a>
  80101d:	0f b6 c0             	movzbl %al,%eax
  801020:	ff 24 85 c0 36 80 00 	jmp    *0x8036c0(,%eax,4)
  801027:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  80102a:	c6 45 d4 30          	movb   $0x30,-0x2c(%ebp)
  80102e:	eb d6                	jmp    801006 <vprintfmt+0x59>
  801030:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  801033:	b8 00 00 00 00       	mov    $0x0,%eax
  801038:	89 55 e4             	mov    %edx,-0x1c(%ebp)
  80103b:	8d 04 80             	lea    (%eax,%eax,4),%eax
  80103e:	8d 44 41 d0          	lea    -0x30(%ecx,%eax,2),%eax
  801042:	0f be 0f             	movsbl (%edi),%ecx
  801045:	8d 51 d0             	lea    -0x30(%ecx),%edx
  801048:	83 fa 09             	cmp    $0x9,%edx
  80104b:	77 39                	ja     801086 <vprintfmt+0xd9>
  80104d:	83 c7 01             	add    $0x1,%edi
  801050:	eb e9                	jmp    80103b <vprintfmt+0x8e>
  801052:	8b 45 14             	mov    0x14(%ebp),%eax
  801055:	8d 48 04             	lea    0x4(%eax),%ecx
  801058:	89 4d 14             	mov    %ecx,0x14(%ebp)
  80105b:	8b 00                	mov    (%eax),%eax
  80105d:	89 45 d0             	mov    %eax,-0x30(%ebp)
  801060:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  801063:	eb 27                	jmp    80108c <vprintfmt+0xdf>
  801065:	8b 45 e0             	mov    -0x20(%ebp),%eax
  801068:	85 c0                	test   %eax,%eax
  80106a:	b9 00 00 00 00       	mov    $0x0,%ecx
  80106f:	0f 49 c8             	cmovns %eax,%ecx
  801072:	89 4d e0             	mov    %ecx,-0x20(%ebp)
  801075:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  801078:	eb 8c                	jmp    801006 <vprintfmt+0x59>
  80107a:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  80107d:	c7 45 d8 01 00 00 00 	movl   $0x1,-0x28(%ebp)
  801084:	eb 80                	jmp    801006 <vprintfmt+0x59>
  801086:	8b 55 e4             	mov    -0x1c(%ebp),%edx
  801089:	89 45 d0             	mov    %eax,-0x30(%ebp)
  80108c:	83 7d e0 00          	cmpl   $0x0,-0x20(%ebp)
  801090:	0f 89 70 ff ff ff    	jns    801006 <vprintfmt+0x59>
  801096:	8b 45 d0             	mov    -0x30(%ebp),%eax
  801099:	89 45 e0             	mov    %eax,-0x20(%ebp)
  80109c:	c7 45 d0 ff ff ff ff 	movl   $0xffffffff,-0x30(%ebp)
  8010a3:	e9 5e ff ff ff       	jmp    801006 <vprintfmt+0x59>
  8010a8:	83 c2 01             	add    $0x1,%edx
  8010ab:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  8010ae:	e9 53 ff ff ff       	jmp    801006 <vprintfmt+0x59>
  8010b3:	8b 45 14             	mov    0x14(%ebp),%eax
  8010b6:	8d 50 04             	lea    0x4(%eax),%edx
  8010b9:	89 55 14             	mov    %edx,0x14(%ebp)
  8010bc:	83 ec 08             	sub    $0x8,%esp
  8010bf:	53                   	push   %ebx
  8010c0:	ff 30                	pushl  (%eax)
  8010c2:	ff d6                	call   *%esi
  8010c4:	83 c4 10             	add    $0x10,%esp
  8010c7:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  8010ca:	e9 04 ff ff ff       	jmp    800fd3 <vprintfmt+0x26>
  8010cf:	8b 45 14             	mov    0x14(%ebp),%eax
  8010d2:	8d 50 04             	lea    0x4(%eax),%edx
  8010d5:	89 55 14             	mov    %edx,0x14(%ebp)
  8010d8:	8b 00                	mov    (%eax),%eax
  8010da:	99                   	cltd   
  8010db:	31 d0                	xor    %edx,%eax
  8010dd:	29 d0                	sub    %edx,%eax
  8010df:	83 f8 0f             	cmp    $0xf,%eax
  8010e2:	7f 0b                	jg     8010ef <vprintfmt+0x142>
  8010e4:	8b 14 85 20 38 80 00 	mov    0x803820(,%eax,4),%edx
  8010eb:	85 d2                	test   %edx,%edx
  8010ed:	75 18                	jne    801107 <vprintfmt+0x15a>
  8010ef:	50                   	push   %eax
  8010f0:	68 93 35 80 00       	push   $0x803593
  8010f5:	53                   	push   %ebx
  8010f6:	56                   	push   %esi
  8010f7:	e8 94 fe ff ff       	call   800f90 <printfmt>
  8010fc:	83 c4 10             	add    $0x10,%esp
  8010ff:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  801102:	e9 cc fe ff ff       	jmp    800fd3 <vprintfmt+0x26>
  801107:	52                   	push   %edx
  801108:	68 59 39 80 00       	push   $0x803959
  80110d:	53                   	push   %ebx
  80110e:	56                   	push   %esi
  80110f:	e8 7c fe ff ff       	call   800f90 <printfmt>
  801114:	83 c4 10             	add    $0x10,%esp
  801117:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  80111a:	e9 b4 fe ff ff       	jmp    800fd3 <vprintfmt+0x26>
  80111f:	8b 45 14             	mov    0x14(%ebp),%eax
  801122:	8d 50 04             	lea    0x4(%eax),%edx
  801125:	89 55 14             	mov    %edx,0x14(%ebp)
  801128:	8b 38                	mov    (%eax),%edi
  80112a:	85 ff                	test   %edi,%edi
  80112c:	b8 8c 35 80 00       	mov    $0x80358c,%eax
  801131:	0f 44 f8             	cmove  %eax,%edi
  801134:	83 7d e0 00          	cmpl   $0x0,-0x20(%ebp)
  801138:	0f 8e 94 00 00 00    	jle    8011d2 <vprintfmt+0x225>
  80113e:	80 7d d4 2d          	cmpb   $0x2d,-0x2c(%ebp)
  801142:	0f 84 98 00 00 00    	je     8011e0 <vprintfmt+0x233>
  801148:	83 ec 08             	sub    $0x8,%esp
  80114b:	ff 75 d0             	pushl  -0x30(%ebp)
  80114e:	57                   	push   %edi
  80114f:	e8 86 02 00 00       	call   8013da <strnlen>
  801154:	8b 4d e0             	mov    -0x20(%ebp),%ecx
  801157:	29 c1                	sub    %eax,%ecx
  801159:	89 4d cc             	mov    %ecx,-0x34(%ebp)
  80115c:	83 c4 10             	add    $0x10,%esp
  80115f:	0f be 45 d4          	movsbl -0x2c(%ebp),%eax
  801163:	89 45 e0             	mov    %eax,-0x20(%ebp)
  801166:	89 7d d4             	mov    %edi,-0x2c(%ebp)
  801169:	89 cf                	mov    %ecx,%edi
  80116b:	eb 0f                	jmp    80117c <vprintfmt+0x1cf>
  80116d:	83 ec 08             	sub    $0x8,%esp
  801170:	53                   	push   %ebx
  801171:	ff 75 e0             	pushl  -0x20(%ebp)
  801174:	ff d6                	call   *%esi
  801176:	83 ef 01             	sub    $0x1,%edi
  801179:	83 c4 10             	add    $0x10,%esp
  80117c:	85 ff                	test   %edi,%edi
  80117e:	7f ed                	jg     80116d <vprintfmt+0x1c0>
  801180:	8b 7d d4             	mov    -0x2c(%ebp),%edi
  801183:	8b 4d cc             	mov    -0x34(%ebp),%ecx
  801186:	85 c9                	test   %ecx,%ecx
  801188:	b8 00 00 00 00       	mov    $0x0,%eax
  80118d:	0f 49 c1             	cmovns %ecx,%eax
  801190:	29 c1                	sub    %eax,%ecx
  801192:	89 75 08             	mov    %esi,0x8(%ebp)
  801195:	8b 75 d0             	mov    -0x30(%ebp),%esi
  801198:	89 5d 0c             	mov    %ebx,0xc(%ebp)
  80119b:	89 cb                	mov    %ecx,%ebx
  80119d:	eb 4d                	jmp    8011ec <vprintfmt+0x23f>
  80119f:	83 7d d8 00          	cmpl   $0x0,-0x28(%ebp)
  8011a3:	74 1b                	je     8011c0 <vprintfmt+0x213>
  8011a5:	0f be c0             	movsbl %al,%eax
  8011a8:	83 e8 20             	sub    $0x20,%eax
  8011ab:	83 f8 5e             	cmp    $0x5e,%eax
  8011ae:	76 10                	jbe    8011c0 <vprintfmt+0x213>
  8011b0:	83 ec 08             	sub    $0x8,%esp
  8011b3:	ff 75 0c             	pushl  0xc(%ebp)
  8011b6:	6a 3f                	push   $0x3f
  8011b8:	ff 55 08             	call   *0x8(%ebp)
  8011bb:	83 c4 10             	add    $0x10,%esp
  8011be:	eb 0d                	jmp    8011cd <vprintfmt+0x220>
  8011c0:	83 ec 08             	sub    $0x8,%esp
  8011c3:	ff 75 0c             	pushl  0xc(%ebp)
  8011c6:	52                   	push   %edx
  8011c7:	ff 55 08             	call   *0x8(%ebp)
  8011ca:	83 c4 10             	add    $0x10,%esp
  8011cd:	83 eb 01             	sub    $0x1,%ebx
  8011d0:	eb 1a                	jmp    8011ec <vprintfmt+0x23f>
  8011d2:	89 75 08             	mov    %esi,0x8(%ebp)
  8011d5:	8b 75 d0             	mov    -0x30(%ebp),%esi
  8011d8:	89 5d 0c             	mov    %ebx,0xc(%ebp)
  8011db:	8b 5d e0             	mov    -0x20(%ebp),%ebx
  8011de:	eb 0c                	jmp    8011ec <vprintfmt+0x23f>
  8011e0:	89 75 08             	mov    %esi,0x8(%ebp)
  8011e3:	8b 75 d0             	mov    -0x30(%ebp),%esi
  8011e6:	89 5d 0c             	mov    %ebx,0xc(%ebp)
  8011e9:	8b 5d e0             	mov    -0x20(%ebp),%ebx
  8011ec:	83 c7 01             	add    $0x1,%edi
  8011ef:	0f b6 47 ff          	movzbl -0x1(%edi),%eax
  8011f3:	0f be d0             	movsbl %al,%edx
  8011f6:	85 d2                	test   %edx,%edx
  8011f8:	74 23                	je     80121d <vprintfmt+0x270>
  8011fa:	85 f6                	test   %esi,%esi
  8011fc:	78 a1                	js     80119f <vprintfmt+0x1f2>
  8011fe:	83 ee 01             	sub    $0x1,%esi
  801201:	79 9c                	jns    80119f <vprintfmt+0x1f2>
  801203:	89 df                	mov    %ebx,%edi
  801205:	8b 75 08             	mov    0x8(%ebp),%esi
  801208:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  80120b:	eb 18                	jmp    801225 <vprintfmt+0x278>
  80120d:	83 ec 08             	sub    $0x8,%esp
  801210:	53                   	push   %ebx
  801211:	6a 20                	push   $0x20
  801213:	ff d6                	call   *%esi
  801215:	83 ef 01             	sub    $0x1,%edi
  801218:	83 c4 10             	add    $0x10,%esp
  80121b:	eb 08                	jmp    801225 <vprintfmt+0x278>
  80121d:	89 df                	mov    %ebx,%edi
  80121f:	8b 75 08             	mov    0x8(%ebp),%esi
  801222:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  801225:	85 ff                	test   %edi,%edi
  801227:	7f e4                	jg     80120d <vprintfmt+0x260>
  801229:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  80122c:	e9 a2 fd ff ff       	jmp    800fd3 <vprintfmt+0x26>
  801231:	83 fa 01             	cmp    $0x1,%edx
  801234:	7e 16                	jle    80124c <vprintfmt+0x29f>
  801236:	8b 45 14             	mov    0x14(%ebp),%eax
  801239:	8d 50 08             	lea    0x8(%eax),%edx
  80123c:	89 55 14             	mov    %edx,0x14(%ebp)
  80123f:	8b 50 04             	mov    0x4(%eax),%edx
  801242:	8b 00                	mov    (%eax),%eax
  801244:	89 45 d8             	mov    %eax,-0x28(%ebp)
  801247:	89 55 dc             	mov    %edx,-0x24(%ebp)
  80124a:	eb 32                	jmp    80127e <vprintfmt+0x2d1>
  80124c:	85 d2                	test   %edx,%edx
  80124e:	74 18                	je     801268 <vprintfmt+0x2bb>
  801250:	8b 45 14             	mov    0x14(%ebp),%eax
  801253:	8d 50 04             	lea    0x4(%eax),%edx
  801256:	89 55 14             	mov    %edx,0x14(%ebp)
  801259:	8b 00                	mov    (%eax),%eax
  80125b:	89 45 d8             	mov    %eax,-0x28(%ebp)
  80125e:	89 c1                	mov    %eax,%ecx
  801260:	c1 f9 1f             	sar    $0x1f,%ecx
  801263:	89 4d dc             	mov    %ecx,-0x24(%ebp)
  801266:	eb 16                	jmp    80127e <vprintfmt+0x2d1>
  801268:	8b 45 14             	mov    0x14(%ebp),%eax
  80126b:	8d 50 04             	lea    0x4(%eax),%edx
  80126e:	89 55 14             	mov    %edx,0x14(%ebp)
  801271:	8b 00                	mov    (%eax),%eax
  801273:	89 45 d8             	mov    %eax,-0x28(%ebp)
  801276:	89 c1                	mov    %eax,%ecx
  801278:	c1 f9 1f             	sar    $0x1f,%ecx
  80127b:	89 4d dc             	mov    %ecx,-0x24(%ebp)
  80127e:	8b 45 d8             	mov    -0x28(%ebp),%eax
  801281:	8b 55 dc             	mov    -0x24(%ebp),%edx
  801284:	b9 0a 00 00 00       	mov    $0xa,%ecx
  801289:	83 7d dc 00          	cmpl   $0x0,-0x24(%ebp)
  80128d:	79 74                	jns    801303 <vprintfmt+0x356>
  80128f:	83 ec 08             	sub    $0x8,%esp
  801292:	53                   	push   %ebx
  801293:	6a 2d                	push   $0x2d
  801295:	ff d6                	call   *%esi
  801297:	8b 45 d8             	mov    -0x28(%ebp),%eax
  80129a:	8b 55 dc             	mov    -0x24(%ebp),%edx
  80129d:	f7 d8                	neg    %eax
  80129f:	83 d2 00             	adc    $0x0,%edx
  8012a2:	f7 da                	neg    %edx
  8012a4:	83 c4 10             	add    $0x10,%esp
  8012a7:	b9 0a 00 00 00       	mov    $0xa,%ecx
  8012ac:	eb 55                	jmp    801303 <vprintfmt+0x356>
  8012ae:	8d 45 14             	lea    0x14(%ebp),%eax
  8012b1:	e8 83 fc ff ff       	call   800f39 <getuint>
  8012b6:	b9 0a 00 00 00       	mov    $0xa,%ecx
  8012bb:	eb 46                	jmp    801303 <vprintfmt+0x356>
  8012bd:	8d 45 14             	lea    0x14(%ebp),%eax
  8012c0:	e8 74 fc ff ff       	call   800f39 <getuint>
  8012c5:	b9 08 00 00 00       	mov    $0x8,%ecx
  8012ca:	eb 37                	jmp    801303 <vprintfmt+0x356>
  8012cc:	83 ec 08             	sub    $0x8,%esp
  8012cf:	53                   	push   %ebx
  8012d0:	6a 30                	push   $0x30
  8012d2:	ff d6                	call   *%esi
  8012d4:	83 c4 08             	add    $0x8,%esp
  8012d7:	53                   	push   %ebx
  8012d8:	6a 78                	push   $0x78
  8012da:	ff d6                	call   *%esi
  8012dc:	8b 45 14             	mov    0x14(%ebp),%eax
  8012df:	8d 50 04             	lea    0x4(%eax),%edx
  8012e2:	89 55 14             	mov    %edx,0x14(%ebp)
  8012e5:	8b 00                	mov    (%eax),%eax
  8012e7:	ba 00 00 00 00       	mov    $0x0,%edx
  8012ec:	83 c4 10             	add    $0x10,%esp
  8012ef:	b9 10 00 00 00       	mov    $0x10,%ecx
  8012f4:	eb 0d                	jmp    801303 <vprintfmt+0x356>
  8012f6:	8d 45 14             	lea    0x14(%ebp),%eax
  8012f9:	e8 3b fc ff ff       	call   800f39 <getuint>
  8012fe:	b9 10 00 00 00       	mov    $0x10,%ecx
  801303:	83 ec 0c             	sub    $0xc,%esp
  801306:	0f be 7d d4          	movsbl -0x2c(%ebp),%edi
  80130a:	57                   	push   %edi
  80130b:	ff 75 e0             	pushl  -0x20(%ebp)
  80130e:	51                   	push   %ecx
  80130f:	52                   	push   %edx
  801310:	50                   	push   %eax
  801311:	89 da                	mov    %ebx,%edx
  801313:	89 f0                	mov    %esi,%eax
  801315:	e8 70 fb ff ff       	call   800e8a <printnum>
  80131a:	83 c4 20             	add    $0x20,%esp
  80131d:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  801320:	e9 ae fc ff ff       	jmp    800fd3 <vprintfmt+0x26>
  801325:	83 ec 08             	sub    $0x8,%esp
  801328:	53                   	push   %ebx
  801329:	51                   	push   %ecx
  80132a:	ff d6                	call   *%esi
  80132c:	83 c4 10             	add    $0x10,%esp
  80132f:	8b 7d e4             	mov    -0x1c(%ebp),%edi
  801332:	e9 9c fc ff ff       	jmp    800fd3 <vprintfmt+0x26>
  801337:	83 ec 08             	sub    $0x8,%esp
  80133a:	53                   	push   %ebx
  80133b:	6a 25                	push   $0x25
  80133d:	ff d6                	call   *%esi
  80133f:	83 c4 10             	add    $0x10,%esp
  801342:	eb 03                	jmp    801347 <vprintfmt+0x39a>
  801344:	83 ef 01             	sub    $0x1,%edi
  801347:	80 7f ff 25          	cmpb   $0x25,-0x1(%edi)
  80134b:	75 f7                	jne    801344 <vprintfmt+0x397>
  80134d:	e9 81 fc ff ff       	jmp    800fd3 <vprintfmt+0x26>
  801352:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801355:	5b                   	pop    %ebx
  801356:	5e                   	pop    %esi
  801357:	5f                   	pop    %edi
  801358:	5d                   	pop    %ebp
  801359:	c3                   	ret    

0080135a <vsnprintf>:
  80135a:	55                   	push   %ebp
  80135b:	89 e5                	mov    %esp,%ebp
  80135d:	83 ec 18             	sub    $0x18,%esp
  801360:	8b 45 08             	mov    0x8(%ebp),%eax
  801363:	8b 55 0c             	mov    0xc(%ebp),%edx
  801366:	89 45 ec             	mov    %eax,-0x14(%ebp)
  801369:	8d 4c 10 ff          	lea    -0x1(%eax,%edx,1),%ecx
  80136d:	89 4d f0             	mov    %ecx,-0x10(%ebp)
  801370:	c7 45 f4 00 00 00 00 	movl   $0x0,-0xc(%ebp)
  801377:	85 c0                	test   %eax,%eax
  801379:	74 26                	je     8013a1 <vsnprintf+0x47>
  80137b:	85 d2                	test   %edx,%edx
  80137d:	7e 22                	jle    8013a1 <vsnprintf+0x47>
  80137f:	ff 75 14             	pushl  0x14(%ebp)
  801382:	ff 75 10             	pushl  0x10(%ebp)
  801385:	8d 45 ec             	lea    -0x14(%ebp),%eax
  801388:	50                   	push   %eax
  801389:	68 73 0f 80 00       	push   $0x800f73
  80138e:	e8 1a fc ff ff       	call   800fad <vprintfmt>
  801393:	8b 45 ec             	mov    -0x14(%ebp),%eax
  801396:	c6 00 00             	movb   $0x0,(%eax)
  801399:	8b 45 f4             	mov    -0xc(%ebp),%eax
  80139c:	83 c4 10             	add    $0x10,%esp
  80139f:	eb 05                	jmp    8013a6 <vsnprintf+0x4c>
  8013a1:	b8 fd ff ff ff       	mov    $0xfffffffd,%eax
  8013a6:	c9                   	leave  
  8013a7:	c3                   	ret    

008013a8 <snprintf>:
  8013a8:	55                   	push   %ebp
  8013a9:	89 e5                	mov    %esp,%ebp
  8013ab:	83 ec 08             	sub    $0x8,%esp
  8013ae:	8d 45 14             	lea    0x14(%ebp),%eax
  8013b1:	50                   	push   %eax
  8013b2:	ff 75 10             	pushl  0x10(%ebp)
  8013b5:	ff 75 0c             	pushl  0xc(%ebp)
  8013b8:	ff 75 08             	pushl  0x8(%ebp)
  8013bb:	e8 9a ff ff ff       	call   80135a <vsnprintf>
  8013c0:	c9                   	leave  
  8013c1:	c3                   	ret    

008013c2 <strlen>:
  8013c2:	55                   	push   %ebp
  8013c3:	89 e5                	mov    %esp,%ebp
  8013c5:	8b 55 08             	mov    0x8(%ebp),%edx
  8013c8:	b8 00 00 00 00       	mov    $0x0,%eax
  8013cd:	eb 03                	jmp    8013d2 <strlen+0x10>
  8013cf:	83 c0 01             	add    $0x1,%eax
  8013d2:	80 3c 02 00          	cmpb   $0x0,(%edx,%eax,1)
  8013d6:	75 f7                	jne    8013cf <strlen+0xd>
  8013d8:	5d                   	pop    %ebp
  8013d9:	c3                   	ret    

008013da <strnlen>:
  8013da:	55                   	push   %ebp
  8013db:	89 e5                	mov    %esp,%ebp
  8013dd:	8b 4d 08             	mov    0x8(%ebp),%ecx
  8013e0:	8b 45 0c             	mov    0xc(%ebp),%eax
  8013e3:	ba 00 00 00 00       	mov    $0x0,%edx
  8013e8:	eb 03                	jmp    8013ed <strnlen+0x13>
  8013ea:	83 c2 01             	add    $0x1,%edx
  8013ed:	39 c2                	cmp    %eax,%edx
  8013ef:	74 08                	je     8013f9 <strnlen+0x1f>
  8013f1:	80 3c 11 00          	cmpb   $0x0,(%ecx,%edx,1)
  8013f5:	75 f3                	jne    8013ea <strnlen+0x10>
  8013f7:	89 d0                	mov    %edx,%eax
  8013f9:	5d                   	pop    %ebp
  8013fa:	c3                   	ret    

008013fb <strcpy>:
  8013fb:	55                   	push   %ebp
  8013fc:	89 e5                	mov    %esp,%ebp
  8013fe:	53                   	push   %ebx
  8013ff:	8b 45 08             	mov    0x8(%ebp),%eax
  801402:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801405:	89 c2                	mov    %eax,%edx
  801407:	83 c2 01             	add    $0x1,%edx
  80140a:	83 c1 01             	add    $0x1,%ecx
  80140d:	0f b6 59 ff          	movzbl -0x1(%ecx),%ebx
  801411:	88 5a ff             	mov    %bl,-0x1(%edx)
  801414:	84 db                	test   %bl,%bl
  801416:	75 ef                	jne    801407 <strcpy+0xc>
  801418:	5b                   	pop    %ebx
  801419:	5d                   	pop    %ebp
  80141a:	c3                   	ret    

0080141b <strcat>:
  80141b:	55                   	push   %ebp
  80141c:	89 e5                	mov    %esp,%ebp
  80141e:	53                   	push   %ebx
  80141f:	8b 5d 08             	mov    0x8(%ebp),%ebx
  801422:	53                   	push   %ebx
  801423:	e8 9a ff ff ff       	call   8013c2 <strlen>
  801428:	83 c4 04             	add    $0x4,%esp
  80142b:	ff 75 0c             	pushl  0xc(%ebp)
  80142e:	01 d8                	add    %ebx,%eax
  801430:	50                   	push   %eax
  801431:	e8 c5 ff ff ff       	call   8013fb <strcpy>
  801436:	89 d8                	mov    %ebx,%eax
  801438:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  80143b:	c9                   	leave  
  80143c:	c3                   	ret    

0080143d <strncpy>:
  80143d:	55                   	push   %ebp
  80143e:	89 e5                	mov    %esp,%ebp
  801440:	56                   	push   %esi
  801441:	53                   	push   %ebx
  801442:	8b 75 08             	mov    0x8(%ebp),%esi
  801445:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801448:	89 f3                	mov    %esi,%ebx
  80144a:	03 5d 10             	add    0x10(%ebp),%ebx
  80144d:	89 f2                	mov    %esi,%edx
  80144f:	eb 0f                	jmp    801460 <strncpy+0x23>
  801451:	83 c2 01             	add    $0x1,%edx
  801454:	0f b6 01             	movzbl (%ecx),%eax
  801457:	88 42 ff             	mov    %al,-0x1(%edx)
  80145a:	80 39 01             	cmpb   $0x1,(%ecx)
  80145d:	83 d9 ff             	sbb    $0xffffffff,%ecx
  801460:	39 da                	cmp    %ebx,%edx
  801462:	75 ed                	jne    801451 <strncpy+0x14>
  801464:	89 f0                	mov    %esi,%eax
  801466:	5b                   	pop    %ebx
  801467:	5e                   	pop    %esi
  801468:	5d                   	pop    %ebp
  801469:	c3                   	ret    

0080146a <strlcpy>:
  80146a:	55                   	push   %ebp
  80146b:	89 e5                	mov    %esp,%ebp
  80146d:	56                   	push   %esi
  80146e:	53                   	push   %ebx
  80146f:	8b 75 08             	mov    0x8(%ebp),%esi
  801472:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801475:	8b 55 10             	mov    0x10(%ebp),%edx
  801478:	89 f0                	mov    %esi,%eax
  80147a:	85 d2                	test   %edx,%edx
  80147c:	74 21                	je     80149f <strlcpy+0x35>
  80147e:	8d 44 16 ff          	lea    -0x1(%esi,%edx,1),%eax
  801482:	89 f2                	mov    %esi,%edx
  801484:	eb 09                	jmp    80148f <strlcpy+0x25>
  801486:	83 c2 01             	add    $0x1,%edx
  801489:	83 c1 01             	add    $0x1,%ecx
  80148c:	88 5a ff             	mov    %bl,-0x1(%edx)
  80148f:	39 c2                	cmp    %eax,%edx
  801491:	74 09                	je     80149c <strlcpy+0x32>
  801493:	0f b6 19             	movzbl (%ecx),%ebx
  801496:	84 db                	test   %bl,%bl
  801498:	75 ec                	jne    801486 <strlcpy+0x1c>
  80149a:	89 d0                	mov    %edx,%eax
  80149c:	c6 00 00             	movb   $0x0,(%eax)
  80149f:	29 f0                	sub    %esi,%eax
  8014a1:	5b                   	pop    %ebx
  8014a2:	5e                   	pop    %esi
  8014a3:	5d                   	pop    %ebp
  8014a4:	c3                   	ret    

008014a5 <strcmp>:
  8014a5:	55                   	push   %ebp
  8014a6:	89 e5                	mov    %esp,%ebp
  8014a8:	8b 4d 08             	mov    0x8(%ebp),%ecx
  8014ab:	8b 55 0c             	mov    0xc(%ebp),%edx
  8014ae:	eb 06                	jmp    8014b6 <strcmp+0x11>
  8014b0:	83 c1 01             	add    $0x1,%ecx
  8014b3:	83 c2 01             	add    $0x1,%edx
  8014b6:	0f b6 01             	movzbl (%ecx),%eax
  8014b9:	84 c0                	test   %al,%al
  8014bb:	74 04                	je     8014c1 <strcmp+0x1c>
  8014bd:	3a 02                	cmp    (%edx),%al
  8014bf:	74 ef                	je     8014b0 <strcmp+0xb>
  8014c1:	0f b6 c0             	movzbl %al,%eax
  8014c4:	0f b6 12             	movzbl (%edx),%edx
  8014c7:	29 d0                	sub    %edx,%eax
  8014c9:	5d                   	pop    %ebp
  8014ca:	c3                   	ret    

008014cb <strncmp>:
  8014cb:	55                   	push   %ebp
  8014cc:	89 e5                	mov    %esp,%ebp
  8014ce:	53                   	push   %ebx
  8014cf:	8b 45 08             	mov    0x8(%ebp),%eax
  8014d2:	8b 55 0c             	mov    0xc(%ebp),%edx
  8014d5:	89 c3                	mov    %eax,%ebx
  8014d7:	03 5d 10             	add    0x10(%ebp),%ebx
  8014da:	eb 06                	jmp    8014e2 <strncmp+0x17>
  8014dc:	83 c0 01             	add    $0x1,%eax
  8014df:	83 c2 01             	add    $0x1,%edx
  8014e2:	39 d8                	cmp    %ebx,%eax
  8014e4:	74 15                	je     8014fb <strncmp+0x30>
  8014e6:	0f b6 08             	movzbl (%eax),%ecx
  8014e9:	84 c9                	test   %cl,%cl
  8014eb:	74 04                	je     8014f1 <strncmp+0x26>
  8014ed:	3a 0a                	cmp    (%edx),%cl
  8014ef:	74 eb                	je     8014dc <strncmp+0x11>
  8014f1:	0f b6 00             	movzbl (%eax),%eax
  8014f4:	0f b6 12             	movzbl (%edx),%edx
  8014f7:	29 d0                	sub    %edx,%eax
  8014f9:	eb 05                	jmp    801500 <strncmp+0x35>
  8014fb:	b8 00 00 00 00       	mov    $0x0,%eax
  801500:	5b                   	pop    %ebx
  801501:	5d                   	pop    %ebp
  801502:	c3                   	ret    

00801503 <strchr>:
  801503:	55                   	push   %ebp
  801504:	89 e5                	mov    %esp,%ebp
  801506:	8b 45 08             	mov    0x8(%ebp),%eax
  801509:	0f b6 4d 0c          	movzbl 0xc(%ebp),%ecx
  80150d:	eb 07                	jmp    801516 <strchr+0x13>
  80150f:	38 ca                	cmp    %cl,%dl
  801511:	74 0f                	je     801522 <strchr+0x1f>
  801513:	83 c0 01             	add    $0x1,%eax
  801516:	0f b6 10             	movzbl (%eax),%edx
  801519:	84 d2                	test   %dl,%dl
  80151b:	75 f2                	jne    80150f <strchr+0xc>
  80151d:	b8 00 00 00 00       	mov    $0x0,%eax
  801522:	5d                   	pop    %ebp
  801523:	c3                   	ret    

00801524 <strfind>:
  801524:	55                   	push   %ebp
  801525:	89 e5                	mov    %esp,%ebp
  801527:	8b 45 08             	mov    0x8(%ebp),%eax
  80152a:	0f b6 4d 0c          	movzbl 0xc(%ebp),%ecx
  80152e:	eb 03                	jmp    801533 <strfind+0xf>
  801530:	83 c0 01             	add    $0x1,%eax
  801533:	0f b6 10             	movzbl (%eax),%edx
  801536:	38 ca                	cmp    %cl,%dl
  801538:	74 04                	je     80153e <strfind+0x1a>
  80153a:	84 d2                	test   %dl,%dl
  80153c:	75 f2                	jne    801530 <strfind+0xc>
  80153e:	5d                   	pop    %ebp
  80153f:	c3                   	ret    

00801540 <memset>:
  801540:	55                   	push   %ebp
  801541:	89 e5                	mov    %esp,%ebp
  801543:	57                   	push   %edi
  801544:	56                   	push   %esi
  801545:	53                   	push   %ebx
  801546:	8b 7d 08             	mov    0x8(%ebp),%edi
  801549:	8b 4d 10             	mov    0x10(%ebp),%ecx
  80154c:	85 c9                	test   %ecx,%ecx
  80154e:	74 36                	je     801586 <memset+0x46>
  801550:	f7 c7 03 00 00 00    	test   $0x3,%edi
  801556:	75 28                	jne    801580 <memset+0x40>
  801558:	f6 c1 03             	test   $0x3,%cl
  80155b:	75 23                	jne    801580 <memset+0x40>
  80155d:	0f b6 55 0c          	movzbl 0xc(%ebp),%edx
  801561:	89 d3                	mov    %edx,%ebx
  801563:	c1 e3 08             	shl    $0x8,%ebx
  801566:	89 d6                	mov    %edx,%esi
  801568:	c1 e6 18             	shl    $0x18,%esi
  80156b:	89 d0                	mov    %edx,%eax
  80156d:	c1 e0 10             	shl    $0x10,%eax
  801570:	09 f0                	or     %esi,%eax
  801572:	09 c2                	or     %eax,%edx
  801574:	89 d8                	mov    %ebx,%eax
  801576:	09 d0                	or     %edx,%eax
  801578:	c1 e9 02             	shr    $0x2,%ecx
  80157b:	fc                   	cld    
  80157c:	f3 ab                	rep stos %eax,%es:(%edi)
  80157e:	eb 06                	jmp    801586 <memset+0x46>
  801580:	8b 45 0c             	mov    0xc(%ebp),%eax
  801583:	fc                   	cld    
  801584:	f3 aa                	rep stos %al,%es:(%edi)
  801586:	89 f8                	mov    %edi,%eax
  801588:	5b                   	pop    %ebx
  801589:	5e                   	pop    %esi
  80158a:	5f                   	pop    %edi
  80158b:	5d                   	pop    %ebp
  80158c:	c3                   	ret    

0080158d <memmove>:
  80158d:	55                   	push   %ebp
  80158e:	89 e5                	mov    %esp,%ebp
  801590:	57                   	push   %edi
  801591:	56                   	push   %esi
  801592:	8b 45 08             	mov    0x8(%ebp),%eax
  801595:	8b 75 0c             	mov    0xc(%ebp),%esi
  801598:	8b 4d 10             	mov    0x10(%ebp),%ecx
  80159b:	39 c6                	cmp    %eax,%esi
  80159d:	73 35                	jae    8015d4 <memmove+0x47>
  80159f:	8d 14 0e             	lea    (%esi,%ecx,1),%edx
  8015a2:	39 d0                	cmp    %edx,%eax
  8015a4:	73 2e                	jae    8015d4 <memmove+0x47>
  8015a6:	8d 3c 08             	lea    (%eax,%ecx,1),%edi
  8015a9:	89 d6                	mov    %edx,%esi
  8015ab:	09 fe                	or     %edi,%esi
  8015ad:	f7 c6 03 00 00 00    	test   $0x3,%esi
  8015b3:	75 13                	jne    8015c8 <memmove+0x3b>
  8015b5:	f6 c1 03             	test   $0x3,%cl
  8015b8:	75 0e                	jne    8015c8 <memmove+0x3b>
  8015ba:	83 ef 04             	sub    $0x4,%edi
  8015bd:	8d 72 fc             	lea    -0x4(%edx),%esi
  8015c0:	c1 e9 02             	shr    $0x2,%ecx
  8015c3:	fd                   	std    
  8015c4:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
  8015c6:	eb 09                	jmp    8015d1 <memmove+0x44>
  8015c8:	83 ef 01             	sub    $0x1,%edi
  8015cb:	8d 72 ff             	lea    -0x1(%edx),%esi
  8015ce:	fd                   	std    
  8015cf:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
  8015d1:	fc                   	cld    
  8015d2:	eb 1d                	jmp    8015f1 <memmove+0x64>
  8015d4:	89 f2                	mov    %esi,%edx
  8015d6:	09 c2                	or     %eax,%edx
  8015d8:	f6 c2 03             	test   $0x3,%dl
  8015db:	75 0f                	jne    8015ec <memmove+0x5f>
  8015dd:	f6 c1 03             	test   $0x3,%cl
  8015e0:	75 0a                	jne    8015ec <memmove+0x5f>
  8015e2:	c1 e9 02             	shr    $0x2,%ecx
  8015e5:	89 c7                	mov    %eax,%edi
  8015e7:	fc                   	cld    
  8015e8:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
  8015ea:	eb 05                	jmp    8015f1 <memmove+0x64>
  8015ec:	89 c7                	mov    %eax,%edi
  8015ee:	fc                   	cld    
  8015ef:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
  8015f1:	5e                   	pop    %esi
  8015f2:	5f                   	pop    %edi
  8015f3:	5d                   	pop    %ebp
  8015f4:	c3                   	ret    

008015f5 <memcpy>:
  8015f5:	55                   	push   %ebp
  8015f6:	89 e5                	mov    %esp,%ebp
  8015f8:	ff 75 10             	pushl  0x10(%ebp)
  8015fb:	ff 75 0c             	pushl  0xc(%ebp)
  8015fe:	ff 75 08             	pushl  0x8(%ebp)
  801601:	e8 87 ff ff ff       	call   80158d <memmove>
  801606:	c9                   	leave  
  801607:	c3                   	ret    

00801608 <memcmp>:
  801608:	55                   	push   %ebp
  801609:	89 e5                	mov    %esp,%ebp
  80160b:	56                   	push   %esi
  80160c:	53                   	push   %ebx
  80160d:	8b 45 08             	mov    0x8(%ebp),%eax
  801610:	8b 55 0c             	mov    0xc(%ebp),%edx
  801613:	89 c6                	mov    %eax,%esi
  801615:	03 75 10             	add    0x10(%ebp),%esi
  801618:	eb 1a                	jmp    801634 <memcmp+0x2c>
  80161a:	0f b6 08             	movzbl (%eax),%ecx
  80161d:	0f b6 1a             	movzbl (%edx),%ebx
  801620:	38 d9                	cmp    %bl,%cl
  801622:	74 0a                	je     80162e <memcmp+0x26>
  801624:	0f b6 c1             	movzbl %cl,%eax
  801627:	0f b6 db             	movzbl %bl,%ebx
  80162a:	29 d8                	sub    %ebx,%eax
  80162c:	eb 0f                	jmp    80163d <memcmp+0x35>
  80162e:	83 c0 01             	add    $0x1,%eax
  801631:	83 c2 01             	add    $0x1,%edx
  801634:	39 f0                	cmp    %esi,%eax
  801636:	75 e2                	jne    80161a <memcmp+0x12>
  801638:	b8 00 00 00 00       	mov    $0x0,%eax
  80163d:	5b                   	pop    %ebx
  80163e:	5e                   	pop    %esi
  80163f:	5d                   	pop    %ebp
  801640:	c3                   	ret    

00801641 <memfind>:
  801641:	55                   	push   %ebp
  801642:	89 e5                	mov    %esp,%ebp
  801644:	53                   	push   %ebx
  801645:	8b 45 08             	mov    0x8(%ebp),%eax
  801648:	89 c1                	mov    %eax,%ecx
  80164a:	03 4d 10             	add    0x10(%ebp),%ecx
  80164d:	0f b6 5d 0c          	movzbl 0xc(%ebp),%ebx
  801651:	eb 0a                	jmp    80165d <memfind+0x1c>
  801653:	0f b6 10             	movzbl (%eax),%edx
  801656:	39 da                	cmp    %ebx,%edx
  801658:	74 07                	je     801661 <memfind+0x20>
  80165a:	83 c0 01             	add    $0x1,%eax
  80165d:	39 c8                	cmp    %ecx,%eax
  80165f:	72 f2                	jb     801653 <memfind+0x12>
  801661:	5b                   	pop    %ebx
  801662:	5d                   	pop    %ebp
  801663:	c3                   	ret    

00801664 <strtol>:
  801664:	55                   	push   %ebp
  801665:	89 e5                	mov    %esp,%ebp
  801667:	57                   	push   %edi
  801668:	56                   	push   %esi
  801669:	53                   	push   %ebx
  80166a:	8b 4d 08             	mov    0x8(%ebp),%ecx
  80166d:	8b 5d 10             	mov    0x10(%ebp),%ebx
  801670:	eb 03                	jmp    801675 <strtol+0x11>
  801672:	83 c1 01             	add    $0x1,%ecx
  801675:	0f b6 01             	movzbl (%ecx),%eax
  801678:	3c 20                	cmp    $0x20,%al
  80167a:	74 f6                	je     801672 <strtol+0xe>
  80167c:	3c 09                	cmp    $0x9,%al
  80167e:	74 f2                	je     801672 <strtol+0xe>
  801680:	3c 2b                	cmp    $0x2b,%al
  801682:	75 0a                	jne    80168e <strtol+0x2a>
  801684:	83 c1 01             	add    $0x1,%ecx
  801687:	bf 00 00 00 00       	mov    $0x0,%edi
  80168c:	eb 11                	jmp    80169f <strtol+0x3b>
  80168e:	bf 00 00 00 00       	mov    $0x0,%edi
  801693:	3c 2d                	cmp    $0x2d,%al
  801695:	75 08                	jne    80169f <strtol+0x3b>
  801697:	83 c1 01             	add    $0x1,%ecx
  80169a:	bf 01 00 00 00       	mov    $0x1,%edi
  80169f:	f7 c3 ef ff ff ff    	test   $0xffffffef,%ebx
  8016a5:	75 15                	jne    8016bc <strtol+0x58>
  8016a7:	80 39 30             	cmpb   $0x30,(%ecx)
  8016aa:	75 10                	jne    8016bc <strtol+0x58>
  8016ac:	80 79 01 78          	cmpb   $0x78,0x1(%ecx)
  8016b0:	75 7c                	jne    80172e <strtol+0xca>
  8016b2:	83 c1 02             	add    $0x2,%ecx
  8016b5:	bb 10 00 00 00       	mov    $0x10,%ebx
  8016ba:	eb 16                	jmp    8016d2 <strtol+0x6e>
  8016bc:	85 db                	test   %ebx,%ebx
  8016be:	75 12                	jne    8016d2 <strtol+0x6e>
  8016c0:	bb 0a 00 00 00       	mov    $0xa,%ebx
  8016c5:	80 39 30             	cmpb   $0x30,(%ecx)
  8016c8:	75 08                	jne    8016d2 <strtol+0x6e>
  8016ca:	83 c1 01             	add    $0x1,%ecx
  8016cd:	bb 08 00 00 00       	mov    $0x8,%ebx
  8016d2:	b8 00 00 00 00       	mov    $0x0,%eax
  8016d7:	89 5d 10             	mov    %ebx,0x10(%ebp)
  8016da:	0f b6 11             	movzbl (%ecx),%edx
  8016dd:	8d 72 d0             	lea    -0x30(%edx),%esi
  8016e0:	89 f3                	mov    %esi,%ebx
  8016e2:	80 fb 09             	cmp    $0x9,%bl
  8016e5:	77 08                	ja     8016ef <strtol+0x8b>
  8016e7:	0f be d2             	movsbl %dl,%edx
  8016ea:	83 ea 30             	sub    $0x30,%edx
  8016ed:	eb 22                	jmp    801711 <strtol+0xad>
  8016ef:	8d 72 9f             	lea    -0x61(%edx),%esi
  8016f2:	89 f3                	mov    %esi,%ebx
  8016f4:	80 fb 19             	cmp    $0x19,%bl
  8016f7:	77 08                	ja     801701 <strtol+0x9d>
  8016f9:	0f be d2             	movsbl %dl,%edx
  8016fc:	83 ea 57             	sub    $0x57,%edx
  8016ff:	eb 10                	jmp    801711 <strtol+0xad>
  801701:	8d 72 bf             	lea    -0x41(%edx),%esi
  801704:	89 f3                	mov    %esi,%ebx
  801706:	80 fb 19             	cmp    $0x19,%bl
  801709:	77 16                	ja     801721 <strtol+0xbd>
  80170b:	0f be d2             	movsbl %dl,%edx
  80170e:	83 ea 37             	sub    $0x37,%edx
  801711:	3b 55 10             	cmp    0x10(%ebp),%edx
  801714:	7d 0b                	jge    801721 <strtol+0xbd>
  801716:	83 c1 01             	add    $0x1,%ecx
  801719:	0f af 45 10          	imul   0x10(%ebp),%eax
  80171d:	01 d0                	add    %edx,%eax
  80171f:	eb b9                	jmp    8016da <strtol+0x76>
  801721:	83 7d 0c 00          	cmpl   $0x0,0xc(%ebp)
  801725:	74 0d                	je     801734 <strtol+0xd0>
  801727:	8b 75 0c             	mov    0xc(%ebp),%esi
  80172a:	89 0e                	mov    %ecx,(%esi)
  80172c:	eb 06                	jmp    801734 <strtol+0xd0>
  80172e:	85 db                	test   %ebx,%ebx
  801730:	74 98                	je     8016ca <strtol+0x66>
  801732:	eb 9e                	jmp    8016d2 <strtol+0x6e>
  801734:	89 c2                	mov    %eax,%edx
  801736:	f7 da                	neg    %edx
  801738:	85 ff                	test   %edi,%edi
  80173a:	0f 45 c2             	cmovne %edx,%eax
  80173d:	5b                   	pop    %ebx
  80173e:	5e                   	pop    %esi
  80173f:	5f                   	pop    %edi
  801740:	5d                   	pop    %ebp
  801741:	c3                   	ret    

00801742 <sys_cputs>:
  801742:	55                   	push   %ebp
  801743:	89 e5                	mov    %esp,%ebp
  801745:	57                   	push   %edi
  801746:	56                   	push   %esi
  801747:	53                   	push   %ebx
  801748:	b8 00 00 00 00       	mov    $0x0,%eax
  80174d:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801750:	8b 55 08             	mov    0x8(%ebp),%edx
  801753:	89 c3                	mov    %eax,%ebx
  801755:	89 c7                	mov    %eax,%edi
  801757:	89 c6                	mov    %eax,%esi
  801759:	cd 30                	int    $0x30
  80175b:	5b                   	pop    %ebx
  80175c:	5e                   	pop    %esi
  80175d:	5f                   	pop    %edi
  80175e:	5d                   	pop    %ebp
  80175f:	c3                   	ret    

00801760 <sys_cgetc>:
  801760:	55                   	push   %ebp
  801761:	89 e5                	mov    %esp,%ebp
  801763:	57                   	push   %edi
  801764:	56                   	push   %esi
  801765:	53                   	push   %ebx
  801766:	ba 00 00 00 00       	mov    $0x0,%edx
  80176b:	b8 01 00 00 00       	mov    $0x1,%eax
  801770:	89 d1                	mov    %edx,%ecx
  801772:	89 d3                	mov    %edx,%ebx
  801774:	89 d7                	mov    %edx,%edi
  801776:	89 d6                	mov    %edx,%esi
  801778:	cd 30                	int    $0x30
  80177a:	5b                   	pop    %ebx
  80177b:	5e                   	pop    %esi
  80177c:	5f                   	pop    %edi
  80177d:	5d                   	pop    %ebp
  80177e:	c3                   	ret    

0080177f <sys_env_destroy>:
  80177f:	55                   	push   %ebp
  801780:	89 e5                	mov    %esp,%ebp
  801782:	57                   	push   %edi
  801783:	56                   	push   %esi
  801784:	53                   	push   %ebx
  801785:	83 ec 0c             	sub    $0xc,%esp
  801788:	b9 00 00 00 00       	mov    $0x0,%ecx
  80178d:	b8 03 00 00 00       	mov    $0x3,%eax
  801792:	8b 55 08             	mov    0x8(%ebp),%edx
  801795:	89 cb                	mov    %ecx,%ebx
  801797:	89 cf                	mov    %ecx,%edi
  801799:	89 ce                	mov    %ecx,%esi
  80179b:	cd 30                	int    $0x30
  80179d:	85 c0                	test   %eax,%eax
  80179f:	7e 17                	jle    8017b8 <sys_env_destroy+0x39>
  8017a1:	83 ec 0c             	sub    $0xc,%esp
  8017a4:	50                   	push   %eax
  8017a5:	6a 03                	push   $0x3
  8017a7:	68 7f 38 80 00       	push   $0x80387f
  8017ac:	6a 23                	push   $0x23
  8017ae:	68 9c 38 80 00       	push   $0x80389c
  8017b3:	e8 e5 f5 ff ff       	call   800d9d <_panic>
  8017b8:	8d 65 f4             	lea    -0xc(%ebp),%esp
  8017bb:	5b                   	pop    %ebx
  8017bc:	5e                   	pop    %esi
  8017bd:	5f                   	pop    %edi
  8017be:	5d                   	pop    %ebp
  8017bf:	c3                   	ret    

008017c0 <sys_getenvid>:
  8017c0:	55                   	push   %ebp
  8017c1:	89 e5                	mov    %esp,%ebp
  8017c3:	57                   	push   %edi
  8017c4:	56                   	push   %esi
  8017c5:	53                   	push   %ebx
  8017c6:	ba 00 00 00 00       	mov    $0x0,%edx
  8017cb:	b8 02 00 00 00       	mov    $0x2,%eax
  8017d0:	89 d1                	mov    %edx,%ecx
  8017d2:	89 d3                	mov    %edx,%ebx
  8017d4:	89 d7                	mov    %edx,%edi
  8017d6:	89 d6                	mov    %edx,%esi
  8017d8:	cd 30                	int    $0x30
  8017da:	5b                   	pop    %ebx
  8017db:	5e                   	pop    %esi
  8017dc:	5f                   	pop    %edi
  8017dd:	5d                   	pop    %ebp
  8017de:	c3                   	ret    

008017df <sys_yield>:
  8017df:	55                   	push   %ebp
  8017e0:	89 e5                	mov    %esp,%ebp
  8017e2:	57                   	push   %edi
  8017e3:	56                   	push   %esi
  8017e4:	53                   	push   %ebx
  8017e5:	ba 00 00 00 00       	mov    $0x0,%edx
  8017ea:	b8 0b 00 00 00       	mov    $0xb,%eax
  8017ef:	89 d1                	mov    %edx,%ecx
  8017f1:	89 d3                	mov    %edx,%ebx
  8017f3:	89 d7                	mov    %edx,%edi
  8017f5:	89 d6                	mov    %edx,%esi
  8017f7:	cd 30                	int    $0x30
  8017f9:	5b                   	pop    %ebx
  8017fa:	5e                   	pop    %esi
  8017fb:	5f                   	pop    %edi
  8017fc:	5d                   	pop    %ebp
  8017fd:	c3                   	ret    

008017fe <sys_page_alloc>:
  8017fe:	55                   	push   %ebp
  8017ff:	89 e5                	mov    %esp,%ebp
  801801:	57                   	push   %edi
  801802:	56                   	push   %esi
  801803:	53                   	push   %ebx
  801804:	83 ec 0c             	sub    $0xc,%esp
  801807:	be 00 00 00 00       	mov    $0x0,%esi
  80180c:	b8 04 00 00 00       	mov    $0x4,%eax
  801811:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801814:	8b 55 08             	mov    0x8(%ebp),%edx
  801817:	8b 5d 10             	mov    0x10(%ebp),%ebx
  80181a:	89 f7                	mov    %esi,%edi
  80181c:	cd 30                	int    $0x30
  80181e:	85 c0                	test   %eax,%eax
  801820:	7e 17                	jle    801839 <sys_page_alloc+0x3b>
  801822:	83 ec 0c             	sub    $0xc,%esp
  801825:	50                   	push   %eax
  801826:	6a 04                	push   $0x4
  801828:	68 7f 38 80 00       	push   $0x80387f
  80182d:	6a 23                	push   $0x23
  80182f:	68 9c 38 80 00       	push   $0x80389c
  801834:	e8 64 f5 ff ff       	call   800d9d <_panic>
  801839:	8d 65 f4             	lea    -0xc(%ebp),%esp
  80183c:	5b                   	pop    %ebx
  80183d:	5e                   	pop    %esi
  80183e:	5f                   	pop    %edi
  80183f:	5d                   	pop    %ebp
  801840:	c3                   	ret    

00801841 <sys_page_map>:
  801841:	55                   	push   %ebp
  801842:	89 e5                	mov    %esp,%ebp
  801844:	57                   	push   %edi
  801845:	56                   	push   %esi
  801846:	53                   	push   %ebx
  801847:	83 ec 0c             	sub    $0xc,%esp
  80184a:	b8 05 00 00 00       	mov    $0x5,%eax
  80184f:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801852:	8b 55 08             	mov    0x8(%ebp),%edx
  801855:	8b 5d 10             	mov    0x10(%ebp),%ebx
  801858:	8b 7d 14             	mov    0x14(%ebp),%edi
  80185b:	8b 75 18             	mov    0x18(%ebp),%esi
  80185e:	cd 30                	int    $0x30
  801860:	85 c0                	test   %eax,%eax
  801862:	7e 17                	jle    80187b <sys_page_map+0x3a>
  801864:	83 ec 0c             	sub    $0xc,%esp
  801867:	50                   	push   %eax
  801868:	6a 05                	push   $0x5
  80186a:	68 7f 38 80 00       	push   $0x80387f
  80186f:	6a 23                	push   $0x23
  801871:	68 9c 38 80 00       	push   $0x80389c
  801876:	e8 22 f5 ff ff       	call   800d9d <_panic>
  80187b:	8d 65 f4             	lea    -0xc(%ebp),%esp
  80187e:	5b                   	pop    %ebx
  80187f:	5e                   	pop    %esi
  801880:	5f                   	pop    %edi
  801881:	5d                   	pop    %ebp
  801882:	c3                   	ret    

00801883 <sys_page_unmap>:
  801883:	55                   	push   %ebp
  801884:	89 e5                	mov    %esp,%ebp
  801886:	57                   	push   %edi
  801887:	56                   	push   %esi
  801888:	53                   	push   %ebx
  801889:	83 ec 0c             	sub    $0xc,%esp
  80188c:	bb 00 00 00 00       	mov    $0x0,%ebx
  801891:	b8 06 00 00 00       	mov    $0x6,%eax
  801896:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801899:	8b 55 08             	mov    0x8(%ebp),%edx
  80189c:	89 df                	mov    %ebx,%edi
  80189e:	89 de                	mov    %ebx,%esi
  8018a0:	cd 30                	int    $0x30
  8018a2:	85 c0                	test   %eax,%eax
  8018a4:	7e 17                	jle    8018bd <sys_page_unmap+0x3a>
  8018a6:	83 ec 0c             	sub    $0xc,%esp
  8018a9:	50                   	push   %eax
  8018aa:	6a 06                	push   $0x6
  8018ac:	68 7f 38 80 00       	push   $0x80387f
  8018b1:	6a 23                	push   $0x23
  8018b3:	68 9c 38 80 00       	push   $0x80389c
  8018b8:	e8 e0 f4 ff ff       	call   800d9d <_panic>
  8018bd:	8d 65 f4             	lea    -0xc(%ebp),%esp
  8018c0:	5b                   	pop    %ebx
  8018c1:	5e                   	pop    %esi
  8018c2:	5f                   	pop    %edi
  8018c3:	5d                   	pop    %ebp
  8018c4:	c3                   	ret    

008018c5 <sys_env_set_status>:
  8018c5:	55                   	push   %ebp
  8018c6:	89 e5                	mov    %esp,%ebp
  8018c8:	57                   	push   %edi
  8018c9:	56                   	push   %esi
  8018ca:	53                   	push   %ebx
  8018cb:	83 ec 0c             	sub    $0xc,%esp
  8018ce:	bb 00 00 00 00       	mov    $0x0,%ebx
  8018d3:	b8 08 00 00 00       	mov    $0x8,%eax
  8018d8:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  8018db:	8b 55 08             	mov    0x8(%ebp),%edx
  8018de:	89 df                	mov    %ebx,%edi
  8018e0:	89 de                	mov    %ebx,%esi
  8018e2:	cd 30                	int    $0x30
  8018e4:	85 c0                	test   %eax,%eax
  8018e6:	7e 17                	jle    8018ff <sys_env_set_status+0x3a>
  8018e8:	83 ec 0c             	sub    $0xc,%esp
  8018eb:	50                   	push   %eax
  8018ec:	6a 08                	push   $0x8
  8018ee:	68 7f 38 80 00       	push   $0x80387f
  8018f3:	6a 23                	push   $0x23
  8018f5:	68 9c 38 80 00       	push   $0x80389c
  8018fa:	e8 9e f4 ff ff       	call   800d9d <_panic>
  8018ff:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801902:	5b                   	pop    %ebx
  801903:	5e                   	pop    %esi
  801904:	5f                   	pop    %edi
  801905:	5d                   	pop    %ebp
  801906:	c3                   	ret    

00801907 <sys_env_set_trapframe>:
  801907:	55                   	push   %ebp
  801908:	89 e5                	mov    %esp,%ebp
  80190a:	57                   	push   %edi
  80190b:	56                   	push   %esi
  80190c:	53                   	push   %ebx
  80190d:	83 ec 0c             	sub    $0xc,%esp
  801910:	bb 00 00 00 00       	mov    $0x0,%ebx
  801915:	b8 09 00 00 00       	mov    $0x9,%eax
  80191a:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  80191d:	8b 55 08             	mov    0x8(%ebp),%edx
  801920:	89 df                	mov    %ebx,%edi
  801922:	89 de                	mov    %ebx,%esi
  801924:	cd 30                	int    $0x30
  801926:	85 c0                	test   %eax,%eax
  801928:	7e 17                	jle    801941 <sys_env_set_trapframe+0x3a>
  80192a:	83 ec 0c             	sub    $0xc,%esp
  80192d:	50                   	push   %eax
  80192e:	6a 09                	push   $0x9
  801930:	68 7f 38 80 00       	push   $0x80387f
  801935:	6a 23                	push   $0x23
  801937:	68 9c 38 80 00       	push   $0x80389c
  80193c:	e8 5c f4 ff ff       	call   800d9d <_panic>
  801941:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801944:	5b                   	pop    %ebx
  801945:	5e                   	pop    %esi
  801946:	5f                   	pop    %edi
  801947:	5d                   	pop    %ebp
  801948:	c3                   	ret    

00801949 <sys_env_set_pgfault_upcall>:
  801949:	55                   	push   %ebp
  80194a:	89 e5                	mov    %esp,%ebp
  80194c:	57                   	push   %edi
  80194d:	56                   	push   %esi
  80194e:	53                   	push   %ebx
  80194f:	83 ec 0c             	sub    $0xc,%esp
  801952:	bb 00 00 00 00       	mov    $0x0,%ebx
  801957:	b8 0a 00 00 00       	mov    $0xa,%eax
  80195c:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  80195f:	8b 55 08             	mov    0x8(%ebp),%edx
  801962:	89 df                	mov    %ebx,%edi
  801964:	89 de                	mov    %ebx,%esi
  801966:	cd 30                	int    $0x30
  801968:	85 c0                	test   %eax,%eax
  80196a:	7e 17                	jle    801983 <sys_env_set_pgfault_upcall+0x3a>
  80196c:	83 ec 0c             	sub    $0xc,%esp
  80196f:	50                   	push   %eax
  801970:	6a 0a                	push   $0xa
  801972:	68 7f 38 80 00       	push   $0x80387f
  801977:	6a 23                	push   $0x23
  801979:	68 9c 38 80 00       	push   $0x80389c
  80197e:	e8 1a f4 ff ff       	call   800d9d <_panic>
  801983:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801986:	5b                   	pop    %ebx
  801987:	5e                   	pop    %esi
  801988:	5f                   	pop    %edi
  801989:	5d                   	pop    %ebp
  80198a:	c3                   	ret    

0080198b <sys_ipc_try_send>:
  80198b:	55                   	push   %ebp
  80198c:	89 e5                	mov    %esp,%ebp
  80198e:	57                   	push   %edi
  80198f:	56                   	push   %esi
  801990:	53                   	push   %ebx
  801991:	be 00 00 00 00       	mov    $0x0,%esi
  801996:	b8 0c 00 00 00       	mov    $0xc,%eax
  80199b:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  80199e:	8b 55 08             	mov    0x8(%ebp),%edx
  8019a1:	8b 5d 10             	mov    0x10(%ebp),%ebx
  8019a4:	8b 7d 14             	mov    0x14(%ebp),%edi
  8019a7:	cd 30                	int    $0x30
  8019a9:	5b                   	pop    %ebx
  8019aa:	5e                   	pop    %esi
  8019ab:	5f                   	pop    %edi
  8019ac:	5d                   	pop    %ebp
  8019ad:	c3                   	ret    

008019ae <sys_ipc_recv>:
  8019ae:	55                   	push   %ebp
  8019af:	89 e5                	mov    %esp,%ebp
  8019b1:	57                   	push   %edi
  8019b2:	56                   	push   %esi
  8019b3:	53                   	push   %ebx
  8019b4:	83 ec 0c             	sub    $0xc,%esp
  8019b7:	b9 00 00 00 00       	mov    $0x0,%ecx
  8019bc:	b8 0d 00 00 00       	mov    $0xd,%eax
  8019c1:	8b 55 08             	mov    0x8(%ebp),%edx
  8019c4:	89 cb                	mov    %ecx,%ebx
  8019c6:	89 cf                	mov    %ecx,%edi
  8019c8:	89 ce                	mov    %ecx,%esi
  8019ca:	cd 30                	int    $0x30
  8019cc:	85 c0                	test   %eax,%eax
  8019ce:	7e 17                	jle    8019e7 <sys_ipc_recv+0x39>
  8019d0:	83 ec 0c             	sub    $0xc,%esp
  8019d3:	50                   	push   %eax
  8019d4:	6a 0d                	push   $0xd
  8019d6:	68 7f 38 80 00       	push   $0x80387f
  8019db:	6a 23                	push   $0x23
  8019dd:	68 9c 38 80 00       	push   $0x80389c
  8019e2:	e8 b6 f3 ff ff       	call   800d9d <_panic>
  8019e7:	8d 65 f4             	lea    -0xc(%ebp),%esp
  8019ea:	5b                   	pop    %ebx
  8019eb:	5e                   	pop    %esi
  8019ec:	5f                   	pop    %edi
  8019ed:	5d                   	pop    %ebp
  8019ee:	c3                   	ret    

008019ef <sys_time_msec>:
  8019ef:	55                   	push   %ebp
  8019f0:	89 e5                	mov    %esp,%ebp
  8019f2:	57                   	push   %edi
  8019f3:	56                   	push   %esi
  8019f4:	53                   	push   %ebx
  8019f5:	ba 00 00 00 00       	mov    $0x0,%edx
  8019fa:	b8 0e 00 00 00       	mov    $0xe,%eax
  8019ff:	89 d1                	mov    %edx,%ecx
  801a01:	89 d3                	mov    %edx,%ebx
  801a03:	89 d7                	mov    %edx,%edi
  801a05:	89 d6                	mov    %edx,%esi
  801a07:	cd 30                	int    $0x30
  801a09:	5b                   	pop    %ebx
  801a0a:	5e                   	pop    %esi
  801a0b:	5f                   	pop    %edi
  801a0c:	5d                   	pop    %ebp
  801a0d:	c3                   	ret    

00801a0e <sys_net_try_transmit>:
  801a0e:	55                   	push   %ebp
  801a0f:	89 e5                	mov    %esp,%ebp
  801a11:	57                   	push   %edi
  801a12:	56                   	push   %esi
  801a13:	53                   	push   %ebx
  801a14:	bb 00 00 00 00       	mov    $0x0,%ebx
  801a19:	b8 0f 00 00 00       	mov    $0xf,%eax
  801a1e:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801a21:	8b 55 08             	mov    0x8(%ebp),%edx
  801a24:	89 df                	mov    %ebx,%edi
  801a26:	89 de                	mov    %ebx,%esi
  801a28:	cd 30                	int    $0x30
  801a2a:	5b                   	pop    %ebx
  801a2b:	5e                   	pop    %esi
  801a2c:	5f                   	pop    %edi
  801a2d:	5d                   	pop    %ebp
  801a2e:	c3                   	ret    

00801a2f <sys_net_try_receive>:
  801a2f:	55                   	push   %ebp
  801a30:	89 e5                	mov    %esp,%ebp
  801a32:	57                   	push   %edi
  801a33:	56                   	push   %esi
  801a34:	53                   	push   %ebx
  801a35:	b9 00 00 00 00       	mov    $0x0,%ecx
  801a3a:	b8 10 00 00 00       	mov    $0x10,%eax
  801a3f:	8b 55 08             	mov    0x8(%ebp),%edx
  801a42:	89 cb                	mov    %ecx,%ebx
  801a44:	89 cf                	mov    %ecx,%edi
  801a46:	89 ce                	mov    %ecx,%esi
  801a48:	cd 30                	int    $0x30
  801a4a:	5b                   	pop    %ebx
  801a4b:	5e                   	pop    %esi
  801a4c:	5f                   	pop    %edi
  801a4d:	5d                   	pop    %ebp
  801a4e:	c3                   	ret    

00801a4f <sys_enter_judge>:
  801a4f:	55                   	push   %ebp
  801a50:	89 e5                	mov    %esp,%ebp
  801a52:	57                   	push   %edi
  801a53:	56                   	push   %esi
  801a54:	53                   	push   %ebx
  801a55:	83 ec 0c             	sub    $0xc,%esp
  801a58:	bb 00 00 00 00       	mov    $0x0,%ebx
  801a5d:	b8 11 00 00 00       	mov    $0x11,%eax
  801a62:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801a65:	8b 55 08             	mov    0x8(%ebp),%edx
  801a68:	89 df                	mov    %ebx,%edi
  801a6a:	89 de                	mov    %ebx,%esi
  801a6c:	cd 30                	int    $0x30
  801a6e:	85 c0                	test   %eax,%eax
  801a70:	7e 17                	jle    801a89 <sys_enter_judge+0x3a>
  801a72:	83 ec 0c             	sub    $0xc,%esp
  801a75:	50                   	push   %eax
  801a76:	6a 11                	push   $0x11
  801a78:	68 7f 38 80 00       	push   $0x80387f
  801a7d:	6a 23                	push   $0x23
  801a7f:	68 9c 38 80 00       	push   $0x80389c
  801a84:	e8 14 f3 ff ff       	call   800d9d <_panic>
  801a89:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801a8c:	5b                   	pop    %ebx
  801a8d:	5e                   	pop    %esi
  801a8e:	5f                   	pop    %edi
  801a8f:	5d                   	pop    %ebp
  801a90:	c3                   	ret    

00801a91 <sys_accept_enter_judge>:
  801a91:	55                   	push   %ebp
  801a92:	89 e5                	mov    %esp,%ebp
  801a94:	57                   	push   %edi
  801a95:	56                   	push   %esi
  801a96:	53                   	push   %ebx
  801a97:	bb 00 00 00 00       	mov    $0x0,%ebx
  801a9c:	b8 12 00 00 00       	mov    $0x12,%eax
  801aa1:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801aa4:	8b 55 08             	mov    0x8(%ebp),%edx
  801aa7:	89 df                	mov    %ebx,%edi
  801aa9:	89 de                	mov    %ebx,%esi
  801aab:	cd 30                	int    $0x30
  801aad:	5b                   	pop    %ebx
  801aae:	5e                   	pop    %esi
  801aaf:	5f                   	pop    %edi
  801ab0:	5d                   	pop    %ebp
  801ab1:	c3                   	ret    

00801ab2 <sys_quit_judge>:
  801ab2:	55                   	push   %ebp
  801ab3:	89 e5                	mov    %esp,%ebp
  801ab5:	57                   	push   %edi
  801ab6:	56                   	push   %esi
  801ab7:	53                   	push   %ebx
  801ab8:	83 ec 0c             	sub    $0xc,%esp
  801abb:	ba 00 00 00 00       	mov    $0x0,%edx
  801ac0:	b8 13 00 00 00       	mov    $0x13,%eax
  801ac5:	89 d1                	mov    %edx,%ecx
  801ac7:	89 d3                	mov    %edx,%ebx
  801ac9:	89 d7                	mov    %edx,%edi
  801acb:	89 d6                	mov    %edx,%esi
  801acd:	cd 30                	int    $0x30
  801acf:	85 c0                	test   %eax,%eax
  801ad1:	7e 17                	jle    801aea <sys_quit_judge+0x38>
  801ad3:	83 ec 0c             	sub    $0xc,%esp
  801ad6:	50                   	push   %eax
  801ad7:	6a 13                	push   $0x13
  801ad9:	68 7f 38 80 00       	push   $0x80387f
  801ade:	6a 23                	push   $0x23
  801ae0:	68 9c 38 80 00       	push   $0x80389c
  801ae5:	e8 b3 f2 ff ff       	call   800d9d <_panic>
  801aea:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801aed:	5b                   	pop    %ebx
  801aee:	5e                   	pop    %esi
  801aef:	5f                   	pop    %edi
  801af0:	5d                   	pop    %ebp
  801af1:	c3                   	ret    

00801af2 <sys_halt>:
  801af2:	55                   	push   %ebp
  801af3:	89 e5                	mov    %esp,%ebp
  801af5:	57                   	push   %edi
  801af6:	56                   	push   %esi
  801af7:	53                   	push   %ebx
  801af8:	ba 00 00 00 00       	mov    $0x0,%edx
  801afd:	b8 14 00 00 00       	mov    $0x14,%eax
  801b02:	89 d1                	mov    %edx,%ecx
  801b04:	89 d3                	mov    %edx,%ebx
  801b06:	89 d7                	mov    %edx,%edi
  801b08:	89 d6                	mov    %edx,%esi
  801b0a:	cd 30                	int    $0x30
  801b0c:	5b                   	pop    %ebx
  801b0d:	5e                   	pop    %esi
  801b0e:	5f                   	pop    %edi
  801b0f:	5d                   	pop    %ebp
  801b10:	c3                   	ret    

00801b11 <fd2num>:
  801b11:	55                   	push   %ebp
  801b12:	89 e5                	mov    %esp,%ebp
  801b14:	8b 45 08             	mov    0x8(%ebp),%eax
  801b17:	05 00 00 00 30       	add    $0x30000000,%eax
  801b1c:	c1 e8 0c             	shr    $0xc,%eax
  801b1f:	5d                   	pop    %ebp
  801b20:	c3                   	ret    

00801b21 <fd2data>:
  801b21:	55                   	push   %ebp
  801b22:	89 e5                	mov    %esp,%ebp
  801b24:	8b 45 08             	mov    0x8(%ebp),%eax
  801b27:	05 00 00 00 30       	add    $0x30000000,%eax
  801b2c:	25 00 f0 ff ff       	and    $0xfffff000,%eax
  801b31:	2d 00 00 fe 2f       	sub    $0x2ffe0000,%eax
  801b36:	5d                   	pop    %ebp
  801b37:	c3                   	ret    

00801b38 <fd_alloc>:
  801b38:	55                   	push   %ebp
  801b39:	89 e5                	mov    %esp,%ebp
  801b3b:	8b 4d 08             	mov    0x8(%ebp),%ecx
  801b3e:	b8 00 00 00 d0       	mov    $0xd0000000,%eax
  801b43:	89 c2                	mov    %eax,%edx
  801b45:	c1 ea 16             	shr    $0x16,%edx
  801b48:	8b 14 95 00 d0 7b ef 	mov    -0x10843000(,%edx,4),%edx
  801b4f:	f6 c2 01             	test   $0x1,%dl
  801b52:	74 11                	je     801b65 <fd_alloc+0x2d>
  801b54:	89 c2                	mov    %eax,%edx
  801b56:	c1 ea 0c             	shr    $0xc,%edx
  801b59:	8b 14 95 00 00 40 ef 	mov    -0x10c00000(,%edx,4),%edx
  801b60:	f6 c2 01             	test   $0x1,%dl
  801b63:	75 09                	jne    801b6e <fd_alloc+0x36>
  801b65:	89 01                	mov    %eax,(%ecx)
  801b67:	b8 00 00 00 00       	mov    $0x0,%eax
  801b6c:	eb 17                	jmp    801b85 <fd_alloc+0x4d>
  801b6e:	05 00 10 00 00       	add    $0x1000,%eax
  801b73:	3d 00 00 02 d0       	cmp    $0xd0020000,%eax
  801b78:	75 c9                	jne    801b43 <fd_alloc+0xb>
  801b7a:	c7 01 00 00 00 00    	movl   $0x0,(%ecx)
  801b80:	b8 f6 ff ff ff       	mov    $0xfffffff6,%eax
  801b85:	5d                   	pop    %ebp
  801b86:	c3                   	ret    

00801b87 <fd_lookup>:
  801b87:	55                   	push   %ebp
  801b88:	89 e5                	mov    %esp,%ebp
  801b8a:	8b 45 08             	mov    0x8(%ebp),%eax
  801b8d:	83 f8 1f             	cmp    $0x1f,%eax
  801b90:	77 36                	ja     801bc8 <fd_lookup+0x41>
  801b92:	c1 e0 0c             	shl    $0xc,%eax
  801b95:	2d 00 00 00 30       	sub    $0x30000000,%eax
  801b9a:	89 c2                	mov    %eax,%edx
  801b9c:	c1 ea 16             	shr    $0x16,%edx
  801b9f:	8b 14 95 00 d0 7b ef 	mov    -0x10843000(,%edx,4),%edx
  801ba6:	f6 c2 01             	test   $0x1,%dl
  801ba9:	74 24                	je     801bcf <fd_lookup+0x48>
  801bab:	89 c2                	mov    %eax,%edx
  801bad:	c1 ea 0c             	shr    $0xc,%edx
  801bb0:	8b 14 95 00 00 40 ef 	mov    -0x10c00000(,%edx,4),%edx
  801bb7:	f6 c2 01             	test   $0x1,%dl
  801bba:	74 1a                	je     801bd6 <fd_lookup+0x4f>
  801bbc:	8b 55 0c             	mov    0xc(%ebp),%edx
  801bbf:	89 02                	mov    %eax,(%edx)
  801bc1:	b8 00 00 00 00       	mov    $0x0,%eax
  801bc6:	eb 13                	jmp    801bdb <fd_lookup+0x54>
  801bc8:	b8 fd ff ff ff       	mov    $0xfffffffd,%eax
  801bcd:	eb 0c                	jmp    801bdb <fd_lookup+0x54>
  801bcf:	b8 fd ff ff ff       	mov    $0xfffffffd,%eax
  801bd4:	eb 05                	jmp    801bdb <fd_lookup+0x54>
  801bd6:	b8 fd ff ff ff       	mov    $0xfffffffd,%eax
  801bdb:	5d                   	pop    %ebp
  801bdc:	c3                   	ret    

00801bdd <dev_lookup>:
  801bdd:	55                   	push   %ebp
  801bde:	89 e5                	mov    %esp,%ebp
  801be0:	83 ec 08             	sub    $0x8,%esp
  801be3:	8b 4d 08             	mov    0x8(%ebp),%ecx
  801be6:	ba 2c 39 80 00       	mov    $0x80392c,%edx
  801beb:	eb 13                	jmp    801c00 <dev_lookup+0x23>
  801bed:	83 c2 04             	add    $0x4,%edx
  801bf0:	39 08                	cmp    %ecx,(%eax)
  801bf2:	75 0c                	jne    801c00 <dev_lookup+0x23>
  801bf4:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  801bf7:	89 01                	mov    %eax,(%ecx)
  801bf9:	b8 00 00 00 00       	mov    $0x0,%eax
  801bfe:	eb 2e                	jmp    801c2e <dev_lookup+0x51>
  801c00:	8b 02                	mov    (%edx),%eax
  801c02:	85 c0                	test   %eax,%eax
  801c04:	75 e7                	jne    801bed <dev_lookup+0x10>
  801c06:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  801c0b:	8b 40 48             	mov    0x48(%eax),%eax
  801c0e:	83 ec 04             	sub    $0x4,%esp
  801c11:	51                   	push   %ecx
  801c12:	50                   	push   %eax
  801c13:	68 ac 38 80 00       	push   $0x8038ac
  801c18:	e8 59 f2 ff ff       	call   800e76 <cprintf>
  801c1d:	8b 45 0c             	mov    0xc(%ebp),%eax
  801c20:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  801c26:	83 c4 10             	add    $0x10,%esp
  801c29:	b8 fd ff ff ff       	mov    $0xfffffffd,%eax
  801c2e:	c9                   	leave  
  801c2f:	c3                   	ret    

00801c30 <fd_close>:
  801c30:	55                   	push   %ebp
  801c31:	89 e5                	mov    %esp,%ebp
  801c33:	56                   	push   %esi
  801c34:	53                   	push   %ebx
  801c35:	83 ec 10             	sub    $0x10,%esp
  801c38:	8b 75 08             	mov    0x8(%ebp),%esi
  801c3b:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  801c3e:	8d 45 f4             	lea    -0xc(%ebp),%eax
  801c41:	50                   	push   %eax
  801c42:	8d 86 00 00 00 30    	lea    0x30000000(%esi),%eax
  801c48:	c1 e8 0c             	shr    $0xc,%eax
  801c4b:	50                   	push   %eax
  801c4c:	e8 36 ff ff ff       	call   801b87 <fd_lookup>
  801c51:	83 c4 08             	add    $0x8,%esp
  801c54:	85 c0                	test   %eax,%eax
  801c56:	78 05                	js     801c5d <fd_close+0x2d>
  801c58:	3b 75 f4             	cmp    -0xc(%ebp),%esi
  801c5b:	74 0c                	je     801c69 <fd_close+0x39>
  801c5d:	84 db                	test   %bl,%bl
  801c5f:	ba 00 00 00 00       	mov    $0x0,%edx
  801c64:	0f 44 c2             	cmove  %edx,%eax
  801c67:	eb 41                	jmp    801caa <fd_close+0x7a>
  801c69:	83 ec 08             	sub    $0x8,%esp
  801c6c:	8d 45 f0             	lea    -0x10(%ebp),%eax
  801c6f:	50                   	push   %eax
  801c70:	ff 36                	pushl  (%esi)
  801c72:	e8 66 ff ff ff       	call   801bdd <dev_lookup>
  801c77:	89 c3                	mov    %eax,%ebx
  801c79:	83 c4 10             	add    $0x10,%esp
  801c7c:	85 c0                	test   %eax,%eax
  801c7e:	78 1a                	js     801c9a <fd_close+0x6a>
  801c80:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801c83:	8b 40 10             	mov    0x10(%eax),%eax
  801c86:	bb 00 00 00 00       	mov    $0x0,%ebx
  801c8b:	85 c0                	test   %eax,%eax
  801c8d:	74 0b                	je     801c9a <fd_close+0x6a>
  801c8f:	83 ec 0c             	sub    $0xc,%esp
  801c92:	56                   	push   %esi
  801c93:	ff d0                	call   *%eax
  801c95:	89 c3                	mov    %eax,%ebx
  801c97:	83 c4 10             	add    $0x10,%esp
  801c9a:	83 ec 08             	sub    $0x8,%esp
  801c9d:	56                   	push   %esi
  801c9e:	6a 00                	push   $0x0
  801ca0:	e8 de fb ff ff       	call   801883 <sys_page_unmap>
  801ca5:	83 c4 10             	add    $0x10,%esp
  801ca8:	89 d8                	mov    %ebx,%eax
  801caa:	8d 65 f8             	lea    -0x8(%ebp),%esp
  801cad:	5b                   	pop    %ebx
  801cae:	5e                   	pop    %esi
  801caf:	5d                   	pop    %ebp
  801cb0:	c3                   	ret    

00801cb1 <close>:
  801cb1:	55                   	push   %ebp
  801cb2:	89 e5                	mov    %esp,%ebp
  801cb4:	83 ec 18             	sub    $0x18,%esp
  801cb7:	8d 45 f4             	lea    -0xc(%ebp),%eax
  801cba:	50                   	push   %eax
  801cbb:	ff 75 08             	pushl  0x8(%ebp)
  801cbe:	e8 c4 fe ff ff       	call   801b87 <fd_lookup>
  801cc3:	83 c4 08             	add    $0x8,%esp
  801cc6:	85 c0                	test   %eax,%eax
  801cc8:	78 10                	js     801cda <close+0x29>
  801cca:	83 ec 08             	sub    $0x8,%esp
  801ccd:	6a 01                	push   $0x1
  801ccf:	ff 75 f4             	pushl  -0xc(%ebp)
  801cd2:	e8 59 ff ff ff       	call   801c30 <fd_close>
  801cd7:	83 c4 10             	add    $0x10,%esp
  801cda:	c9                   	leave  
  801cdb:	c3                   	ret    

00801cdc <close_all>:
  801cdc:	55                   	push   %ebp
  801cdd:	89 e5                	mov    %esp,%ebp
  801cdf:	53                   	push   %ebx
  801ce0:	83 ec 04             	sub    $0x4,%esp
  801ce3:	bb 00 00 00 00       	mov    $0x0,%ebx
  801ce8:	83 ec 0c             	sub    $0xc,%esp
  801ceb:	53                   	push   %ebx
  801cec:	e8 c0 ff ff ff       	call   801cb1 <close>
  801cf1:	83 c3 01             	add    $0x1,%ebx
  801cf4:	83 c4 10             	add    $0x10,%esp
  801cf7:	83 fb 20             	cmp    $0x20,%ebx
  801cfa:	75 ec                	jne    801ce8 <close_all+0xc>
  801cfc:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  801cff:	c9                   	leave  
  801d00:	c3                   	ret    

00801d01 <dup>:
  801d01:	55                   	push   %ebp
  801d02:	89 e5                	mov    %esp,%ebp
  801d04:	57                   	push   %edi
  801d05:	56                   	push   %esi
  801d06:	53                   	push   %ebx
  801d07:	83 ec 2c             	sub    $0x2c,%esp
  801d0a:	8b 75 0c             	mov    0xc(%ebp),%esi
  801d0d:	8d 45 e4             	lea    -0x1c(%ebp),%eax
  801d10:	50                   	push   %eax
  801d11:	ff 75 08             	pushl  0x8(%ebp)
  801d14:	e8 6e fe ff ff       	call   801b87 <fd_lookup>
  801d19:	83 c4 08             	add    $0x8,%esp
  801d1c:	85 c0                	test   %eax,%eax
  801d1e:	0f 88 c1 00 00 00    	js     801de5 <dup+0xe4>
  801d24:	83 ec 0c             	sub    $0xc,%esp
  801d27:	56                   	push   %esi
  801d28:	e8 84 ff ff ff       	call   801cb1 <close>
  801d2d:	89 f3                	mov    %esi,%ebx
  801d2f:	c1 e3 0c             	shl    $0xc,%ebx
  801d32:	81 eb 00 00 00 30    	sub    $0x30000000,%ebx
  801d38:	83 c4 04             	add    $0x4,%esp
  801d3b:	ff 75 e4             	pushl  -0x1c(%ebp)
  801d3e:	e8 de fd ff ff       	call   801b21 <fd2data>
  801d43:	89 c7                	mov    %eax,%edi
  801d45:	89 1c 24             	mov    %ebx,(%esp)
  801d48:	e8 d4 fd ff ff       	call   801b21 <fd2data>
  801d4d:	83 c4 10             	add    $0x10,%esp
  801d50:	89 45 d4             	mov    %eax,-0x2c(%ebp)
  801d53:	89 f8                	mov    %edi,%eax
  801d55:	c1 e8 16             	shr    $0x16,%eax
  801d58:	8b 04 85 00 d0 7b ef 	mov    -0x10843000(,%eax,4),%eax
  801d5f:	a8 01                	test   $0x1,%al
  801d61:	74 37                	je     801d9a <dup+0x99>
  801d63:	89 f8                	mov    %edi,%eax
  801d65:	c1 e8 0c             	shr    $0xc,%eax
  801d68:	8b 14 85 00 00 40 ef 	mov    -0x10c00000(,%eax,4),%edx
  801d6f:	f6 c2 01             	test   $0x1,%dl
  801d72:	74 26                	je     801d9a <dup+0x99>
  801d74:	8b 04 85 00 00 40 ef 	mov    -0x10c00000(,%eax,4),%eax
  801d7b:	83 ec 0c             	sub    $0xc,%esp
  801d7e:	25 07 0e 00 00       	and    $0xe07,%eax
  801d83:	50                   	push   %eax
  801d84:	ff 75 d4             	pushl  -0x2c(%ebp)
  801d87:	6a 00                	push   $0x0
  801d89:	57                   	push   %edi
  801d8a:	6a 00                	push   $0x0
  801d8c:	e8 b0 fa ff ff       	call   801841 <sys_page_map>
  801d91:	89 c7                	mov    %eax,%edi
  801d93:	83 c4 20             	add    $0x20,%esp
  801d96:	85 c0                	test   %eax,%eax
  801d98:	78 2e                	js     801dc8 <dup+0xc7>
  801d9a:	8b 55 e4             	mov    -0x1c(%ebp),%edx
  801d9d:	89 d0                	mov    %edx,%eax
  801d9f:	c1 e8 0c             	shr    $0xc,%eax
  801da2:	8b 04 85 00 00 40 ef 	mov    -0x10c00000(,%eax,4),%eax
  801da9:	83 ec 0c             	sub    $0xc,%esp
  801dac:	25 07 0e 00 00       	and    $0xe07,%eax
  801db1:	50                   	push   %eax
  801db2:	53                   	push   %ebx
  801db3:	6a 00                	push   $0x0
  801db5:	52                   	push   %edx
  801db6:	6a 00                	push   $0x0
  801db8:	e8 84 fa ff ff       	call   801841 <sys_page_map>
  801dbd:	89 c7                	mov    %eax,%edi
  801dbf:	83 c4 20             	add    $0x20,%esp
  801dc2:	89 f0                	mov    %esi,%eax
  801dc4:	85 ff                	test   %edi,%edi
  801dc6:	79 1d                	jns    801de5 <dup+0xe4>
  801dc8:	83 ec 08             	sub    $0x8,%esp
  801dcb:	53                   	push   %ebx
  801dcc:	6a 00                	push   $0x0
  801dce:	e8 b0 fa ff ff       	call   801883 <sys_page_unmap>
  801dd3:	83 c4 08             	add    $0x8,%esp
  801dd6:	ff 75 d4             	pushl  -0x2c(%ebp)
  801dd9:	6a 00                	push   $0x0
  801ddb:	e8 a3 fa ff ff       	call   801883 <sys_page_unmap>
  801de0:	83 c4 10             	add    $0x10,%esp
  801de3:	89 f8                	mov    %edi,%eax
  801de5:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801de8:	5b                   	pop    %ebx
  801de9:	5e                   	pop    %esi
  801dea:	5f                   	pop    %edi
  801deb:	5d                   	pop    %ebp
  801dec:	c3                   	ret    

00801ded <read>:
  801ded:	55                   	push   %ebp
  801dee:	89 e5                	mov    %esp,%ebp
  801df0:	53                   	push   %ebx
  801df1:	83 ec 14             	sub    $0x14,%esp
  801df4:	8b 5d 08             	mov    0x8(%ebp),%ebx
  801df7:	8d 45 f0             	lea    -0x10(%ebp),%eax
  801dfa:	50                   	push   %eax
  801dfb:	53                   	push   %ebx
  801dfc:	e8 86 fd ff ff       	call   801b87 <fd_lookup>
  801e01:	83 c4 08             	add    $0x8,%esp
  801e04:	89 c2                	mov    %eax,%edx
  801e06:	85 c0                	test   %eax,%eax
  801e08:	78 6d                	js     801e77 <read+0x8a>
  801e0a:	83 ec 08             	sub    $0x8,%esp
  801e0d:	8d 45 f4             	lea    -0xc(%ebp),%eax
  801e10:	50                   	push   %eax
  801e11:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801e14:	ff 30                	pushl  (%eax)
  801e16:	e8 c2 fd ff ff       	call   801bdd <dev_lookup>
  801e1b:	83 c4 10             	add    $0x10,%esp
  801e1e:	85 c0                	test   %eax,%eax
  801e20:	78 4c                	js     801e6e <read+0x81>
  801e22:	8b 55 f0             	mov    -0x10(%ebp),%edx
  801e25:	8b 42 08             	mov    0x8(%edx),%eax
  801e28:	83 e0 03             	and    $0x3,%eax
  801e2b:	83 f8 01             	cmp    $0x1,%eax
  801e2e:	75 21                	jne    801e51 <read+0x64>
  801e30:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  801e35:	8b 40 48             	mov    0x48(%eax),%eax
  801e38:	83 ec 04             	sub    $0x4,%esp
  801e3b:	53                   	push   %ebx
  801e3c:	50                   	push   %eax
  801e3d:	68 f0 38 80 00       	push   $0x8038f0
  801e42:	e8 2f f0 ff ff       	call   800e76 <cprintf>
  801e47:	83 c4 10             	add    $0x10,%esp
  801e4a:	ba fd ff ff ff       	mov    $0xfffffffd,%edx
  801e4f:	eb 26                	jmp    801e77 <read+0x8a>
  801e51:	8b 45 f4             	mov    -0xc(%ebp),%eax
  801e54:	8b 40 08             	mov    0x8(%eax),%eax
  801e57:	85 c0                	test   %eax,%eax
  801e59:	74 17                	je     801e72 <read+0x85>
  801e5b:	83 ec 04             	sub    $0x4,%esp
  801e5e:	ff 75 10             	pushl  0x10(%ebp)
  801e61:	ff 75 0c             	pushl  0xc(%ebp)
  801e64:	52                   	push   %edx
  801e65:	ff d0                	call   *%eax
  801e67:	89 c2                	mov    %eax,%edx
  801e69:	83 c4 10             	add    $0x10,%esp
  801e6c:	eb 09                	jmp    801e77 <read+0x8a>
  801e6e:	89 c2                	mov    %eax,%edx
  801e70:	eb 05                	jmp    801e77 <read+0x8a>
  801e72:	ba f1 ff ff ff       	mov    $0xfffffff1,%edx
  801e77:	89 d0                	mov    %edx,%eax
  801e79:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  801e7c:	c9                   	leave  
  801e7d:	c3                   	ret    

00801e7e <readn>:
  801e7e:	55                   	push   %ebp
  801e7f:	89 e5                	mov    %esp,%ebp
  801e81:	57                   	push   %edi
  801e82:	56                   	push   %esi
  801e83:	53                   	push   %ebx
  801e84:	83 ec 0c             	sub    $0xc,%esp
  801e87:	8b 7d 08             	mov    0x8(%ebp),%edi
  801e8a:	8b 75 10             	mov    0x10(%ebp),%esi
  801e8d:	bb 00 00 00 00       	mov    $0x0,%ebx
  801e92:	eb 21                	jmp    801eb5 <readn+0x37>
  801e94:	83 ec 04             	sub    $0x4,%esp
  801e97:	89 f0                	mov    %esi,%eax
  801e99:	29 d8                	sub    %ebx,%eax
  801e9b:	50                   	push   %eax
  801e9c:	89 d8                	mov    %ebx,%eax
  801e9e:	03 45 0c             	add    0xc(%ebp),%eax
  801ea1:	50                   	push   %eax
  801ea2:	57                   	push   %edi
  801ea3:	e8 45 ff ff ff       	call   801ded <read>
  801ea8:	83 c4 10             	add    $0x10,%esp
  801eab:	85 c0                	test   %eax,%eax
  801ead:	78 10                	js     801ebf <readn+0x41>
  801eaf:	85 c0                	test   %eax,%eax
  801eb1:	74 0a                	je     801ebd <readn+0x3f>
  801eb3:	01 c3                	add    %eax,%ebx
  801eb5:	39 f3                	cmp    %esi,%ebx
  801eb7:	72 db                	jb     801e94 <readn+0x16>
  801eb9:	89 d8                	mov    %ebx,%eax
  801ebb:	eb 02                	jmp    801ebf <readn+0x41>
  801ebd:	89 d8                	mov    %ebx,%eax
  801ebf:	8d 65 f4             	lea    -0xc(%ebp),%esp
  801ec2:	5b                   	pop    %ebx
  801ec3:	5e                   	pop    %esi
  801ec4:	5f                   	pop    %edi
  801ec5:	5d                   	pop    %ebp
  801ec6:	c3                   	ret    

00801ec7 <write>:
  801ec7:	55                   	push   %ebp
  801ec8:	89 e5                	mov    %esp,%ebp
  801eca:	53                   	push   %ebx
  801ecb:	83 ec 14             	sub    $0x14,%esp
  801ece:	8b 5d 08             	mov    0x8(%ebp),%ebx
  801ed1:	8d 45 f0             	lea    -0x10(%ebp),%eax
  801ed4:	50                   	push   %eax
  801ed5:	53                   	push   %ebx
  801ed6:	e8 ac fc ff ff       	call   801b87 <fd_lookup>
  801edb:	83 c4 08             	add    $0x8,%esp
  801ede:	89 c2                	mov    %eax,%edx
  801ee0:	85 c0                	test   %eax,%eax
  801ee2:	78 68                	js     801f4c <write+0x85>
  801ee4:	83 ec 08             	sub    $0x8,%esp
  801ee7:	8d 45 f4             	lea    -0xc(%ebp),%eax
  801eea:	50                   	push   %eax
  801eeb:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801eee:	ff 30                	pushl  (%eax)
  801ef0:	e8 e8 fc ff ff       	call   801bdd <dev_lookup>
  801ef5:	83 c4 10             	add    $0x10,%esp
  801ef8:	85 c0                	test   %eax,%eax
  801efa:	78 47                	js     801f43 <write+0x7c>
  801efc:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801eff:	f6 40 08 03          	testb  $0x3,0x8(%eax)
  801f03:	75 21                	jne    801f26 <write+0x5f>
  801f05:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  801f0a:	8b 40 48             	mov    0x48(%eax),%eax
  801f0d:	83 ec 04             	sub    $0x4,%esp
  801f10:	53                   	push   %ebx
  801f11:	50                   	push   %eax
  801f12:	68 0c 39 80 00       	push   $0x80390c
  801f17:	e8 5a ef ff ff       	call   800e76 <cprintf>
  801f1c:	83 c4 10             	add    $0x10,%esp
  801f1f:	ba fd ff ff ff       	mov    $0xfffffffd,%edx
  801f24:	eb 26                	jmp    801f4c <write+0x85>
  801f26:	8b 55 f4             	mov    -0xc(%ebp),%edx
  801f29:	8b 52 0c             	mov    0xc(%edx),%edx
  801f2c:	85 d2                	test   %edx,%edx
  801f2e:	74 17                	je     801f47 <write+0x80>
  801f30:	83 ec 04             	sub    $0x4,%esp
  801f33:	ff 75 10             	pushl  0x10(%ebp)
  801f36:	ff 75 0c             	pushl  0xc(%ebp)
  801f39:	50                   	push   %eax
  801f3a:	ff d2                	call   *%edx
  801f3c:	89 c2                	mov    %eax,%edx
  801f3e:	83 c4 10             	add    $0x10,%esp
  801f41:	eb 09                	jmp    801f4c <write+0x85>
  801f43:	89 c2                	mov    %eax,%edx
  801f45:	eb 05                	jmp    801f4c <write+0x85>
  801f47:	ba f1 ff ff ff       	mov    $0xfffffff1,%edx
  801f4c:	89 d0                	mov    %edx,%eax
  801f4e:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  801f51:	c9                   	leave  
  801f52:	c3                   	ret    

00801f53 <seek>:
  801f53:	55                   	push   %ebp
  801f54:	89 e5                	mov    %esp,%ebp
  801f56:	83 ec 10             	sub    $0x10,%esp
  801f59:	8d 45 fc             	lea    -0x4(%ebp),%eax
  801f5c:	50                   	push   %eax
  801f5d:	ff 75 08             	pushl  0x8(%ebp)
  801f60:	e8 22 fc ff ff       	call   801b87 <fd_lookup>
  801f65:	83 c4 08             	add    $0x8,%esp
  801f68:	85 c0                	test   %eax,%eax
  801f6a:	78 0e                	js     801f7a <seek+0x27>
  801f6c:	8b 45 fc             	mov    -0x4(%ebp),%eax
  801f6f:	8b 55 0c             	mov    0xc(%ebp),%edx
  801f72:	89 50 04             	mov    %edx,0x4(%eax)
  801f75:	b8 00 00 00 00       	mov    $0x0,%eax
  801f7a:	c9                   	leave  
  801f7b:	c3                   	ret    

00801f7c <ftruncate>:
  801f7c:	55                   	push   %ebp
  801f7d:	89 e5                	mov    %esp,%ebp
  801f7f:	53                   	push   %ebx
  801f80:	83 ec 14             	sub    $0x14,%esp
  801f83:	8b 5d 08             	mov    0x8(%ebp),%ebx
  801f86:	8d 45 f0             	lea    -0x10(%ebp),%eax
  801f89:	50                   	push   %eax
  801f8a:	53                   	push   %ebx
  801f8b:	e8 f7 fb ff ff       	call   801b87 <fd_lookup>
  801f90:	83 c4 08             	add    $0x8,%esp
  801f93:	89 c2                	mov    %eax,%edx
  801f95:	85 c0                	test   %eax,%eax
  801f97:	78 65                	js     801ffe <ftruncate+0x82>
  801f99:	83 ec 08             	sub    $0x8,%esp
  801f9c:	8d 45 f4             	lea    -0xc(%ebp),%eax
  801f9f:	50                   	push   %eax
  801fa0:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801fa3:	ff 30                	pushl  (%eax)
  801fa5:	e8 33 fc ff ff       	call   801bdd <dev_lookup>
  801faa:	83 c4 10             	add    $0x10,%esp
  801fad:	85 c0                	test   %eax,%eax
  801faf:	78 44                	js     801ff5 <ftruncate+0x79>
  801fb1:	8b 45 f0             	mov    -0x10(%ebp),%eax
  801fb4:	f6 40 08 03          	testb  $0x3,0x8(%eax)
  801fb8:	75 21                	jne    801fdb <ftruncate+0x5f>
  801fba:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  801fbf:	8b 40 48             	mov    0x48(%eax),%eax
  801fc2:	83 ec 04             	sub    $0x4,%esp
  801fc5:	53                   	push   %ebx
  801fc6:	50                   	push   %eax
  801fc7:	68 cc 38 80 00       	push   $0x8038cc
  801fcc:	e8 a5 ee ff ff       	call   800e76 <cprintf>
  801fd1:	83 c4 10             	add    $0x10,%esp
  801fd4:	ba fd ff ff ff       	mov    $0xfffffffd,%edx
  801fd9:	eb 23                	jmp    801ffe <ftruncate+0x82>
  801fdb:	8b 55 f4             	mov    -0xc(%ebp),%edx
  801fde:	8b 52 18             	mov    0x18(%edx),%edx
  801fe1:	85 d2                	test   %edx,%edx
  801fe3:	74 14                	je     801ff9 <ftruncate+0x7d>
  801fe5:	83 ec 08             	sub    $0x8,%esp
  801fe8:	ff 75 0c             	pushl  0xc(%ebp)
  801feb:	50                   	push   %eax
  801fec:	ff d2                	call   *%edx
  801fee:	89 c2                	mov    %eax,%edx
  801ff0:	83 c4 10             	add    $0x10,%esp
  801ff3:	eb 09                	jmp    801ffe <ftruncate+0x82>
  801ff5:	89 c2                	mov    %eax,%edx
  801ff7:	eb 05                	jmp    801ffe <ftruncate+0x82>
  801ff9:	ba f1 ff ff ff       	mov    $0xfffffff1,%edx
  801ffe:	89 d0                	mov    %edx,%eax
  802000:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802003:	c9                   	leave  
  802004:	c3                   	ret    

00802005 <fstat>:
  802005:	55                   	push   %ebp
  802006:	89 e5                	mov    %esp,%ebp
  802008:	53                   	push   %ebx
  802009:	83 ec 14             	sub    $0x14,%esp
  80200c:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  80200f:	8d 45 f0             	lea    -0x10(%ebp),%eax
  802012:	50                   	push   %eax
  802013:	ff 75 08             	pushl  0x8(%ebp)
  802016:	e8 6c fb ff ff       	call   801b87 <fd_lookup>
  80201b:	83 c4 08             	add    $0x8,%esp
  80201e:	89 c2                	mov    %eax,%edx
  802020:	85 c0                	test   %eax,%eax
  802022:	78 58                	js     80207c <fstat+0x77>
  802024:	83 ec 08             	sub    $0x8,%esp
  802027:	8d 45 f4             	lea    -0xc(%ebp),%eax
  80202a:	50                   	push   %eax
  80202b:	8b 45 f0             	mov    -0x10(%ebp),%eax
  80202e:	ff 30                	pushl  (%eax)
  802030:	e8 a8 fb ff ff       	call   801bdd <dev_lookup>
  802035:	83 c4 10             	add    $0x10,%esp
  802038:	85 c0                	test   %eax,%eax
  80203a:	78 37                	js     802073 <fstat+0x6e>
  80203c:	8b 45 f4             	mov    -0xc(%ebp),%eax
  80203f:	83 78 14 00          	cmpl   $0x0,0x14(%eax)
  802043:	74 32                	je     802077 <fstat+0x72>
  802045:	c6 03 00             	movb   $0x0,(%ebx)
  802048:	c7 83 80 00 00 00 00 	movl   $0x0,0x80(%ebx)
  80204f:	00 00 00 
  802052:	c7 83 84 00 00 00 00 	movl   $0x0,0x84(%ebx)
  802059:	00 00 00 
  80205c:	89 83 88 00 00 00    	mov    %eax,0x88(%ebx)
  802062:	83 ec 08             	sub    $0x8,%esp
  802065:	53                   	push   %ebx
  802066:	ff 75 f0             	pushl  -0x10(%ebp)
  802069:	ff 50 14             	call   *0x14(%eax)
  80206c:	89 c2                	mov    %eax,%edx
  80206e:	83 c4 10             	add    $0x10,%esp
  802071:	eb 09                	jmp    80207c <fstat+0x77>
  802073:	89 c2                	mov    %eax,%edx
  802075:	eb 05                	jmp    80207c <fstat+0x77>
  802077:	ba f1 ff ff ff       	mov    $0xfffffff1,%edx
  80207c:	89 d0                	mov    %edx,%eax
  80207e:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802081:	c9                   	leave  
  802082:	c3                   	ret    

00802083 <stat>:
  802083:	55                   	push   %ebp
  802084:	89 e5                	mov    %esp,%ebp
  802086:	56                   	push   %esi
  802087:	53                   	push   %ebx
  802088:	83 ec 08             	sub    $0x8,%esp
  80208b:	6a 00                	push   $0x0
  80208d:	ff 75 08             	pushl  0x8(%ebp)
  802090:	e8 e3 01 00 00       	call   802278 <open>
  802095:	89 c3                	mov    %eax,%ebx
  802097:	83 c4 10             	add    $0x10,%esp
  80209a:	85 c0                	test   %eax,%eax
  80209c:	78 1b                	js     8020b9 <stat+0x36>
  80209e:	83 ec 08             	sub    $0x8,%esp
  8020a1:	ff 75 0c             	pushl  0xc(%ebp)
  8020a4:	50                   	push   %eax
  8020a5:	e8 5b ff ff ff       	call   802005 <fstat>
  8020aa:	89 c6                	mov    %eax,%esi
  8020ac:	89 1c 24             	mov    %ebx,(%esp)
  8020af:	e8 fd fb ff ff       	call   801cb1 <close>
  8020b4:	83 c4 10             	add    $0x10,%esp
  8020b7:	89 f0                	mov    %esi,%eax
  8020b9:	8d 65 f8             	lea    -0x8(%ebp),%esp
  8020bc:	5b                   	pop    %ebx
  8020bd:	5e                   	pop    %esi
  8020be:	5d                   	pop    %ebp
  8020bf:	c3                   	ret    

008020c0 <fsipc>:
  8020c0:	55                   	push   %ebp
  8020c1:	89 e5                	mov    %esp,%ebp
  8020c3:	56                   	push   %esi
  8020c4:	53                   	push   %ebx
  8020c5:	89 c6                	mov    %eax,%esi
  8020c7:	89 d3                	mov    %edx,%ebx
  8020c9:	83 3d 34 c4 c4 14 00 	cmpl   $0x0,0x14c4c434
  8020d0:	75 12                	jne    8020e4 <fsipc+0x24>
  8020d2:	83 ec 0c             	sub    $0xc,%esp
  8020d5:	6a 01                	push   $0x1
  8020d7:	e8 47 0f 00 00       	call   803023 <ipc_find_env>
  8020dc:	a3 34 c4 c4 14       	mov    %eax,0x14c4c434
  8020e1:	83 c4 10             	add    $0x10,%esp
  8020e4:	6a 07                	push   $0x7
  8020e6:	68 00 f0 eb 3b       	push   $0x3bebf000
  8020eb:	56                   	push   %esi
  8020ec:	ff 35 34 c4 c4 14    	pushl  0x14c4c434
  8020f2:	e8 dd 0e 00 00       	call   802fd4 <ipc_send>
  8020f7:	83 c4 0c             	add    $0xc,%esp
  8020fa:	6a 00                	push   $0x0
  8020fc:	53                   	push   %ebx
  8020fd:	6a 00                	push   $0x0
  8020ff:	e8 67 0e 00 00       	call   802f6b <ipc_recv>
  802104:	8d 65 f8             	lea    -0x8(%ebp),%esp
  802107:	5b                   	pop    %ebx
  802108:	5e                   	pop    %esi
  802109:	5d                   	pop    %ebp
  80210a:	c3                   	ret    

0080210b <devfile_trunc>:
  80210b:	55                   	push   %ebp
  80210c:	89 e5                	mov    %esp,%ebp
  80210e:	83 ec 08             	sub    $0x8,%esp
  802111:	8b 45 08             	mov    0x8(%ebp),%eax
  802114:	8b 40 0c             	mov    0xc(%eax),%eax
  802117:	a3 00 f0 eb 3b       	mov    %eax,0x3bebf000
  80211c:	8b 45 0c             	mov    0xc(%ebp),%eax
  80211f:	a3 04 f0 eb 3b       	mov    %eax,0x3bebf004
  802124:	ba 00 00 00 00       	mov    $0x0,%edx
  802129:	b8 02 00 00 00       	mov    $0x2,%eax
  80212e:	e8 8d ff ff ff       	call   8020c0 <fsipc>
  802133:	c9                   	leave  
  802134:	c3                   	ret    

00802135 <devfile_flush>:
  802135:	55                   	push   %ebp
  802136:	89 e5                	mov    %esp,%ebp
  802138:	83 ec 08             	sub    $0x8,%esp
  80213b:	8b 45 08             	mov    0x8(%ebp),%eax
  80213e:	8b 40 0c             	mov    0xc(%eax),%eax
  802141:	a3 00 f0 eb 3b       	mov    %eax,0x3bebf000
  802146:	ba 00 00 00 00       	mov    $0x0,%edx
  80214b:	b8 06 00 00 00       	mov    $0x6,%eax
  802150:	e8 6b ff ff ff       	call   8020c0 <fsipc>
  802155:	c9                   	leave  
  802156:	c3                   	ret    

00802157 <devfile_stat>:
  802157:	55                   	push   %ebp
  802158:	89 e5                	mov    %esp,%ebp
  80215a:	53                   	push   %ebx
  80215b:	83 ec 04             	sub    $0x4,%esp
  80215e:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  802161:	8b 45 08             	mov    0x8(%ebp),%eax
  802164:	8b 40 0c             	mov    0xc(%eax),%eax
  802167:	a3 00 f0 eb 3b       	mov    %eax,0x3bebf000
  80216c:	ba 00 00 00 00       	mov    $0x0,%edx
  802171:	b8 05 00 00 00       	mov    $0x5,%eax
  802176:	e8 45 ff ff ff       	call   8020c0 <fsipc>
  80217b:	85 c0                	test   %eax,%eax
  80217d:	78 2c                	js     8021ab <devfile_stat+0x54>
  80217f:	83 ec 08             	sub    $0x8,%esp
  802182:	68 00 f0 eb 3b       	push   $0x3bebf000
  802187:	53                   	push   %ebx
  802188:	e8 6e f2 ff ff       	call   8013fb <strcpy>
  80218d:	a1 80 f0 eb 3b       	mov    0x3bebf080,%eax
  802192:	89 83 80 00 00 00    	mov    %eax,0x80(%ebx)
  802198:	a1 84 f0 eb 3b       	mov    0x3bebf084,%eax
  80219d:	89 83 84 00 00 00    	mov    %eax,0x84(%ebx)
  8021a3:	83 c4 10             	add    $0x10,%esp
  8021a6:	b8 00 00 00 00       	mov    $0x0,%eax
  8021ab:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  8021ae:	c9                   	leave  
  8021af:	c3                   	ret    

008021b0 <devfile_write>:
  8021b0:	55                   	push   %ebp
  8021b1:	89 e5                	mov    %esp,%ebp
  8021b3:	83 ec 0c             	sub    $0xc,%esp
  8021b6:	8b 45 10             	mov    0x10(%ebp),%eax
  8021b9:	8b 55 08             	mov    0x8(%ebp),%edx
  8021bc:	8b 52 0c             	mov    0xc(%edx),%edx
  8021bf:	89 15 00 f0 eb 3b    	mov    %edx,0x3bebf000
  8021c5:	3d f8 0f 00 00       	cmp    $0xff8,%eax
  8021ca:	ba f8 0f 00 00       	mov    $0xff8,%edx
  8021cf:	0f 47 c2             	cmova  %edx,%eax
  8021d2:	a3 04 f0 eb 3b       	mov    %eax,0x3bebf004
  8021d7:	50                   	push   %eax
  8021d8:	ff 75 0c             	pushl  0xc(%ebp)
  8021db:	68 08 f0 eb 3b       	push   $0x3bebf008
  8021e0:	e8 a8 f3 ff ff       	call   80158d <memmove>
  8021e5:	ba 00 00 00 00       	mov    $0x0,%edx
  8021ea:	b8 04 00 00 00       	mov    $0x4,%eax
  8021ef:	e8 cc fe ff ff       	call   8020c0 <fsipc>
  8021f4:	c9                   	leave  
  8021f5:	c3                   	ret    

008021f6 <devfile_read>:
  8021f6:	55                   	push   %ebp
  8021f7:	89 e5                	mov    %esp,%ebp
  8021f9:	56                   	push   %esi
  8021fa:	53                   	push   %ebx
  8021fb:	8b 75 10             	mov    0x10(%ebp),%esi
  8021fe:	8b 45 08             	mov    0x8(%ebp),%eax
  802201:	8b 40 0c             	mov    0xc(%eax),%eax
  802204:	a3 00 f0 eb 3b       	mov    %eax,0x3bebf000
  802209:	89 35 04 f0 eb 3b    	mov    %esi,0x3bebf004
  80220f:	ba 00 00 00 00       	mov    $0x0,%edx
  802214:	b8 03 00 00 00       	mov    $0x3,%eax
  802219:	e8 a2 fe ff ff       	call   8020c0 <fsipc>
  80221e:	89 c3                	mov    %eax,%ebx
  802220:	85 c0                	test   %eax,%eax
  802222:	78 4b                	js     80226f <devfile_read+0x79>
  802224:	39 c6                	cmp    %eax,%esi
  802226:	73 16                	jae    80223e <devfile_read+0x48>
  802228:	68 40 39 80 00       	push   $0x803940
  80222d:	68 47 39 80 00       	push   $0x803947
  802232:	6a 7c                	push   $0x7c
  802234:	68 5c 39 80 00       	push   $0x80395c
  802239:	e8 5f eb ff ff       	call   800d9d <_panic>
  80223e:	3d 00 10 00 00       	cmp    $0x1000,%eax
  802243:	7e 16                	jle    80225b <devfile_read+0x65>
  802245:	68 67 39 80 00       	push   $0x803967
  80224a:	68 47 39 80 00       	push   $0x803947
  80224f:	6a 7d                	push   $0x7d
  802251:	68 5c 39 80 00       	push   $0x80395c
  802256:	e8 42 eb ff ff       	call   800d9d <_panic>
  80225b:	83 ec 04             	sub    $0x4,%esp
  80225e:	50                   	push   %eax
  80225f:	68 00 f0 eb 3b       	push   $0x3bebf000
  802264:	ff 75 0c             	pushl  0xc(%ebp)
  802267:	e8 21 f3 ff ff       	call   80158d <memmove>
  80226c:	83 c4 10             	add    $0x10,%esp
  80226f:	89 d8                	mov    %ebx,%eax
  802271:	8d 65 f8             	lea    -0x8(%ebp),%esp
  802274:	5b                   	pop    %ebx
  802275:	5e                   	pop    %esi
  802276:	5d                   	pop    %ebp
  802277:	c3                   	ret    

00802278 <open>:
  802278:	55                   	push   %ebp
  802279:	89 e5                	mov    %esp,%ebp
  80227b:	53                   	push   %ebx
  80227c:	83 ec 20             	sub    $0x20,%esp
  80227f:	8b 5d 08             	mov    0x8(%ebp),%ebx
  802282:	53                   	push   %ebx
  802283:	e8 3a f1 ff ff       	call   8013c2 <strlen>
  802288:	83 c4 10             	add    $0x10,%esp
  80228b:	3d ff 03 00 00       	cmp    $0x3ff,%eax
  802290:	7f 67                	jg     8022f9 <open+0x81>
  802292:	83 ec 0c             	sub    $0xc,%esp
  802295:	8d 45 f4             	lea    -0xc(%ebp),%eax
  802298:	50                   	push   %eax
  802299:	e8 9a f8 ff ff       	call   801b38 <fd_alloc>
  80229e:	83 c4 10             	add    $0x10,%esp
  8022a1:	89 c2                	mov    %eax,%edx
  8022a3:	85 c0                	test   %eax,%eax
  8022a5:	78 57                	js     8022fe <open+0x86>
  8022a7:	83 ec 08             	sub    $0x8,%esp
  8022aa:	53                   	push   %ebx
  8022ab:	68 00 f0 eb 3b       	push   $0x3bebf000
  8022b0:	e8 46 f1 ff ff       	call   8013fb <strcpy>
  8022b5:	8b 45 0c             	mov    0xc(%ebp),%eax
  8022b8:	a3 00 f4 eb 3b       	mov    %eax,0x3bebf400
  8022bd:	8b 55 f4             	mov    -0xc(%ebp),%edx
  8022c0:	b8 01 00 00 00       	mov    $0x1,%eax
  8022c5:	e8 f6 fd ff ff       	call   8020c0 <fsipc>
  8022ca:	89 c3                	mov    %eax,%ebx
  8022cc:	83 c4 10             	add    $0x10,%esp
  8022cf:	85 c0                	test   %eax,%eax
  8022d1:	79 14                	jns    8022e7 <open+0x6f>
  8022d3:	83 ec 08             	sub    $0x8,%esp
  8022d6:	6a 00                	push   $0x0
  8022d8:	ff 75 f4             	pushl  -0xc(%ebp)
  8022db:	e8 50 f9 ff ff       	call   801c30 <fd_close>
  8022e0:	83 c4 10             	add    $0x10,%esp
  8022e3:	89 da                	mov    %ebx,%edx
  8022e5:	eb 17                	jmp    8022fe <open+0x86>
  8022e7:	83 ec 0c             	sub    $0xc,%esp
  8022ea:	ff 75 f4             	pushl  -0xc(%ebp)
  8022ed:	e8 1f f8 ff ff       	call   801b11 <fd2num>
  8022f2:	89 c2                	mov    %eax,%edx
  8022f4:	83 c4 10             	add    $0x10,%esp
  8022f7:	eb 05                	jmp    8022fe <open+0x86>
  8022f9:	ba f4 ff ff ff       	mov    $0xfffffff4,%edx
  8022fe:	89 d0                	mov    %edx,%eax
  802300:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802303:	c9                   	leave  
  802304:	c3                   	ret    

00802305 <sync>:
  802305:	55                   	push   %ebp
  802306:	89 e5                	mov    %esp,%ebp
  802308:	83 ec 08             	sub    $0x8,%esp
  80230b:	ba 00 00 00 00       	mov    $0x0,%edx
  802310:	b8 08 00 00 00       	mov    $0x8,%eax
  802315:	e8 a6 fd ff ff       	call   8020c0 <fsipc>
  80231a:	c9                   	leave  
  80231b:	c3                   	ret    

0080231c <writebuf>:
  80231c:	83 78 0c 00          	cmpl   $0x0,0xc(%eax)
  802320:	7e 37                	jle    802359 <writebuf+0x3d>
  802322:	55                   	push   %ebp
  802323:	89 e5                	mov    %esp,%ebp
  802325:	53                   	push   %ebx
  802326:	83 ec 08             	sub    $0x8,%esp
  802329:	89 c3                	mov    %eax,%ebx
  80232b:	ff 70 04             	pushl  0x4(%eax)
  80232e:	8d 40 10             	lea    0x10(%eax),%eax
  802331:	50                   	push   %eax
  802332:	ff 33                	pushl  (%ebx)
  802334:	e8 8e fb ff ff       	call   801ec7 <write>
  802339:	83 c4 10             	add    $0x10,%esp
  80233c:	85 c0                	test   %eax,%eax
  80233e:	7e 03                	jle    802343 <writebuf+0x27>
  802340:	01 43 08             	add    %eax,0x8(%ebx)
  802343:	3b 43 04             	cmp    0x4(%ebx),%eax
  802346:	74 0d                	je     802355 <writebuf+0x39>
  802348:	85 c0                	test   %eax,%eax
  80234a:	ba 00 00 00 00       	mov    $0x0,%edx
  80234f:	0f 4f c2             	cmovg  %edx,%eax
  802352:	89 43 0c             	mov    %eax,0xc(%ebx)
  802355:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802358:	c9                   	leave  
  802359:	f3 c3                	repz ret 

0080235b <putch>:
  80235b:	55                   	push   %ebp
  80235c:	89 e5                	mov    %esp,%ebp
  80235e:	53                   	push   %ebx
  80235f:	83 ec 04             	sub    $0x4,%esp
  802362:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  802365:	8b 53 04             	mov    0x4(%ebx),%edx
  802368:	8d 42 01             	lea    0x1(%edx),%eax
  80236b:	89 43 04             	mov    %eax,0x4(%ebx)
  80236e:	8b 4d 08             	mov    0x8(%ebp),%ecx
  802371:	88 4c 13 10          	mov    %cl,0x10(%ebx,%edx,1)
  802375:	3d 00 01 00 00       	cmp    $0x100,%eax
  80237a:	75 0e                	jne    80238a <putch+0x2f>
  80237c:	89 d8                	mov    %ebx,%eax
  80237e:	e8 99 ff ff ff       	call   80231c <writebuf>
  802383:	c7 43 04 00 00 00 00 	movl   $0x0,0x4(%ebx)
  80238a:	83 c4 04             	add    $0x4,%esp
  80238d:	5b                   	pop    %ebx
  80238e:	5d                   	pop    %ebp
  80238f:	c3                   	ret    

00802390 <vfprintf>:
  802390:	55                   	push   %ebp
  802391:	89 e5                	mov    %esp,%ebp
  802393:	81 ec 18 01 00 00    	sub    $0x118,%esp
  802399:	8b 45 08             	mov    0x8(%ebp),%eax
  80239c:	89 85 e8 fe ff ff    	mov    %eax,-0x118(%ebp)
  8023a2:	c7 85 ec fe ff ff 00 	movl   $0x0,-0x114(%ebp)
  8023a9:	00 00 00 
  8023ac:	c7 85 f0 fe ff ff 00 	movl   $0x0,-0x110(%ebp)
  8023b3:	00 00 00 
  8023b6:	c7 85 f4 fe ff ff 01 	movl   $0x1,-0x10c(%ebp)
  8023bd:	00 00 00 
  8023c0:	ff 75 10             	pushl  0x10(%ebp)
  8023c3:	ff 75 0c             	pushl  0xc(%ebp)
  8023c6:	8d 85 e8 fe ff ff    	lea    -0x118(%ebp),%eax
  8023cc:	50                   	push   %eax
  8023cd:	68 5b 23 80 00       	push   $0x80235b
  8023d2:	e8 d6 eb ff ff       	call   800fad <vprintfmt>
  8023d7:	83 c4 10             	add    $0x10,%esp
  8023da:	83 bd ec fe ff ff 00 	cmpl   $0x0,-0x114(%ebp)
  8023e1:	7e 0b                	jle    8023ee <vfprintf+0x5e>
  8023e3:	8d 85 e8 fe ff ff    	lea    -0x118(%ebp),%eax
  8023e9:	e8 2e ff ff ff       	call   80231c <writebuf>
  8023ee:	8b 85 f0 fe ff ff    	mov    -0x110(%ebp),%eax
  8023f4:	85 c0                	test   %eax,%eax
  8023f6:	0f 44 85 f4 fe ff ff 	cmove  -0x10c(%ebp),%eax
  8023fd:	c9                   	leave  
  8023fe:	c3                   	ret    

008023ff <fprintf>:
  8023ff:	55                   	push   %ebp
  802400:	89 e5                	mov    %esp,%ebp
  802402:	83 ec 0c             	sub    $0xc,%esp
  802405:	8d 45 10             	lea    0x10(%ebp),%eax
  802408:	50                   	push   %eax
  802409:	ff 75 0c             	pushl  0xc(%ebp)
  80240c:	ff 75 08             	pushl  0x8(%ebp)
  80240f:	e8 7c ff ff ff       	call   802390 <vfprintf>
  802414:	c9                   	leave  
  802415:	c3                   	ret    

00802416 <printf>:
  802416:	55                   	push   %ebp
  802417:	89 e5                	mov    %esp,%ebp
  802419:	83 ec 0c             	sub    $0xc,%esp
  80241c:	8d 45 0c             	lea    0xc(%ebp),%eax
  80241f:	50                   	push   %eax
  802420:	ff 75 08             	pushl  0x8(%ebp)
  802423:	6a 01                	push   $0x1
  802425:	e8 66 ff ff ff       	call   802390 <vfprintf>
  80242a:	c9                   	leave  
  80242b:	c3                   	ret    

0080242c <devsock_stat>:
  80242c:	55                   	push   %ebp
  80242d:	89 e5                	mov    %esp,%ebp
  80242f:	83 ec 10             	sub    $0x10,%esp
  802432:	68 73 39 80 00       	push   $0x803973
  802437:	ff 75 0c             	pushl  0xc(%ebp)
  80243a:	e8 bc ef ff ff       	call   8013fb <strcpy>
  80243f:	b8 00 00 00 00       	mov    $0x0,%eax
  802444:	c9                   	leave  
  802445:	c3                   	ret    

00802446 <devsock_close>:
  802446:	55                   	push   %ebp
  802447:	89 e5                	mov    %esp,%ebp
  802449:	53                   	push   %ebx
  80244a:	83 ec 10             	sub    $0x10,%esp
  80244d:	8b 5d 08             	mov    0x8(%ebp),%ebx
  802450:	53                   	push   %ebx
  802451:	e8 0c 0c 00 00       	call   803062 <pageref>
  802456:	83 c4 10             	add    $0x10,%esp
  802459:	ba 00 00 00 00       	mov    $0x0,%edx
  80245e:	83 f8 01             	cmp    $0x1,%eax
  802461:	75 10                	jne    802473 <devsock_close+0x2d>
  802463:	83 ec 0c             	sub    $0xc,%esp
  802466:	ff 73 0c             	pushl  0xc(%ebx)
  802469:	e8 a1 03 00 00       	call   80280f <nsipc_close>
  80246e:	89 c2                	mov    %eax,%edx
  802470:	83 c4 10             	add    $0x10,%esp
  802473:	89 d0                	mov    %edx,%eax
  802475:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802478:	c9                   	leave  
  802479:	c3                   	ret    

0080247a <devsock_write>:
  80247a:	55                   	push   %ebp
  80247b:	89 e5                	mov    %esp,%ebp
  80247d:	83 ec 08             	sub    $0x8,%esp
  802480:	6a 00                	push   $0x0
  802482:	ff 75 10             	pushl  0x10(%ebp)
  802485:	ff 75 0c             	pushl  0xc(%ebp)
  802488:	8b 45 08             	mov    0x8(%ebp),%eax
  80248b:	ff 70 0c             	pushl  0xc(%eax)
  80248e:	e8 59 04 00 00       	call   8028ec <nsipc_send>
  802493:	c9                   	leave  
  802494:	c3                   	ret    

00802495 <devsock_read>:
  802495:	55                   	push   %ebp
  802496:	89 e5                	mov    %esp,%ebp
  802498:	83 ec 08             	sub    $0x8,%esp
  80249b:	6a 00                	push   $0x0
  80249d:	ff 75 10             	pushl  0x10(%ebp)
  8024a0:	ff 75 0c             	pushl  0xc(%ebp)
  8024a3:	8b 45 08             	mov    0x8(%ebp),%eax
  8024a6:	ff 70 0c             	pushl  0xc(%eax)
  8024a9:	e8 d2 03 00 00       	call   802880 <nsipc_recv>
  8024ae:	c9                   	leave  
  8024af:	c3                   	ret    

008024b0 <fd2sockid>:
  8024b0:	55                   	push   %ebp
  8024b1:	89 e5                	mov    %esp,%ebp
  8024b3:	83 ec 20             	sub    $0x20,%esp
  8024b6:	8d 55 f4             	lea    -0xc(%ebp),%edx
  8024b9:	52                   	push   %edx
  8024ba:	50                   	push   %eax
  8024bb:	e8 c7 f6 ff ff       	call   801b87 <fd_lookup>
  8024c0:	83 c4 10             	add    $0x10,%esp
  8024c3:	85 c0                	test   %eax,%eax
  8024c5:	78 17                	js     8024de <fd2sockid+0x2e>
  8024c7:	8b 45 f4             	mov    -0xc(%ebp),%eax
  8024ca:	8b 0d 24 00 00 10    	mov    0x10000024,%ecx
  8024d0:	39 08                	cmp    %ecx,(%eax)
  8024d2:	75 05                	jne    8024d9 <fd2sockid+0x29>
  8024d4:	8b 40 0c             	mov    0xc(%eax),%eax
  8024d7:	eb 05                	jmp    8024de <fd2sockid+0x2e>
  8024d9:	b8 f1 ff ff ff       	mov    $0xfffffff1,%eax
  8024de:	c9                   	leave  
  8024df:	c3                   	ret    

008024e0 <alloc_sockfd>:
  8024e0:	55                   	push   %ebp
  8024e1:	89 e5                	mov    %esp,%ebp
  8024e3:	56                   	push   %esi
  8024e4:	53                   	push   %ebx
  8024e5:	83 ec 1c             	sub    $0x1c,%esp
  8024e8:	89 c6                	mov    %eax,%esi
  8024ea:	8d 45 f4             	lea    -0xc(%ebp),%eax
  8024ed:	50                   	push   %eax
  8024ee:	e8 45 f6 ff ff       	call   801b38 <fd_alloc>
  8024f3:	89 c3                	mov    %eax,%ebx
  8024f5:	83 c4 10             	add    $0x10,%esp
  8024f8:	85 c0                	test   %eax,%eax
  8024fa:	78 1b                	js     802517 <alloc_sockfd+0x37>
  8024fc:	83 ec 04             	sub    $0x4,%esp
  8024ff:	68 07 04 00 00       	push   $0x407
  802504:	ff 75 f4             	pushl  -0xc(%ebp)
  802507:	6a 00                	push   $0x0
  802509:	e8 f0 f2 ff ff       	call   8017fe <sys_page_alloc>
  80250e:	89 c3                	mov    %eax,%ebx
  802510:	83 c4 10             	add    $0x10,%esp
  802513:	85 c0                	test   %eax,%eax
  802515:	79 10                	jns    802527 <alloc_sockfd+0x47>
  802517:	83 ec 0c             	sub    $0xc,%esp
  80251a:	56                   	push   %esi
  80251b:	e8 ef 02 00 00       	call   80280f <nsipc_close>
  802520:	83 c4 10             	add    $0x10,%esp
  802523:	89 d8                	mov    %ebx,%eax
  802525:	eb 24                	jmp    80254b <alloc_sockfd+0x6b>
  802527:	8b 15 24 00 00 10    	mov    0x10000024,%edx
  80252d:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802530:	89 10                	mov    %edx,(%eax)
  802532:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802535:	c7 40 08 02 00 00 00 	movl   $0x2,0x8(%eax)
  80253c:	89 70 0c             	mov    %esi,0xc(%eax)
  80253f:	83 ec 0c             	sub    $0xc,%esp
  802542:	50                   	push   %eax
  802543:	e8 c9 f5 ff ff       	call   801b11 <fd2num>
  802548:	83 c4 10             	add    $0x10,%esp
  80254b:	8d 65 f8             	lea    -0x8(%ebp),%esp
  80254e:	5b                   	pop    %ebx
  80254f:	5e                   	pop    %esi
  802550:	5d                   	pop    %ebp
  802551:	c3                   	ret    

00802552 <accept>:
  802552:	55                   	push   %ebp
  802553:	89 e5                	mov    %esp,%ebp
  802555:	83 ec 08             	sub    $0x8,%esp
  802558:	8b 45 08             	mov    0x8(%ebp),%eax
  80255b:	e8 50 ff ff ff       	call   8024b0 <fd2sockid>
  802560:	89 c1                	mov    %eax,%ecx
  802562:	85 c0                	test   %eax,%eax
  802564:	78 1f                	js     802585 <accept+0x33>
  802566:	83 ec 04             	sub    $0x4,%esp
  802569:	ff 75 10             	pushl  0x10(%ebp)
  80256c:	ff 75 0c             	pushl  0xc(%ebp)
  80256f:	50                   	push   %eax
  802570:	e8 f3 01 00 00       	call   802768 <nsipc_accept>
  802575:	83 c4 10             	add    $0x10,%esp
  802578:	89 c1                	mov    %eax,%ecx
  80257a:	85 c0                	test   %eax,%eax
  80257c:	78 07                	js     802585 <accept+0x33>
  80257e:	e8 5d ff ff ff       	call   8024e0 <alloc_sockfd>
  802583:	89 c1                	mov    %eax,%ecx
  802585:	89 c8                	mov    %ecx,%eax
  802587:	c9                   	leave  
  802588:	c3                   	ret    

00802589 <bind>:
  802589:	55                   	push   %ebp
  80258a:	89 e5                	mov    %esp,%ebp
  80258c:	83 ec 08             	sub    $0x8,%esp
  80258f:	8b 45 08             	mov    0x8(%ebp),%eax
  802592:	e8 19 ff ff ff       	call   8024b0 <fd2sockid>
  802597:	85 c0                	test   %eax,%eax
  802599:	78 12                	js     8025ad <bind+0x24>
  80259b:	83 ec 04             	sub    $0x4,%esp
  80259e:	ff 75 10             	pushl  0x10(%ebp)
  8025a1:	ff 75 0c             	pushl  0xc(%ebp)
  8025a4:	50                   	push   %eax
  8025a5:	e8 0e 02 00 00       	call   8027b8 <nsipc_bind>
  8025aa:	83 c4 10             	add    $0x10,%esp
  8025ad:	c9                   	leave  
  8025ae:	c3                   	ret    

008025af <shutdown>:
  8025af:	55                   	push   %ebp
  8025b0:	89 e5                	mov    %esp,%ebp
  8025b2:	83 ec 08             	sub    $0x8,%esp
  8025b5:	8b 45 08             	mov    0x8(%ebp),%eax
  8025b8:	e8 f3 fe ff ff       	call   8024b0 <fd2sockid>
  8025bd:	85 c0                	test   %eax,%eax
  8025bf:	78 0f                	js     8025d0 <shutdown+0x21>
  8025c1:	83 ec 08             	sub    $0x8,%esp
  8025c4:	ff 75 0c             	pushl  0xc(%ebp)
  8025c7:	50                   	push   %eax
  8025c8:	e8 20 02 00 00       	call   8027ed <nsipc_shutdown>
  8025cd:	83 c4 10             	add    $0x10,%esp
  8025d0:	c9                   	leave  
  8025d1:	c3                   	ret    

008025d2 <connect>:
  8025d2:	55                   	push   %ebp
  8025d3:	89 e5                	mov    %esp,%ebp
  8025d5:	83 ec 08             	sub    $0x8,%esp
  8025d8:	8b 45 08             	mov    0x8(%ebp),%eax
  8025db:	e8 d0 fe ff ff       	call   8024b0 <fd2sockid>
  8025e0:	85 c0                	test   %eax,%eax
  8025e2:	78 12                	js     8025f6 <connect+0x24>
  8025e4:	83 ec 04             	sub    $0x4,%esp
  8025e7:	ff 75 10             	pushl  0x10(%ebp)
  8025ea:	ff 75 0c             	pushl  0xc(%ebp)
  8025ed:	50                   	push   %eax
  8025ee:	e8 36 02 00 00       	call   802829 <nsipc_connect>
  8025f3:	83 c4 10             	add    $0x10,%esp
  8025f6:	c9                   	leave  
  8025f7:	c3                   	ret    

008025f8 <listen>:
  8025f8:	55                   	push   %ebp
  8025f9:	89 e5                	mov    %esp,%ebp
  8025fb:	83 ec 08             	sub    $0x8,%esp
  8025fe:	8b 45 08             	mov    0x8(%ebp),%eax
  802601:	e8 aa fe ff ff       	call   8024b0 <fd2sockid>
  802606:	85 c0                	test   %eax,%eax
  802608:	78 0f                	js     802619 <listen+0x21>
  80260a:	83 ec 08             	sub    $0x8,%esp
  80260d:	ff 75 0c             	pushl  0xc(%ebp)
  802610:	50                   	push   %eax
  802611:	e8 48 02 00 00       	call   80285e <nsipc_listen>
  802616:	83 c4 10             	add    $0x10,%esp
  802619:	c9                   	leave  
  80261a:	c3                   	ret    

0080261b <recvfrom>:
  80261b:	55                   	push   %ebp
  80261c:	89 e5                	mov    %esp,%ebp
  80261e:	53                   	push   %ebx
  80261f:	83 ec 10             	sub    $0x10,%esp
  802622:	68 7a 39 80 00       	push   $0x80397a
  802627:	e8 4a e8 ff ff       	call   800e76 <cprintf>
  80262c:	8b 45 08             	mov    0x8(%ebp),%eax
  80262f:	e8 7c fe ff ff       	call   8024b0 <fd2sockid>
  802634:	83 c4 10             	add    $0x10,%esp
  802637:	89 c2                	mov    %eax,%edx
  802639:	85 c0                	test   %eax,%eax
  80263b:	78 2c                	js     802669 <recvfrom+0x4e>
  80263d:	83 ec 08             	sub    $0x8,%esp
  802640:	ff 75 1c             	pushl  0x1c(%ebp)
  802643:	ff 75 18             	pushl  0x18(%ebp)
  802646:	ff 75 14             	pushl  0x14(%ebp)
  802649:	ff 75 10             	pushl  0x10(%ebp)
  80264c:	ff 75 0c             	pushl  0xc(%ebp)
  80264f:	50                   	push   %eax
  802650:	e8 f5 02 00 00       	call   80294a <nsipc_recvfrom>
  802655:	89 c3                	mov    %eax,%ebx
  802657:	83 c4 14             	add    $0x14,%esp
  80265a:	68 8b 39 80 00       	push   $0x80398b
  80265f:	e8 12 e8 ff ff       	call   800e76 <cprintf>
  802664:	83 c4 10             	add    $0x10,%esp
  802667:	89 da                	mov    %ebx,%edx
  802669:	89 d0                	mov    %edx,%eax
  80266b:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  80266e:	c9                   	leave  
  80266f:	c3                   	ret    

00802670 <sendto>:
  802670:	55                   	push   %ebp
  802671:	89 e5                	mov    %esp,%ebp
  802673:	57                   	push   %edi
  802674:	56                   	push   %esi
  802675:	53                   	push   %ebx
  802676:	83 ec 14             	sub    $0x14,%esp
  802679:	8b 7d 18             	mov    0x18(%ebp),%edi
  80267c:	6a 10                	push   $0x10
  80267e:	68 9b 39 80 00       	push   $0x80399b
  802683:	e8 ee e7 ff ff       	call   800e76 <cprintf>
  802688:	89 fb                	mov    %edi,%ebx
  80268a:	8d 77 10             	lea    0x10(%edi),%esi
  80268d:	83 c4 10             	add    $0x10,%esp
  802690:	83 ec 08             	sub    $0x8,%esp
  802693:	0f be 03             	movsbl (%ebx),%eax
  802696:	50                   	push   %eax
  802697:	68 a9 39 80 00       	push   $0x8039a9
  80269c:	e8 d5 e7 ff ff       	call   800e76 <cprintf>
  8026a1:	83 c3 01             	add    $0x1,%ebx
  8026a4:	83 c4 10             	add    $0x10,%esp
  8026a7:	39 f3                	cmp    %esi,%ebx
  8026a9:	75 e5                	jne    802690 <sendto+0x20>
  8026ab:	83 ec 0c             	sub    $0xc,%esp
  8026ae:	68 4a 35 80 00       	push   $0x80354a
  8026b3:	e8 be e7 ff ff       	call   800e76 <cprintf>
  8026b8:	8b 45 08             	mov    0x8(%ebp),%eax
  8026bb:	e8 f0 fd ff ff       	call   8024b0 <fd2sockid>
  8026c0:	83 c4 10             	add    $0x10,%esp
  8026c3:	89 c2                	mov    %eax,%edx
  8026c5:	85 c0                	test   %eax,%eax
  8026c7:	78 29                	js     8026f2 <sendto+0x82>
  8026c9:	83 ec 08             	sub    $0x8,%esp
  8026cc:	6a 10                	push   $0x10
  8026ce:	57                   	push   %edi
  8026cf:	ff 75 14             	pushl  0x14(%ebp)
  8026d2:	ff 75 10             	pushl  0x10(%ebp)
  8026d5:	ff 75 0c             	pushl  0xc(%ebp)
  8026d8:	50                   	push   %eax
  8026d9:	e8 f9 02 00 00       	call   8029d7 <nsipc_sendto>
  8026de:	89 c3                	mov    %eax,%ebx
  8026e0:	83 c4 14             	add    $0x14,%esp
  8026e3:	68 af 39 80 00       	push   $0x8039af
  8026e8:	e8 89 e7 ff ff       	call   800e76 <cprintf>
  8026ed:	83 c4 10             	add    $0x10,%esp
  8026f0:	89 da                	mov    %ebx,%edx
  8026f2:	89 d0                	mov    %edx,%eax
  8026f4:	8d 65 f4             	lea    -0xc(%ebp),%esp
  8026f7:	5b                   	pop    %ebx
  8026f8:	5e                   	pop    %esi
  8026f9:	5f                   	pop    %edi
  8026fa:	5d                   	pop    %ebp
  8026fb:	c3                   	ret    

008026fc <socket>:
  8026fc:	55                   	push   %ebp
  8026fd:	89 e5                	mov    %esp,%ebp
  8026ff:	83 ec 0c             	sub    $0xc,%esp
  802702:	ff 75 10             	pushl  0x10(%ebp)
  802705:	ff 75 0c             	pushl  0xc(%ebp)
  802708:	ff 75 08             	pushl  0x8(%ebp)
  80270b:	e8 52 03 00 00       	call   802a62 <nsipc_socket>
  802710:	83 c4 10             	add    $0x10,%esp
  802713:	85 c0                	test   %eax,%eax
  802715:	78 05                	js     80271c <socket+0x20>
  802717:	e8 c4 fd ff ff       	call   8024e0 <alloc_sockfd>
  80271c:	c9                   	leave  
  80271d:	c3                   	ret    

0080271e <nsipc>:
  80271e:	55                   	push   %ebp
  80271f:	89 e5                	mov    %esp,%ebp
  802721:	53                   	push   %ebx
  802722:	83 ec 04             	sub    $0x4,%esp
  802725:	89 c3                	mov    %eax,%ebx
  802727:	83 3d 38 c4 c4 14 00 	cmpl   $0x0,0x14c4c438
  80272e:	75 12                	jne    802742 <nsipc+0x24>
  802730:	83 ec 0c             	sub    $0xc,%esp
  802733:	6a 02                	push   $0x2
  802735:	e8 e9 08 00 00       	call   803023 <ipc_find_env>
  80273a:	a3 38 c4 c4 14       	mov    %eax,0x14c4c438
  80273f:	83 c4 10             	add    $0x10,%esp
  802742:	6a 07                	push   $0x7
  802744:	68 00 00 ec 3b       	push   $0x3bec0000
  802749:	53                   	push   %ebx
  80274a:	ff 35 38 c4 c4 14    	pushl  0x14c4c438
  802750:	e8 7f 08 00 00       	call   802fd4 <ipc_send>
  802755:	83 c4 0c             	add    $0xc,%esp
  802758:	6a 00                	push   $0x0
  80275a:	6a 00                	push   $0x0
  80275c:	6a 00                	push   $0x0
  80275e:	e8 08 08 00 00       	call   802f6b <ipc_recv>
  802763:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802766:	c9                   	leave  
  802767:	c3                   	ret    

00802768 <nsipc_accept>:
  802768:	55                   	push   %ebp
  802769:	89 e5                	mov    %esp,%ebp
  80276b:	56                   	push   %esi
  80276c:	53                   	push   %ebx
  80276d:	8b 75 10             	mov    0x10(%ebp),%esi
  802770:	8b 45 08             	mov    0x8(%ebp),%eax
  802773:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  802778:	8b 06                	mov    (%esi),%eax
  80277a:	a3 04 00 ec 3b       	mov    %eax,0x3bec0004
  80277f:	b8 01 00 00 00       	mov    $0x1,%eax
  802784:	e8 95 ff ff ff       	call   80271e <nsipc>
  802789:	89 c3                	mov    %eax,%ebx
  80278b:	85 c0                	test   %eax,%eax
  80278d:	78 20                	js     8027af <nsipc_accept+0x47>
  80278f:	83 ec 04             	sub    $0x4,%esp
  802792:	ff 35 10 00 ec 3b    	pushl  0x3bec0010
  802798:	68 00 00 ec 3b       	push   $0x3bec0000
  80279d:	ff 75 0c             	pushl  0xc(%ebp)
  8027a0:	e8 e8 ed ff ff       	call   80158d <memmove>
  8027a5:	a1 10 00 ec 3b       	mov    0x3bec0010,%eax
  8027aa:	89 06                	mov    %eax,(%esi)
  8027ac:	83 c4 10             	add    $0x10,%esp
  8027af:	89 d8                	mov    %ebx,%eax
  8027b1:	8d 65 f8             	lea    -0x8(%ebp),%esp
  8027b4:	5b                   	pop    %ebx
  8027b5:	5e                   	pop    %esi
  8027b6:	5d                   	pop    %ebp
  8027b7:	c3                   	ret    

008027b8 <nsipc_bind>:
  8027b8:	55                   	push   %ebp
  8027b9:	89 e5                	mov    %esp,%ebp
  8027bb:	53                   	push   %ebx
  8027bc:	83 ec 08             	sub    $0x8,%esp
  8027bf:	8b 5d 10             	mov    0x10(%ebp),%ebx
  8027c2:	8b 45 08             	mov    0x8(%ebp),%eax
  8027c5:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  8027ca:	53                   	push   %ebx
  8027cb:	ff 75 0c             	pushl  0xc(%ebp)
  8027ce:	68 04 00 ec 3b       	push   $0x3bec0004
  8027d3:	e8 b5 ed ff ff       	call   80158d <memmove>
  8027d8:	89 1d 14 00 ec 3b    	mov    %ebx,0x3bec0014
  8027de:	b8 02 00 00 00       	mov    $0x2,%eax
  8027e3:	e8 36 ff ff ff       	call   80271e <nsipc>
  8027e8:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  8027eb:	c9                   	leave  
  8027ec:	c3                   	ret    

008027ed <nsipc_shutdown>:
  8027ed:	55                   	push   %ebp
  8027ee:	89 e5                	mov    %esp,%ebp
  8027f0:	83 ec 08             	sub    $0x8,%esp
  8027f3:	8b 45 08             	mov    0x8(%ebp),%eax
  8027f6:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  8027fb:	8b 45 0c             	mov    0xc(%ebp),%eax
  8027fe:	a3 04 00 ec 3b       	mov    %eax,0x3bec0004
  802803:	b8 03 00 00 00       	mov    $0x3,%eax
  802808:	e8 11 ff ff ff       	call   80271e <nsipc>
  80280d:	c9                   	leave  
  80280e:	c3                   	ret    

0080280f <nsipc_close>:
  80280f:	55                   	push   %ebp
  802810:	89 e5                	mov    %esp,%ebp
  802812:	83 ec 08             	sub    $0x8,%esp
  802815:	8b 45 08             	mov    0x8(%ebp),%eax
  802818:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  80281d:	b8 04 00 00 00       	mov    $0x4,%eax
  802822:	e8 f7 fe ff ff       	call   80271e <nsipc>
  802827:	c9                   	leave  
  802828:	c3                   	ret    

00802829 <nsipc_connect>:
  802829:	55                   	push   %ebp
  80282a:	89 e5                	mov    %esp,%ebp
  80282c:	53                   	push   %ebx
  80282d:	83 ec 08             	sub    $0x8,%esp
  802830:	8b 5d 10             	mov    0x10(%ebp),%ebx
  802833:	8b 45 08             	mov    0x8(%ebp),%eax
  802836:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  80283b:	53                   	push   %ebx
  80283c:	ff 75 0c             	pushl  0xc(%ebp)
  80283f:	68 04 00 ec 3b       	push   $0x3bec0004
  802844:	e8 44 ed ff ff       	call   80158d <memmove>
  802849:	89 1d 14 00 ec 3b    	mov    %ebx,0x3bec0014
  80284f:	b8 05 00 00 00       	mov    $0x5,%eax
  802854:	e8 c5 fe ff ff       	call   80271e <nsipc>
  802859:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  80285c:	c9                   	leave  
  80285d:	c3                   	ret    

0080285e <nsipc_listen>:
  80285e:	55                   	push   %ebp
  80285f:	89 e5                	mov    %esp,%ebp
  802861:	83 ec 08             	sub    $0x8,%esp
  802864:	8b 45 08             	mov    0x8(%ebp),%eax
  802867:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  80286c:	8b 45 0c             	mov    0xc(%ebp),%eax
  80286f:	a3 04 00 ec 3b       	mov    %eax,0x3bec0004
  802874:	b8 06 00 00 00       	mov    $0x6,%eax
  802879:	e8 a0 fe ff ff       	call   80271e <nsipc>
  80287e:	c9                   	leave  
  80287f:	c3                   	ret    

00802880 <nsipc_recv>:
  802880:	55                   	push   %ebp
  802881:	89 e5                	mov    %esp,%ebp
  802883:	56                   	push   %esi
  802884:	53                   	push   %ebx
  802885:	8b 75 10             	mov    0x10(%ebp),%esi
  802888:	8b 45 08             	mov    0x8(%ebp),%eax
  80288b:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  802890:	89 35 04 00 ec 3b    	mov    %esi,0x3bec0004
  802896:	8b 45 14             	mov    0x14(%ebp),%eax
  802899:	a3 08 00 ec 3b       	mov    %eax,0x3bec0008
  80289e:	b8 07 00 00 00       	mov    $0x7,%eax
  8028a3:	e8 76 fe ff ff       	call   80271e <nsipc>
  8028a8:	89 c3                	mov    %eax,%ebx
  8028aa:	85 c0                	test   %eax,%eax
  8028ac:	78 35                	js     8028e3 <nsipc_recv+0x63>
  8028ae:	3d 3f 06 00 00       	cmp    $0x63f,%eax
  8028b3:	7f 04                	jg     8028b9 <nsipc_recv+0x39>
  8028b5:	39 c6                	cmp    %eax,%esi
  8028b7:	7d 16                	jge    8028cf <nsipc_recv+0x4f>
  8028b9:	68 bf 39 80 00       	push   $0x8039bf
  8028be:	68 47 39 80 00       	push   $0x803947
  8028c3:	6a 62                	push   $0x62
  8028c5:	68 d4 39 80 00       	push   $0x8039d4
  8028ca:	e8 ce e4 ff ff       	call   800d9d <_panic>
  8028cf:	83 ec 04             	sub    $0x4,%esp
  8028d2:	50                   	push   %eax
  8028d3:	68 00 00 ec 3b       	push   $0x3bec0000
  8028d8:	ff 75 0c             	pushl  0xc(%ebp)
  8028db:	e8 ad ec ff ff       	call   80158d <memmove>
  8028e0:	83 c4 10             	add    $0x10,%esp
  8028e3:	89 d8                	mov    %ebx,%eax
  8028e5:	8d 65 f8             	lea    -0x8(%ebp),%esp
  8028e8:	5b                   	pop    %ebx
  8028e9:	5e                   	pop    %esi
  8028ea:	5d                   	pop    %ebp
  8028eb:	c3                   	ret    

008028ec <nsipc_send>:
  8028ec:	55                   	push   %ebp
  8028ed:	89 e5                	mov    %esp,%ebp
  8028ef:	53                   	push   %ebx
  8028f0:	83 ec 04             	sub    $0x4,%esp
  8028f3:	8b 5d 10             	mov    0x10(%ebp),%ebx
  8028f6:	8b 45 08             	mov    0x8(%ebp),%eax
  8028f9:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  8028fe:	81 fb 3f 06 00 00    	cmp    $0x63f,%ebx
  802904:	7e 16                	jle    80291c <nsipc_send+0x30>
  802906:	68 e0 39 80 00       	push   $0x8039e0
  80290b:	68 47 39 80 00       	push   $0x803947
  802910:	6a 6d                	push   $0x6d
  802912:	68 d4 39 80 00       	push   $0x8039d4
  802917:	e8 81 e4 ff ff       	call   800d9d <_panic>
  80291c:	83 ec 04             	sub    $0x4,%esp
  80291f:	53                   	push   %ebx
  802920:	ff 75 0c             	pushl  0xc(%ebp)
  802923:	68 0c 00 ec 3b       	push   $0x3bec000c
  802928:	e8 60 ec ff ff       	call   80158d <memmove>
  80292d:	89 1d 04 00 ec 3b    	mov    %ebx,0x3bec0004
  802933:	8b 45 14             	mov    0x14(%ebp),%eax
  802936:	a3 08 00 ec 3b       	mov    %eax,0x3bec0008
  80293b:	b8 08 00 00 00       	mov    $0x8,%eax
  802940:	e8 d9 fd ff ff       	call   80271e <nsipc>
  802945:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802948:	c9                   	leave  
  802949:	c3                   	ret    

0080294a <nsipc_recvfrom>:
  80294a:	55                   	push   %ebp
  80294b:	89 e5                	mov    %esp,%ebp
  80294d:	56                   	push   %esi
  80294e:	53                   	push   %ebx
  80294f:	8b 75 10             	mov    0x10(%ebp),%esi
  802952:	8b 45 08             	mov    0x8(%ebp),%eax
  802955:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  80295a:	89 35 04 00 ec 3b    	mov    %esi,0x3bec0004
  802960:	8b 45 14             	mov    0x14(%ebp),%eax
  802963:	a3 08 00 ec 3b       	mov    %eax,0x3bec0008
  802968:	b8 09 00 00 00       	mov    $0x9,%eax
  80296d:	e8 ac fd ff ff       	call   80271e <nsipc>
  802972:	89 c3                	mov    %eax,%ebx
  802974:	85 c0                	test   %eax,%eax
  802976:	78 56                	js     8029ce <nsipc_recvfrom+0x84>
  802978:	3d 3f 06 00 00       	cmp    $0x63f,%eax
  80297d:	7f 04                	jg     802983 <nsipc_recvfrom+0x39>
  80297f:	39 c6                	cmp    %eax,%esi
  802981:	7d 16                	jge    802999 <nsipc_recvfrom+0x4f>
  802983:	68 bf 39 80 00       	push   $0x8039bf
  802988:	68 47 39 80 00       	push   $0x803947
  80298d:	6a 7f                	push   $0x7f
  80298f:	68 d4 39 80 00       	push   $0x8039d4
  802994:	e8 04 e4 ff ff       	call   800d9d <_panic>
  802999:	83 ec 04             	sub    $0x4,%esp
  80299c:	50                   	push   %eax
  80299d:	68 14 00 ec 3b       	push   $0x3bec0014
  8029a2:	ff 75 0c             	pushl  0xc(%ebp)
  8029a5:	e8 e3 eb ff ff       	call   80158d <memmove>
  8029aa:	83 c4 0c             	add    $0xc,%esp
  8029ad:	ff 35 10 00 ec 3b    	pushl  0x3bec0010
  8029b3:	68 00 00 ec 3b       	push   $0x3bec0000
  8029b8:	ff 75 18             	pushl  0x18(%ebp)
  8029bb:	e8 cd eb ff ff       	call   80158d <memmove>
  8029c0:	8b 45 1c             	mov    0x1c(%ebp),%eax
  8029c3:	8b 15 10 00 ec 3b    	mov    0x3bec0010,%edx
  8029c9:	89 10                	mov    %edx,(%eax)
  8029cb:	83 c4 10             	add    $0x10,%esp
  8029ce:	89 d8                	mov    %ebx,%eax
  8029d0:	8d 65 f8             	lea    -0x8(%ebp),%esp
  8029d3:	5b                   	pop    %ebx
  8029d4:	5e                   	pop    %esi
  8029d5:	5d                   	pop    %ebp
  8029d6:	c3                   	ret    

008029d7 <nsipc_sendto>:
  8029d7:	55                   	push   %ebp
  8029d8:	89 e5                	mov    %esp,%ebp
  8029da:	56                   	push   %esi
  8029db:	53                   	push   %ebx
  8029dc:	8b 75 10             	mov    0x10(%ebp),%esi
  8029df:	8b 5d 18             	mov    0x18(%ebp),%ebx
  8029e2:	8b 45 08             	mov    0x8(%ebp),%eax
  8029e5:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  8029ea:	81 fe 3f 06 00 00    	cmp    $0x63f,%esi
  8029f0:	7e 19                	jle    802a0b <nsipc_sendto+0x34>
  8029f2:	68 ec 39 80 00       	push   $0x8039ec
  8029f7:	68 47 39 80 00       	push   $0x803947
  8029fc:	68 8c 00 00 00       	push   $0x8c
  802a01:	68 d4 39 80 00       	push   $0x8039d4
  802a06:	e8 92 e3 ff ff       	call   800d9d <_panic>
  802a0b:	83 ec 04             	sub    $0x4,%esp
  802a0e:	56                   	push   %esi
  802a0f:	ff 75 0c             	pushl  0xc(%ebp)
  802a12:	68 20 00 ec 3b       	push   $0x3bec0020
  802a17:	e8 71 eb ff ff       	call   80158d <memmove>
  802a1c:	89 35 04 00 ec 3b    	mov    %esi,0x3bec0004
  802a22:	8b 45 14             	mov    0x14(%ebp),%eax
  802a25:	a3 08 00 ec 3b       	mov    %eax,0x3bec0008
  802a2a:	8b 03                	mov    (%ebx),%eax
  802a2c:	a3 0c 00 ec 3b       	mov    %eax,0x3bec000c
  802a31:	8b 43 04             	mov    0x4(%ebx),%eax
  802a34:	a3 10 00 ec 3b       	mov    %eax,0x3bec0010
  802a39:	8b 43 08             	mov    0x8(%ebx),%eax
  802a3c:	a3 14 00 ec 3b       	mov    %eax,0x3bec0014
  802a41:	8b 43 0c             	mov    0xc(%ebx),%eax
  802a44:	a3 18 00 ec 3b       	mov    %eax,0x3bec0018
  802a49:	8b 45 1c             	mov    0x1c(%ebp),%eax
  802a4c:	a3 1c 00 ec 3b       	mov    %eax,0x3bec001c
  802a51:	b8 0a 00 00 00       	mov    $0xa,%eax
  802a56:	e8 c3 fc ff ff       	call   80271e <nsipc>
  802a5b:	8d 65 f8             	lea    -0x8(%ebp),%esp
  802a5e:	5b                   	pop    %ebx
  802a5f:	5e                   	pop    %esi
  802a60:	5d                   	pop    %ebp
  802a61:	c3                   	ret    

00802a62 <nsipc_socket>:
  802a62:	55                   	push   %ebp
  802a63:	89 e5                	mov    %esp,%ebp
  802a65:	83 ec 08             	sub    $0x8,%esp
  802a68:	8b 45 08             	mov    0x8(%ebp),%eax
  802a6b:	a3 00 00 ec 3b       	mov    %eax,0x3bec0000
  802a70:	8b 45 0c             	mov    0xc(%ebp),%eax
  802a73:	a3 04 00 ec 3b       	mov    %eax,0x3bec0004
  802a78:	8b 45 10             	mov    0x10(%ebp),%eax
  802a7b:	a3 08 00 ec 3b       	mov    %eax,0x3bec0008
  802a80:	b8 0b 00 00 00       	mov    $0xb,%eax
  802a85:	e8 94 fc ff ff       	call   80271e <nsipc>
  802a8a:	c9                   	leave  
  802a8b:	c3                   	ret    

00802a8c <devpipe_stat>:
  802a8c:	55                   	push   %ebp
  802a8d:	89 e5                	mov    %esp,%ebp
  802a8f:	56                   	push   %esi
  802a90:	53                   	push   %ebx
  802a91:	8b 5d 0c             	mov    0xc(%ebp),%ebx
  802a94:	83 ec 0c             	sub    $0xc,%esp
  802a97:	ff 75 08             	pushl  0x8(%ebp)
  802a9a:	e8 82 f0 ff ff       	call   801b21 <fd2data>
  802a9f:	89 c6                	mov    %eax,%esi
  802aa1:	83 c4 08             	add    $0x8,%esp
  802aa4:	68 f7 39 80 00       	push   $0x8039f7
  802aa9:	53                   	push   %ebx
  802aaa:	e8 4c e9 ff ff       	call   8013fb <strcpy>
  802aaf:	8b 46 04             	mov    0x4(%esi),%eax
  802ab2:	2b 06                	sub    (%esi),%eax
  802ab4:	89 83 80 00 00 00    	mov    %eax,0x80(%ebx)
  802aba:	c7 83 84 00 00 00 00 	movl   $0x0,0x84(%ebx)
  802ac1:	00 00 00 
  802ac4:	c7 83 88 00 00 00 40 	movl   $0x10000040,0x88(%ebx)
  802acb:	00 00 10 
  802ace:	b8 00 00 00 00       	mov    $0x0,%eax
  802ad3:	8d 65 f8             	lea    -0x8(%ebp),%esp
  802ad6:	5b                   	pop    %ebx
  802ad7:	5e                   	pop    %esi
  802ad8:	5d                   	pop    %ebp
  802ad9:	c3                   	ret    

00802ada <devpipe_close>:
  802ada:	55                   	push   %ebp
  802adb:	89 e5                	mov    %esp,%ebp
  802add:	53                   	push   %ebx
  802ade:	83 ec 0c             	sub    $0xc,%esp
  802ae1:	8b 5d 08             	mov    0x8(%ebp),%ebx
  802ae4:	53                   	push   %ebx
  802ae5:	6a 00                	push   $0x0
  802ae7:	e8 97 ed ff ff       	call   801883 <sys_page_unmap>
  802aec:	89 1c 24             	mov    %ebx,(%esp)
  802aef:	e8 2d f0 ff ff       	call   801b21 <fd2data>
  802af4:	83 c4 08             	add    $0x8,%esp
  802af7:	50                   	push   %eax
  802af8:	6a 00                	push   $0x0
  802afa:	e8 84 ed ff ff       	call   801883 <sys_page_unmap>
  802aff:	8b 5d fc             	mov    -0x4(%ebp),%ebx
  802b02:	c9                   	leave  
  802b03:	c3                   	ret    

00802b04 <_pipeisclosed>:
  802b04:	55                   	push   %ebp
  802b05:	89 e5                	mov    %esp,%ebp
  802b07:	57                   	push   %edi
  802b08:	56                   	push   %esi
  802b09:	53                   	push   %ebx
  802b0a:	83 ec 1c             	sub    $0x1c,%esp
  802b0d:	89 45 e0             	mov    %eax,-0x20(%ebp)
  802b10:	89 d7                	mov    %edx,%edi
  802b12:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  802b17:	8b 70 58             	mov    0x58(%eax),%esi
  802b1a:	83 ec 0c             	sub    $0xc,%esp
  802b1d:	ff 75 e0             	pushl  -0x20(%ebp)
  802b20:	e8 3d 05 00 00       	call   803062 <pageref>
  802b25:	89 c3                	mov    %eax,%ebx
  802b27:	89 3c 24             	mov    %edi,(%esp)
  802b2a:	e8 33 05 00 00       	call   803062 <pageref>
  802b2f:	83 c4 10             	add    $0x10,%esp
  802b32:	39 c3                	cmp    %eax,%ebx
  802b34:	0f 94 c1             	sete   %cl
  802b37:	0f b6 c9             	movzbl %cl,%ecx
  802b3a:	89 4d e4             	mov    %ecx,-0x1c(%ebp)
  802b3d:	8b 15 00 e0 eb 3b    	mov    0x3bebe000,%edx
  802b43:	8b 4a 58             	mov    0x58(%edx),%ecx
  802b46:	39 ce                	cmp    %ecx,%esi
  802b48:	74 1b                	je     802b65 <_pipeisclosed+0x61>
  802b4a:	39 c3                	cmp    %eax,%ebx
  802b4c:	75 c4                	jne    802b12 <_pipeisclosed+0xe>
  802b4e:	8b 42 58             	mov    0x58(%edx),%eax
  802b51:	ff 75 e4             	pushl  -0x1c(%ebp)
  802b54:	50                   	push   %eax
  802b55:	56                   	push   %esi
  802b56:	68 fe 39 80 00       	push   $0x8039fe
  802b5b:	e8 16 e3 ff ff       	call   800e76 <cprintf>
  802b60:	83 c4 10             	add    $0x10,%esp
  802b63:	eb ad                	jmp    802b12 <_pipeisclosed+0xe>
  802b65:	8b 45 e4             	mov    -0x1c(%ebp),%eax
  802b68:	8d 65 f4             	lea    -0xc(%ebp),%esp
  802b6b:	5b                   	pop    %ebx
  802b6c:	5e                   	pop    %esi
  802b6d:	5f                   	pop    %edi
  802b6e:	5d                   	pop    %ebp
  802b6f:	c3                   	ret    

00802b70 <devpipe_write>:
  802b70:	55                   	push   %ebp
  802b71:	89 e5                	mov    %esp,%ebp
  802b73:	57                   	push   %edi
  802b74:	56                   	push   %esi
  802b75:	53                   	push   %ebx
  802b76:	83 ec 28             	sub    $0x28,%esp
  802b79:	8b 75 08             	mov    0x8(%ebp),%esi
  802b7c:	56                   	push   %esi
  802b7d:	e8 9f ef ff ff       	call   801b21 <fd2data>
  802b82:	89 c3                	mov    %eax,%ebx
  802b84:	83 c4 10             	add    $0x10,%esp
  802b87:	bf 00 00 00 00       	mov    $0x0,%edi
  802b8c:	eb 4b                	jmp    802bd9 <devpipe_write+0x69>
  802b8e:	89 da                	mov    %ebx,%edx
  802b90:	89 f0                	mov    %esi,%eax
  802b92:	e8 6d ff ff ff       	call   802b04 <_pipeisclosed>
  802b97:	85 c0                	test   %eax,%eax
  802b99:	75 48                	jne    802be3 <devpipe_write+0x73>
  802b9b:	e8 3f ec ff ff       	call   8017df <sys_yield>
  802ba0:	8b 43 04             	mov    0x4(%ebx),%eax
  802ba3:	8b 0b                	mov    (%ebx),%ecx
  802ba5:	8d 51 20             	lea    0x20(%ecx),%edx
  802ba8:	39 d0                	cmp    %edx,%eax
  802baa:	73 e2                	jae    802b8e <devpipe_write+0x1e>
  802bac:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  802baf:	0f b6 0c 39          	movzbl (%ecx,%edi,1),%ecx
  802bb3:	88 4d e7             	mov    %cl,-0x19(%ebp)
  802bb6:	89 c2                	mov    %eax,%edx
  802bb8:	c1 fa 1f             	sar    $0x1f,%edx
  802bbb:	89 d1                	mov    %edx,%ecx
  802bbd:	c1 e9 1b             	shr    $0x1b,%ecx
  802bc0:	8d 14 08             	lea    (%eax,%ecx,1),%edx
  802bc3:	83 e2 1f             	and    $0x1f,%edx
  802bc6:	29 ca                	sub    %ecx,%edx
  802bc8:	0f b6 4d e7          	movzbl -0x19(%ebp),%ecx
  802bcc:	88 4c 13 08          	mov    %cl,0x8(%ebx,%edx,1)
  802bd0:	83 c0 01             	add    $0x1,%eax
  802bd3:	89 43 04             	mov    %eax,0x4(%ebx)
  802bd6:	83 c7 01             	add    $0x1,%edi
  802bd9:	3b 7d 10             	cmp    0x10(%ebp),%edi
  802bdc:	75 c2                	jne    802ba0 <devpipe_write+0x30>
  802bde:	8b 45 10             	mov    0x10(%ebp),%eax
  802be1:	eb 05                	jmp    802be8 <devpipe_write+0x78>
  802be3:	b8 00 00 00 00       	mov    $0x0,%eax
  802be8:	8d 65 f4             	lea    -0xc(%ebp),%esp
  802beb:	5b                   	pop    %ebx
  802bec:	5e                   	pop    %esi
  802bed:	5f                   	pop    %edi
  802bee:	5d                   	pop    %ebp
  802bef:	c3                   	ret    

00802bf0 <devpipe_read>:
  802bf0:	55                   	push   %ebp
  802bf1:	89 e5                	mov    %esp,%ebp
  802bf3:	57                   	push   %edi
  802bf4:	56                   	push   %esi
  802bf5:	53                   	push   %ebx
  802bf6:	83 ec 18             	sub    $0x18,%esp
  802bf9:	8b 7d 08             	mov    0x8(%ebp),%edi
  802bfc:	57                   	push   %edi
  802bfd:	e8 1f ef ff ff       	call   801b21 <fd2data>
  802c02:	89 c6                	mov    %eax,%esi
  802c04:	83 c4 10             	add    $0x10,%esp
  802c07:	bb 00 00 00 00       	mov    $0x0,%ebx
  802c0c:	eb 3d                	jmp    802c4b <devpipe_read+0x5b>
  802c0e:	85 db                	test   %ebx,%ebx
  802c10:	74 04                	je     802c16 <devpipe_read+0x26>
  802c12:	89 d8                	mov    %ebx,%eax
  802c14:	eb 44                	jmp    802c5a <devpipe_read+0x6a>
  802c16:	89 f2                	mov    %esi,%edx
  802c18:	89 f8                	mov    %edi,%eax
  802c1a:	e8 e5 fe ff ff       	call   802b04 <_pipeisclosed>
  802c1f:	85 c0                	test   %eax,%eax
  802c21:	75 32                	jne    802c55 <devpipe_read+0x65>
  802c23:	e8 b7 eb ff ff       	call   8017df <sys_yield>
  802c28:	8b 06                	mov    (%esi),%eax
  802c2a:	3b 46 04             	cmp    0x4(%esi),%eax
  802c2d:	74 df                	je     802c0e <devpipe_read+0x1e>
  802c2f:	99                   	cltd   
  802c30:	c1 ea 1b             	shr    $0x1b,%edx
  802c33:	01 d0                	add    %edx,%eax
  802c35:	83 e0 1f             	and    $0x1f,%eax
  802c38:	29 d0                	sub    %edx,%eax
  802c3a:	0f b6 44 06 08       	movzbl 0x8(%esi,%eax,1),%eax
  802c3f:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  802c42:	88 04 19             	mov    %al,(%ecx,%ebx,1)
  802c45:	83 06 01             	addl   $0x1,(%esi)
  802c48:	83 c3 01             	add    $0x1,%ebx
  802c4b:	3b 5d 10             	cmp    0x10(%ebp),%ebx
  802c4e:	75 d8                	jne    802c28 <devpipe_read+0x38>
  802c50:	8b 45 10             	mov    0x10(%ebp),%eax
  802c53:	eb 05                	jmp    802c5a <devpipe_read+0x6a>
  802c55:	b8 00 00 00 00       	mov    $0x0,%eax
  802c5a:	8d 65 f4             	lea    -0xc(%ebp),%esp
  802c5d:	5b                   	pop    %ebx
  802c5e:	5e                   	pop    %esi
  802c5f:	5f                   	pop    %edi
  802c60:	5d                   	pop    %ebp
  802c61:	c3                   	ret    

00802c62 <pipe>:
  802c62:	55                   	push   %ebp
  802c63:	89 e5                	mov    %esp,%ebp
  802c65:	56                   	push   %esi
  802c66:	53                   	push   %ebx
  802c67:	83 ec 1c             	sub    $0x1c,%esp
  802c6a:	8d 45 f4             	lea    -0xc(%ebp),%eax
  802c6d:	50                   	push   %eax
  802c6e:	e8 c5 ee ff ff       	call   801b38 <fd_alloc>
  802c73:	83 c4 10             	add    $0x10,%esp
  802c76:	89 c2                	mov    %eax,%edx
  802c78:	85 c0                	test   %eax,%eax
  802c7a:	0f 88 2c 01 00 00    	js     802dac <pipe+0x14a>
  802c80:	83 ec 04             	sub    $0x4,%esp
  802c83:	68 07 04 00 00       	push   $0x407
  802c88:	ff 75 f4             	pushl  -0xc(%ebp)
  802c8b:	6a 00                	push   $0x0
  802c8d:	e8 6c eb ff ff       	call   8017fe <sys_page_alloc>
  802c92:	83 c4 10             	add    $0x10,%esp
  802c95:	89 c2                	mov    %eax,%edx
  802c97:	85 c0                	test   %eax,%eax
  802c99:	0f 88 0d 01 00 00    	js     802dac <pipe+0x14a>
  802c9f:	83 ec 0c             	sub    $0xc,%esp
  802ca2:	8d 45 f0             	lea    -0x10(%ebp),%eax
  802ca5:	50                   	push   %eax
  802ca6:	e8 8d ee ff ff       	call   801b38 <fd_alloc>
  802cab:	89 c3                	mov    %eax,%ebx
  802cad:	83 c4 10             	add    $0x10,%esp
  802cb0:	85 c0                	test   %eax,%eax
  802cb2:	0f 88 e2 00 00 00    	js     802d9a <pipe+0x138>
  802cb8:	83 ec 04             	sub    $0x4,%esp
  802cbb:	68 07 04 00 00       	push   $0x407
  802cc0:	ff 75 f0             	pushl  -0x10(%ebp)
  802cc3:	6a 00                	push   $0x0
  802cc5:	e8 34 eb ff ff       	call   8017fe <sys_page_alloc>
  802cca:	89 c3                	mov    %eax,%ebx
  802ccc:	83 c4 10             	add    $0x10,%esp
  802ccf:	85 c0                	test   %eax,%eax
  802cd1:	0f 88 c3 00 00 00    	js     802d9a <pipe+0x138>
  802cd7:	83 ec 0c             	sub    $0xc,%esp
  802cda:	ff 75 f4             	pushl  -0xc(%ebp)
  802cdd:	e8 3f ee ff ff       	call   801b21 <fd2data>
  802ce2:	89 c6                	mov    %eax,%esi
  802ce4:	83 c4 0c             	add    $0xc,%esp
  802ce7:	68 07 04 00 00       	push   $0x407
  802cec:	50                   	push   %eax
  802ced:	6a 00                	push   $0x0
  802cef:	e8 0a eb ff ff       	call   8017fe <sys_page_alloc>
  802cf4:	89 c3                	mov    %eax,%ebx
  802cf6:	83 c4 10             	add    $0x10,%esp
  802cf9:	85 c0                	test   %eax,%eax
  802cfb:	0f 88 89 00 00 00    	js     802d8a <pipe+0x128>
  802d01:	83 ec 0c             	sub    $0xc,%esp
  802d04:	ff 75 f0             	pushl  -0x10(%ebp)
  802d07:	e8 15 ee ff ff       	call   801b21 <fd2data>
  802d0c:	c7 04 24 07 04 00 00 	movl   $0x407,(%esp)
  802d13:	50                   	push   %eax
  802d14:	6a 00                	push   $0x0
  802d16:	56                   	push   %esi
  802d17:	6a 00                	push   $0x0
  802d19:	e8 23 eb ff ff       	call   801841 <sys_page_map>
  802d1e:	89 c3                	mov    %eax,%ebx
  802d20:	83 c4 20             	add    $0x20,%esp
  802d23:	85 c0                	test   %eax,%eax
  802d25:	78 55                	js     802d7c <pipe+0x11a>
  802d27:	8b 15 40 00 00 10    	mov    0x10000040,%edx
  802d2d:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802d30:	89 10                	mov    %edx,(%eax)
  802d32:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802d35:	c7 40 08 00 00 00 00 	movl   $0x0,0x8(%eax)
  802d3c:	8b 15 40 00 00 10    	mov    0x10000040,%edx
  802d42:	8b 45 f0             	mov    -0x10(%ebp),%eax
  802d45:	89 10                	mov    %edx,(%eax)
  802d47:	8b 45 f0             	mov    -0x10(%ebp),%eax
  802d4a:	c7 40 08 01 00 00 00 	movl   $0x1,0x8(%eax)
  802d51:	83 ec 0c             	sub    $0xc,%esp
  802d54:	ff 75 f4             	pushl  -0xc(%ebp)
  802d57:	e8 b5 ed ff ff       	call   801b11 <fd2num>
  802d5c:	8b 4d 08             	mov    0x8(%ebp),%ecx
  802d5f:	89 01                	mov    %eax,(%ecx)
  802d61:	83 c4 04             	add    $0x4,%esp
  802d64:	ff 75 f0             	pushl  -0x10(%ebp)
  802d67:	e8 a5 ed ff ff       	call   801b11 <fd2num>
  802d6c:	8b 4d 08             	mov    0x8(%ebp),%ecx
  802d6f:	89 41 04             	mov    %eax,0x4(%ecx)
  802d72:	83 c4 10             	add    $0x10,%esp
  802d75:	ba 00 00 00 00       	mov    $0x0,%edx
  802d7a:	eb 30                	jmp    802dac <pipe+0x14a>
  802d7c:	83 ec 08             	sub    $0x8,%esp
  802d7f:	56                   	push   %esi
  802d80:	6a 00                	push   $0x0
  802d82:	e8 fc ea ff ff       	call   801883 <sys_page_unmap>
  802d87:	83 c4 10             	add    $0x10,%esp
  802d8a:	83 ec 08             	sub    $0x8,%esp
  802d8d:	ff 75 f0             	pushl  -0x10(%ebp)
  802d90:	6a 00                	push   $0x0
  802d92:	e8 ec ea ff ff       	call   801883 <sys_page_unmap>
  802d97:	83 c4 10             	add    $0x10,%esp
  802d9a:	83 ec 08             	sub    $0x8,%esp
  802d9d:	ff 75 f4             	pushl  -0xc(%ebp)
  802da0:	6a 00                	push   $0x0
  802da2:	e8 dc ea ff ff       	call   801883 <sys_page_unmap>
  802da7:	83 c4 10             	add    $0x10,%esp
  802daa:	89 da                	mov    %ebx,%edx
  802dac:	89 d0                	mov    %edx,%eax
  802dae:	8d 65 f8             	lea    -0x8(%ebp),%esp
  802db1:	5b                   	pop    %ebx
  802db2:	5e                   	pop    %esi
  802db3:	5d                   	pop    %ebp
  802db4:	c3                   	ret    

00802db5 <pipeisclosed>:
  802db5:	55                   	push   %ebp
  802db6:	89 e5                	mov    %esp,%ebp
  802db8:	83 ec 20             	sub    $0x20,%esp
  802dbb:	8d 45 f4             	lea    -0xc(%ebp),%eax
  802dbe:	50                   	push   %eax
  802dbf:	ff 75 08             	pushl  0x8(%ebp)
  802dc2:	e8 c0 ed ff ff       	call   801b87 <fd_lookup>
  802dc7:	83 c4 10             	add    $0x10,%esp
  802dca:	85 c0                	test   %eax,%eax
  802dcc:	78 18                	js     802de6 <pipeisclosed+0x31>
  802dce:	83 ec 0c             	sub    $0xc,%esp
  802dd1:	ff 75 f4             	pushl  -0xc(%ebp)
  802dd4:	e8 48 ed ff ff       	call   801b21 <fd2data>
  802dd9:	89 c2                	mov    %eax,%edx
  802ddb:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802dde:	e8 21 fd ff ff       	call   802b04 <_pipeisclosed>
  802de3:	83 c4 10             	add    $0x10,%esp
  802de6:	c9                   	leave  
  802de7:	c3                   	ret    

00802de8 <devcons_close>:
  802de8:	55                   	push   %ebp
  802de9:	89 e5                	mov    %esp,%ebp
  802deb:	b8 00 00 00 00       	mov    $0x0,%eax
  802df0:	5d                   	pop    %ebp
  802df1:	c3                   	ret    

00802df2 <devcons_stat>:
  802df2:	55                   	push   %ebp
  802df3:	89 e5                	mov    %esp,%ebp
  802df5:	83 ec 10             	sub    $0x10,%esp
  802df8:	68 16 3a 80 00       	push   $0x803a16
  802dfd:	ff 75 0c             	pushl  0xc(%ebp)
  802e00:	e8 f6 e5 ff ff       	call   8013fb <strcpy>
  802e05:	b8 00 00 00 00       	mov    $0x0,%eax
  802e0a:	c9                   	leave  
  802e0b:	c3                   	ret    

00802e0c <devcons_write>:
  802e0c:	55                   	push   %ebp
  802e0d:	89 e5                	mov    %esp,%ebp
  802e0f:	57                   	push   %edi
  802e10:	56                   	push   %esi
  802e11:	53                   	push   %ebx
  802e12:	81 ec 8c 00 00 00    	sub    $0x8c,%esp
  802e18:	be 00 00 00 00       	mov    $0x0,%esi
  802e1d:	8d bd 68 ff ff ff    	lea    -0x98(%ebp),%edi
  802e23:	eb 2d                	jmp    802e52 <devcons_write+0x46>
  802e25:	8b 5d 10             	mov    0x10(%ebp),%ebx
  802e28:	29 f3                	sub    %esi,%ebx
  802e2a:	83 fb 7f             	cmp    $0x7f,%ebx
  802e2d:	ba 7f 00 00 00       	mov    $0x7f,%edx
  802e32:	0f 47 da             	cmova  %edx,%ebx
  802e35:	83 ec 04             	sub    $0x4,%esp
  802e38:	53                   	push   %ebx
  802e39:	03 45 0c             	add    0xc(%ebp),%eax
  802e3c:	50                   	push   %eax
  802e3d:	57                   	push   %edi
  802e3e:	e8 4a e7 ff ff       	call   80158d <memmove>
  802e43:	83 c4 08             	add    $0x8,%esp
  802e46:	53                   	push   %ebx
  802e47:	57                   	push   %edi
  802e48:	e8 f5 e8 ff ff       	call   801742 <sys_cputs>
  802e4d:	01 de                	add    %ebx,%esi
  802e4f:	83 c4 10             	add    $0x10,%esp
  802e52:	89 f0                	mov    %esi,%eax
  802e54:	3b 75 10             	cmp    0x10(%ebp),%esi
  802e57:	72 cc                	jb     802e25 <devcons_write+0x19>
  802e59:	8d 65 f4             	lea    -0xc(%ebp),%esp
  802e5c:	5b                   	pop    %ebx
  802e5d:	5e                   	pop    %esi
  802e5e:	5f                   	pop    %edi
  802e5f:	5d                   	pop    %ebp
  802e60:	c3                   	ret    

00802e61 <devcons_read>:
  802e61:	55                   	push   %ebp
  802e62:	89 e5                	mov    %esp,%ebp
  802e64:	83 ec 08             	sub    $0x8,%esp
  802e67:	b8 00 00 00 00       	mov    $0x0,%eax
  802e6c:	83 7d 10 00          	cmpl   $0x0,0x10(%ebp)
  802e70:	74 2a                	je     802e9c <devcons_read+0x3b>
  802e72:	eb 05                	jmp    802e79 <devcons_read+0x18>
  802e74:	e8 66 e9 ff ff       	call   8017df <sys_yield>
  802e79:	e8 e2 e8 ff ff       	call   801760 <sys_cgetc>
  802e7e:	85 c0                	test   %eax,%eax
  802e80:	74 f2                	je     802e74 <devcons_read+0x13>
  802e82:	85 c0                	test   %eax,%eax
  802e84:	78 16                	js     802e9c <devcons_read+0x3b>
  802e86:	83 f8 04             	cmp    $0x4,%eax
  802e89:	74 0c                	je     802e97 <devcons_read+0x36>
  802e8b:	8b 55 0c             	mov    0xc(%ebp),%edx
  802e8e:	88 02                	mov    %al,(%edx)
  802e90:	b8 01 00 00 00       	mov    $0x1,%eax
  802e95:	eb 05                	jmp    802e9c <devcons_read+0x3b>
  802e97:	b8 00 00 00 00       	mov    $0x0,%eax
  802e9c:	c9                   	leave  
  802e9d:	c3                   	ret    

00802e9e <cputchar>:
  802e9e:	55                   	push   %ebp
  802e9f:	89 e5                	mov    %esp,%ebp
  802ea1:	83 ec 20             	sub    $0x20,%esp
  802ea4:	8b 45 08             	mov    0x8(%ebp),%eax
  802ea7:	88 45 f7             	mov    %al,-0x9(%ebp)
  802eaa:	6a 01                	push   $0x1
  802eac:	8d 45 f7             	lea    -0x9(%ebp),%eax
  802eaf:	50                   	push   %eax
  802eb0:	e8 8d e8 ff ff       	call   801742 <sys_cputs>
  802eb5:	83 c4 10             	add    $0x10,%esp
  802eb8:	c9                   	leave  
  802eb9:	c3                   	ret    

00802eba <getchar>:
  802eba:	55                   	push   %ebp
  802ebb:	89 e5                	mov    %esp,%ebp
  802ebd:	83 ec 1c             	sub    $0x1c,%esp
  802ec0:	6a 01                	push   $0x1
  802ec2:	8d 45 f7             	lea    -0x9(%ebp),%eax
  802ec5:	50                   	push   %eax
  802ec6:	6a 00                	push   $0x0
  802ec8:	e8 20 ef ff ff       	call   801ded <read>
  802ecd:	83 c4 10             	add    $0x10,%esp
  802ed0:	85 c0                	test   %eax,%eax
  802ed2:	78 0f                	js     802ee3 <getchar+0x29>
  802ed4:	85 c0                	test   %eax,%eax
  802ed6:	7e 06                	jle    802ede <getchar+0x24>
  802ed8:	0f b6 45 f7          	movzbl -0x9(%ebp),%eax
  802edc:	eb 05                	jmp    802ee3 <getchar+0x29>
  802ede:	b8 f8 ff ff ff       	mov    $0xfffffff8,%eax
  802ee3:	c9                   	leave  
  802ee4:	c3                   	ret    

00802ee5 <iscons>:
  802ee5:	55                   	push   %ebp
  802ee6:	89 e5                	mov    %esp,%ebp
  802ee8:	83 ec 20             	sub    $0x20,%esp
  802eeb:	8d 45 f4             	lea    -0xc(%ebp),%eax
  802eee:	50                   	push   %eax
  802eef:	ff 75 08             	pushl  0x8(%ebp)
  802ef2:	e8 90 ec ff ff       	call   801b87 <fd_lookup>
  802ef7:	83 c4 10             	add    $0x10,%esp
  802efa:	85 c0                	test   %eax,%eax
  802efc:	78 11                	js     802f0f <iscons+0x2a>
  802efe:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802f01:	8b 15 5c 00 00 10    	mov    0x1000005c,%edx
  802f07:	39 10                	cmp    %edx,(%eax)
  802f09:	0f 94 c0             	sete   %al
  802f0c:	0f b6 c0             	movzbl %al,%eax
  802f0f:	c9                   	leave  
  802f10:	c3                   	ret    

00802f11 <opencons>:
  802f11:	55                   	push   %ebp
  802f12:	89 e5                	mov    %esp,%ebp
  802f14:	83 ec 24             	sub    $0x24,%esp
  802f17:	8d 45 f4             	lea    -0xc(%ebp),%eax
  802f1a:	50                   	push   %eax
  802f1b:	e8 18 ec ff ff       	call   801b38 <fd_alloc>
  802f20:	83 c4 10             	add    $0x10,%esp
  802f23:	89 c2                	mov    %eax,%edx
  802f25:	85 c0                	test   %eax,%eax
  802f27:	78 3e                	js     802f67 <opencons+0x56>
  802f29:	83 ec 04             	sub    $0x4,%esp
  802f2c:	68 07 04 00 00       	push   $0x407
  802f31:	ff 75 f4             	pushl  -0xc(%ebp)
  802f34:	6a 00                	push   $0x0
  802f36:	e8 c3 e8 ff ff       	call   8017fe <sys_page_alloc>
  802f3b:	83 c4 10             	add    $0x10,%esp
  802f3e:	89 c2                	mov    %eax,%edx
  802f40:	85 c0                	test   %eax,%eax
  802f42:	78 23                	js     802f67 <opencons+0x56>
  802f44:	8b 15 5c 00 00 10    	mov    0x1000005c,%edx
  802f4a:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802f4d:	89 10                	mov    %edx,(%eax)
  802f4f:	8b 45 f4             	mov    -0xc(%ebp),%eax
  802f52:	c7 40 08 02 00 00 00 	movl   $0x2,0x8(%eax)
  802f59:	83 ec 0c             	sub    $0xc,%esp
  802f5c:	50                   	push   %eax
  802f5d:	e8 af eb ff ff       	call   801b11 <fd2num>
  802f62:	89 c2                	mov    %eax,%edx
  802f64:	83 c4 10             	add    $0x10,%esp
  802f67:	89 d0                	mov    %edx,%eax
  802f69:	c9                   	leave  
  802f6a:	c3                   	ret    

00802f6b <ipc_recv>:
  802f6b:	55                   	push   %ebp
  802f6c:	89 e5                	mov    %esp,%ebp
  802f6e:	56                   	push   %esi
  802f6f:	53                   	push   %ebx
  802f70:	8b 75 08             	mov    0x8(%ebp),%esi
  802f73:	8b 45 0c             	mov    0xc(%ebp),%eax
  802f76:	8b 5d 10             	mov    0x10(%ebp),%ebx
  802f79:	85 c0                	test   %eax,%eax
  802f7b:	ba ff ff ff ff       	mov    $0xffffffff,%edx
  802f80:	0f 44 c2             	cmove  %edx,%eax
  802f83:	83 ec 0c             	sub    $0xc,%esp
  802f86:	50                   	push   %eax
  802f87:	e8 22 ea ff ff       	call   8019ae <sys_ipc_recv>
  802f8c:	83 c4 10             	add    $0x10,%esp
  802f8f:	85 c0                	test   %eax,%eax
  802f91:	79 16                	jns    802fa9 <ipc_recv+0x3e>
  802f93:	85 f6                	test   %esi,%esi
  802f95:	74 06                	je     802f9d <ipc_recv+0x32>
  802f97:	c7 06 00 00 00 00    	movl   $0x0,(%esi)
  802f9d:	85 db                	test   %ebx,%ebx
  802f9f:	74 2c                	je     802fcd <ipc_recv+0x62>
  802fa1:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  802fa7:	eb 24                	jmp    802fcd <ipc_recv+0x62>
  802fa9:	85 f6                	test   %esi,%esi
  802fab:	74 0a                	je     802fb7 <ipc_recv+0x4c>
  802fad:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  802fb2:	8b 40 74             	mov    0x74(%eax),%eax
  802fb5:	89 06                	mov    %eax,(%esi)
  802fb7:	85 db                	test   %ebx,%ebx
  802fb9:	74 0a                	je     802fc5 <ipc_recv+0x5a>
  802fbb:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  802fc0:	8b 40 78             	mov    0x78(%eax),%eax
  802fc3:	89 03                	mov    %eax,(%ebx)
  802fc5:	a1 00 e0 eb 3b       	mov    0x3bebe000,%eax
  802fca:	8b 40 70             	mov    0x70(%eax),%eax
  802fcd:	8d 65 f8             	lea    -0x8(%ebp),%esp
  802fd0:	5b                   	pop    %ebx
  802fd1:	5e                   	pop    %esi
  802fd2:	5d                   	pop    %ebp
  802fd3:	c3                   	ret    

00802fd4 <ipc_send>:
  802fd4:	55                   	push   %ebp
  802fd5:	89 e5                	mov    %esp,%ebp
  802fd7:	57                   	push   %edi
  802fd8:	56                   	push   %esi
  802fd9:	53                   	push   %ebx
  802fda:	83 ec 0c             	sub    $0xc,%esp
  802fdd:	8b 7d 08             	mov    0x8(%ebp),%edi
  802fe0:	8b 75 0c             	mov    0xc(%ebp),%esi
  802fe3:	8b 5d 10             	mov    0x10(%ebp),%ebx
  802fe6:	85 db                	test   %ebx,%ebx
  802fe8:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  802fed:	0f 44 d8             	cmove  %eax,%ebx
  802ff0:	ff 75 14             	pushl  0x14(%ebp)
  802ff3:	53                   	push   %ebx
  802ff4:	56                   	push   %esi
  802ff5:	57                   	push   %edi
  802ff6:	e8 90 e9 ff ff       	call   80198b <sys_ipc_try_send>
  802ffb:	83 c4 10             	add    $0x10,%esp
  802ffe:	85 c0                	test   %eax,%eax
  803000:	74 19                	je     80301b <ipc_send+0x47>
  803002:	83 f8 f9             	cmp    $0xfffffff9,%eax
  803005:	74 e9                	je     802ff0 <ipc_send+0x1c>
  803007:	83 ec 04             	sub    $0x4,%esp
  80300a:	68 22 3a 80 00       	push   $0x803a22
  80300f:	6a 44                	push   $0x44
  803011:	68 31 3a 80 00       	push   $0x803a31
  803016:	e8 82 dd ff ff       	call   800d9d <_panic>
  80301b:	8d 65 f4             	lea    -0xc(%ebp),%esp
  80301e:	5b                   	pop    %ebx
  80301f:	5e                   	pop    %esi
  803020:	5f                   	pop    %edi
  803021:	5d                   	pop    %ebp
  803022:	c3                   	ret    

00803023 <ipc_find_env>:
  803023:	55                   	push   %ebp
  803024:	89 e5                	mov    %esp,%ebp
  803026:	8b 4d 08             	mov    0x8(%ebp),%ecx
  803029:	b8 00 00 00 00       	mov    $0x0,%eax
  80302e:	69 d0 2c 01 00 00    	imul   $0x12c,%eax,%edx
  803034:	81 c2 00 00 c0 ee    	add    $0xeec00000,%edx
  80303a:	8b 52 50             	mov    0x50(%edx),%edx
  80303d:	39 ca                	cmp    %ecx,%edx
  80303f:	75 10                	jne    803051 <ipc_find_env+0x2e>
  803041:	69 c0 2c 01 00 00    	imul   $0x12c,%eax,%eax
  803047:	05 00 00 c0 ee       	add    $0xeec00000,%eax
  80304c:	8b 40 48             	mov    0x48(%eax),%eax
  80304f:	eb 0f                	jmp    803060 <ipc_find_env+0x3d>
  803051:	83 c0 01             	add    $0x1,%eax
  803054:	3d 00 04 00 00       	cmp    $0x400,%eax
  803059:	75 d3                	jne    80302e <ipc_find_env+0xb>
  80305b:	b8 00 00 00 00       	mov    $0x0,%eax
  803060:	5d                   	pop    %ebp
  803061:	c3                   	ret    

00803062 <pageref>:
  803062:	55                   	push   %ebp
  803063:	89 e5                	mov    %esp,%ebp
  803065:	8b 55 08             	mov    0x8(%ebp),%edx
  803068:	89 d0                	mov    %edx,%eax
  80306a:	c1 e8 16             	shr    $0x16,%eax
  80306d:	8b 0c 85 00 d0 7b ef 	mov    -0x10843000(,%eax,4),%ecx
  803074:	b8 00 00 00 00       	mov    $0x0,%eax
  803079:	f6 c1 01             	test   $0x1,%cl
  80307c:	74 23                	je     8030a1 <pageref+0x3f>
  80307e:	c1 ea 0c             	shr    $0xc,%edx
  803081:	8b 14 95 00 00 40 ef 	mov    -0x10c00000(,%edx,4),%edx
  803088:	f6 c2 01             	test   $0x1,%dl
  80308b:	74 14                	je     8030a1 <pageref+0x3f>
  80308d:	c1 ea 0c             	shr    $0xc,%edx
  803090:	8d 04 52             	lea    (%edx,%edx,2),%eax
  803093:	8d 04 c5 00 00 00 ef 	lea    -0x11000000(,%eax,8),%eax
  80309a:	0f b7 40 14          	movzwl 0x14(%eax),%eax
  80309e:	0f b7 c0             	movzwl %ax,%eax
  8030a1:	5d                   	pop    %ebp
  8030a2:	c3                   	ret    
  8030a3:	66 90                	xchg   %ax,%ax
  8030a5:	66 90                	xchg   %ax,%ax
  8030a7:	66 90                	xchg   %ax,%ax
  8030a9:	66 90                	xchg   %ax,%ax
  8030ab:	66 90                	xchg   %ax,%ax
  8030ad:	66 90                	xchg   %ax,%ax
  8030af:	90                   	nop

008030b0 <__moddi3>:
  8030b0:	55                   	push   %ebp
  8030b1:	57                   	push   %edi
  8030b2:	31 ed                	xor    %ebp,%ebp
  8030b4:	56                   	push   %esi
  8030b5:	53                   	push   %ebx
  8030b6:	83 ec 2c             	sub    $0x2c,%esp
  8030b9:	8b 5c 24 44          	mov    0x44(%esp),%ebx
  8030bd:	8b 44 24 4c          	mov    0x4c(%esp),%eax
  8030c1:	8b 7c 24 40          	mov    0x40(%esp),%edi
  8030c5:	8b 74 24 48          	mov    0x48(%esp),%esi
  8030c9:	85 db                	test   %ebx,%ebx
  8030cb:	89 c2                	mov    %eax,%edx
  8030cd:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
  8030d1:	89 7c 24 08          	mov    %edi,0x8(%esp)
  8030d5:	0f 88 b5 00 00 00    	js     803190 <__moddi3+0xe0>
  8030db:	85 d2                	test   %edx,%edx
  8030dd:	89 f1                	mov    %esi,%ecx
  8030df:	89 c3                	mov    %eax,%ebx
  8030e1:	0f 88 99 00 00 00    	js     803180 <__moddi3+0xd0>
  8030e7:	8b 74 24 08          	mov    0x8(%esp),%esi
  8030eb:	8b 7c 24 0c          	mov    0xc(%esp),%edi
  8030ef:	89 4c 24 10          	mov    %ecx,0x10(%esp)
  8030f3:	89 f0                	mov    %esi,%eax
  8030f5:	89 fe                	mov    %edi,%esi
  8030f7:	89 df                	mov    %ebx,%edi
  8030f9:	85 ff                	test   %edi,%edi
  8030fb:	8b 5c 24 08          	mov    0x8(%esp),%ebx
  8030ff:	89 74 24 14          	mov    %esi,0x14(%esp)
  803103:	75 13                	jne    803118 <__moddi3+0x68>
  803105:	39 f1                	cmp    %esi,%ecx
  803107:	76 57                	jbe    803160 <__moddi3+0xb0>
  803109:	89 f2                	mov    %esi,%edx
  80310b:	f7 f1                	div    %ecx
  80310d:	89 d6                	mov    %edx,%esi
  80310f:	89 f0                	mov    %esi,%eax
  803111:	31 d2                	xor    %edx,%edx
  803113:	eb 0d                	jmp    803122 <__moddi3+0x72>
  803115:	8d 76 00             	lea    0x0(%esi),%esi
  803118:	39 f7                	cmp    %esi,%edi
  80311a:	76 1c                	jbe    803138 <__moddi3+0x88>
  80311c:	8b 44 24 08          	mov    0x8(%esp),%eax
  803120:	89 f2                	mov    %esi,%edx
  803122:	85 ed                	test   %ebp,%ebp
  803124:	74 07                	je     80312d <__moddi3+0x7d>
  803126:	f7 d8                	neg    %eax
  803128:	83 d2 00             	adc    $0x0,%edx
  80312b:	f7 da                	neg    %edx
  80312d:	83 c4 2c             	add    $0x2c,%esp
  803130:	5b                   	pop    %ebx
  803131:	5e                   	pop    %esi
  803132:	5f                   	pop    %edi
  803133:	5d                   	pop    %ebp
  803134:	c3                   	ret    
  803135:	8d 76 00             	lea    0x0(%esi),%esi
  803138:	0f bd d7             	bsr    %edi,%edx
  80313b:	83 f2 1f             	xor    $0x1f,%edx
  80313e:	89 54 24 08          	mov    %edx,0x8(%esp)
  803142:	75 6c                	jne    8031b0 <__moddi3+0x100>
  803144:	39 f7                	cmp    %esi,%edi
  803146:	0f 82 14 01 00 00    	jb     803260 <__moddi3+0x1b0>
  80314c:	39 d9                	cmp    %ebx,%ecx
  80314e:	0f 86 0c 01 00 00    	jbe    803260 <__moddi3+0x1b0>
  803154:	89 d8                	mov    %ebx,%eax
  803156:	8b 54 24 14          	mov    0x14(%esp),%edx
  80315a:	eb c6                	jmp    803122 <__moddi3+0x72>
  80315c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  803160:	85 c9                	test   %ecx,%ecx
  803162:	75 0b                	jne    80316f <__moddi3+0xbf>
  803164:	b8 01 00 00 00       	mov    $0x1,%eax
  803169:	31 d2                	xor    %edx,%edx
  80316b:	f7 f1                	div    %ecx
  80316d:	89 c1                	mov    %eax,%ecx
  80316f:	89 f0                	mov    %esi,%eax
  803171:	31 d2                	xor    %edx,%edx
  803173:	f7 f1                	div    %ecx
  803175:	89 d8                	mov    %ebx,%eax
  803177:	f7 f1                	div    %ecx
  803179:	89 d6                	mov    %edx,%esi
  80317b:	eb 92                	jmp    80310f <__moddi3+0x5f>
  80317d:	8d 76 00             	lea    0x0(%esi),%esi
  803180:	f7 d9                	neg    %ecx
  803182:	83 d3 00             	adc    $0x0,%ebx
  803185:	f7 db                	neg    %ebx
  803187:	e9 5b ff ff ff       	jmp    8030e7 <__moddi3+0x37>
  80318c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  803190:	f7 5c 24 08          	negl   0x8(%esp)
  803194:	bd ff ff ff ff       	mov    $0xffffffff,%ebp
  803199:	83 54 24 0c 00       	adcl   $0x0,0xc(%esp)
  80319e:	f7 5c 24 0c          	negl   0xc(%esp)
  8031a2:	e9 34 ff ff ff       	jmp    8030db <__moddi3+0x2b>
  8031a7:	89 f6                	mov    %esi,%esi
  8031a9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  8031b0:	8b 4c 24 08          	mov    0x8(%esp),%ecx
  8031b4:	ba 20 00 00 00       	mov    $0x20,%edx
  8031b9:	29 ca                	sub    %ecx,%edx
  8031bb:	d3 e7                	shl    %cl,%edi
  8031bd:	89 d3                	mov    %edx,%ebx
  8031bf:	89 54 24 18          	mov    %edx,0x18(%esp)
  8031c3:	8b 54 24 10          	mov    0x10(%esp),%edx
  8031c7:	89 d9                	mov    %ebx,%ecx
  8031c9:	d3 ea                	shr    %cl,%edx
  8031cb:	0f b6 4c 24 08       	movzbl 0x8(%esp),%ecx
  8031d0:	09 d7                	or     %edx,%edi
  8031d2:	8b 54 24 10          	mov    0x10(%esp),%edx
  8031d6:	89 7c 24 14          	mov    %edi,0x14(%esp)
  8031da:	89 f7                	mov    %esi,%edi
  8031dc:	d3 e2                	shl    %cl,%edx
  8031de:	89 d9                	mov    %ebx,%ecx
  8031e0:	d3 ef                	shr    %cl,%edi
  8031e2:	0f b6 4c 24 08       	movzbl 0x8(%esp),%ecx
  8031e7:	89 54 24 10          	mov    %edx,0x10(%esp)
  8031eb:	89 da                	mov    %ebx,%edx
  8031ed:	89 fb                	mov    %edi,%ebx
  8031ef:	89 c7                	mov    %eax,%edi
  8031f1:	d3 e6                	shl    %cl,%esi
  8031f3:	89 d1                	mov    %edx,%ecx
  8031f5:	89 da                	mov    %ebx,%edx
  8031f7:	d3 ef                	shr    %cl,%edi
  8031f9:	0f b6 4c 24 08       	movzbl 0x8(%esp),%ecx
  8031fe:	09 fe                	or     %edi,%esi
  803200:	d3 e0                	shl    %cl,%eax
  803202:	89 c7                	mov    %eax,%edi
  803204:	89 44 24 1c          	mov    %eax,0x1c(%esp)
  803208:	89 f0                	mov    %esi,%eax
  80320a:	f7 74 24 14          	divl   0x14(%esp)
  80320e:	89 d3                	mov    %edx,%ebx
  803210:	f7 64 24 10          	mull   0x10(%esp)
  803214:	39 d3                	cmp    %edx,%ebx
  803216:	89 c6                	mov    %eax,%esi
  803218:	89 d1                	mov    %edx,%ecx
  80321a:	72 06                	jb     803222 <__moddi3+0x172>
  80321c:	75 10                	jne    80322e <__moddi3+0x17e>
  80321e:	39 c7                	cmp    %eax,%edi
  803220:	73 0c                	jae    80322e <__moddi3+0x17e>
  803222:	2b 44 24 10          	sub    0x10(%esp),%eax
  803226:	1b 54 24 14          	sbb    0x14(%esp),%edx
  80322a:	89 d1                	mov    %edx,%ecx
  80322c:	89 c6                	mov    %eax,%esi
  80322e:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  803232:	8b 7c 24 08          	mov    0x8(%esp),%edi
  803236:	29 f0                	sub    %esi,%eax
  803238:	19 cb                	sbb    %ecx,%ebx
  80323a:	0f b6 4c 24 18       	movzbl 0x18(%esp),%ecx
  80323f:	89 de                	mov    %ebx,%esi
  803241:	d3 e6                	shl    %cl,%esi
  803243:	89 f9                	mov    %edi,%ecx
  803245:	d3 e8                	shr    %cl,%eax
  803247:	89 f9                	mov    %edi,%ecx
  803249:	09 c6                	or     %eax,%esi
  80324b:	d3 eb                	shr    %cl,%ebx
  80324d:	89 f0                	mov    %esi,%eax
  80324f:	89 da                	mov    %ebx,%edx
  803251:	e9 cc fe ff ff       	jmp    803122 <__moddi3+0x72>
  803256:	8d 76 00             	lea    0x0(%esi),%esi
  803259:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi
  803260:	29 cb                	sub    %ecx,%ebx
  803262:	19 fe                	sbb    %edi,%esi
  803264:	89 74 24 14          	mov    %esi,0x14(%esp)
  803268:	e9 e7 fe ff ff       	jmp    803154 <__moddi3+0xa4>
  80326d:	66 90                	xchg   %ax,%ax
  80326f:	90                   	nop

00803270 <__udivdi3>:
  803270:	55                   	push   %ebp
  803271:	57                   	push   %edi
  803272:	56                   	push   %esi
  803273:	53                   	push   %ebx
  803274:	83 ec 1c             	sub    $0x1c,%esp
  803277:	8b 74 24 3c          	mov    0x3c(%esp),%esi
  80327b:	8b 5c 24 30          	mov    0x30(%esp),%ebx
  80327f:	8b 4c 24 34          	mov    0x34(%esp),%ecx
  803283:	8b 7c 24 38          	mov    0x38(%esp),%edi
  803287:	85 f6                	test   %esi,%esi
  803289:	89 5c 24 08          	mov    %ebx,0x8(%esp)
  80328d:	89 ca                	mov    %ecx,%edx
  80328f:	89 f8                	mov    %edi,%eax
  803291:	75 3d                	jne    8032d0 <__udivdi3+0x60>
  803293:	39 cf                	cmp    %ecx,%edi
  803295:	0f 87 c5 00 00 00    	ja     803360 <__udivdi3+0xf0>
  80329b:	85 ff                	test   %edi,%edi
  80329d:	89 fd                	mov    %edi,%ebp
  80329f:	75 0b                	jne    8032ac <__udivdi3+0x3c>
  8032a1:	b8 01 00 00 00       	mov    $0x1,%eax
  8032a6:	31 d2                	xor    %edx,%edx
  8032a8:	f7 f7                	div    %edi
  8032aa:	89 c5                	mov    %eax,%ebp
  8032ac:	89 c8                	mov    %ecx,%eax
  8032ae:	31 d2                	xor    %edx,%edx
  8032b0:	f7 f5                	div    %ebp
  8032b2:	89 c1                	mov    %eax,%ecx
  8032b4:	89 d8                	mov    %ebx,%eax
  8032b6:	89 cf                	mov    %ecx,%edi
  8032b8:	f7 f5                	div    %ebp
  8032ba:	89 c3                	mov    %eax,%ebx
  8032bc:	89 d8                	mov    %ebx,%eax
  8032be:	89 fa                	mov    %edi,%edx
  8032c0:	83 c4 1c             	add    $0x1c,%esp
  8032c3:	5b                   	pop    %ebx
  8032c4:	5e                   	pop    %esi
  8032c5:	5f                   	pop    %edi
  8032c6:	5d                   	pop    %ebp
  8032c7:	c3                   	ret    
  8032c8:	90                   	nop
  8032c9:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  8032d0:	39 ce                	cmp    %ecx,%esi
  8032d2:	77 74                	ja     803348 <__udivdi3+0xd8>
  8032d4:	0f bd fe             	bsr    %esi,%edi
  8032d7:	83 f7 1f             	xor    $0x1f,%edi
  8032da:	0f 84 98 00 00 00    	je     803378 <__udivdi3+0x108>
  8032e0:	bb 20 00 00 00       	mov    $0x20,%ebx
  8032e5:	89 f9                	mov    %edi,%ecx
  8032e7:	89 c5                	mov    %eax,%ebp
  8032e9:	29 fb                	sub    %edi,%ebx
  8032eb:	d3 e6                	shl    %cl,%esi
  8032ed:	89 d9                	mov    %ebx,%ecx
  8032ef:	d3 ed                	shr    %cl,%ebp
  8032f1:	89 f9                	mov    %edi,%ecx
  8032f3:	d3 e0                	shl    %cl,%eax
  8032f5:	09 ee                	or     %ebp,%esi
  8032f7:	89 d9                	mov    %ebx,%ecx
  8032f9:	89 44 24 0c          	mov    %eax,0xc(%esp)
  8032fd:	89 d5                	mov    %edx,%ebp
  8032ff:	8b 44 24 08          	mov    0x8(%esp),%eax
  803303:	d3 ed                	shr    %cl,%ebp
  803305:	89 f9                	mov    %edi,%ecx
  803307:	d3 e2                	shl    %cl,%edx
  803309:	89 d9                	mov    %ebx,%ecx
  80330b:	d3 e8                	shr    %cl,%eax
  80330d:	09 c2                	or     %eax,%edx
  80330f:	89 d0                	mov    %edx,%eax
  803311:	89 ea                	mov    %ebp,%edx
  803313:	f7 f6                	div    %esi
  803315:	89 d5                	mov    %edx,%ebp
  803317:	89 c3                	mov    %eax,%ebx
  803319:	f7 64 24 0c          	mull   0xc(%esp)
  80331d:	39 d5                	cmp    %edx,%ebp
  80331f:	72 10                	jb     803331 <__udivdi3+0xc1>
  803321:	8b 74 24 08          	mov    0x8(%esp),%esi
  803325:	89 f9                	mov    %edi,%ecx
  803327:	d3 e6                	shl    %cl,%esi
  803329:	39 c6                	cmp    %eax,%esi
  80332b:	73 07                	jae    803334 <__udivdi3+0xc4>
  80332d:	39 d5                	cmp    %edx,%ebp
  80332f:	75 03                	jne    803334 <__udivdi3+0xc4>
  803331:	83 eb 01             	sub    $0x1,%ebx
  803334:	31 ff                	xor    %edi,%edi
  803336:	89 d8                	mov    %ebx,%eax
  803338:	89 fa                	mov    %edi,%edx
  80333a:	83 c4 1c             	add    $0x1c,%esp
  80333d:	5b                   	pop    %ebx
  80333e:	5e                   	pop    %esi
  80333f:	5f                   	pop    %edi
  803340:	5d                   	pop    %ebp
  803341:	c3                   	ret    
  803342:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  803348:	31 ff                	xor    %edi,%edi
  80334a:	31 db                	xor    %ebx,%ebx
  80334c:	89 d8                	mov    %ebx,%eax
  80334e:	89 fa                	mov    %edi,%edx
  803350:	83 c4 1c             	add    $0x1c,%esp
  803353:	5b                   	pop    %ebx
  803354:	5e                   	pop    %esi
  803355:	5f                   	pop    %edi
  803356:	5d                   	pop    %ebp
  803357:	c3                   	ret    
  803358:	90                   	nop
  803359:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  803360:	89 d8                	mov    %ebx,%eax
  803362:	f7 f7                	div    %edi
  803364:	31 ff                	xor    %edi,%edi
  803366:	89 c3                	mov    %eax,%ebx
  803368:	89 d8                	mov    %ebx,%eax
  80336a:	89 fa                	mov    %edi,%edx
  80336c:	83 c4 1c             	add    $0x1c,%esp
  80336f:	5b                   	pop    %ebx
  803370:	5e                   	pop    %esi
  803371:	5f                   	pop    %edi
  803372:	5d                   	pop    %ebp
  803373:	c3                   	ret    
  803374:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  803378:	39 ce                	cmp    %ecx,%esi
  80337a:	72 0c                	jb     803388 <__udivdi3+0x118>
  80337c:	31 db                	xor    %ebx,%ebx
  80337e:	3b 44 24 08          	cmp    0x8(%esp),%eax
  803382:	0f 87 34 ff ff ff    	ja     8032bc <__udivdi3+0x4c>
  803388:	bb 01 00 00 00       	mov    $0x1,%ebx
  80338d:	e9 2a ff ff ff       	jmp    8032bc <__udivdi3+0x4c>
  803392:	66 90                	xchg   %ax,%ax
  803394:	66 90                	xchg   %ax,%ax
  803396:	66 90                	xchg   %ax,%ax
  803398:	66 90                	xchg   %ax,%ax
  80339a:	66 90                	xchg   %ax,%ax
  80339c:	66 90                	xchg   %ax,%ax
  80339e:	66 90                	xchg   %ax,%ax

008033a0 <__umoddi3>:
  8033a0:	55                   	push   %ebp
  8033a1:	57                   	push   %edi
  8033a2:	56                   	push   %esi
  8033a3:	53                   	push   %ebx
  8033a4:	83 ec 1c             	sub    $0x1c,%esp
  8033a7:	8b 54 24 3c          	mov    0x3c(%esp),%edx
  8033ab:	8b 4c 24 30          	mov    0x30(%esp),%ecx
  8033af:	8b 74 24 34          	mov    0x34(%esp),%esi
  8033b3:	8b 7c 24 38          	mov    0x38(%esp),%edi
  8033b7:	85 d2                	test   %edx,%edx
  8033b9:	89 4c 24 0c          	mov    %ecx,0xc(%esp)
  8033bd:	89 4c 24 08          	mov    %ecx,0x8(%esp)
  8033c1:	89 f3                	mov    %esi,%ebx
  8033c3:	89 3c 24             	mov    %edi,(%esp)
  8033c6:	89 74 24 04          	mov    %esi,0x4(%esp)
  8033ca:	75 1c                	jne    8033e8 <__umoddi3+0x48>
  8033cc:	39 f7                	cmp    %esi,%edi
  8033ce:	76 50                	jbe    803420 <__umoddi3+0x80>
  8033d0:	89 c8                	mov    %ecx,%eax
  8033d2:	89 f2                	mov    %esi,%edx
  8033d4:	f7 f7                	div    %edi
  8033d6:	89 d0                	mov    %edx,%eax
  8033d8:	31 d2                	xor    %edx,%edx
  8033da:	83 c4 1c             	add    $0x1c,%esp
  8033dd:	5b                   	pop    %ebx
  8033de:	5e                   	pop    %esi
  8033df:	5f                   	pop    %edi
  8033e0:	5d                   	pop    %ebp
  8033e1:	c3                   	ret    
  8033e2:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  8033e8:	39 f2                	cmp    %esi,%edx
  8033ea:	89 d0                	mov    %edx,%eax
  8033ec:	77 52                	ja     803440 <__umoddi3+0xa0>
  8033ee:	0f bd ea             	bsr    %edx,%ebp
  8033f1:	83 f5 1f             	xor    $0x1f,%ebp
  8033f4:	75 5a                	jne    803450 <__umoddi3+0xb0>
  8033f6:	3b 54 24 04          	cmp    0x4(%esp),%edx
  8033fa:	0f 82 e0 00 00 00    	jb     8034e0 <__umoddi3+0x140>
  803400:	39 0c 24             	cmp    %ecx,(%esp)
  803403:	0f 86 d7 00 00 00    	jbe    8034e0 <__umoddi3+0x140>
  803409:	8b 44 24 08          	mov    0x8(%esp),%eax
  80340d:	8b 54 24 04          	mov    0x4(%esp),%edx
  803411:	83 c4 1c             	add    $0x1c,%esp
  803414:	5b                   	pop    %ebx
  803415:	5e                   	pop    %esi
  803416:	5f                   	pop    %edi
  803417:	5d                   	pop    %ebp
  803418:	c3                   	ret    
  803419:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  803420:	85 ff                	test   %edi,%edi
  803422:	89 fd                	mov    %edi,%ebp
  803424:	75 0b                	jne    803431 <__umoddi3+0x91>
  803426:	b8 01 00 00 00       	mov    $0x1,%eax
  80342b:	31 d2                	xor    %edx,%edx
  80342d:	f7 f7                	div    %edi
  80342f:	89 c5                	mov    %eax,%ebp
  803431:	89 f0                	mov    %esi,%eax
  803433:	31 d2                	xor    %edx,%edx
  803435:	f7 f5                	div    %ebp
  803437:	89 c8                	mov    %ecx,%eax
  803439:	f7 f5                	div    %ebp
  80343b:	89 d0                	mov    %edx,%eax
  80343d:	eb 99                	jmp    8033d8 <__umoddi3+0x38>
  80343f:	90                   	nop
  803440:	89 c8                	mov    %ecx,%eax
  803442:	89 f2                	mov    %esi,%edx
  803444:	83 c4 1c             	add    $0x1c,%esp
  803447:	5b                   	pop    %ebx
  803448:	5e                   	pop    %esi
  803449:	5f                   	pop    %edi
  80344a:	5d                   	pop    %ebp
  80344b:	c3                   	ret    
  80344c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  803450:	8b 34 24             	mov    (%esp),%esi
  803453:	bf 20 00 00 00       	mov    $0x20,%edi
  803458:	89 e9                	mov    %ebp,%ecx
  80345a:	29 ef                	sub    %ebp,%edi
  80345c:	d3 e0                	shl    %cl,%eax
  80345e:	89 f9                	mov    %edi,%ecx
  803460:	89 f2                	mov    %esi,%edx
  803462:	d3 ea                	shr    %cl,%edx
  803464:	89 e9                	mov    %ebp,%ecx
  803466:	09 c2                	or     %eax,%edx
  803468:	89 d8                	mov    %ebx,%eax
  80346a:	89 14 24             	mov    %edx,(%esp)
  80346d:	89 f2                	mov    %esi,%edx
  80346f:	d3 e2                	shl    %cl,%edx
  803471:	89 f9                	mov    %edi,%ecx
  803473:	89 54 24 04          	mov    %edx,0x4(%esp)
  803477:	8b 54 24 0c          	mov    0xc(%esp),%edx
  80347b:	d3 e8                	shr    %cl,%eax
  80347d:	89 e9                	mov    %ebp,%ecx
  80347f:	89 c6                	mov    %eax,%esi
  803481:	d3 e3                	shl    %cl,%ebx
  803483:	89 f9                	mov    %edi,%ecx
  803485:	89 d0                	mov    %edx,%eax
  803487:	d3 e8                	shr    %cl,%eax
  803489:	89 e9                	mov    %ebp,%ecx
  80348b:	09 d8                	or     %ebx,%eax
  80348d:	89 d3                	mov    %edx,%ebx
  80348f:	89 f2                	mov    %esi,%edx
  803491:	f7 34 24             	divl   (%esp)
  803494:	89 d6                	mov    %edx,%esi
  803496:	d3 e3                	shl    %cl,%ebx
  803498:	f7 64 24 04          	mull   0x4(%esp)
  80349c:	39 d6                	cmp    %edx,%esi
  80349e:	89 5c 24 08          	mov    %ebx,0x8(%esp)
  8034a2:	89 d1                	mov    %edx,%ecx
  8034a4:	89 c3                	mov    %eax,%ebx
  8034a6:	72 08                	jb     8034b0 <__umoddi3+0x110>
  8034a8:	75 11                	jne    8034bb <__umoddi3+0x11b>
  8034aa:	39 44 24 08          	cmp    %eax,0x8(%esp)
  8034ae:	73 0b                	jae    8034bb <__umoddi3+0x11b>
  8034b0:	2b 44 24 04          	sub    0x4(%esp),%eax
  8034b4:	1b 14 24             	sbb    (%esp),%edx
  8034b7:	89 d1                	mov    %edx,%ecx
  8034b9:	89 c3                	mov    %eax,%ebx
  8034bb:	8b 54 24 08          	mov    0x8(%esp),%edx
  8034bf:	29 da                	sub    %ebx,%edx
  8034c1:	19 ce                	sbb    %ecx,%esi
  8034c3:	89 f9                	mov    %edi,%ecx
  8034c5:	89 f0                	mov    %esi,%eax
  8034c7:	d3 e0                	shl    %cl,%eax
  8034c9:	89 e9                	mov    %ebp,%ecx
  8034cb:	d3 ea                	shr    %cl,%edx
  8034cd:	89 e9                	mov    %ebp,%ecx
  8034cf:	d3 ee                	shr    %cl,%esi
  8034d1:	09 d0                	or     %edx,%eax
  8034d3:	89 f2                	mov    %esi,%edx
  8034d5:	83 c4 1c             	add    $0x1c,%esp
  8034d8:	5b                   	pop    %ebx
  8034d9:	5e                   	pop    %esi
  8034da:	5f                   	pop    %edi
  8034db:	5d                   	pop    %ebp
  8034dc:	c3                   	ret    
  8034dd:	8d 76 00             	lea    0x0(%esi),%esi
  8034e0:	29 f9                	sub    %edi,%ecx
  8034e2:	19 d6                	sbb    %edx,%esi
  8034e4:	89 74 24 04          	mov    %esi,0x4(%esp)
  8034e8:	89 4c 24 08          	mov    %ecx,0x8(%esp)
  8034ec:	e9 18 ff ff ff       	jmp    803409 <__umoddi3+0x69>
