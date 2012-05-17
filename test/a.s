
a:     file format elf32-i386


Disassembly of section .init:

080482b4 <_init>:
 80482b4:	55                   	push   %ebp
 80482b5:	89 e5                	mov    %esp,%ebp
 80482b7:	53                   	push   %ebx
 80482b8:	83 ec 04             	sub    $0x4,%esp
 80482bb:	e8 00 00 00 00       	call   80482c0 <_init+0xc>
 80482c0:	5b                   	pop    %ebx
 80482c1:	81 c3 34 1d 00 00    	add    $0x1d34,%ebx
 80482c7:	8b 93 fc ff ff ff    	mov    -0x4(%ebx),%edx
 80482cd:	85 d2                	test   %edx,%edx
 80482cf:	74 05                	je     80482d6 <_init+0x22>
 80482d1:	e8 1e 00 00 00       	call   80482f4 <__gmon_start__@plt>
 80482d6:	e8 e5 00 00 00       	call   80483c0 <frame_dummy>
 80482db:	e8 00 03 00 00       	call   80485e0 <__do_global_ctors_aux>
 80482e0:	58                   	pop    %eax
 80482e1:	5b                   	pop    %ebx
 80482e2:	c9                   	leave  
 80482e3:	c3                   	ret    

Disassembly of section .plt:

080482e4 <__gmon_start__@plt-0x10>:
 80482e4:	ff 35 f8 9f 04 08    	pushl  0x8049ff8
 80482ea:	ff 25 fc 9f 04 08    	jmp    *0x8049ffc
 80482f0:	00 00                	add    %al,(%eax)
	...

080482f4 <__gmon_start__@plt>:
 80482f4:	ff 25 00 a0 04 08    	jmp    *0x804a000
 80482fa:	68 00 00 00 00       	push   $0x0
 80482ff:	e9 e0 ff ff ff       	jmp    80482e4 <_init+0x30>

08048304 <__printf_chk@plt>:
 8048304:	ff 25 04 a0 04 08    	jmp    *0x804a004
 804830a:	68 08 00 00 00       	push   $0x8
 804830f:	e9 d0 ff ff ff       	jmp    80482e4 <_init+0x30>

08048314 <__libc_start_main@plt>:
 8048314:	ff 25 08 a0 04 08    	jmp    *0x804a008
 804831a:	68 10 00 00 00       	push   $0x10
 804831f:	e9 c0 ff ff ff       	jmp    80482e4 <_init+0x30>

Disassembly of section .text:

08048330 <_start>:
 8048330:	31 ed                	xor    %ebp,%ebp
 8048332:	5e                   	pop    %esi
 8048333:	89 e1                	mov    %esp,%ecx
 8048335:	83 e4 f0             	and    $0xfffffff0,%esp
 8048338:	50                   	push   %eax
 8048339:	54                   	push   %esp
 804833a:	52                   	push   %edx
 804833b:	68 d0 85 04 08       	push   $0x80485d0
 8048340:	68 70 85 04 08       	push   $0x8048570
 8048345:	51                   	push   %ecx
 8048346:	56                   	push   %esi
 8048347:	68 49 85 04 08       	push   $0x8048549
 804834c:	e8 c3 ff ff ff       	call   8048314 <__libc_start_main@plt>
 8048351:	f4                   	hlt    
 8048352:	90                   	nop
 8048353:	90                   	nop
 8048354:	90                   	nop
 8048355:	90                   	nop
 8048356:	90                   	nop
 8048357:	90                   	nop
 8048358:	90                   	nop
 8048359:	90                   	nop
 804835a:	90                   	nop
 804835b:	90                   	nop
 804835c:	90                   	nop
 804835d:	90                   	nop
 804835e:	90                   	nop
 804835f:	90                   	nop

08048360 <__do_global_dtors_aux>:
 8048360:	55                   	push   %ebp
 8048361:	89 e5                	mov    %esp,%ebp
 8048363:	53                   	push   %ebx
 8048364:	83 ec 04             	sub    $0x4,%esp
 8048367:	80 3d 14 a0 04 08 00 	cmpb   $0x0,0x804a014
 804836e:	75 3f                	jne    80483af <__do_global_dtors_aux+0x4f>
 8048370:	a1 18 a0 04 08       	mov    0x804a018,%eax
 8048375:	bb 20 9f 04 08       	mov    $0x8049f20,%ebx
 804837a:	81 eb 1c 9f 04 08    	sub    $0x8049f1c,%ebx
 8048380:	c1 fb 02             	sar    $0x2,%ebx
 8048383:	83 eb 01             	sub    $0x1,%ebx
 8048386:	39 d8                	cmp    %ebx,%eax
 8048388:	73 1e                	jae    80483a8 <__do_global_dtors_aux+0x48>
 804838a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
 8048390:	83 c0 01             	add    $0x1,%eax
 8048393:	a3 18 a0 04 08       	mov    %eax,0x804a018
 8048398:	ff 14 85 1c 9f 04 08 	call   *0x8049f1c(,%eax,4)
 804839f:	a1 18 a0 04 08       	mov    0x804a018,%eax
 80483a4:	39 d8                	cmp    %ebx,%eax
 80483a6:	72 e8                	jb     8048390 <__do_global_dtors_aux+0x30>
 80483a8:	c6 05 14 a0 04 08 01 	movb   $0x1,0x804a014
 80483af:	83 c4 04             	add    $0x4,%esp
 80483b2:	5b                   	pop    %ebx
 80483b3:	5d                   	pop    %ebp
 80483b4:	c3                   	ret    
 80483b5:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 80483b9:	8d bc 27 00 00 00 00 	lea    0x0(%edi,%eiz,1),%edi

080483c0 <frame_dummy>:
 80483c0:	55                   	push   %ebp
 80483c1:	89 e5                	mov    %esp,%ebp
 80483c3:	83 ec 18             	sub    $0x18,%esp
 80483c6:	a1 24 9f 04 08       	mov    0x8049f24,%eax
 80483cb:	85 c0                	test   %eax,%eax
 80483cd:	74 12                	je     80483e1 <frame_dummy+0x21>
 80483cf:	b8 00 00 00 00       	mov    $0x0,%eax
 80483d4:	85 c0                	test   %eax,%eax
 80483d6:	74 09                	je     80483e1 <frame_dummy+0x21>
 80483d8:	c7 04 24 24 9f 04 08 	movl   $0x8049f24,(%esp)
 80483df:	ff d0                	call   *%eax
 80483e1:	c9                   	leave  
 80483e2:	c3                   	ret    
 80483e3:	90                   	nop

080483e4 <bbbbbbbbb>:
 80483e4:	55                   	push   %ebp
 80483e5:	89 e5                	mov    %esp,%ebp
 80483e7:	53                   	push   %ebx
 80483e8:	83 ec 14             	sub    $0x14,%esp
 80483eb:	8b 5d 08             	mov    0x8(%ebp),%ebx
 80483ee:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 80483f2:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 80483f9:	08 
 80483fa:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048401:	e8 fe fe ff ff       	call   8048304 <__printf_chk@plt>
 8048406:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 804840a:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 8048411:	08 
 8048412:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048419:	e8 e6 fe ff ff       	call   8048304 <__printf_chk@plt>
 804841e:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 8048422:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 8048429:	08 
 804842a:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048431:	e8 ce fe ff ff       	call   8048304 <__printf_chk@plt>
 8048436:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 804843a:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 8048441:	08 
 8048442:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048449:	e8 b6 fe ff ff       	call   8048304 <__printf_chk@plt>
 804844e:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 8048452:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 8048459:	08 
 804845a:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048461:	e8 9e fe ff ff       	call   8048304 <__printf_chk@plt>
 8048466:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 804846a:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 8048471:	08 
 8048472:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048479:	e8 86 fe ff ff       	call   8048304 <__printf_chk@plt>
 804847e:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 8048482:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 8048489:	08 
 804848a:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048491:	e8 6e fe ff ff       	call   8048304 <__printf_chk@plt>
 8048496:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 804849a:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 80484a1:	08 
 80484a2:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 80484a9:	e8 56 fe ff ff       	call   8048304 <__printf_chk@plt>
 80484ae:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 80484b2:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 80484b9:	08 
 80484ba:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 80484c1:	e8 3e fe ff ff       	call   8048304 <__printf_chk@plt>
 80484c6:	89 1c 24             	mov    %ebx,(%esp)
 80484c9:	e8 16 ff ff ff       	call   80483e4 <bbbbbbbbb>
 80484ce:	b8 00 00 00 00       	mov    $0x0,%eax
 80484d3:	83 c4 14             	add    $0x14,%esp
 80484d6:	5b                   	pop    %ebx
 80484d7:	5d                   	pop    %ebp
 80484d8:	c3                   	ret    

080484d9 <aaaaaaaaaaa>:
 80484d9:	55                   	push   %ebp
 80484da:	89 e5                	mov    %esp,%ebp
 80484dc:	53                   	push   %ebx
 80484dd:	83 ec 14             	sub    $0x14,%esp
 80484e0:	8b 5d 08             	mov    0x8(%ebp),%ebx
 80484e3:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 80484e7:	c7 44 24 04 41 86 04 	movl   $0x8048641,0x4(%esp)
 80484ee:	08 
 80484ef:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 80484f6:	e8 09 fe ff ff       	call   8048304 <__printf_chk@plt>
 80484fb:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 80484ff:	c7 44 24 04 4d 86 04 	movl   $0x804864d,0x4(%esp)
 8048506:	08 
 8048507:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 804850e:	e8 f1 fd ff ff       	call   8048304 <__printf_chk@plt>
 8048513:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 8048517:	c7 44 24 04 30 86 04 	movl   $0x8048630,0x4(%esp)
 804851e:	08 
 804851f:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048526:	e8 d9 fd ff ff       	call   8048304 <__printf_chk@plt>
 804852b:	89 1c 24             	mov    %ebx,(%esp)
 804852e:	e8 b1 fe ff ff       	call   80483e4 <bbbbbbbbb>
 8048533:	b8 00 00 00 00       	mov    $0x0,%eax
 8048538:	83 c4 14             	add    $0x14,%esp
 804853b:	5b                   	pop    %ebx
 804853c:	5d                   	pop    %ebp
 804853d:	c3                   	ret    

0804853e <dddddddd>:
 804853e:	55                   	push   %ebp
 804853f:	89 e5                	mov    %esp,%ebp
 8048541:	8b 45 08             	mov    0x8(%ebp),%eax
 8048544:	83 c0 02             	add    $0x2,%eax
 8048547:	5d                   	pop    %ebp
 8048548:	c3                   	ret    

08048549 <main>:
 8048549:	55                   	push   %ebp
 804854a:	89 e5                	mov    %esp,%ebp
 804854c:	83 e4 f0             	and    $0xfffffff0,%esp
 804854f:	83 ec 10             	sub    $0x10,%esp
 8048552:	8b 45 08             	mov    0x8(%ebp),%eax
 8048555:	89 04 24             	mov    %eax,(%esp)
 8048558:	e8 e1 ff ff ff       	call   804853e <dddddddd>
 804855d:	89 04 24             	mov    %eax,(%esp)
 8048560:	e8 74 ff ff ff       	call   80484d9 <aaaaaaaaaaa>
 8048565:	c9                   	leave  
 8048566:	c3                   	ret    
 8048567:	90                   	nop
 8048568:	90                   	nop
 8048569:	90                   	nop
 804856a:	90                   	nop
 804856b:	90                   	nop
 804856c:	90                   	nop
 804856d:	90                   	nop
 804856e:	90                   	nop
 804856f:	90                   	nop

08048570 <__libc_csu_init>:
 8048570:	55                   	push   %ebp
 8048571:	89 e5                	mov    %esp,%ebp
 8048573:	57                   	push   %edi
 8048574:	56                   	push   %esi
 8048575:	53                   	push   %ebx
 8048576:	e8 5a 00 00 00       	call   80485d5 <__i686.get_pc_thunk.bx>
 804857b:	81 c3 79 1a 00 00    	add    $0x1a79,%ebx
 8048581:	83 ec 1c             	sub    $0x1c,%esp
 8048584:	e8 2b fd ff ff       	call   80482b4 <_init>
 8048589:	8d bb 20 ff ff ff    	lea    -0xe0(%ebx),%edi
 804858f:	8d 83 20 ff ff ff    	lea    -0xe0(%ebx),%eax
 8048595:	29 c7                	sub    %eax,%edi
 8048597:	c1 ff 02             	sar    $0x2,%edi
 804859a:	85 ff                	test   %edi,%edi
 804859c:	74 24                	je     80485c2 <__libc_csu_init+0x52>
 804859e:	31 f6                	xor    %esi,%esi
 80485a0:	8b 45 10             	mov    0x10(%ebp),%eax
 80485a3:	89 44 24 08          	mov    %eax,0x8(%esp)
 80485a7:	8b 45 0c             	mov    0xc(%ebp),%eax
 80485aa:	89 44 24 04          	mov    %eax,0x4(%esp)
 80485ae:	8b 45 08             	mov    0x8(%ebp),%eax
 80485b1:	89 04 24             	mov    %eax,(%esp)
 80485b4:	ff 94 b3 20 ff ff ff 	call   *-0xe0(%ebx,%esi,4)
 80485bb:	83 c6 01             	add    $0x1,%esi
 80485be:	39 fe                	cmp    %edi,%esi
 80485c0:	72 de                	jb     80485a0 <__libc_csu_init+0x30>
 80485c2:	83 c4 1c             	add    $0x1c,%esp
 80485c5:	5b                   	pop    %ebx
 80485c6:	5e                   	pop    %esi
 80485c7:	5f                   	pop    %edi
 80485c8:	5d                   	pop    %ebp
 80485c9:	c3                   	ret    
 80485ca:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi

080485d0 <__libc_csu_fini>:
 80485d0:	55                   	push   %ebp
 80485d1:	89 e5                	mov    %esp,%ebp
 80485d3:	5d                   	pop    %ebp
 80485d4:	c3                   	ret    

080485d5 <__i686.get_pc_thunk.bx>:
 80485d5:	8b 1c 24             	mov    (%esp),%ebx
 80485d8:	c3                   	ret    
 80485d9:	90                   	nop
 80485da:	90                   	nop
 80485db:	90                   	nop
 80485dc:	90                   	nop
 80485dd:	90                   	nop
 80485de:	90                   	nop
 80485df:	90                   	nop

080485e0 <__do_global_ctors_aux>:
 80485e0:	55                   	push   %ebp
 80485e1:	89 e5                	mov    %esp,%ebp
 80485e3:	53                   	push   %ebx
 80485e4:	83 ec 04             	sub    $0x4,%esp
 80485e7:	a1 14 9f 04 08       	mov    0x8049f14,%eax
 80485ec:	83 f8 ff             	cmp    $0xffffffff,%eax
 80485ef:	74 13                	je     8048604 <__do_global_ctors_aux+0x24>
 80485f1:	bb 14 9f 04 08       	mov    $0x8049f14,%ebx
 80485f6:	66 90                	xchg   %ax,%ax
 80485f8:	83 eb 04             	sub    $0x4,%ebx
 80485fb:	ff d0                	call   *%eax
 80485fd:	8b 03                	mov    (%ebx),%eax
 80485ff:	83 f8 ff             	cmp    $0xffffffff,%eax
 8048602:	75 f4                	jne    80485f8 <__do_global_ctors_aux+0x18>
 8048604:	83 c4 04             	add    $0x4,%esp
 8048607:	5b                   	pop    %ebx
 8048608:	5d                   	pop    %ebp
 8048609:	c3                   	ret    
 804860a:	90                   	nop
 804860b:	90                   	nop

Disassembly of section .fini:

0804860c <_fini>:
 804860c:	55                   	push   %ebp
 804860d:	89 e5                	mov    %esp,%ebp
 804860f:	53                   	push   %ebx
 8048610:	83 ec 04             	sub    $0x4,%esp
 8048613:	e8 00 00 00 00       	call   8048618 <_fini+0xc>
 8048618:	5b                   	pop    %ebx
 8048619:	81 c3 dc 19 00 00    	add    $0x19dc,%ebx
 804861f:	e8 3c fd ff ff       	call   8048360 <__do_global_dtors_aux>
 8048624:	59                   	pop    %ecx
 8048625:	5b                   	pop    %ebx
 8048626:	c9                   	leave  
 8048627:	c3                   	ret    
