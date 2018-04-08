# mark_description "Intel(R) C Intel(R) 64 Compiler for applications running on Intel(R) 64, Version 17.0.5.239 Build 20170817";
# mark_description "-I/home/xjcorbalan/git/EAR/src -axMIC-AVX512 -c -S -O3";
	.file "dynais.c"
	.text
..TXTST0:
# -- Begin  dynais_dispose
	.text
# mark_begin;
       .align    16,0x90
	.globl dynais_dispose
# --- dynais_dispose()
dynais_dispose:
..B1.1:                         # Preds ..B1.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_dynais_dispose.1:
..L2:
                                                          #127.1
        pushq     %rsi                                          #127.1
	.cfi_def_cfa_offset 16
        movq      samples_vec(%rip), %rdi                       #128.5
#       free(void *)
        call      free                                          #128.5
                                # LOE rbx rbp r12 r13 r14 r15
..B1.2:                         # Preds ..B1.1
                                # Execution count [1.00e+00]
        movq      zeros_vec(%rip), %rdi                         #129.5
#       free(void *)
        call      free                                          #129.5
                                # LOE rbx rbp r12 r13 r14 r15
..B1.3:                         # Preds ..B1.2
                                # Execution count [1.00e+00]
        movq      sizes_vec(%rip), %rdi                         #130.5
#       free(void *)
        call      free                                          #130.5
                                # LOE rbx rbp r12 r13 r14 r15
..B1.4:                         # Preds ..B1.3
                                # Execution count [1.00e+00]
        movq      indes_vec(%rip), %rdi                         #131.2
        addq      $8, %rsp                                      #131.2
	.cfi_def_cfa_offset 8
#       free(void *)
        jmp       free                                          #131.2
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	dynais_dispose,@function
	.size	dynais_dispose,.-dynais_dispose
	.data
# -- End  dynais_dispose
	.text
# -- Begin  dynais
	.text
# mark_begin;
       .align    16,0x90
	.globl dynais
# --- dynais(unsigned long, unsigned int *, unsigned int *)
dynais:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
..B2.1:                         # Preds ..B2.0
                                # Execution count [1.00e+00]
        pushq     %rsi                                          #546.1
        movq      $0x7189d97ff, %rcx                            #546.1
                                # LOE rdx rcx rbx rbp rsi rdi r12 r13 r14 r15
..B2.3:                         # Preds ..B2.1 ..B2.8
                                # Execution count [1.00e+00]
        movq      __intel_cpu_feature_indicator(%rip), %rax     #546.1
        andq      %rcx, %rax                                    #546.1
        cmpq      %rcx, %rax                                    #546.1
        jne       ..B2.6        # Prob 10%                      #546.1
                                # LOE rdx rcx rbx rbp rsi rdi r12 r13 r14 r15
..B2.4:                         # Preds ..B2.3
                                # Execution count [1.00e+01]
        addq      $8, %rsp                                      #546.1
        jmp       dynais.Z                                      #546.1
                                # LOE
..B2.6:                         # Preds ..B2.3
                                # Execution count [1.00e+00]
        testb     $1, __intel_cpu_feature_indicator(%rip)       #546.1
        je        ..B2.8        # Prob 10%                      #546.1
                                # LOE rdx rcx rbx rbp rsi rdi r12 r13 r14 r15
..B2.7:                         # Preds ..B2.6
                                # Execution count [1.00e+01]
        addq      $8, %rsp                                      #546.1
        jmp       dynais.A                                      #546.1
                                # LOE
..B2.8:                         # Preds ..B2.6
                                # Execution count [1.00e+00]
        call      __intel_cpu_features_init                     #546.1
        jmp       ..B2.3        # Prob 100%                     #546.1
        .align    16,0x90
                                # LOE rdx rcx rbx rbp rsi rdi r12 r13 r14 r15
# mark_end;
	.type	dynais,@function
	.size	dynais,.-dynais
# -- End  dynais
# -- Begin  dynais.Z
	.text
# mark_begin;
# Threads 2
        .align    16,0x90
# --- dynais.Z..0(unsigned long, unsigned int *, unsigned int *)
dynais.Z:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
..B3.1:                         # Preds ..B3.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_dynais.Z.6:
..L7:
                                                          #546.1
        pushq     %rbp                                          #546.1
	.cfi_def_cfa_offset 16
        movq      %rsp, %rbp                                    #546.1
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
        andq      $-64, %rsp                                    #546.1
        subq      $64, %rsp                                     #546.1 c1
        movq      %r15, 8(%rsp)                                 #546.1[spill] c3
        movl      _levels(%rip), %r8d                           #556.13 c3
        movq      %r14, 16(%rsp)                                #546.1[spill] c5
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc8, 0xff, 0xff, 0xff, 0x22
        movq      %rdx, %r14                                    #546.1 c5
        movq      %r13, 24(%rsp)                                #546.1[spill] c7
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
        movq      %rsi, %r13                                    #546.1 c7
        movq      %r12, (%rsp)                                  #546.1[spill] c7
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x22
        movq      %rdi, %r15                                    #546.1 c7
        xorl      %r12d, %r12d                                  #547.18 c9
        testl     %r8d, %r8d                                    #556.13 c9
        ja        ..B3.3        # Prob 42%                      #556.13 c11
                                # LOE rbx r13 r14 r15 r8d r12d
..B3.2:                         # Preds ..B3.1
                                # Execution count [5.80e-01]
        movl      $-1, %eax                                     #556.13 c1
        jmp       ..B3.7        # Prob 100%                     #556.13 c1
                                # LOE rbx r13 r14 eax r8d r12d
..B3.3:                         # Preds ..B3.1
                                # Execution count [4.20e-01]
        movq      %r15, %rdi                                    #556.13 c1
        movl      $1, %esi                                      #556.13 c1
        xorl      %edx, %edx                                    #556.13 c3
..___tag_value_dynais.Z.15:
#       dynais_basic(unsigned long, unsigned int, unsigned int)
        call      dynais_basic                                  #556.13
..___tag_value_dynais.Z.16:
                                # LOE rbx r13 r14 r15 eax r12d
..B3.4:                         # Preds ..B3.3
                                # Execution count [4.20e-01]
        movl      %eax, level_result(%rip)                      #556.13 c1
        cmpl      $3, %eax                                      #556.13 c1
        jl        ..B3.27       # Prob 5%                       #556.13 c3
                                # LOE rbx r13 r14 r15 r12d
..B3.5:                         # Preds ..B3.4
                                # Execution count [3.11e-01]
        movq      %r15, %rdi                                    #556.13 c1
        movl      level_length(%rip), %esi                      #556.13 c1
        movl      $1, %edx                                      #556.13 c1
..___tag_value_dynais.Z.17:
#       dynais_hierarchical(unsigned long, unsigned int, unsigned int)
        call      dynais_hierarchical                           #556.13
..___tag_value_dynais.Z.18:
                                # LOE rbx r13 r14 eax r12d
..B3.6:                         # Preds ..B3.5
                                # Execution count [3.99e-01]
        movl      _levels(%rip), %r8d                           #562.14 c1
                                # LOE rbx r13 r14 eax r8d r12d
..B3.7:                         # Preds ..B3.2 ..B3.6 ..B3.27
                                # Execution count [1.00e+00]
        lea       -1(%r8), %edx                                 #562.10 c1
        cmpl      %eax, %edx                                    #562.31 c3
        jle       ..B3.11       # Prob 50%                      #562.31 c5
                                # LOE rbx r13 r14 eax edx r8d r12d
..B3.8:                         # Preds ..B3.7
                                # Execution count [8.47e-01]
        movl      %r8d, %ecx                                    #562.10 c1
        vmovups   .L_2il0floatpacket.0(%rip), %zmm5             #562.5 c1
        vmovdqu32 .L_2il0floatpacket.2(%rip), %zmm4             #562.5 c1
        xorl      %edi, %edi                                    #562.5 c1
        vpxord    %zmm1, %zmm1, %zmm1                           #566.22 c1
        subl      %eax, %ecx                                    #562.10 c3
        negl      %eax                                          #562.10 c3
        addl      $-1, %ecx                                     #562.10 c5
        vpbroadcastd %ecx, %zmm3                                #562.10 c7
        vmovups   .L_2il0floatpacket.1(%rip), %zmm2             #565.22 c7
        xorl      %ecx, %ecx                                    #564.9 c7
        lea       -1(%r8,%rax), %esi                            #562.10 c13 stall 2
        vmovdqu32 .L_2il0floatpacket.3(%rip), %zmm0             #564.18 c13
                                # LOE rbx r13 r14 edx ecx esi edi r8d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B3.9:                         # Preds ..B3.9 ..B3.8
                                # Execution count [8.47e+00]
        lea       (%r8,%rcx), %r9d                              #562.5 c1
        vpcmpgtd  %zmm4, %zmm3, %k3                             #562.5 c1
        vpxord    %zmm6, %zmm6, %zmm6                           #564.18 c1
        addl      $16, %edi                                     #562.5 c1
        addl      $-16, %ecx                                    #562.5 c1
        movslq    %r9d, %r9                                     #564.18 c3
        kmovw     %k3, %k1                                      #564.18 c3
        vpaddd    %zmm5, %zmm4, %zmm4                           #562.5 c3
        shlq      $2, %r9                                       #564.18 c5
        vpgatherdd -4+level_result(%r9,%zmm0,4), %zmm6{%k1}     #564.18 c7
        vpcmpgtd  %zmm2, %zmm6, %k2{%k3}                        #565.22 c13 stall 2
        vscatterdps %zmm2, -4+level_result(%r9,%zmm0,4){%k2}    #565.31 c15
        vpcmpgtd  %zmm6, %zmm1, %k4{%k3}                        #566.22 c15
        vscatterdps %zmm1, -4+level_result(%r9,%zmm0,4){%k4}    #566.31 c17
        cmpl      %esi, %edi                                    #562.5 c17
        jb        ..B3.9        # Prob 90%                      #562.5 c19
                                # LOE rbx r13 r14 edx ecx esi edi r8d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B3.11:                        # Preds ..B3.9 ..B3.7
                                # Execution count [1.00e+00]
        movslq    %edx, %rcx                                    #573.10 c1
        testl     %edx, %edx                                    #573.32 c1
        js        ..B3.16       # Prob 0%                       #573.32 c3
                                # LOE rcx rbx r13 r14 edx r12d
..B3.12:                        # Preds ..B3.11
                                # Execution count [9.92e-01]
        movl      $1, %edi                                      #575.9 c1
                                # LOE rcx rbx r13 r14 edx edi r12d
..B3.13:                        # Preds ..B3.14 ..B3.12
                                # Execution count [3.54e+00]
        xorl      %esi, %esi                                    #575.9 c1
        cmpl      $-1, level_result(,%rcx,4)                    #575.9 c1
        cmove     %edi, %esi                                    #575.9 c3
        orl       %esi, %r12d                                   #575.51 c5
        cmpl      $0, level_result(,%rcx,4)                     #577.32 c5
        jg        ..B3.17       # Prob 20%                      #577.32 c7
                                # LOE rcx rbx r13 r14 edx edi r12d
..B3.14:                        # Preds ..B3.13
                                # Execution count [2.83e+00]
        addq      $-1, %rcx                                     #573.37 c1
        addl      $-1, %edx                                     #573.37 c1
        jns       ..B3.13       # Prob 90%                      #573.32 c3
                                # LOE rcx rbx r13 r14 edx edi r12d
..B3.16:                        # Preds ..B3.14 ..B3.11
                                # Execution count [2.92e-01]
        negl      %r12d                                         #621.13 c1
        movl      $0, (%r13)                                    #618.6 c1
        movq      24(%rsp), %r13                                #621.13[spill] c1
	.cfi_restore 13
        movl      $0, (%r14)                                    #619.6 c3
        movl      %r12d, %eax                                   #621.13 c3
        movq      (%rsp), %r12                                  #621.13[spill] c5
	.cfi_restore 12
        movq      16(%rsp), %r14                                #621.13[spill] c5
	.cfi_restore 14
        movq      8(%rsp), %r15                                 #621.13[spill] c9 stall 1
	.cfi_restore 15
        movq      %rbp, %rsp                                    #621.13 c11
        popq      %rbp                                          #621.13
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #621.13
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc8, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B3.17:                        # Preds ..B3.13
                                # Execution count [7.08e-01]: Infreq
        movl      %edx, (%r14)                                  #579.14 c1
        movl      level_length(,%rcx,4), %esi                   #580.21 c1
        movl      %esi, (%r13)                                  #580.14 c5 stall 1
        testl     %r12d, %r12d                                  #586.17 c5
        jne       ..B3.26       # Prob 28%                      #586.17 c7
                                # LOE rcx rbx edx edi r12d
..B3.18:                        # Preds ..B3.17
                                # Execution count [5.10e-01]: Infreq
        movl      level_result(,%rcx,4), %eax                   #594.17 c1
        cmpl      $3, %eax                                      #594.36 c5 stall 1
        je        ..B3.21       # Prob 16%                      #594.36 c7
                                # LOE rbx eax edx edi r12d
..B3.19:                        # Preds ..B3.25 ..B3.21 ..B3.18
                                # Execution count [2.26e-01]: Infreq
        movq      (%rsp), %r12                                  #611.20[spill] c1
	.cfi_restore 12
        movq      24(%rsp), %r13                                #611.20[spill] c1
	.cfi_restore 13
        movq      16(%rsp), %r14                                #611.20[spill] c5 stall 1
	.cfi_restore 14
        movq      8(%rsp), %r15                                 #611.20[spill] c5
	.cfi_restore 15
        movq      %rbp, %rsp                                    #611.20 c7
        popq      %rbp                                          #611.20
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #611.20
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc8, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B3.21:                        # Preds ..B3.18
                                # Execution count [8.16e-02]: Infreq
        xorl      %esi, %esi                                    #596.31 c1
        addl      $-1, %edx                                     #596.31 c1
        movslq    %edx, %rdx                                    #596.22 c3
        js        ..B3.19       # Prob 2%                       #596.40 c3
                                # LOE rdx rbx rsi eax edi r12d
..B3.22:                        # Preds ..B3.21
                                # Execution count [7.99e-02]: Infreq
        lea       (,%rdx,4), %rcx                               #598.33 c3 stall 1
        addq      $1, %rdx                                      #596.22 c3
                                # LOE rdx rcx rbx rsi eax edi r12d
..B3.23:                        # Preds ..B3.24 ..B3.22
                                # Execution count [2.85e-01]: Infreq
        xorl      %r8d, %r8d                                    #598.21 c1
        cmpl      $4, level_result(%rcx)                        #598.21 c1
        cmove     %edi, %r8d                                    #598.21 c3
        orl       %r8d, %r12d                                   #598.21 c5
        cmpl      $3, level_result(%rcx)                        #600.44 c5
        jl        ..B3.28       # Prob 20%                      #600.44 c7
                                # LOE rdx rcx rbx rsi eax edi r12d
..B3.24:                        # Preds ..B3.23
                                # Execution count [2.28e-01]: Infreq
        addq      $1, %rsi                                      #596.17 c1
        addq      $-4, %rcx                                     #596.17 c1
        cmpq      %rdx, %rsi                                    #596.17 c3
        jb        ..B3.23       # Prob 90%                      #596.17 c5
                                # LOE rdx rcx rbx rsi eax edi r12d
..B3.25:                        # Preds ..B3.24
                                # Execution count [2.28e-02]: Infreq
        testl     %r12d, %r12d                                  #607.17 c1
        je        ..B3.19       # Prob 50%                      #607.17 c3
                                # LOE rbx eax
..B3.26:                        # Preds ..B3.17 ..B3.25
                                # Execution count [2.26e-01]: Infreq
        movq      (%rsp), %r12                                  #608.24[spill] c1
	.cfi_restore 12
        movq      24(%rsp), %r13                                #608.24[spill] c1
	.cfi_restore 13
        movl      $4, %eax                                      #608.24 c1
        movq      16(%rsp), %r14                                #608.24[spill] c5 stall 1
	.cfi_restore 14
        movq      8(%rsp), %r15                                 #608.24[spill] c5
	.cfi_restore 15
        movq      %rbp, %rsp                                    #608.24 c7
        popq      %rbp                                          #608.24
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #608.24
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc8, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B3.27:                        # Preds ..B3.4
                                # Execution count [2.12e-02]: Infreq
        xorl      %eax, %eax                                    #556.13 c1
        movl      _levels(%rip), %r8d                           #562.14 c1
        jmp       ..B3.7        # Prob 100%                     #562.14 c1
                                # LOE rbx r13 r14 eax r8d r12d
..B3.28:                        # Preds ..B3.23
                                # Execution count [5.71e-02]: Infreq
        movq      (%rsp), %r12                                  #602.32[spill] c1
	.cfi_restore 12
        movq      24(%rsp), %r13                                #602.32[spill] c1
	.cfi_restore 13
        movl      $1, %eax                                      #602.32 c1
        movq      16(%rsp), %r14                                #602.32[spill] c5 stall 1
	.cfi_restore 14
        movq      8(%rsp), %r15                                 #602.32[spill] c5
	.cfi_restore 15
        movq      %rbp, %rsp                                    #602.32 c7
        popq      %rbp                                          #602.32
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #602.32
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	dynais.Z,@function
	.size	dynais.Z,.-dynais.Z
	.data
# -- End  dynais.Z
	.text
# -- Begin  dynais.A
	.text
# mark_begin;
       .align    16,0x90
# --- dynais.A..0(unsigned long, unsigned int *, unsigned int *)
dynais.A:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
..B4.1:                         # Preds ..B4.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_dynais.A.62:
..L63:
                                                         #546.1
        pushq     %rbp                                          #546.1
	.cfi_def_cfa_offset 16
        movq      %rsp, %rbp                                    #546.1
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
        andq      $-64, %rsp                                    #546.1
        pushq     %r12                                          #546.1
        pushq     %r13                                          #546.1
        pushq     %r14                                          #546.1
        pushq     %r15                                          #546.1
        pushq     %rbx                                          #546.1
        subq      $24, %rsp                                     #546.1
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
        movq      %rdx, %r14                                    #546.1
        movl      _levels(%rip), %r8d                           #556.13
        movq      %rsi, %r13                                    #546.1
        movq      %rdi, %rbx                                    #546.1
        xorl      %r12d, %r12d                                  #547.18
        xorl      %r15d, %r15d                                  #547.18
        testl     %r8d, %r8d                                    #556.13
        ja        ..B4.3        # Prob 42%                      #556.13
                                # LOE rbx r13 r14 r8d r12d r15d
..B4.2:                         # Preds ..B4.1
                                # Execution count [5.80e-01]
        movl      $-1, %eax                                     #556.13
        jmp       ..B4.7        # Prob 100%                     #556.13
                                # LOE r13 r14 eax r8d r12d r15d
..B4.3:                         # Preds ..B4.1
                                # Execution count [4.20e-01]
        movq      %rbx, %rdi                                    #556.13
        movl      $1, %esi                                      #556.13
        xorl      %edx, %edx                                    #556.13
..___tag_value_dynais.A.72:
#       dynais_basic(unsigned long, unsigned int, unsigned int)
        call      dynais_basic                                  #556.13
..___tag_value_dynais.A.73:
                                # LOE rbx r13 r14 eax r12d r15d
..B4.4:                         # Preds ..B4.3
                                # Execution count [4.20e-01]
        movl      %eax, level_result(%rip)                      #556.13
        cmpl      $3, %eax                                      #556.13
        jl        ..B4.39       # Prob 5%                       #556.13
                                # LOE rbx r13 r14 r12d r15d
..B4.5:                         # Preds ..B4.4
                                # Execution count [3.11e-01]
        movq      %rbx, %rdi                                    #556.13
        movl      $1, %edx                                      #556.13
        movl      level_length(%rip), %esi                      #556.13
..___tag_value_dynais.A.74:
#       dynais_hierarchical(unsigned long, unsigned int, unsigned int)
        call      dynais_hierarchical                           #556.13
..___tag_value_dynais.A.75:
                                # LOE r13 r14 eax r12d r15d
..B4.6:                         # Preds ..B4.5
                                # Execution count [3.99e-01]
        movl      _levels(%rip), %r8d                           #562.14
                                # LOE r13 r14 eax r8d r12d r15d
..B4.7:                         # Preds ..B4.2 ..B4.6 ..B4.39
                                # Execution count [1.00e+00]
        lea       -1(%r8), %r9d                                 #562.10
        cmpl      %eax, %r9d                                    #562.31
        jle       ..B4.23       # Prob 50%                      #562.31
                                # LOE r13 r14 eax r8d r9d r12d r15d
..B4.8:                         # Preds ..B4.7
                                # Execution count [1.00e+00]
        negl      %eax                                          #562.5
        movl      %r15d, %ebx                                   #562.5
        addl      %r8d, %eax                                    #562.5
        movl      $1, %edi                                      #562.5
        lea       -1(%rax), %esi                                #562.5
        movl      %esi, %edx                                    #562.5
        shrl      $31, %edx                                     #562.5
        lea       -1(%rax,%rdx), %ecx                           #562.5
        movl      %ebx, %edx                                    #564.9
        sarl      $1, %ecx                                      #562.5
        testl     %ecx, %ecx                                    #562.5
        jbe       ..B4.18       # Prob 15%                      #562.5
                                # LOE r13 r14 edx ecx ebx esi edi r8d r9d r12d r15d
..B4.10:                        # Preds ..B4.8 ..B4.16
                                # Execution count [4.23e+00]
        lea       (%r8,%rdx), %edi                              #564.18
        movslq    %edi, %rdi                                    #564.18
        movl      -4+level_result(,%rdi,4), %r10d               #564.18
        cmpl      $1, %r10d                                     #565.22
        jle       ..B4.12       # Prob 50%                      #565.22
                                # LOE rdi r13 r14 edx ecx ebx esi r8d r9d r10d r12d r15d
..B4.11:                        # Preds ..B4.10
                                # Execution count [2.12e+00]
        movl      $1, %r10d                                     #565.31
        jmp       ..B4.13       # Prob 100%                     #565.31
                                # LOE rdi r13 r14 edx ecx ebx esi r8d r9d r10d r12d r15d
..B4.12:                        # Preds ..B4.10
                                # Execution count [2.12e+00]
        testl     %r10d, %r10d                                  #566.31
        cmovl     %r15d, %r10d                                  #566.31
                                # LOE rdi r13 r14 edx ecx ebx esi r8d r9d r10d r12d r15d
..B4.13:                        # Preds ..B4.11 ..B4.12
                                # Execution count [4.23e+00]
        movl      %r10d, -4+level_result(,%rdi,4)               #566.31
        movl      -8+level_result(,%rdi,4), %r10d               #564.18
        cmpl      $1, %r10d                                     #565.22
        jle       ..B4.15       # Prob 50%                      #565.22
                                # LOE rdi r13 r14 edx ecx ebx esi r8d r9d r10d r12d r15d
..B4.14:                        # Preds ..B4.13
                                # Execution count [2.12e+00]
        movl      $1, %r10d                                     #565.31
        jmp       ..B4.16       # Prob 100%                     #565.31
                                # LOE rdi r13 r14 edx ecx ebx esi r8d r9d r10d r12d r15d
..B4.15:                        # Preds ..B4.13
                                # Execution count [2.12e+00]
        testl     %r10d, %r10d                                  #566.31
        cmovl     %r15d, %r10d                                  #566.31
                                # LOE rdi r13 r14 edx ecx ebx esi r8d r9d r10d r12d r15d
..B4.16:                        # Preds ..B4.14 ..B4.15
                                # Execution count [4.23e+00]
        incl      %ebx                                          #562.5
        addl      $-2, %edx                                     #562.5
        movl      %r10d, -8+level_result(,%rdi,4)               #566.31
        cmpl      %ecx, %ebx                                    #562.5
        jb        ..B4.10       # Prob 79%                      #562.5
                                # LOE r13 r14 edx ecx ebx esi r8d r9d r12d r15d
..B4.17:                        # Preds ..B4.16
                                # Execution count [8.47e-01]
        lea       1(%rbx,%rbx), %edi                            #564.9
                                # LOE r13 r14 esi edi r8d r9d r12d r15d
..B4.18:                        # Preds ..B4.17 ..B4.8
                                # Execution count [1.00e+00]
        lea       -1(%rdi), %edx                                #562.5
        cmpl      %esi, %edx                                    #562.5
        jae       ..B4.23       # Prob 15%                      #562.5
                                # LOE r13 r14 edi r8d r9d r12d r15d
..B4.19:                        # Preds ..B4.18
                                # Execution count [8.47e-01]
        movslq    %r8d, %r8                                     #564.18
        movslq    %edi, %rdi                                    #564.18
        subq      %rdi, %r8                                     #564.18
        movl      level_result(,%r8,4), %edx                    #564.18
        cmpl      $1, %edx                                      #565.22
        jle       ..B4.21       # Prob 50%                      #565.22
                                # LOE r8 r13 r14 edx r9d r12d r15d
..B4.20:                        # Preds ..B4.19
                                # Execution count [4.23e-01]
        movl      $1, level_result(,%r8,4)                      #565.31
        jmp       ..B4.23       # Prob 100%                     #565.31
                                # LOE r13 r14 r9d r12d r15d
..B4.21:                        # Preds ..B4.19
                                # Execution count [4.23e-01]
        testl     %edx, %edx                                    #566.22
        jge       ..B4.23       # Prob 84%                      #566.22
                                # LOE r8 r13 r14 r9d r12d r15d
..B4.22:                        # Preds ..B4.21
                                # Execution count [1.35e-01]
        movl      $0, level_result(,%r8,4)                      #566.31
                                # LOE r13 r14 r9d r12d r15d
..B4.23:                        # Preds ..B4.18 ..B4.7 ..B4.21 ..B4.20 ..B4.22
                                #      
                                # Execution count [1.00e+00]
        movslq    %r9d, %rcx                                    #573.10
        testl     %r9d, %r9d                                    #573.32
        js        ..B4.28       # Prob 0%                       #573.32
                                # LOE rcx r13 r14 r9d r12d r15d
..B4.24:                        # Preds ..B4.23
                                # Execution count [9.92e-01]
        movl      $1, %edx                                      #562.5
                                # LOE rcx r13 r14 edx r9d r12d r15d
..B4.25:                        # Preds ..B4.26 ..B4.24
                                # Execution count [3.54e+00]
        movl      level_result(,%rcx,4), %esi                   #575.32
        cmpl      $-1, %esi                                     #575.9
        movl      %r15d, %ebx                                   #575.9
        cmove     %edx, %ebx                                    #575.9
        orl       %ebx, %r12d                                   #575.51
        testl     %esi, %esi                                    #577.32
        jg        ..B4.29       # Prob 20%                      #577.32
                                # LOE rcx r13 r14 edx r9d r12d r15d
..B4.26:                        # Preds ..B4.25
                                # Execution count [2.83e+00]
        decq      %rcx                                          #573.37
        decl      %r9d                                          #573.37
        jns       ..B4.25       # Prob 90%                      #573.32
                                # LOE rcx r13 r14 edx r9d r12d r15d
..B4.28:                        # Preds ..B4.26 ..B4.23
                                # Execution count [2.92e-01]
        negl      %r12d                                         #621.13
        movl      %r15d, (%r13)                                 #618.6
        movl      %r12d, %eax                                   #621.13
        movl      %r15d, (%r14)                                 #619.6
        addq      $24, %rsp                                     #621.13
	.cfi_restore 3
        popq      %rbx                                          #621.13
	.cfi_restore 15
        popq      %r15                                          #621.13
	.cfi_restore 14
        popq      %r14                                          #621.13
	.cfi_restore 13
        popq      %r13                                          #621.13
	.cfi_restore 12
        popq      %r12                                          #621.13
        movq      %rbp, %rsp                                    #621.13
        popq      %rbp                                          #621.13
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #621.13
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B4.29:                        # Preds ..B4.25
                                # Execution count [7.08e-01]: Infreq
        movl      %r9d, (%r14)                                  #579.14
        movl      level_length(,%rcx,4), %ebx                   #580.21
        movl      %ebx, (%r13)                                  #580.14
        testl     %r12d, %r12d                                  #586.17
        jne       ..B4.38       # Prob 28%                      #586.17
                                # LOE rcx edx r9d r12d r15d
..B4.30:                        # Preds ..B4.29
                                # Execution count [5.10e-01]: Infreq
        movl      level_result(,%rcx,4), %eax                   #594.17
        cmpl      $3, %eax                                      #594.36
        je        ..B4.33       # Prob 16%                      #594.36
                                # LOE eax edx r9d r12d r15d
..B4.31:                        # Preds ..B4.37 ..B4.33 ..B4.30
                                # Execution count [2.26e-01]: Infreq
        addq      $24, %rsp                                     #611.20
	.cfi_restore 3
        popq      %rbx                                          #611.20
	.cfi_restore 15
        popq      %r15                                          #611.20
	.cfi_restore 14
        popq      %r14                                          #611.20
	.cfi_restore 13
        popq      %r13                                          #611.20
	.cfi_restore 12
        popq      %r12                                          #611.20
        movq      %rbp, %rsp                                    #611.20
        popq      %rbp                                          #611.20
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #611.20
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B4.33:                        # Preds ..B4.30
                                # Execution count [8.16e-02]: Infreq
        xorl      %ebx, %ebx                                    #596.31
        decl      %r9d                                          #596.31
        movslq    %r9d, %r9                                     #596.22
        js        ..B4.31       # Prob 2%                       #596.40
                                # LOE rbx r9 eax edx r12d r15d
..B4.34:                        # Preds ..B4.33
                                # Execution count [7.99e-02]: Infreq
        lea       (,%r9,4), %rcx                                #598.33
        incq      %r9                                           #596.22
                                # LOE rcx rbx r9 eax edx r12d r15d
..B4.35:                        # Preds ..B4.36 ..B4.34
                                # Execution count [2.85e-01]: Infreq
        movl      level_result(%rcx), %edi                      #598.33
        cmpl      $4, %edi                                      #598.21
        movl      %r15d, %esi                                   #598.21
        cmove     %edx, %esi                                    #598.21
        orl       %esi, %r12d                                   #598.21
        cmpl      $3, %edi                                      #600.44
        jl        ..B4.40       # Prob 20%                      #600.44
                                # LOE rcx rbx r9 eax edx r12d r15d
..B4.36:                        # Preds ..B4.35
                                # Execution count [2.28e-01]: Infreq
        incq      %rbx                                          #596.17
        addq      $-4, %rcx                                     #596.17
        cmpq      %r9, %rbx                                     #596.17
        jb        ..B4.35       # Prob 90%                      #596.17
                                # LOE rcx rbx r9 eax edx r12d r15d
..B4.37:                        # Preds ..B4.36
                                # Execution count [2.28e-02]: Infreq
        testl     %r12d, %r12d                                  #607.17
        je        ..B4.31       # Prob 50%                      #607.17
                                # LOE eax
..B4.38:                        # Preds ..B4.29 ..B4.37
                                # Execution count [2.26e-01]: Infreq
        movl      $4, %eax                                      #608.24
        addq      $24, %rsp                                     #608.24
	.cfi_restore 3
        popq      %rbx                                          #608.24
	.cfi_restore 15
        popq      %r15                                          #608.24
	.cfi_restore 14
        popq      %r14                                          #608.24
	.cfi_restore 13
        popq      %r13                                          #608.24
	.cfi_restore 12
        popq      %r12                                          #608.24
        movq      %rbp, %rsp                                    #608.24
        popq      %rbp                                          #608.24
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #608.24
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B4.39:                        # Preds ..B4.4
                                # Execution count [2.12e-02]: Infreq
        movl      _levels(%rip), %r8d                           #562.14
        movl      %r15d, %eax                                   #556.13
        jmp       ..B4.7        # Prob 100%                     #556.13
                                # LOE r13 r14 eax r8d r12d r15d
..B4.40:                        # Preds ..B4.35
                                # Execution count [5.71e-02]: Infreq
        movl      $1, %eax                                      #602.32
        addq      $24, %rsp                                     #602.32
	.cfi_restore 3
        popq      %rbx                                          #602.32
	.cfi_restore 15
        popq      %r15                                          #602.32
	.cfi_restore 14
        popq      %r14                                          #602.32
	.cfi_restore 13
        popq      %r13                                          #602.32
	.cfi_restore 12
        popq      %r12                                          #602.32
        movq      %rbp, %rsp                                    #602.32
        popq      %rbp                                          #602.32
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #602.32
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	dynais.A,@function
	.size	dynais.A,.-dynais.A
	.data
# -- End  dynais.A
	.text
# -- Begin  dynais_hierarchical
	.text
# mark_begin;
       .align    16,0x90
# --- dynais_hierarchical(unsigned long, unsigned int, unsigned int)
dynais_hierarchical:
# parameter 1: %rdi
# parameter 2: %esi
# parameter 3: %edx
..B5.1:                         # Preds ..B5.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_dynais_hierarchical.126:
..L127:
                                                        #515.1
        pushq     %r12                                          #515.1
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
        pushq     %r13                                          #515.1
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
        pushq     %r14                                          #515.1
	.cfi_def_cfa_offset 32
	.cfi_offset 14, -32
        pushq     %r15                                          #515.1
	.cfi_def_cfa_offset 40
	.cfi_offset 15, -40
        pushq     %rsi                                          #515.1
	.cfi_def_cfa_offset 48
        xorl      %eax, %eax                                    #515.1
        movl      %eax, %r14d                                   #515.1
        movl      %edx, %r15d                                   #515.1
        movq      %rdi, %r13                                    #515.1
                                # LOE rbx rbp r13 esi r14d r15d
..B5.2:                         # Preds ..B5.7 ..B5.14 ..B5.13 ..B5.8 ..B5.1
                                #      
                                # Execution count [1.66e+00]
        incl      %r14d                                         #515.1
        lea       -1(%r15,%r14), %r12d                          #538.16
        cmpl      _levels(%rip), %r12d                          #518.18
        jb        ..B5.4        # Prob 42%                      #518.18
                                # LOE rbx rbp r13 esi r12d r14d r15d
..B5.3:                         # Preds ..B5.2
                                # Execution count [9.65e-01]
        movl      %r14d, %eax                                   #
        movl      %r15d, %edx                                   #
        lea       -2(%rdx,%rax), %eax                           #538.16
        popq      %rcx                                          #519.24
	.cfi_def_cfa_offset 40
	.cfi_restore 15
        popq      %r15                                          #519.24
	.cfi_def_cfa_offset 32
	.cfi_restore 14
        popq      %r14                                          #519.24
	.cfi_def_cfa_offset 24
	.cfi_restore 13
        popq      %r13                                          #519.24
	.cfi_def_cfa_offset 16
	.cfi_restore 12
        popq      %r12                                          #519.24
	.cfi_def_cfa_offset 8
        ret                                                     #519.24
	.cfi_def_cfa_offset 48
	.cfi_offset 12, -16
	.cfi_offset 13, -24
	.cfi_offset 14, -32
	.cfi_offset 15, -40
                                # LOE
..B5.4:                         # Preds ..B5.2
                                # Execution count [7.00e-01]
        movq      %r13, %rdi                                    #523.27
        movl      %r12d, %edx                                   #523.27
..___tag_value_dynais_hierarchical.151:
#       dynais_basic(unsigned long, unsigned int, unsigned int)
        call      dynais_basic                                  #523.27
..___tag_value_dynais_hierarchical.152:
                                # LOE rbx rbp r13 eax r12d r14d r15d
..B5.5:                         # Preds ..B5.4
                                # Execution count [7.00e-01]
        movl      %r12d, %ecx                                   #523.5
        movl      %eax, level_result(,%rcx,4)                   #523.5
        cmpl      $3, %eax                                      #527.32
        jl        ..B5.17       # Prob 5%                       #527.32
                                # LOE rcx rbx rbp r13 r12d r14d r15d
..B5.6:                         # Preds ..B5.5
                                # Execution count [6.64e-01]
        movl      level_length(,%rcx,4), %edi                   #533.55
        testl     %r12d, %r12d                                  #532.21
        ja        ..B5.8        # Prob 22%                      #532.21
                                # LOE rcx rbx rbp r13 edi r14d r15d
..B5.7:                         # Preds ..B5.6
                                # Execution count [5.18e-01]
        movl      %edi, %esi                                    #535.13
        jmp       ..B5.2        # Prob 100%                     #535.13
                                # LOE rbx rbp r13 esi r14d r15d
..B5.8:                         # Preds ..B5.6
                                # Execution count [1.46e-01]
        movl      level_index(,%rcx,4), %r9d                    #533.31
        xorl      %eax, %eax                                    #533.31
        addl      $2, %r9d                                      #533.31
        xorl      %esi, %esi                                    #533.31
        movl      _window(%rip), %r10d                          #533.31
        cmpl      %r10d, %r9d                                   #533.31
        movq      sizes_vec(,%rcx,8), %r8                       #533.31
        movl      $0, %ecx                                      #533.31
        setae     %al                                           #533.31
        cmpl      %r10d, %r9d                                   #533.31
        seta      %cl                                           #533.31
        decl      %eax                                          #533.31
        andl      %r9d, %eax                                    #533.31
        addl      %ecx, %eax                                    #533.31
        testl     %edi, %edi                                    #533.31
        jbe       ..B5.2        # Prob 50%                      #533.31
                                # LOE rax rbx rbp r8 r13 esi edi r10d r14d r15d
..B5.9:                         # Preds ..B5.8
                                # Execution count [1.46e-01]
        movl      %edi, %r11d                                   #533.31
        movl      $1, %ecx                                      #533.31
        shrl      $1, %r11d                                     #533.31
        movl      %ecx, %r9d                                    #533.31
        xorl      %edx, %edx                                    #533.31
        testl     %r11d, %r11d                                  #533.31
        jbe       ..B5.13       # Prob 10%                      #533.31
                                # LOE rax rbx rbp r8 r13 edx ecx esi edi r9d r10d r11d r14d r15d
..B5.10:                        # Preds ..B5.9
                                # Execution count [1.32e-01]
        xorl      %ecx, %ecx                                    #533.31
                                # LOE rax rbx rbp r8 r13 edx ecx esi edi r9d r10d r11d r14d r15d
..B5.11:                        # Preds ..B5.11 ..B5.10
                                # Execution count [3.65e-01]
        movl      %eax, %eax                                    #533.31
        xorl      %r12d, %r12d                                  #533.31
        incl      %edx                                          #533.31
        addl      (%r8,%rax,4), %esi                            #533.31
        incl      %eax                                          #533.31
        cmpl      %r10d, %eax                                   #533.31
        cmove     %r9d, %r12d                                   #533.31
        decl      %r12d                                         #533.31
        andl      %r12d, %eax                                   #533.31
        xorl      %r12d, %r12d                                  #533.31
        movl      %eax, %eax                                    #533.31
        addl      (%r8,%rax,4), %ecx                            #533.31
        incl      %eax                                          #533.31
        cmpl      %r10d, %eax                                   #533.31
        cmove     %r9d, %r12d                                   #533.31
        decl      %r12d                                         #533.31
        andl      %r12d, %eax                                   #533.31
        cmpl      %r11d, %edx                                   #533.31
        jb        ..B5.11       # Prob 63%                      #533.31
                                # LOE rax rbx rbp r8 r13 edx ecx esi edi r9d r10d r11d r14d r15d
..B5.12:                        # Preds ..B5.11
                                # Execution count [1.32e-01]
        addl      %ecx, %esi                                    #533.31
        lea       1(%rdx,%rdx), %ecx                            #533.31
                                # LOE rax rbx rbp r8 r13 ecx esi edi r14d r15d
..B5.13:                        # Preds ..B5.12 ..B5.9
                                # Execution count [1.46e-01]
        decl      %ecx                                          #533.31
        cmpl      %edi, %ecx                                    #533.31
        jae       ..B5.2        # Prob 10%                      #533.31
                                # LOE rax rbx rbp r8 r13 esi r14d r15d
..B5.14:                        # Preds ..B5.13
                                # Execution count [1.32e-01]
        addl      (%r8,%rax,4), %esi                            #533.31
        jmp       ..B5.2        # Prob 100%                     #533.31
                                # LOE rbx rbp r13 esi r14d r15d
..B5.17:                        # Preds ..B5.5
                                # Execution count [3.52e-02]: Infreq
        movl      %r12d, %eax                                   #
        popq      %rcx                                          #542.12
	.cfi_def_cfa_offset 40
	.cfi_restore 15
        popq      %r15                                          #542.12
	.cfi_def_cfa_offset 32
	.cfi_restore 14
        popq      %r14                                          #542.12
	.cfi_def_cfa_offset 24
	.cfi_restore 13
        popq      %r13                                          #542.12
	.cfi_def_cfa_offset 16
	.cfi_restore 12
        popq      %r12                                          #542.12
	.cfi_def_cfa_offset 8
        ret                                                     #542.12
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	dynais_hierarchical,@function
	.size	dynais_hierarchical,.-dynais_hierarchical
	.data
# -- End  dynais_hierarchical
	.text
# -- Begin  dynais_basic
	.text
# mark_begin;
       .align    16,0x90
# --- dynais_basic(unsigned long, unsigned int, unsigned int)
dynais_basic:
# parameter 1: %rdi
# parameter 2: %esi
# parameter 3: %edx
..B6.1:                         # Preds ..B6.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_dynais_basic.163:
..L164:
                                                        #191.1
        pushq     %r12                                          #191.1
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
        pushq     %r13                                          #191.1
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
        pushq     %r14                                          #191.1
	.cfi_def_cfa_offset 32
	.cfi_offset 14, -32
        pushq     %r15                                          #191.1
	.cfi_def_cfa_offset 40
	.cfi_offset 15, -40
        pushq     %rbx                                          #191.1
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
        movl      %edx, %r11d                                   #191.1
        vpbroadcastq %rdi, %zmm5                                #244.20
        vpbroadcastd %esi, %zmm4                                #245.20
        vpternlogd $255, %zmm1, %zmm1, %zmm1                    #249.20
        vmovups   shifts.510.0.3(%rip), %zmm3                   #246.51
        vmovups   .L_2il0floatpacket.1(%rip), %zmm2             #247.20
        movl      level_index(,%r11,4), %ebx                    #232.14
        movq      sizes_vec(,%r11,8), %rdx                      #235.14
        movl      level_limit(,%r11,4), %r10d                   #233.14
        movq      samples_vec(,%r11,8), %rcx                    #234.14
        movq      zeros_vec(,%r11,8), %r14                      #236.14
        movq      indes_vec(,%r11,8), %r13                      #237.14
        movl      $0, (%rdx,%rbx,4)                             #240.2
        movl      _window(%rip), %r8d                           #241.8
        movl      (%r14), %eax                                  #241.20
        movq      $0, (%rcx,%rbx,8)                             #239.2
        movl      %eax, (%r14,%r8,4)                            #241.2
        movl      _window(%rip), %eax                           #242.8
        movl      (%r13), %r9d                                  #242.20
        movl      %r9d, (%r13,%rax,4)                           #242.2
        xorl      %r9d, %r9d                                    #255.7
        movl      _window(%rip), %r12d                          #255.25
        vpxord    %zmm0, %zmm0, %zmm0                           #248.20
        testl     %r12d, %r12d                                  #255.25
        jbe       ..B6.7        # Prob 10%                      #255.25
                                # LOE rdx rcx rbx rbp rdi r9 r11 r13 r14 esi r10d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B6.2:                         # Preds ..B6.1
                                # Execution count [9.00e-01]
        movl      %esi, -16(%rsp)                               #259.21[spill]
        xorl      %r8d, %r8d                                    #259.21
                                # LOE rdx rcx rbx rbp rdi r9 r11 r13 r14 r8d r10d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B6.3:                         # Preds ..B6.5 ..B6.2
                                # Execution count [5.00e+00]
        movl      $32768, %eax                                  #281.17
        kmovw     %eax, %k1                                     #281.17
        lea       8(%r8), %eax                                  #259.64
        vpcmpeqq  (%rcx,%rax,8), %zmm5, %k0                     #264.19
        vpcmpeqq  (%rcx,%r9,8), %zmm5, %k2                      #263.19
        vpermd    (%r14,%r9,4), %zmm3, %zmm6                    #278.23
        vpermd    (%r13,%r9,4), %zmm3, %zmm7                    #279.23
        kmovw     %k0, %r15d                                    #264.19
        kmovw     %k2, %esi                                     #263.19
        shll      $8, %r15d                                     #268.20
        addl      $16, %r8d                                     #281.71
        orl       %esi, %r15d                                   #268.20
        movl      %r8d, %eax                                    #281.61
        kmovw     %r15d, %k3                                    #271.17
        vpbroadcastd (%r14,%rax,4), %zmm6{%k1}                  #281.17
        vpbroadcastd (%r13,%rax,4), %zmm7{%k1}                  #282.17
        vpcmpeqd  (%rdx,%r9,4), %zmm4, %k4{%k3}                 #271.17
        vpaddd    %zmm2, %zmm6, %zmm6{%k4}{z}                   #285.21
        vpcmpgtd  %zmm7, %zmm6, %k1                             #291.17
        vmovups   %zmm6, (%r14,%r9,4)                           #287.36
        vmovups   %zmm7, (%r13,%r9,4)                           #288.42
        kmovw     %k1, %r9d                                     #291.17
        testl     %r9d, %r9d                                    #293.22
        je        ..B6.5        # Prob 50%                      #293.22
                                # LOE rax rdx rcx rbx rbp rdi r11 r13 r14 r8d r10d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5 zmm6 zmm7 k1
..B6.4:                         # Preds ..B6.3
                                # Execution count [2.50e+00]
        vmovdqa32 %zmm6, %zmm6{%k1}{z}                          #296.25
        vmovdqa32 %zmm7, %zmm7{%k1}{z}                          #297.25
        vpcmpud   $1, %zmm6, %zmm0, %k1                         #301.17
        vpcmpud   $1, %zmm1, %zmm7, %k2{%k1}                    #302.17
        vmovdqa32 %zmm6, %zmm0{%k2}                             #305.21
        vmovdqa32 %zmm7, %zmm1{%k2}                             #306.21
                                # LOE rax rdx rcx rbx rbp rdi r11 r13 r14 r8d r10d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B6.5:                         # Preds ..B6.4 ..B6.3
                                # Execution count [5.00e+00]
        movq      %rax, %r9                                     #255.34
        cmpl      %r12d, %r8d                                   #255.25
        jb        ..B6.3        # Prob 82%                      #255.25
                                # LOE rdx rcx rbx rbp rdi r9 r11 r13 r14 r8d r10d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B6.6:                         # Preds ..B6.5
                                # Execution count [9.00e-01]
        movl      -16(%rsp), %esi                               #[spill]
                                # LOE rdx rcx rbx rbp rdi r11 esi r10d zmm0 zmm1
..B6.7:                         # Preds ..B6.6 ..B6.1
                                # Execution count [1.00e+00]
        vextractf64x4 $1, %zmm0, %ymm2                          #309.21
        xorl      %r12d, %r12d                                  #328.5
        vbroadcastss .L_2il0floatpacket.4(%rip), %zmm13         #312.21
        movl      level_previous_length(,%r11,4), %r8d          #322.20
        movl      %esi, (%rdx,%rbx,4)                           #324.5
        movl      $1, %esi                                      #328.5
        movq      %rdi, (%rcx,%rbx,8)                           #323.5
        testl     %ebx, %ebx                                    #328.5
        movl      _window(%rip), %r13d                          #329.38
        movl      $0, %ecx                                      #328.5
        cmove     %esi, %r12d                                   #328.5
        decl      %r13d                                         #333.33
        cmpl      %r13d, %r10d                                  #333.5
        movl      $0, %edi                                      #340.5
        movl      %r12d, -16(%rsp)                              #328.5[spill]
        cmovne    %ecx, %esi                                    #333.5
        decl      %esi                                          #333.39
        lea       -1(%r12), %r12d                               #328.27
        vpmaxud   %ymm0, %ymm2, %ymm4                           #309.21
        vpermpd   $255, %ymm4, %ymm3                            #309.21
        vpermpd   $170, %ymm4, %ymm5                            #309.21
        vpermpd   $85, %ymm4, %ymm6                             #309.21
        vpmaxud   %ymm4, %ymm3, %ymm7                           #309.21
        vpmaxud   %ymm6, %ymm5, %ymm8                           #309.21
        vpmaxud   %ymm8, %ymm7, %ymm9                           #309.21
        vpsrlq    $32, %xmm9, %xmm10                            #309.21
        vpmaxud   %xmm9, %xmm10, %xmm11                         #309.21
        vmovd     %xmm11, %eax                                  #309.21
        vpbroadcastd %eax, %zmm12                               #310.24
        vpcmpeqd  %zmm12, %zmm0, %k1                            #311.21
        vblendmps %zmm1, %zmm13, %zmm1{%k1}                     #312.21
        vextractf64x4 $1, %zmm1, %ymm14                         #312.21
        vpminud   %ymm1, %ymm14, %ymm0                          #312.21
        vpermpd   $255, %ymm0, %ymm15                           #312.21
        vpermpd   $170, %ymm0, %ymm1                            #312.21
        vpermpd   $85, %ymm0, %ymm2                             #312.21
        vpminud   %ymm0, %ymm15, %ymm3                          #312.21
        vpminud   %ymm2, %ymm1, %ymm4                           #312.21
        vpminud   %ymm4, %ymm3, %ymm5                           #312.21
        vpsrlq    $32, %xmm5, %xmm6                             #312.21
        vpminud   %xmm5, %xmm6, %xmm7                           #312.21
        vmovd     %xmm7, %r9d                                   #312.21
        testl     %r9d, %r9d                                    #340.5
        seta      %dil                                          #340.5
        cmpl      %r9d, %eax                                    #340.5
        seta      %cl                                           #340.5
        andl      %ecx, %edi                                    #340.45
        je        ..B6.12       # Prob 50%                      #347.21
                                # LOE rbp r11 eax ebx esi edi r8d r9d r10d r12d r13d
..B6.8:                         # Preds ..B6.7
                                # Execution count [5.00e-01]
        cmpl      $1, %r9d                                      #347.47
        je        ..B6.10       # Prob 50%                      #347.47
                                # LOE rbp r11 eax ebx esi edi r8d r9d r10d r12d r13d
..B6.9:                         # Preds ..B6.8
                                # Execution count [2.50e-01]
        xorl      %edx, %edx                                    #347.65
        divl      %r9d                                          #347.65
        cmpl      $1, %edx                                      #347.80
        jne       ..B6.12       # Prob 50%                      #347.80
                                # LOE rbp r11 ebx esi edi r8d r9d r10d r12d r13d
..B6.10:                        # Preds ..B6.9 ..B6.8
                                # Execution count [3.75e-01]
        movl      $1, %edx                                      #347.21
        jmp       ..B6.13       # Prob 100%                     #347.21
                                # LOE rbp r11 edx ebx esi edi r8d r9d r10d r12d r13d
..B6.12:                        # Preds ..B6.9 ..B6.7
                                # Execution count [5.00e-01]
        xorl      %edx, %edx                                    #347.21
                                # LOE rbp r11 edx ebx esi edi r8d r9d r10d r12d r13d
..B6.13:                        # Preds ..B6.10 ..B6.12
                                # Execution count [1.00e+00]
        cmpl      %r9d, %r8d                                    #352.5
        movl      $1, %r14d                                     #352.5
        movl      $0, %ecx                                      #352.5
        cmovne    %r14d, %ecx                                   #352.5
        andl      %edx, %ecx                                    #352.52
        cmpl      %r9d, %r8d                                    #358.36
        je        ..B6.15       # Prob 50%                      #358.36
                                # LOE rbp r11 edx ecx ebx esi edi r8d r9d r10d r12d r13d
..B6.14:                        # Preds ..B6.13
                                # Execution count [5.00e-01]
        testl     %r8d, %r8d                                    #358.36
        movl      $1, %r15d                                     #358.36
        cmove     %r8d, %r15d                                   #358.36
        jmp       ..B6.16       # Prob 100%                     #358.36
                                # LOE rbp r11 edx ecx ebx esi edi r8d r9d r10d r12d r13d r15d
..B6.15:                        # Preds ..B6.13
                                # Execution count [5.00e-01]
        xorl      %r15d, %r15d                                  #358.36
                                # LOE rbp r11 edx ecx ebx esi edi r8d r9d r10d r12d r13d r15d
..B6.16:                        # Preds ..B6.14 ..B6.15
                                # Execution count [1.00e+00]
        testl     %edi, %edi                                    #365.5
        movl      $1, %r14d                                     #365.5
        movl      $0, %eax                                      #365.5
        cmove     %r14d, %eax                                   #365.5
        addl      %ecx, %edi                                    #363.5
        andl      %r15d, %eax                                   #361.26
        andl      %r15d, %ecx                                   #365.26
        addl      %edx, %edi                                    #364.5
        subl      %ecx, %eax                                    #365.5
        subl      %eax, %edi                                    #365.5
        cmpl      $3, %edi                                      #368.19
        jl        ..B6.18       # Prob 50%                      #368.19
                                # LOE rbp r11 ebx esi edi r8d r9d r10d r12d r13d
..B6.17:                        # Preds ..B6.16
                                # Execution count [5.00e-01]
        movl      %r9d, %ecx                                    #368.22
        jmp       ..B6.19       # Prob 100%                     #368.22
                                # LOE rbp r11 ecx ebx esi edi r9d r10d r12d r13d
..B6.18:                        # Preds ..B6.16
                                # Execution count [5.00e-01]
        testl     %edi, %edi                                    #369.27
        movl      $0, %ecx                                      #369.27
        cmovg     %r8d, %ecx                                    #369.27
                                # LOE rbp r11 ecx ebx esi edi r9d r10d r12d r13d
..B6.19:                        # Preds ..B6.17 ..B6.18
                                # Execution count [1.00e+00]
        movl      %ecx, level_previous_length(,%r11,4)          #373.5
        incl      %r10d                                         #334.23
        movl      -16(%rsp), %ecx                               #329.21[spill]
        notl      %r12d                                         #329.30
        andl      %esi, %r10d                                   #334.28
        notl      %esi                                          #334.54
        andl      %r13d, %r12d                                  #329.48
        andl      %esi, %r13d                                   #334.54
        addl      %r13d, %r10d                                  #334.54
        lea       -1(%rbx,%rcx), %ebx                           #329.21
        addl      %r12d, %ebx                                   #329.48
        movl      %edi, %eax                                    #377.12
        movl      %r9d, level_length(,%r11,4)                   #372.5
        movl      %ebx, level_index(,%r11,4)                    #374.5
        movl      %r10d, level_limit(,%r11,4)                   #375.5
	.cfi_restore 3
        popq      %rbx                                          #377.12
	.cfi_def_cfa_offset 40
	.cfi_restore 15
        popq      %r15                                          #377.12
	.cfi_def_cfa_offset 32
	.cfi_restore 14
        popq      %r14                                          #377.12
	.cfi_def_cfa_offset 24
	.cfi_restore 13
        popq      %r13                                          #377.12
	.cfi_def_cfa_offset 16
	.cfi_restore 12
        popq      %r12                                          #377.12
	.cfi_def_cfa_offset 8
        ret                                                     #377.12
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	dynais_basic,@function
	.size	dynais_basic,.-dynais_basic
	.section .rodata, "a"
	.align 64
	.align 64
shifts.510.0.3:
	.long	1
	.long	2
	.long	3
	.long	4
	.long	5
	.long	6
	.long	7
	.long	8
	.long	9
	.long	10
	.long	11
	.long	12
	.long	13
	.long	14
	.long	15
	.long	15
	.data
# -- End  dynais_basic
	.text
# -- Begin  dynais_init
	.text
# mark_begin;
       .align    16,0x90
	.globl dynais_init
# --- dynais_init(unsigned int, unsigned int)
dynais_init:
# parameter 1: %edi
# parameter 2: %esi
..B7.1:                         # Preds ..B7.0
                                # Execution count [1.00e+00]
        pushq     %rsi                                          #76.1
        movq      $0x7189d97ff, %rdx                            #76.1
                                # LOE rdx rbx rbp r12 r13 r14 r15 esi edi
..B7.3:                         # Preds ..B7.1 ..B7.8
                                # Execution count [1.00e+00]
        movq      __intel_cpu_feature_indicator(%rip), %rax     #76.1
        andq      %rdx, %rax                                    #76.1
        cmpq      %rdx, %rax                                    #76.1
        jne       ..B7.6        # Prob 10%                      #76.1
                                # LOE rdx rbx rbp r12 r13 r14 r15 esi edi
..B7.4:                         # Preds ..B7.3
                                # Execution count [1.00e+01]
        addq      $8, %rsp                                      #76.1
        jmp       dynais_init.Z                                 #76.1
                                # LOE
..B7.6:                         # Preds ..B7.3
                                # Execution count [1.00e+00]
        testb     $1, __intel_cpu_feature_indicator(%rip)       #76.1
        je        ..B7.8        # Prob 10%                      #76.1
                                # LOE rdx rbx rbp r12 r13 r14 r15 esi edi
..B7.7:                         # Preds ..B7.6
                                # Execution count [1.00e+01]
        addq      $8, %rsp                                      #76.1
        jmp       dynais_init.A                                 #76.1
                                # LOE
..B7.8:                         # Preds ..B7.6
                                # Execution count [1.00e+00]
        call      __intel_cpu_features_init                     #76.1
        jmp       ..B7.3        # Prob 100%                     #76.1
        .align    16,0x90
                                # LOE rdx rbx rbp r12 r13 r14 r15 esi edi
# mark_end;
	.type	dynais_init,@function
	.size	dynais_init,.-dynais_init
# -- End  dynais_init
# -- Begin  dynais_init.Z
	.text
# mark_begin;
# Threads 2
        .align    16,0x90
# --- dynais_init.Z..0(unsigned int, unsigned int)
dynais_init.Z:
# parameter 1: %edi
# parameter 2: %esi
..B8.1:                         # Preds ..B8.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_dynais_init.Z.186:
..L187:
                                                        #76.1
        pushq     %rbp                                          #76.1
	.cfi_def_cfa_offset 16
        movq      %rsp, %rbp                                    #76.1
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
        andq      $-64, %rsp                                    #76.1
        subq      $128, %rsp                                    #76.1 c1
        movq      %rbx, 64(%rsp)                                #76.1[spill] c3
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x22
        movl      %esi, %ebx                                    #76.1 c3
        movq      %r12, 96(%rsp)                                #76.1[spill] c3
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
        movl      %edi, %r12d                                   #76.1 c3
        movq      %r13, 72(%rsp)                                #76.1[spill] c5
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc8, 0xff, 0xff, 0xff, 0x22
        movl      %r12d, %r13d                                  #92.82 c5
        movq      %r14, 80(%rsp)                                #76.1[spill] c5
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd0, 0xff, 0xff, 0xff, 0x22
        movl      %ebx, %r14d                                   #92.91 c5
        movq      %r15, 88(%rsp)                                #76.1[spill] c7
        movl      $10, %eax                                     #83.5 c7
        cmpl      $10, %ebx                                     #83.5 c7
        lea       (,%r13,8), %rdx                               #92.16 c7
        cmovbe    %ebx, %eax                                    #83.5 c9
        movl      %eax, _levels(%rip)                           #83.5 c11
        movl      $40000, %ecx                                  #87.2 c11
        cmpl      $40000, %r12d                                 #87.2 c11
        lea       32(%rsp), %rdi                                #92.16 c11
        cmovbe    %r12d, %ecx                                   #87.2 c13
        imulq     %r14, %rdx                                    #92.16 c13
        andl      $-16, %ecx                                    #87.29 c15
        addl      $16, %ecx                                     #87.29 c17
        movl      %ecx, _window(%rip)                           #87.2 c19
        movl      $64, %esi                                     #92.16 c19
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #92.16 c21
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
                                # LOE r13 r14 eax ebx r12d
..B8.66:                        # Preds ..B8.1
                                # Execution count [1.00e+00]
        movl      %eax, %r15d                                   #92.16 c1
                                # LOE r13 r14 ebx r12d r15d
..B8.2:                         # Preds ..B8.66
                                # Execution count [1.00e+00]
        shlq      $2, %r13                                      #93.16 c1
        lea       40(%rsp), %rdi                                #93.16 c1
        movl      $64, %esi                                     #93.16 c1
        imulq     %r14, %r13                                    #93.16 c3
        movq      %r13, %rdx                                    #93.16 c7 stall 1
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #93.16 c9
                                # LOE r14 eax ebx r12d r15d
..B8.67:                        # Preds ..B8.2
                                # Execution count [1.00e+00]
        movl      %eax, (%rsp)                                  #93.16[spill] c1
                                # LOE r14 ebx r12d r15d
..B8.3:                         # Preds ..B8.67
                                # Execution count [1.00e+00]
        lea       8(%r12), %r13d                                #94.92 c1
        movl      %r13d, %edx                                   #94.92 c1
        shlq      $2, %rdx                                      #94.92 c1
        lea       48(%rsp), %rdi                                #94.16 c1
        movl      $64, %esi                                     #94.16 c1
        imulq     %r14, %rdx                                    #94.97 c3
        movq      %rdx, 8(%rsp)                                 #94.97[spill] c7 stall 1
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #94.16 c7
                                # LOE eax ebx r12d r13d r15d
..B8.68:                        # Preds ..B8.3
                                # Execution count [1.00e+00]
        movl      %eax, %r14d                                   #94.16 c1
                                # LOE ebx r12d r13d r14d r15d
..B8.4:                         # Preds ..B8.68
                                # Execution count [1.00e+00]
        lea       56(%rsp), %rdi                                #95.16 c1
        movl      $64, %esi                                     #95.16 c1
        movq      -48(%rdi), %rdx                               #95.16[spill] c3
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #95.16 c5
                                # LOE eax ebx r12d r13d r14d r15d
..B8.5:                         # Preds ..B8.4
                                # Execution count [1.00e+00]
        orl       %r14d, %r15d                                  #98.21 c1
        orl       (%rsp), %r15d                                 #98.21[spill] c3
        orl       %eax, %r15d                                   #98.21 c5
        jne       ..B8.57       # Prob 28%                      #98.21 c7
                                # LOE ebx r12d r13d
..B8.6:                         # Preds ..B8.5
                                # Execution count [6.48e-01]
        testl     %ebx, %ebx                                    #102.21 c1
        jbe       ..B8.56       # Prob 0%                       #102.21 c3
                                # LOE ebx r12d r13d
..B8.7:                         # Preds ..B8.6
                                # Execution count [6.48e-01]
        movq      32(%rsp), %rax                                #110.27 c1
        movq      48(%rsp), %rdx                                #114.25 c1
        movq      40(%rsp), %rcx                                #111.25 c5 stall 1
        movq      56(%rsp), %r8                                 #115.19 c5
        cmpl      $8, %ebx                                      #102.5 c5
        jb        ..B8.63       # Prob 10%                      #102.5 c7
                                # LOE rax rdx rcx r8 ebx r12d r13d
..B8.8:                         # Preds ..B8.7
                                # Execution count [6.48e-01]
        movl      $8, %esi                                      #120.3 c1
        movl      %ebx, %edi                                    #102.5 c1
        vmovd     %r13d, %xmm0                                  #115.41 c1
        vmovd     %esi, %xmm2                                   #120.3 c3
        andl      $-8, %edi                                     #102.5 c3
        xorl      %r9d, %r9d                                    #102.5 c3
        vpbroadcastd %xmm2, %ymm3                               #120.3 c5
        vmovdqu   .L_2il0floatpacket.5(%rip), %ymm2             #120.3 c5
        vpbroadcastd %xmm0, %ymm1                               #115.41 c5
        vpbroadcastq %r8, %zmm0                                 #115.19 c5
        movslq    %edi, %rsi                                    #102.5 c7
                                # LOE rax rdx rcx rsi r8 r9 ebx edi r12d r13d ymm1 ymm2 ymm3 zmm0
..B8.9:                         # Preds ..B8.9 ..B8.8
                                # Execution count [3.60e+00]
        vpmulld   %ymm1, %ymm2, %ymm4                           #115.19 c1
        vpaddd    %ymm3, %ymm2, %ymm2                           #120.3 c1
        vpmovzxdq %ymm4, %zmm5                                  #115.19 c3
        vpsllq    $2, %zmm5, %zmm6                              #115.19 c5
        vpaddq    %zmm6, %zmm0, %zmm7                           #115.19 c7
        vmovdqu64 %zmm7, indes_vec(,%r9,8)                      #115.3 c9
        addq      $8, %r9                                       #102.5 c9
        cmpq      %rsi, %r9                                     #102.5 c11
        jb        ..B8.9        # Prob 82%                      #102.5 c13
                                # LOE rax rdx rcx rsi r8 r9 ebx edi r12d r13d ymm1 ymm2 ymm3 zmm0
..B8.11:                        # Preds ..B8.9 ..B8.63
                                # Execution count [7.20e-01]
        lea       1(%rdi), %r9d                                 #102.5 c1
        movslq    %edi, %rsi                                    #102.5 c1
        xorl      %r10d, %r10d                                  #102.5 c1
        cmpl      %ebx, %r9d                                    #102.5 c3
        ja        ..B8.15       # Prob 10%                      #102.5 c5
                                # LOE rax rdx rcx rsi r8 ebx edi r10d r12d r13d
..B8.12:                        # Preds ..B8.11
                                # Execution count [6.48e-01]
        movl      %edi, %r9d                                    #115.41 c1
        negl      %edi                                          #102.5 c1
        imull     %r13d, %r9d                                   #115.41 c3
        addl      %ebx, %edi                                    #102.5 c3
                                # LOE rax rdx rcx rsi r8 r9 ebx edi r10d r12d r13d
..B8.13:                        # Preds ..B8.13 ..B8.12
                                # Execution count [3.60e+00]
        addl      $1, %r10d                                     #102.5 c1
        lea       (%r8,%r9,4), %r11                             #115.19 c1
        movq      %r11, indes_vec(,%rsi,8)                      #115.3 c3
        addq      $1, %rsi                                      #102.5 c3
        lea       8(%r9,%r12), %r9d                             #102.5 c3
        cmpl      %edi, %r10d                                   #102.5 c3
        jb        ..B8.13       # Prob 82%                      #102.5 c5
                                # LOE rax rdx rcx rsi r8 r9 ebx edi r10d r12d r13d
..B8.15:                        # Preds ..B8.13 ..B8.11
                                # Execution count [6.48e-01]
        movl      %r12d, %r9d                                   #117.3 c1
        movl      %r13d, 24(%rsp)                               #117.3[spill] c1
        movl      %r12d, %edi                                   #117.3 c1
        movq      %rcx, 16(%rsp)                                #117.3[spill] c1
        andl      $15, %r9d                                     #117.3 c3
        movq      %rdx, 8(%rsp)                                 #117.3[spill] c3
        xorl      %r8d, %r8d                                    #102.5 c3
        movq      %rax, (%rsp)                                  #117.3[spill] c3
        negl      %r9d                                          #117.3 c5
        vmovups   .L_2il0floatpacket.0(%rip), %zmm2             #117.3 c5
        vmovdqu32 .L_2il0floatpacket.2(%rip), %zmm1             #117.3 c5
        movslq    %ebx, %r10                                    #102.5 c5
        addl      %r12d, %r9d                                   #117.3 c7
        andl      $-16, %edi                                    #117.3 c7
        xorl      %ecx, %ecx                                    #117.3 c9
        vmovdqu32 .L_2il0floatpacket.6(%rip), %zmm0             #118.24 c11
        lea       -1(%r12), %esi                                #120.30 c11
                                # LOE r8 r10 ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.16:                        # Preds ..B8.33 ..B8.15
                                # Execution count [3.60e+00]
        movl      %ecx, level_limit(,%r8,4)                     #104.9 c1
        testl     %r12d, %r12d                                  #117.19 c1
        movl      %ecx, level_index(,%r8,4)                     #105.9 c1
        movl      %ecx, level_result(,%r8,4)                    #106.9 c3
        movl      %ecx, level_length(,%r8,4)                    #107.9 c3
        jbe       ..B8.33       # Prob 50%                      #117.19 c3
                                # LOE r8 r10 ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.17:                        # Preds ..B8.16
                                # Execution count [3.24e+00]
        cmpl      $81, %r12d                                    #117.3 c1
        jb        ..B8.62       # Prob 10%                      #117.3 c3
                                # LOE r8 r10 ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.18:                        # Preds ..B8.17
                                # Execution count [3.24e+00]
        movl      indes_vec(,%r8,8), %eax                       #118.4 c1
        andl      $63, %eax                                     #117.3 c5 stall 1
        je        ..B8.24       # Prob 50%                      #117.3 c7
                                # LOE r8 r10 eax ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.19:                        # Preds ..B8.18
                                # Execution count [3.24e+00]
        testb     $3, %al                                       #117.3 c1
        jne       ..B8.58       # Prob 10%                      #117.3 c3
                                # LOE r8 r10 eax ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.20:                        # Preds ..B8.19
                                # Execution count [0.00e+00]
        negl      %eax                                          #117.3 c1
        movl      %r12d, %edx                                   #117.3 c1
        addl      $64, %eax                                     #117.3 c3
        shrl      $2, %eax                                      #117.3 c5
        cmpl      %eax, %r12d                                   #117.3 c7
        cmovb     %r12d, %eax                                   #117.3 c9
        subl      %eax, %edx                                    #117.3 c11
        andl      $15, %edx                                     #117.3 c13
        negl      %edx                                          #117.3 c15
        addl      %r12d, %edx                                   #117.3 c17
        cmpl      $1, %eax                                      #117.3 c17
        jb        ..B8.25       # Prob 50%                      #117.3 c19
                                # LOE r8 r10 eax edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.21:                        # Preds ..B8.20
                                # Execution count [3.24e+00]
        vmovaps   %zmm1, %zmm5                                  #117.3 c1
        vmovaps   %zmm0, %zmm4                                  #118.24 c1
        vpbroadcastd %eax, %zmm3                                #117.3 c1
        xorl      %r14d, %r14d                                  #117.3 c1
        movslq    %eax, %r13                                    #117.3 c1
        xorl      %r11d, %r11d                                  #117.3 c3
                                # LOE r8 r10 r11 r13 r14 eax edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B8.22:                        # Preds ..B8.22 ..B8.21
                                # Execution count [1.80e+01]
        movq      indes_vec(,%r8,8), %r15                       #118.4 c1
        vpcmpud   $6, %zmm5, %zmm3, %k1                         #117.3 c1
        vmovdqu32 %zmm4, (%r15,%r11){%k1}                       #118.4 c5 stall 1
        addq      $16, %r14                                     #117.3 c5
        addq      $64, %r11                                     #117.3 c5
        vpaddd    %zmm2, %zmm5, %zmm5                           #117.3 c5
        vpaddd    %zmm2, %zmm4, %zmm4                           #118.24 c5
        cmpq      %r13, %r14                                    #117.3 c7
        jb        ..B8.22       # Prob 82%                      #117.3 c9
                                # LOE r8 r10 r11 r13 r14 eax edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B8.23:                        # Preds ..B8.22
                                # Execution count [3.24e+00]
        cmpl      %eax, %r12d                                   #117.3 c1
        je        ..B8.33       # Prob 10%                      #117.3 c3
        jmp       ..B8.25       # Prob 100%                     #117.3 c3
                                # LOE r8 r10 eax edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.24:                        # Preds ..B8.18
                                # Execution count [1.62e+00]
        movl      %r9d, %edx                                    #117.3 c1
                                # LOE r8 r10 eax edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.25:                        # Preds ..B8.24 ..B8.23 ..B8.20 ..B8.62
                                # Execution count [0.00e+00]
        lea       -1(%rax), %r11d                               #118.24 c1
        lea       16(%rax), %r13d                               #117.3 c1
        vpbroadcastd %r11d, %zmm3                               #118.24 c3
        vpaddd    .L_2il0floatpacket.2(%rip), %zmm3, %zmm3      #118.24 c9 stall 2
        cmpl      %r13d, %edx                                   #117.3 c9
        jb        ..B8.29       # Prob 50%                      #117.3 c11
                                # LOE r8 r10 eax edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2 zmm3
..B8.26:                        # Preds ..B8.25
                                # Execution count [3.24e+00]
        movslq    %eax, %rax                                    #117.3 c1
        movslq    %edx, %r11                                    #117.3 c1
                                # LOE rax r8 r10 r11 edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2 zmm3
..B8.27:                        # Preds ..B8.27 ..B8.26
                                # Execution count [1.80e+01]
        movq      indes_vec(,%r8,8), %r13                       #118.4 c1
        vmovdqu32 %zmm3, (%r13,%rax,4)                          #118.4 c5 stall 1
        addq      $16, %rax                                     #117.3 c5
        vpaddd    %zmm2, %zmm3, %zmm3                           #118.24 c5
        cmpq      %r11, %rax                                    #117.3 c7
        jb        ..B8.27       # Prob 82%                      #117.3 c9
                                # LOE rax r8 r10 r11 edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2 zmm3
..B8.29:                        # Preds ..B8.27 ..B8.25 ..B8.58
                                # Execution count [3.60e+00]
        lea       1(%rdx), %eax                                 #117.3 c1
        cmpl      %r12d, %eax                                   #117.3 c3
        ja        ..B8.33       # Prob 50%                      #117.3 c5
                                # LOE r8 r10 edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.30:                        # Preds ..B8.29
                                # Execution count [3.24e+00]
        lea       -1(%rdx), %eax                                #118.24 c1
        vmovaps   %zmm1, %zmm5                                  #117.3 c1
        movl      %ecx, %r11d                                   #117.3 c1
        vpbroadcastd %eax, %zmm3                                #118.24 c3
        movl      %r12d, %eax                                   #117.3 c3
        subl      %edx, %eax                                    #117.3 c5
        movslq    %edx, %rdx                                    #117.3 c5
        shlq      $2, %rdx                                      #118.4 c7
        vpaddd    .L_2il0floatpacket.2(%rip), %zmm3, %zmm4      #118.24 c9
        vpbroadcastd %eax, %zmm3                                #117.3 c9
                                # LOE rdx r8 r10 eax ecx ebx esi edi r9d r11d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B8.31:                        # Preds ..B8.31 ..B8.30
                                # Execution count [1.80e+01]
        movq      indes_vec(,%r8,8), %r13                       #118.4 c1
        vpcmpud   $6, %zmm5, %zmm3, %k1                         #117.3 c1
        vmovdqu32 %zmm4, (%rdx,%r13){%k1}                       #118.4 c5 stall 1
        addl      $16, %r11d                                    #117.3 c5
        addq      $64, %rdx                                     #117.3 c5
        vpaddd    %zmm2, %zmm5, %zmm5                           #117.3 c5
        vpaddd    %zmm2, %zmm4, %zmm4                           #118.24 c5
        cmpl      %eax, %r11d                                   #117.3 c7
        jb        ..B8.31       # Prob 82%                      #117.3 c9
                                # LOE rdx r8 r10 eax ecx ebx esi edi r9d r11d r12d zmm0 zmm1 zmm2 zmm3 zmm4 zmm5
..B8.33:                        # Preds ..B8.31 ..B8.16 ..B8.23 ..B8.29
                                # Execution count [3.60e+00]
        movq      indes_vec(,%r8,8), %rax                       #120.3 c1
        movl      %esi, (%rax)                                  #120.3 c5 stall 1
        addq      $1, %r8                                       #102.5 c5
        cmpq      %r10, %r8                                     #102.5 c7
        jb        ..B8.16       # Prob 82%                      #102.5 c9
                                # LOE r8 r10 ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.34:                        # Preds ..B8.33
                                # Execution count [6.48e-01]
        movl      24(%rsp), %r13d                               #[spill] c1
        movq      16(%rsp), %rcx                                #[spill] c1
        movq      8(%rsp), %rdx                                 #[spill] c5 stall 1
        movq      (%rsp), %rax                                  #[spill] c5
        cmpl      $8, %ebx                                      #102.5 c5
        jb        ..B8.61       # Prob 10%                      #102.5 c7
                                # LOE rax rdx rcx ebx r12d r13d
..B8.35:                        # Preds ..B8.34
                                # Execution count [6.48e-01]
        vmovdqu   .L_2il0floatpacket.5(%rip), %ymm1             #120.3 c1
        movl      $8, %edi                                      #120.3 c1
        movl      %ebx, %esi                                    #102.5 c1
        vmovd     %r12d, %xmm2                                  #75.5 c1
        vmovd     %edi, %xmm0                                   #120.3 c3
        andl      $-8, %esi                                     #102.5 c3
        vpbroadcastd %xmm2, %ymm3                               #75.5 c3
        vpbroadcastq %rcx, %zmm2                                #111.25 c3
        vpbroadcastd %xmm0, %ymm0                               #120.3 c5
        xorl      %edi, %edi                                    #102.5 c5
        vmovdqa   %ymm1, %ymm4                                  #120.3 c7
        movslq    %esi, %r8                                     #102.5 c7
                                # LOE rax rdx rcx rdi r8 ebx esi r12d r13d ymm0 ymm1 ymm3 ymm4 zmm2
..B8.36:                        # Preds ..B8.36 ..B8.35
                                # Execution count [3.60e+00]
        vpmulld   %ymm3, %ymm4, %ymm5                           #111.25 c1
        vpaddd    %ymm0, %ymm4, %ymm4                           #120.3 c1
        vpmovzxdq %ymm5, %zmm6                                  #111.25 c3
        vpsllq    $2, %zmm6, %zmm7                              #111.25 c5
        vpaddq    %zmm7, %zmm2, %zmm8                           #111.25 c7
        vmovdqu64 %zmm8, sizes_vec(,%rdi,8)                     #111.9 c9
        addq      $8, %rdi                                      #102.5 c9
        cmpq      %r8, %rdi                                     #102.5 c11
        jb        ..B8.36       # Prob 82%                      #102.5 c13
                                # LOE rax rdx rcx rdi r8 ebx esi r12d r13d ymm0 ymm1 ymm3 ymm4 zmm2
..B8.37:                        # Preds ..B8.36
                                # Execution count [6.55e-01]
        lea       1(%rsi), %edi                                 #102.5 c1
        cmpl      %ebx, %edi                                    #102.5 c3
        ja        ..B8.42       # Prob 10%                      #102.5 c5
                                # LOE rax rdx rcx r8 ebx esi edi r12d r13d ymm0 ymm1
..B8.38:                        # Preds ..B8.37 ..B8.61
                                # Execution count [6.48e-01]
        movl      %r12d, %edi                                   #111.37 c1
        movslq    %esi, %r9                                     #110.9 c1
        imull     %esi, %edi                                    #111.37 c3
        negl      %esi                                          #102.5 c3
        xorl      %r8d, %r8d                                    #110.9 c5
        addl      %ebx, %esi                                    #102.5 c7
                                # LOE rax rdx rcx rdi r9 ebx esi r8d r12d r13d
..B8.39:                        # Preds ..B8.39 ..B8.38
                                # Execution count [3.60e+00]
        addl      $1, %r8d                                      #102.5 c1
        lea       (%rcx,%rdi,4), %r10                           #111.25 c1
        movq      %r10, sizes_vec(,%r9,8)                       #111.9 c3
        addl      %r12d, %edi                                   #102.5 c3
        addq      $1, %r9                                       #102.5 c3
        cmpl      %esi, %r8d                                    #102.5 c5
        jb        ..B8.39       # Prob 82%                      #102.5 c7
                                # LOE rax rdx rcx rdi r9 ebx esi r8d r12d r13d
..B8.40:                        # Preds ..B8.39
                                # Execution count [5.76e-01]
        cmpl      $8, %ebx                                      #102.5 c1
        jb        ..B8.60       # Prob 10%                      #102.5 c3
                                # LOE rax rdx ebx r12d r13d
..B8.41:                        # Preds ..B8.40
                                # Execution count [5.18e-01]
        movl      $8, %ecx                                      #120.3 c1
        movl      %ebx, %esi                                    #102.5 c1
        vmovdqu   .L_2il0floatpacket.5(%rip), %ymm1             #120.3 c1
        vmovd     %ecx, %xmm0                                   #120.3 c3
        andl      $-8, %esi                                     #102.5 c3
        vpbroadcastd %xmm0, %ymm0                               #120.3 c5
        movslq    %esi, %r8                                     #102.5 c5
        lea       1(%rsi), %edi                                 #102.5 c5
                                # LOE rax rdx r8 ebx esi edi r12d r13d ymm0 ymm1
..B8.42:                        # Preds ..B8.37 ..B8.41
                                # Execution count [6.48e-01]
        vmovd     %r13d, %xmm2                                  #114.47 c1
        vmovdqa   %ymm1, %ymm4                                  #120.3 c1
        xorl      %ecx, %ecx                                    #102.5 c1
        vpbroadcastd %xmm2, %ymm3                               #114.47 c3
        vpbroadcastq %rdx, %zmm2                                #114.25 c3
                                # LOE rax rdx rcx r8 ebx esi edi r12d r13d ymm0 ymm1 ymm3 ymm4 zmm2
..B8.43:                        # Preds ..B8.43 ..B8.42
                                # Execution count [3.60e+00]
        vpmulld   %ymm3, %ymm4, %ymm5                           #114.25 c1
        vpaddd    %ymm0, %ymm4, %ymm4                           #120.3 c1
        vpmovzxdq %ymm5, %zmm6                                  #114.25 c3
        vpsllq    $2, %zmm6, %zmm7                              #114.25 c5
        vpaddq    %zmm7, %zmm2, %zmm8                           #114.25 c7
        vmovdqu64 %zmm8, zeros_vec(,%rcx,8)                     #114.9 c9
        addq      $8, %rcx                                      #102.5 c9
        cmpq      %r8, %rcx                                     #102.5 c11
        jb        ..B8.43       # Prob 82%                      #102.5 c13
                                # LOE rax rdx rcx r8 ebx esi edi r12d r13d ymm0 ymm1 ymm3 ymm4 zmm2
..B8.44:                        # Preds ..B8.43
                                # Execution count [6.55e-01]
        cmpl      %ebx, %edi                                    #102.5 c1
        ja        ..B8.49       # Prob 10%                      #102.5 c3
                                # LOE rax rdx r8 ebx esi r12d r13d ymm0 ymm1
..B8.45:                        # Preds ..B8.44 ..B8.60
                                # Execution count [6.48e-01]
        movslq    %esi, %rdi                                    #110.9 c1
        imull     %esi, %r13d                                   #114.47 c1
        negl      %esi                                          #102.5 c3
        addl      %ebx, %esi                                    #102.5 c5
        xorl      %ecx, %ecx                                    #110.9 c5
                                # LOE rax rdx rdi r13 ecx ebx esi r12d
..B8.46:                        # Preds ..B8.46 ..B8.45
                                # Execution count [3.60e+00]
        addl      $1, %ecx                                      #102.5 c1
        lea       (%rdx,%r13,4), %r8                            #114.25 c1
        movq      %r8, zeros_vec(,%rdi,8)                       #114.9 c3
        addq      $1, %rdi                                      #102.5 c3
        lea       8(%r13,%r12), %r13d                           #102.5 c3
        cmpl      %esi, %ecx                                    #102.5 c3
        jb        ..B8.46       # Prob 82%                      #102.5 c5
                                # LOE rax rdx rdi r13 ecx ebx esi r12d
..B8.47:                        # Preds ..B8.46
                                # Execution count [5.76e-01]
        cmpl      $8, %ebx                                      #102.5 c1
        jb        ..B8.59       # Prob 10%                      #102.5 c3
                                # LOE rax ebx r12d
..B8.48:                        # Preds ..B8.47
                                # Execution count [5.18e-01]
        movl      $8, %edx                                      #120.3 c1
        movl      %ebx, %esi                                    #102.5 c1
        vmovdqu   .L_2il0floatpacket.5(%rip), %ymm1             #120.3 c1
        vmovd     %edx, %xmm0                                   #120.3 c3
        andl      $-8, %esi                                     #102.5 c3
        vpbroadcastd %xmm0, %ymm0                               #120.3 c5
        movslq    %esi, %r8                                     #102.5 c5
                                # LOE rax r8 ebx esi r12d ymm0 ymm1
..B8.49:                        # Preds ..B8.44 ..B8.48
                                # Execution count [6.48e-01]
        vmovd     %r12d, %xmm2                                  #75.5 c1
        xorl      %edx, %edx                                    #102.5 c1
        vpbroadcastd %xmm2, %ymm3                               #75.5 c3
        vpbroadcastq %rax, %zmm2                                #110.27 c3
                                # LOE rax rdx r8 ebx esi r12d ymm0 ymm1 ymm3 zmm2
..B8.50:                        # Preds ..B8.50 ..B8.49
                                # Execution count [3.60e+00]
        vpmulld   %ymm3, %ymm1, %ymm4                           #110.27 c1
        vpaddd    %ymm0, %ymm1, %ymm1                           #120.3 c1
        vpmovzxdq %ymm4, %zmm5                                  #110.27 c3
        vpsllq    $3, %zmm5, %zmm6                              #110.27 c5
        vpaddq    %zmm6, %zmm2, %zmm7                           #110.27 c7
        vmovdqu64 %zmm7, samples_vec(,%rdx,8)                   #110.9 c9
        addq      $8, %rdx                                      #102.5 c9
        cmpq      %r8, %rdx                                     #102.5 c11
        jb        ..B8.50       # Prob 82%                      #102.5 c13
                                # LOE rax rdx r8 ebx esi r12d ymm0 ymm1 ymm3 zmm2
..B8.52:                        # Preds ..B8.50 ..B8.59
                                # Execution count [7.20e-01]
        lea       1(%rsi), %edx                                 #102.5 c1
        movslq    %esi, %rdi                                    #102.5 c1
        xorl      %ecx, %ecx                                    #102.5 c1
        cmpl      %ebx, %edx                                    #102.5 c3
        ja        ..B8.56       # Prob 10%                      #102.5 c5
                                # LOE rax rdi ecx ebx esi r12d
..B8.53:                        # Preds ..B8.52
                                # Execution count [6.48e-01]
        movl      %r12d, %edx                                   #110.39 c1
        subl      %esi, %ebx                                    #102.5 c1
        imull     %esi, %edx                                    #110.39 c3
                                # LOE rax rdx rdi ecx ebx r12d
..B8.54:                        # Preds ..B8.54 ..B8.53
                                # Execution count [3.60e+00]
        lea       (%rax,%rdx,8), %rsi                           #110.27 c1
        movq      %rsi, samples_vec(,%rdi,8)                    #110.9 c3
        addl      $1, %ecx                                      #102.5 c3
        addl      %r12d, %edx                                   #102.5 c3
        addq      $1, %rdi                                      #102.5 c5
        cmpl      %ebx, %ecx                                    #102.5 c5
        jb        ..B8.54       # Prob 82%                      #102.5 c7
                                # LOE rax rdx rdi ecx ebx r12d
..B8.56:                        # Preds ..B8.54 ..B8.6 ..B8.52
                                # Execution count [7.20e-01]
        movq      64(%rsp), %rbx                                #123.9[spill] c1
	.cfi_restore 3
        movq      96(%rsp), %r12                                #123.9[spill] c1
	.cfi_restore 12
        xorl      %eax, %eax                                    #123.9 c1
        movq      72(%rsp), %r13                                #123.9[spill] c5 stall 1
	.cfi_restore 13
        movq      80(%rsp), %r14                                #123.9[spill] c5
	.cfi_restore 14
        movq      88(%rsp), %r15                                #123.9[spill] c9 stall 1
	.cfi_restore 15
        movq      %rbp, %rsp                                    #123.9 c11
        popq      %rbp                                          #123.9
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #123.9
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B8.57:                        # Preds ..B8.5
                                # Execution count [2.80e-01]
        movq      64(%rsp), %rbx                                #99.16[spill] c1
	.cfi_restore 3
        movq      96(%rsp), %r12                                #99.16[spill] c1
	.cfi_restore 12
        movl      $-1, %eax                                     #99.16 c1
        movq      72(%rsp), %r13                                #99.16[spill] c5 stall 1
	.cfi_restore 13
        movq      80(%rsp), %r14                                #99.16[spill] c5
	.cfi_restore 14
        movq      88(%rsp), %r15                                #99.16[spill] c9 stall 1
	.cfi_restore 15
        movq      %rbp, %rsp                                    #99.16 c11
        popq      %rbp                                          #99.16
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #99.16
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xc8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B8.58:                        # Preds ..B8.19
                                # Execution count [3.24e-01]: Infreq
        movl      %ecx, %edx                                    #117.3 c1
        jmp       ..B8.29       # Prob 100%                     #117.3 c1
                                # LOE r8 r10 edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.59:                        # Preds ..B8.61 ..B8.60 ..B8.47
                                # Execution count [6.48e-02]: Infreq
        xorl      %esi, %esi                                    #102.5 c1
        jmp       ..B8.52       # Prob 100%                     #102.5 c1
                                # LOE rax ebx esi r12d
..B8.60:                        # Preds ..B8.40
                                # Execution count [5.83e-02]: Infreq
        xorl      %esi, %esi                                    #102.5 c1
        cmpl      $1, %ebx                                      #102.5 c1
        jae       ..B8.45       # Prob 90%                      #102.5 c3
        jmp       ..B8.59       # Prob 100%                     #102.5 c3
                                # LOE rax rdx ebx esi r12d r13d
..B8.61:                        # Preds ..B8.34
                                # Execution count [6.48e-02]: Infreq
        xorl      %esi, %esi                                    #102.5 c1
        cmpl      $1, %ebx                                      #102.5 c1
        jae       ..B8.38       # Prob 90%                      #102.5 c3
        jmp       ..B8.59       # Prob 100%                     #102.5 c3
                                # LOE rax rdx rcx ebx esi r12d r13d
..B8.62:                        # Preds ..B8.17
                                # Execution count [3.24e-01]: Infreq
        movl      %edi, %edx                                    #117.3 c1
        movl      %ecx, %eax                                    #117.3 c1
        jmp       ..B8.25       # Prob 100%                     #117.3 c1
                                # LOE r8 r10 eax edx ecx ebx esi edi r9d r12d zmm0 zmm1 zmm2
..B8.63:                        # Preds ..B8.7
                                # Execution count [6.48e-02]: Infreq
        xorl      %edi, %edi                                    #102.5 c1
        jmp       ..B8.11       # Prob 100%                     #102.5 c1
        .align    16,0x90
                                # LOE rax rdx rcx r8 ebx edi r12d r13d
	.cfi_endproc
# mark_end;
	.type	dynais_init.Z,@function
	.size	dynais_init.Z,.-dynais_init.Z
	.data
# -- End  dynais_init.Z
	.text
# -- Begin  dynais_init.A
	.text
# mark_begin;
       .align    16,0x90
# --- dynais_init.A..0(unsigned int, unsigned int)
dynais_init.A:
# parameter 1: %edi
# parameter 2: %esi
..B9.1:                         # Preds ..B9.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_dynais_init.A.225:
..L226:
                                                        #76.1
        pushq     %rbp                                          #76.1
	.cfi_def_cfa_offset 16
        movq      %rsp, %rbp                                    #76.1
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
        andq      $-64, %rsp                                    #76.1
        pushq     %r12                                          #76.1
        pushq     %r13                                          #76.1
        pushq     %r14                                          #76.1
        pushq     %r15                                          #76.1
        pushq     %rbx                                          #76.1
        subq      $88, %rsp                                     #76.1
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
        movl      %edi, %r12d                                   #76.1
        movl      %r12d, %r13d                                  #92.82
        movl      %esi, %ebx                                    #76.1
        lea       (,%r13,8), %rdx                               #92.16
        movl      %ebx, %r14d                                   #92.91
        imulq     %r14, %rdx                                    #92.16
        cmpl      $10, %ebx                                     #83.5
        movl      $10, %eax                                     #83.5
        movl      $40000, %ecx                                  #87.2
        lea       40(%rsp), %rdi                                #92.16
        cmovbe    %ebx, %eax                                    #83.5
        cmpl      $40000, %r12d                                 #87.2
        movl      $64, %esi                                     #92.16
        cmovbe    %r12d, %ecx                                   #87.2
        andl      $-16, %ecx                                    #87.29
        addl      $16, %ecx                                     #87.29
        movl      %eax, _levels(%rip)                           #83.5
        movl      %ecx, _window(%rip)                           #87.2
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #92.16
                                # LOE r13 r14 eax ebx r12d
..B9.112:                       # Preds ..B9.1
                                # Execution count [1.00e+00]
        movl      %eax, %r15d                                   #92.16
                                # LOE r13 r14 ebx r12d r15d
..B9.2:                         # Preds ..B9.112
                                # Execution count [1.00e+00]
        shlq      $2, %r13                                      #93.16
        lea       48(%rsp), %rdi                                #93.16
        imulq     %r14, %r13                                    #93.16
        movl      $64, %esi                                     #93.16
        movq      %r13, %rdx                                    #93.16
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #93.16
                                # LOE r14 eax ebx r12d r15d
..B9.113:                       # Preds ..B9.2
                                # Execution count [1.00e+00]
        movl      %eax, (%rsp)                                  #93.16[spill]
                                # LOE r14 ebx r12d r15d
..B9.3:                         # Preds ..B9.113
                                # Execution count [1.00e+00]
        movl      $64, %esi                                     #94.16
        lea       8(%r12), %r13d                                #94.92
        movl      %r13d, %edx                                   #94.92
        lea       56(%rsp), %rdi                                #94.16
        shlq      $2, %rdx                                      #94.92
        imulq     %r14, %rdx                                    #94.97
        movq      %rdx, 8(%rsp)                                 #94.97[spill]
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #94.16
                                # LOE eax ebx r12d r13d r15d
..B9.114:                       # Preds ..B9.3
                                # Execution count [1.00e+00]
        movl      %eax, %r14d                                   #94.16
                                # LOE ebx r12d r13d r14d r15d
..B9.4:                         # Preds ..B9.114
                                # Execution count [1.00e+00]
        movl      $64, %esi                                     #95.16
        lea       64(%rsp), %rdi                                #95.16
        movq      -56(%rdi), %rdx                               #95.16[spill]
#       posix_memalign(void **, size_t, size_t)
        call      posix_memalign                                #95.16
                                # LOE eax ebx r12d r13d r14d r15d
..B9.5:                         # Preds ..B9.4
                                # Execution count [1.00e+00]
        orl       %r14d, %r15d                                  #98.21
        orl       (%rsp), %r15d                                 #98.21[spill]
        orl       %eax, %r15d                                   #98.21
        jne       ..B9.103      # Prob 28%                      #98.21
                                # LOE ebx r12d r13d
..B9.6:                         # Preds ..B9.5
                                # Execution count [6.48e-01]
        testl     %ebx, %ebx                                    #102.21
        jbe       ..B9.102      # Prob 0%                       #102.21
                                # LOE ebx r12d r13d
..B9.7:                         # Preds ..B9.6
                                # Execution count [6.48e-01]
        movl      %ebx, %eax                                    #75.5
        movl      $1, %edi                                      #102.5
        shrl      $3, %eax                                      #75.5
        xorl      %r10d, %r10d                                  #102.5
        movq      40(%rsp), %rsi                                #110.27
        movq      56(%rsp), %rcx                                #114.25
        movq      48(%rsp), %rdx                                #111.25
        movq      64(%rsp), %r11                                #115.19
        testl     %eax, %eax                                    #102.5
        jbe       ..B9.11       # Prob 10%                      #102.5
                                # LOE rdx rcx rsi r11 eax ebx edi r10d r12d r13d
..B9.8:                         # Preds ..B9.7
                                # Execution count [2.25e-03]
        xorl      %r8d, %r8d                                    #102.5
        lea       (%r12,%r12,2), %r14d                          #115.41
        movl      %r14d, (%rsp)                                 #115.41[spill]
        lea       (,%r12,8), %r14d                              #115.41
        movl      %ebx, 24(%rsp)                                #102.5[spill]
        lea       40(%r12,%r12,4), %r15d                        #102.5
        movl      %r8d, %r8d                                    #102.5
        movq      %rsi, 8(%rsp)                                 #102.5[spill]
        lea       64(,%r12,8), %r9d                             #115.41
        movl      %r13d, 16(%rsp)                               #102.5[spill]
        subl      %r12d, %r14d                                  #115.41
        movl      (%rsp), %ebx                                  #102.5[spill]
        lea       24(%r12,%r12,2), %edi                         #102.5
        movl      %r15d, %esi                                   #102.5
                                # LOE rdx rcx r8 r11 eax ebx esi edi r9d r10d r12d r14d
..B9.9:                         # Preds ..B9.9 ..B9.8
                                # Execution count [4.50e-01]
        movslq    %r10d, %r13                                   #115.3
        lea       (%r11,%r8,4), %r15                            #115.19
        shlq      $6, %r13                                      #115.3
        incl      %r10d                                         #102.5
        movq      %r15, indes_vec(%r13)                         #115.3
        lea       8(%r8,%r12), %r15d                            #115.3
        lea       (%r11,%r15,4), %r15                           #115.19
        movq      %r15, 8+indes_vec(%r13)                       #115.3
        lea       16(%r8,%r12,2), %r15d                         #115.3
        lea       (%r11,%r15,4), %r15                           #115.19
        movq      %r15, 16+indes_vec(%r13)                      #115.3
        lea       (%rdi,%r8), %r15d                             #115.3
        lea       (%r11,%r15,4), %r15                           #115.19
        movq      %r15, 24+indes_vec(%r13)                      #115.3
        lea       32(%r8,%r12,4), %r15d                         #115.3
        lea       (%r11,%r15,4), %r15                           #115.19
        movq      %r15, 32+indes_vec(%r13)                      #115.3
        lea       (%rsi,%r8), %r15d                             #115.3
        lea       (%r11,%r15,4), %r15                           #115.19
        movq      %r15, 40+indes_vec(%r13)                      #115.3
        lea       48(%r8,%rbx,2), %r15d                         #115.3
        lea       (%r11,%r15,4), %r15                           #115.19
        movq      %r15, 48+indes_vec(%r13)                      #115.3
        lea       56(%r8,%r14), %r15d                           #115.3
        addl      %r9d, %r8d                                    #102.5
        movl      %r15d, %r15d                                  #115.19
        movl      %r8d, %r8d                                    #102.5
        lea       (%r11,%r15,4), %r15                           #115.19
        movq      %r15, 56+indes_vec(%r13)                      #115.3
        cmpl      %eax, %r10d                                   #102.5
        jb        ..B9.9        # Prob 99%                      #102.5
                                # LOE rdx rcx r8 r11 eax ebx esi edi r9d r10d r12d r14d
..B9.10:                        # Preds ..B9.9
                                # Execution count [6.48e-01]
        movq      8(%rsp), %rsi                                 #[spill]
        lea       1(,%r10,8), %edi                              #102.5
        movl      16(%rsp), %r13d                               #[spill]
        movl      24(%rsp), %ebx                                #[spill]
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..B9.11:                        # Preds ..B9.10 ..B9.7
                                # Execution count [7.20e-01]
        cmpl      %ebx, %edi                                    #102.5
        ja        ..B9.27       # Prob 50%                      #102.5
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..B9.12:                        # Preds ..B9.11
                                # Execution count [0.00e+00]
        movl      %ebx, %r8d                                    #102.5
        subl      %edi, %r8d                                    #102.5
        jmp       *.2.21_2.switchtab.2(,%r8,8)                  #102.5
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..1.21_0.TAG.6:
..B9.14:                        # Preds ..B9.12
                                # Execution count [0.00e+00]
        movl      %r13d, %r8d                                   #115.41
        lea       5(%rdi), %r9d                                 #120.3
        imull     %r9d, %r8d                                    #115.41
        movslq    %r9d, %r9                                     #115.3
        movl      %r8d, %r8d                                    #115.19
        lea       (%r11,%r8,4), %r10                            #115.19
        movq      %r10, indes_vec(,%r9,8)                       #115.3
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..1.21_0.TAG.5:
..B9.16:                        # Preds ..B9.12 ..B9.14
                                # Execution count [0.00e+00]
        movl      %r13d, %r8d                                   #115.41
        lea       4(%rdi), %r9d                                 #120.3
        imull     %r9d, %r8d                                    #115.41
        movslq    %r9d, %r9                                     #115.3
        movl      %r8d, %r8d                                    #115.19
        lea       (%r11,%r8,4), %r10                            #115.19
        movq      %r10, indes_vec(,%r9,8)                       #115.3
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..1.21_0.TAG.4:
..B9.18:                        # Preds ..B9.12 ..B9.16
                                # Execution count [0.00e+00]
        movl      %r13d, %r8d                                   #115.41
        lea       3(%rdi), %r9d                                 #120.3
        imull     %r9d, %r8d                                    #115.41
        movslq    %r9d, %r9                                     #115.3
        movl      %r8d, %r8d                                    #115.19
        lea       (%r11,%r8,4), %r10                            #115.19
        movq      %r10, indes_vec(,%r9,8)                       #115.3
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..1.21_0.TAG.3:
..B9.20:                        # Preds ..B9.12 ..B9.18
                                # Execution count [0.00e+00]
        movl      %r13d, %r8d                                   #115.41
        lea       2(%rdi), %r9d                                 #120.3
        imull     %r9d, %r8d                                    #115.41
        movslq    %r9d, %r9                                     #115.3
        movl      %r8d, %r8d                                    #115.19
        lea       (%r11,%r8,4), %r10                            #115.19
        movq      %r10, indes_vec(,%r9,8)                       #115.3
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..1.21_0.TAG.2:
..B9.22:                        # Preds ..B9.12 ..B9.20
                                # Execution count [0.00e+00]
        movl      %r13d, %r8d                                   #115.41
        lea       1(%rdi), %r9d                                 #120.3
        imull     %r9d, %r8d                                    #115.41
        movslq    %r9d, %r9                                     #115.3
        movl      %r8d, %r8d                                    #115.19
        lea       (%r11,%r8,4), %r10                            #115.19
        movq      %r10, indes_vec(,%r9,8)                       #115.3
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..1.21_0.TAG.1:
..B9.24:                        # Preds ..B9.12 ..B9.22
                                # Execution count [0.00e+00]
        movl      %edi, %r8d                                    #115.41
        imull     %r13d, %r8d                                   #115.41
        movslq    %edi, %rdi                                    #115.3
        movl      %r8d, %r8d                                    #115.19
        lea       (%r11,%r8,4), %r9                             #115.19
        movq      %r9, indes_vec(,%rdi,8)                       #115.3
                                # LOE rdx rcx rsi r11 eax ebx edi r12d r13d
..1.21_0.TAG.0:
..B9.26:                        # Preds ..B9.12 ..B9.24
                                # Execution count [6.48e-01]
        decl      %edi                                          #120.3
        movl      %r13d, %r8d                                   #115.41
        imull     %edi, %r8d                                    #115.41
        movslq    %edi, %rdi                                    #115.3
        movl      %r8d, %r8d                                    #115.19
        lea       (%r11,%r8,4), %r9                             #115.19
        movq      %r9, indes_vec(,%rdi,8)                       #115.3
                                # LOE rdx rcx rsi eax ebx r12d r13d
..B9.27:                        # Preds ..B9.11 ..B9.26
                                # Execution count [6.48e-01]
        movl      $4, %r11d                                     #118.24
        movl      %r12d, %r8d                                   #117.3
        andl      $3, %r8d                                      #117.3
        xorl      %r10d, %r10d                                  #102.5
        negl      %r8d                                          #117.3
        lea       -1(%r12), %r9d                                #120.30
        movslq    %ebx, %rdi                                    #102.5
        addl      %r12d, %r8d                                   #117.3
        movd      %r11d, %xmm0                                  #118.24
        pshufd    $0, %xmm0, %xmm0                              #118.24
        movq      %rdx, 32(%rsp)                                #118.24[spill]
        movq      %rcx, (%rsp)                                  #118.24[spill]
        movq      %rsi, 8(%rsp)                                 #118.24[spill]
        xorl      %esi, %esi                                    #118.24
        movl      %r13d, 16(%rsp)                               #118.24[spill]
        movl      %ebx, 24(%rsp)                                #118.24[spill]
                                # LOE rdi r10 eax esi r8d r9d r12d xmm0
..B9.28:                        # Preds ..B9.45 ..B9.27
                                # Execution count [3.60e+00]
        movl      %esi, level_limit(,%r10,4)                    #104.9
        movl      %esi, level_index(,%r10,4)                    #105.9
        movl      %esi, level_result(,%r10,4)                   #106.9
        movl      %esi, level_length(,%r10,4)                   #107.9
        testl     %r12d, %r12d                                  #117.19
        jbe       ..B9.45       # Prob 50%                      #117.19
                                # LOE rdi r10 eax esi r8d r9d r12d xmm0
..B9.29:                        # Preds ..B9.28
                                # Execution count [3.24e+00]
        cmpl      $4, %r12d                                     #117.3
        jb        ..B9.104      # Prob 10%                      #117.3
                                # LOE rdi r10 eax esi r8d r9d r12d xmm0
..B9.30:                        # Preds ..B9.29
                                # Execution count [3.24e+00]
        movl      indes_vec(,%r10,8), %edx                      #118.4
        andl      $15, %edx                                     #117.3
        je        ..B9.37       # Prob 50%                      #117.3
                                # LOE rdx rdi r10 eax esi r8d r9d r12d xmm0
..B9.31:                        # Preds ..B9.30
                                # Execution count [3.24e+00]
        testb     $3, %dl                                       #117.3
        jne       ..B9.104      # Prob 10%                      #117.3
                                # LOE rdx rdi r10 eax esi r8d r9d r12d xmm0
..B9.32:                        # Preds ..B9.31
                                # Execution count [1.62e+00]
        negl      %edx                                          #117.3
        addl      $16, %edx                                     #117.3
        shrl      $2, %edx                                      #117.3
        lea       4(%rdx), %ecx                                 #117.3
        cmpl      %ecx, %r12d                                   #117.3
        jb        ..B9.104      # Prob 10%                      #117.3
                                # LOE rdx rdi r10 eax esi r8d r9d r12d xmm0
..B9.33:                        # Preds ..B9.32
                                # Execution count [2.14e+00]
        movl      %r12d, %ecx                                   #117.3
        movl      %esi, %r14d                                   #117.3
        subl      %edx, %ecx                                    #117.3
        xorl      %r13d, %r13d                                  #117.3
        andl      $3, %ecx                                      #117.3
        movl      $-1, %r11d                                    #117.3
        negl      %ecx                                          #117.3
        lea       (,%r10,8), %rbx                               #118.4
        addl      %r12d, %ecx                                   #117.3
        testl     %edx, %edx                                    #117.3
        jbe       ..B9.38       # Prob 10%                      #117.3
                                # LOE rdx rbx rdi r10 r13 eax ecx esi r8d r9d r11d r12d r14d xmm0
..B9.35:                        # Preds ..B9.33 ..B9.35
                                # Execution count [1.80e+01]
        movq      indes_vec(%rbx), %r15                         #118.4
        incl      %r14d                                         #117.3
        movl      %r11d, (%r15,%r13,4)                          #118.4
        incq      %r13                                          #117.3
        incl      %r11d                                         #117.3
        cmpl      %edx, %r14d                                   #117.3
        jb        ..B9.35       # Prob 82%                      #117.3
        jmp       ..B9.38       # Prob 100%                     #117.3
                                # LOE rdx rbx rdi r10 r13 eax ecx esi r8d r9d r11d r12d r14d xmm0
..B9.37:                        # Preds ..B9.30
                                # Execution count [1.46e+00]
        movl      %r8d, %ecx                                    #117.3
        lea       (,%r10,8), %rbx                               #118.4
                                # LOE rdx rbx rdi r10 eax ecx esi r8d r9d r12d xmm0
..B9.38:                        # Preds ..B9.35 ..B9.33 ..B9.37
                                # Execution count [3.24e+00]
        movd      %edx, %xmm2                                   #118.24
        lea       -1(%rdx), %r11d                               #118.24
        lea       1(%rdx), %r13d                                #118.24
        lea       2(%rdx), %r14d                                #118.24
        movd      %r11d, %xmm1                                  #118.24
        movd      %r13d, %xmm4                                  #118.24
        movd      %r14d, %xmm3                                  #118.24
        punpcklqdq %xmm2, %xmm1                                 #118.24
        punpcklqdq %xmm3, %xmm4                                 #118.24
        shufps    $136, %xmm4, %xmm1                            #118.24
        movslq    %ecx, %r11                                    #117.3
                                # LOE rdx rbx rdi r10 r11 eax ecx esi r8d r9d r12d xmm0 xmm1
..B9.39:                        # Preds ..B9.39 ..B9.38
                                # Execution count [1.80e+01]
        movq      indes_vec(%rbx), %r13                         #118.4
        movdqu    %xmm1, (%r13,%rdx,4)                          #118.4
        addq      $4, %rdx                                      #117.3
        paddd     %xmm0, %xmm1                                  #118.24
        cmpq      %r11, %rdx                                    #117.3
        jb        ..B9.39       # Prob 82%                      #117.3
                                # LOE rdx rbx rdi r10 r11 eax ecx esi r8d r9d r12d xmm0 xmm1
..B9.41:                        # Preds ..B9.39 ..B9.104
                                # Execution count [3.60e+00]
        movslq    %ecx, %rdx                                    #117.3
        lea       -1(%rcx), %ebx                                #117.3
        cmpl      %r12d, %ecx                                   #117.3
        jae       ..B9.45       # Prob 10%                      #117.3
                                # LOE rdx rdi r10 eax ecx ebx esi r8d r9d r12d xmm0
..B9.43:                        # Preds ..B9.41 ..B9.43
                                # Execution count [1.80e+01]
        movq      indes_vec(,%r10,8), %r11                      #118.4
        incl      %ecx                                          #117.3
        movl      %ebx, (%r11,%rdx,4)                           #118.4
        incq      %rdx                                          #117.3
        incl      %ebx                                          #117.3
        cmpl      %r12d, %ecx                                   #117.3
        jb        ..B9.43       # Prob 82%                      #117.3
                                # LOE rdx rdi r10 eax ecx ebx esi r8d r9d r12d xmm0
..B9.45:                        # Preds ..B9.43 ..B9.41 ..B9.28
                                # Execution count [3.60e+00]
        movq      indes_vec(,%r10,8), %rdx                      #120.3
        incq      %r10                                          #102.5
        movl      %r9d, (%rdx)                                  #120.3
        cmpq      %rdi, %r10                                    #102.5
        jb        ..B9.28       # Prob 82%                      #102.5
                                # LOE rdi r10 eax esi r8d r9d r12d xmm0
..B9.46:                        # Preds ..B9.45
                                # Execution count [7.20e-01]
        movq      32(%rsp), %rdx                                #[spill]
        movl      $1, %r9d                                      #102.5
        movq      (%rsp), %rcx                                  #[spill]
        xorl      %r14d, %r14d                                  #102.5
        movq      8(%rsp), %rsi                                 #[spill]
        movl      16(%rsp), %r13d                               #[spill]
        movl      24(%rsp), %ebx                                #[spill]
        testl     %eax, %eax                                    #102.5
        jbe       ..B9.108      # Prob 10%                      #102.5
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d r14d
..B9.47:                        # Preds ..B9.46
                                # Execution count [2.25e-03]
        xorl      %r9d, %r9d                                    #102.5
        lea       (,%r12,8), %edi                               #111.37
        movl      %r9d, %r9d                                    #102.5
        subl      %r12d, %edi                                   #111.37
        lea       (%r12,%r12,2), %r8d                           #111.37
        lea       (%r12,%r12,4), %r11d                          #111.37
                                # LOE rdx rcx rsi r9 eax ebx edi r8d r11d r12d r13d r14d
..B9.48:                        # Preds ..B9.48 ..B9.47
                                # Execution count [4.50e-01]
        movslq    %r14d, %r10                                   #111.9
        lea       (%rdx,%r9,4), %r15                            #111.25
        shlq      $6, %r10                                      #111.9
        incl      %r14d                                         #102.5
        movq      %r15, sizes_vec(%r10)                         #111.9
        lea       (%r12,%r9), %r15d                             #111.37
        lea       (%rdx,%r15,4), %r15                           #111.25
        movq      %r15, 8+sizes_vec(%r10)                       #111.9
        lea       (%r9,%r12,2), %r15d                           #111.37
        lea       (%rdx,%r15,4), %r15                           #111.25
        movq      %r15, 16+sizes_vec(%r10)                      #111.9
        lea       (%r8,%r9), %r15d                              #111.37
        lea       (%rdx,%r15,4), %r15                           #111.25
        movq      %r15, 24+sizes_vec(%r10)                      #111.9
        lea       (%r9,%r12,4), %r15d                           #111.37
        lea       (%rdx,%r15,4), %r15                           #111.25
        movq      %r15, 32+sizes_vec(%r10)                      #111.9
        lea       (%r11,%r9), %r15d                             #111.37
        lea       (%rdx,%r15,4), %r15                           #111.25
        movq      %r15, 40+sizes_vec(%r10)                      #111.9
        lea       (%r9,%r8,2), %r15d                            #111.37
        lea       (%rdx,%r15,4), %r15                           #111.25
        movq      %r15, 48+sizes_vec(%r10)                      #111.9
        lea       (%rdi,%r9), %r15d                             #111.37
        lea       (%r9,%r12,8), %r9d                            #102.5
        lea       (%rdx,%r15,4), %r15                           #111.25
        movq      %r15, 56+sizes_vec(%r10)                      #111.9
        cmpl      %eax, %r14d                                   #102.5
        jb        ..B9.48       # Prob 99%                      #102.5
                                # LOE rdx rcx rsi r9 eax ebx edi r8d r11d r12d r13d r14d
..B9.49:                        # Preds ..B9.48
                                # Execution count [6.48e-01]
        lea       1(,%r14,8), %r9d                              #102.5
        cmpl      %ebx, %r9d                                    #102.5
        ja        ..B9.65       # Prob 50%                      #102.5
                                # LOE rdx rcx rsi eax ebx edi r8d r9d r12d r13d
..B9.50:                        # Preds ..B9.108 ..B9.49
                                # Execution count [0.00e+00]
        movl      %ebx, %edi                                    #102.5
        subl      %r9d, %edi                                    #102.5
        jmp       *.2.21_2.switchtab.3(,%rdi,8)                 #102.5
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
..1.21_1.TAG.6:
..B9.52:                        # Preds ..B9.50
                                # Execution count [0.00e+00]
        movl      %r12d, %edi                                   #111.37
        lea       5(%r9), %r8d                                  #120.3
        imull     %r8d, %edi                                    #111.37
        movslq    %r8d, %r8                                     #111.9
        movl      %edi, %edi                                    #111.25
        lea       (%rdx,%rdi,4), %r10                           #111.25
        movq      %r10, sizes_vec(,%r8,8)                       #111.9
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
..1.21_1.TAG.5:
..B9.54:                        # Preds ..B9.50 ..B9.52
                                # Execution count [0.00e+00]
        movl      %r12d, %edi                                   #111.37
        lea       4(%r9), %r8d                                  #120.3
        imull     %r8d, %edi                                    #111.37
        movslq    %r8d, %r8                                     #111.9
        movl      %edi, %edi                                    #111.25
        lea       (%rdx,%rdi,4), %r10                           #111.25
        movq      %r10, sizes_vec(,%r8,8)                       #111.9
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
..1.21_1.TAG.4:
..B9.56:                        # Preds ..B9.50 ..B9.54
                                # Execution count [0.00e+00]
        movl      %r12d, %edi                                   #111.37
        lea       3(%r9), %r8d                                  #120.3
        imull     %r8d, %edi                                    #111.37
        movslq    %r8d, %r8                                     #111.9
        movl      %edi, %edi                                    #111.25
        lea       (%rdx,%rdi,4), %r10                           #111.25
        movq      %r10, sizes_vec(,%r8,8)                       #111.9
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
..1.21_1.TAG.3:
..B9.58:                        # Preds ..B9.50 ..B9.56
                                # Execution count [0.00e+00]
        movl      %r12d, %edi                                   #111.37
        lea       2(%r9), %r8d                                  #120.3
        imull     %r8d, %edi                                    #111.37
        movslq    %r8d, %r8                                     #111.9
        movl      %edi, %edi                                    #111.25
        lea       (%rdx,%rdi,4), %r10                           #111.25
        movq      %r10, sizes_vec(,%r8,8)                       #111.9
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
..1.21_1.TAG.2:
..B9.60:                        # Preds ..B9.50 ..B9.58
                                # Execution count [0.00e+00]
        movl      %r12d, %edi                                   #111.37
        lea       1(%r9), %r8d                                  #120.3
        imull     %r8d, %edi                                    #111.37
        movslq    %r8d, %r8                                     #111.9
        movl      %edi, %edi                                    #111.25
        lea       (%rdx,%rdi,4), %r10                           #111.25
        movq      %r10, sizes_vec(,%r8,8)                       #111.9
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
..1.21_1.TAG.1:
..B9.62:                        # Preds ..B9.50 ..B9.60
                                # Execution count [0.00e+00]
        movl      %r12d, %edi                                   #111.37
        imull     %r9d, %edi                                    #111.37
        movslq    %r9d, %r9                                     #111.9
        movl      %edi, %edi                                    #111.25
        lea       (%rdx,%rdi,4), %r8                            #111.25
        movq      %r8, sizes_vec(,%r9,8)                        #111.9
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
..1.21_1.TAG.0:
..B9.64:                        # Preds ..B9.50 ..B9.62
                                # Execution count [6.48e-01]
        decl      %r9d                                          #120.3
        movl      %r12d, %edi                                   #111.37
        imull     %r9d, %edi                                    #111.37
        movslq    %r9d, %r9                                     #111.9
        movl      %edi, %edi                                    #111.25
        lea       (%rdx,%rdi,4), %rdx                           #111.25
        movq      %rdx, sizes_vec(,%r9,8)                       #111.9
        movl      $1, %edx                                      #102.5
        testl     %eax, %eax                                    #102.5
        jbe       ..B9.106      # Prob 10%                      #102.5
                                # LOE rcx rsi eax edx ebx r12d r13d
..B9.117:                       # Preds ..B9.64
                                # Execution count [5.83e-01]
        lea       (,%r12,8), %edi                               #115.41
        subl      %r12d, %edi                                   #115.41
        lea       (%r12,%r12,2), %r8d                           #115.41
                                # LOE rcx rsi eax ebx edi r8d r12d r13d
..B9.65:                        # Preds ..B9.49 ..B9.117
                                # Execution count [2.25e-03]
        xorl      %r9d, %r9d                                    #114.47
        xorl      %r10d, %r10d                                  #114.47
        movl      %r9d, %r9d                                    #114.47
        movl      %ebx, 24(%rsp)                                #114.47[spill]
        lea       64(,%r12,8), %r11d                            #114.47
        lea       24(%r12,%r12,2), %r14d                        #114.47
        lea       40(%r12,%r12,4), %r15d                        #114.47
                                # LOE rcx rsi r9 eax edi r8d r10d r11d r12d r13d r14d r15d
..B9.66:                        # Preds ..B9.66 ..B9.65
                                # Execution count [4.50e-01]
        movslq    %r10d, %rdx                                   #114.9
        lea       (%rcx,%r9,4), %rbx                            #114.25
        shlq      $6, %rdx                                      #114.9
        incl      %r10d                                         #102.5
        movq      %rbx, zeros_vec(%rdx)                         #114.9
        lea       8(%r9,%r12), %ebx                             #114.9
        lea       (%rcx,%rbx,4), %rbx                           #114.25
        movq      %rbx, 8+zeros_vec(%rdx)                       #114.9
        lea       16(%r9,%r12,2), %ebx                          #114.9
        lea       (%rcx,%rbx,4), %rbx                           #114.25
        movq      %rbx, 16+zeros_vec(%rdx)                      #114.9
        lea       (%r14,%r9), %ebx                              #114.9
        lea       (%rcx,%rbx,4), %rbx                           #114.25
        movq      %rbx, 24+zeros_vec(%rdx)                      #114.9
        lea       32(%r9,%r12,4), %ebx                          #114.9
        lea       (%rcx,%rbx,4), %rbx                           #114.25
        movq      %rbx, 32+zeros_vec(%rdx)                      #114.9
        lea       (%r15,%r9), %ebx                              #114.9
        lea       (%rcx,%rbx,4), %rbx                           #114.25
        movq      %rbx, 40+zeros_vec(%rdx)                      #114.9
        lea       48(%r9,%r8,2), %ebx                           #114.9
        lea       (%rcx,%rbx,4), %rbx                           #114.25
        movq      %rbx, 48+zeros_vec(%rdx)                      #114.9
        lea       56(%r9,%rdi), %ebx                            #114.9
        addl      %r11d, %r9d                                   #102.5
        movl      %ebx, %ebx                                    #114.25
        movl      %r9d, %r9d                                    #102.5
        lea       (%rcx,%rbx,4), %rbx                           #114.25
        movq      %rbx, 56+zeros_vec(%rdx)                      #114.9
        cmpl      %eax, %r10d                                   #102.5
        jb        ..B9.66       # Prob 99%                      #102.5
                                # LOE rcx rsi r9 eax edi r8d r10d r11d r12d r13d r14d r15d
..B9.67:                        # Preds ..B9.66
                                # Execution count [6.48e-01]
        movl      24(%rsp), %ebx                                #[spill]
        lea       1(,%r10,8), %edx                              #102.5
        cmpl      %ebx, %edx                                    #102.5
        ja        ..B9.83       # Prob 50%                      #102.5
                                # LOE rcx rsi eax edx ebx edi r8d r12d r13d
..B9.68:                        # Preds ..B9.106 ..B9.67
                                # Execution count [0.00e+00]
        movl      %ebx, %edi                                    #102.5
        subl      %edx, %edi                                    #102.5
        jmp       *.2.21_2.switchtab.4(,%rdi,8)                 #102.5
                                # LOE rcx rsi eax edx ebx r12d r13d
..1.21_2.TAG.6:
..B9.70:                        # Preds ..B9.68
                                # Execution count [0.00e+00]
        movl      %r13d, %edi                                   #114.47
        lea       5(%rdx), %r8d                                 #120.3
        imull     %r8d, %edi                                    #114.47
        movslq    %r8d, %r8                                     #114.9
        movl      %edi, %edi                                    #114.25
        lea       (%rcx,%rdi,4), %r9                            #114.25
        movq      %r9, zeros_vec(,%r8,8)                        #114.9
                                # LOE rcx rsi eax edx ebx r12d r13d
..1.21_2.TAG.5:
..B9.72:                        # Preds ..B9.68 ..B9.70
                                # Execution count [0.00e+00]
        movl      %r13d, %edi                                   #114.47
        lea       4(%rdx), %r8d                                 #120.3
        imull     %r8d, %edi                                    #114.47
        movslq    %r8d, %r8                                     #114.9
        movl      %edi, %edi                                    #114.25
        lea       (%rcx,%rdi,4), %r9                            #114.25
        movq      %r9, zeros_vec(,%r8,8)                        #114.9
                                # LOE rcx rsi eax edx ebx r12d r13d
..1.21_2.TAG.4:
..B9.74:                        # Preds ..B9.68 ..B9.72
                                # Execution count [0.00e+00]
        movl      %r13d, %edi                                   #114.47
        lea       3(%rdx), %r8d                                 #120.3
        imull     %r8d, %edi                                    #114.47
        movslq    %r8d, %r8                                     #114.9
        movl      %edi, %edi                                    #114.25
        lea       (%rcx,%rdi,4), %r9                            #114.25
        movq      %r9, zeros_vec(,%r8,8)                        #114.9
                                # LOE rcx rsi eax edx ebx r12d r13d
..1.21_2.TAG.3:
..B9.76:                        # Preds ..B9.68 ..B9.74
                                # Execution count [0.00e+00]
        movl      %r13d, %edi                                   #114.47
        lea       2(%rdx), %r8d                                 #120.3
        imull     %r8d, %edi                                    #114.47
        movslq    %r8d, %r8                                     #114.9
        movl      %edi, %edi                                    #114.25
        lea       (%rcx,%rdi,4), %r9                            #114.25
        movq      %r9, zeros_vec(,%r8,8)                        #114.9
                                # LOE rcx rsi eax edx ebx r12d r13d
..1.21_2.TAG.2:
..B9.78:                        # Preds ..B9.68 ..B9.76
                                # Execution count [0.00e+00]
        movl      %r13d, %edi                                   #114.47
        lea       1(%rdx), %r8d                                 #120.3
        imull     %r8d, %edi                                    #114.47
        movslq    %r8d, %r8                                     #114.9
        movl      %edi, %edi                                    #114.25
        lea       (%rcx,%rdi,4), %r9                            #114.25
        movq      %r9, zeros_vec(,%r8,8)                        #114.9
                                # LOE rcx rsi eax edx ebx r12d r13d
..1.21_2.TAG.1:
..B9.80:                        # Preds ..B9.68 ..B9.78
                                # Execution count [0.00e+00]
        movl      %edx, %edi                                    #114.47
        imull     %r13d, %edi                                   #114.47
        movslq    %edx, %rdx                                    #114.9
        movl      %edi, %edi                                    #114.25
        lea       (%rcx,%rdi,4), %r8                            #114.25
        movq      %r8, zeros_vec(,%rdx,8)                       #114.9
                                # LOE rcx rsi eax edx ebx r12d r13d
..1.21_2.TAG.0:
..B9.82:                        # Preds ..B9.68 ..B9.80
                                # Execution count [6.48e-01]
        decl      %edx                                          #120.3
        imull     %edx, %r13d                                   #114.47
        movslq    %edx, %rdx                                    #114.9
        movl      %r13d, %r13d                                  #114.25
        lea       (%rcx,%r13,4), %rcx                           #114.25
        movq      %rcx, zeros_vec(,%rdx,8)                      #114.9
        movl      $1, %edx                                      #102.5
        testl     %eax, %eax                                    #102.5
        jbe       ..B9.86       # Prob 10%                      #102.5
                                # LOE rsi eax edx ebx r12d
..B9.116:                       # Preds ..B9.82
                                # Execution count [5.83e-01]
        lea       (,%r12,8), %edi                               #115.41
        subl      %r12d, %edi                                   #115.41
        lea       (%r12,%r12,2), %r8d                           #115.41
                                # LOE rsi eax ebx edi r8d r12d
..B9.83:                        # Preds ..B9.67 ..B9.116
                                # Execution count [2.25e-03]
        xorl      %edx, %edx                                    #110.39
        xorl      %r9d, %r9d                                    #110.39
        movl      %edx, %edx                                    #110.39
        lea       (%r12,%r12,4), %ecx                           #110.39
                                # LOE rdx rsi eax ecx ebx edi r8d r9d r12d
..B9.84:                        # Preds ..B9.84 ..B9.83
                                # Execution count [4.50e-01]
        movslq    %r9d, %r10                                    #110.9
        lea       (%r12,%rdx), %r13d                            #110.39
        shlq      $6, %r10                                      #110.9
        movl      %r13d, %r13d                                  #110.27
        lea       (%rsi,%r13,8), %r14                           #110.27
        incl      %r9d                                          #102.5
        lea       (%rdx,%r12,2), %r15d                          #110.39
        lea       (%r8,%rdx), %r13d                             #110.39
        lea       (%rsi,%rdx,8), %r11                           #110.27
        movq      %r11, samples_vec(%r10)                       #110.9
        lea       (%rsi,%r15,8), %r11                           #110.27
        movq      %r14, 8+samples_vec(%r10)                     #110.9
        lea       (%rsi,%r13,8), %r14                           #110.27
        movq      %r11, 16+samples_vec(%r10)                    #110.9
        lea       (%rdx,%r12,4), %r11d                          #110.39
        movq      %r14, 24+samples_vec(%r10)                    #110.9
        lea       (%rcx,%rdx), %r14d                            #110.39
        movl      %r11d, %r11d                                  #110.27
        movl      %r14d, %r14d                                  #110.27
        lea       (%rsi,%r11,8), %r13                           #110.27
        movq      %r13, 32+samples_vec(%r10)                    #110.9
        lea       (%rsi,%r14,8), %r15                           #110.27
        movq      %r15, 40+samples_vec(%r10)                    #110.9
        lea       (%rdx,%r8,2), %r11d                           #110.39
        lea       (%rdi,%rdx), %r14d                            #110.39
        lea       (%rdx,%r12,8), %edx                           #102.5
        lea       (%rsi,%r11,8), %r13                           #110.27
        movq      %r13, 48+samples_vec(%r10)                    #110.9
        lea       (%rsi,%r14,8), %r15                           #110.27
        movq      %r15, 56+samples_vec(%r10)                    #110.9
        cmpl      %eax, %r9d                                    #102.5
        jb        ..B9.84       # Prob 99%                      #102.5
                                # LOE rdx rsi eax ecx ebx edi r8d r9d r12d
..B9.85:                        # Preds ..B9.84
                                # Execution count [6.48e-01]
        lea       1(,%r9,8), %edx                               #102.5
                                # LOE rsi edx ebx r12d
..B9.86:                        # Preds ..B9.107 ..B9.85 ..B9.82
                                # Execution count [7.20e-01]
        cmpl      %ebx, %edx                                    #102.5
        ja        ..B9.102      # Prob 50%                      #102.5
                                # LOE rsi edx ebx r12d
..B9.87:                        # Preds ..B9.86
                                # Execution count [0.00e+00]
        subl      %edx, %ebx                                    #102.5
        jmp       *.2.21_2.switchtab.5(,%rbx,8)                 #102.5
                                # LOE rsi edx r12d
..1.21_3.TAG.6:
..B9.89:                        # Preds ..B9.87
                                # Execution count [0.00e+00]
        movl      %r12d, %eax                                   #110.39
        lea       5(%rdx), %ecx                                 #120.3
        imull     %ecx, %eax                                    #110.39
        movslq    %ecx, %rcx                                    #110.9
        movl      %eax, %eax                                    #110.27
        lea       (%rsi,%rax,8), %rbx                           #110.27
        movq      %rbx, samples_vec(,%rcx,8)                    #110.9
                                # LOE rsi edx r12d
..1.21_3.TAG.5:
..B9.91:                        # Preds ..B9.87 ..B9.89
                                # Execution count [0.00e+00]
        movl      %r12d, %eax                                   #110.39
        lea       4(%rdx), %ecx                                 #120.3
        imull     %ecx, %eax                                    #110.39
        movslq    %ecx, %rcx                                    #110.9
        movl      %eax, %eax                                    #110.27
        lea       (%rsi,%rax,8), %rbx                           #110.27
        movq      %rbx, samples_vec(,%rcx,8)                    #110.9
                                # LOE rsi edx r12d
..1.21_3.TAG.4:
..B9.93:                        # Preds ..B9.87 ..B9.91
                                # Execution count [0.00e+00]
        movl      %r12d, %eax                                   #110.39
        lea       3(%rdx), %ecx                                 #120.3
        imull     %ecx, %eax                                    #110.39
        movslq    %ecx, %rcx                                    #110.9
        movl      %eax, %eax                                    #110.27
        lea       (%rsi,%rax,8), %rbx                           #110.27
        movq      %rbx, samples_vec(,%rcx,8)                    #110.9
                                # LOE rsi edx r12d
..1.21_3.TAG.3:
..B9.95:                        # Preds ..B9.87 ..B9.93
                                # Execution count [0.00e+00]
        movl      %r12d, %eax                                   #110.39
        lea       2(%rdx), %ecx                                 #120.3
        imull     %ecx, %eax                                    #110.39
        movslq    %ecx, %rcx                                    #110.9
        movl      %eax, %eax                                    #110.27
        lea       (%rsi,%rax,8), %rbx                           #110.27
        movq      %rbx, samples_vec(,%rcx,8)                    #110.9
                                # LOE rsi edx r12d
..1.21_3.TAG.2:
..B9.97:                        # Preds ..B9.87 ..B9.95
                                # Execution count [0.00e+00]
        movl      %r12d, %eax                                   #110.39
        lea       1(%rdx), %ecx                                 #120.3
        imull     %ecx, %eax                                    #110.39
        movslq    %ecx, %rcx                                    #110.9
        movl      %eax, %eax                                    #110.27
        lea       (%rsi,%rax,8), %rbx                           #110.27
        movq      %rbx, samples_vec(,%rcx,8)                    #110.9
                                # LOE rsi edx r12d
..1.21_3.TAG.1:
..B9.99:                        # Preds ..B9.87 ..B9.97
                                # Execution count [0.00e+00]
        movl      %r12d, %eax                                   #110.39
        imull     %edx, %eax                                    #110.39
        movslq    %edx, %rdx                                    #110.9
        movl      %eax, %eax                                    #110.27
        lea       (%rsi,%rax,8), %rcx                           #110.27
        movq      %rcx, samples_vec(,%rdx,8)                    #110.9
                                # LOE rsi edx r12d
..1.21_3.TAG.0:
..B9.101:                       # Preds ..B9.87 ..B9.99
                                # Execution count [6.48e-01]
        decl      %edx                                          #120.3
        imull     %edx, %r12d                                   #110.39
        movslq    %edx, %rdx                                    #110.9
        movl      %r12d, %r12d                                  #110.27
        lea       (%rsi,%r12,8), %rax                           #110.27
        movq      %rax, samples_vec(,%rdx,8)                    #110.9
                                # LOE
..B9.102:                       # Preds ..B9.6 ..B9.86 ..B9.101
                                # Execution count [7.20e-01]
        xorl      %eax, %eax                                    #123.9
        addq      $88, %rsp                                     #123.9
	.cfi_restore 3
        popq      %rbx                                          #123.9
	.cfi_restore 15
        popq      %r15                                          #123.9
	.cfi_restore 14
        popq      %r14                                          #123.9
	.cfi_restore 13
        popq      %r13                                          #123.9
	.cfi_restore 12
        popq      %r12                                          #123.9
        movq      %rbp, %rsp                                    #123.9
        popq      %rbp                                          #123.9
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #123.9
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B9.103:                       # Preds ..B9.5
                                # Execution count [2.80e-01]
        movl      $-1, %eax                                     #99.16
        addq      $88, %rsp                                     #99.16
	.cfi_restore 3
        popq      %rbx                                          #99.16
	.cfi_restore 15
        popq      %r15                                          #99.16
	.cfi_restore 14
        popq      %r14                                          #99.16
	.cfi_restore 13
        popq      %r13                                          #99.16
	.cfi_restore 12
        popq      %r12                                          #99.16
        movq      %rbp, %rsp                                    #99.16
        popq      %rbp                                          #99.16
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #99.16
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0xc0, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B9.104:                       # Preds ..B9.29 ..B9.31 ..B9.32
                                # Execution count [3.24e-01]: Infreq
        movl      %esi, %ecx                                    #117.3
        jmp       ..B9.41       # Prob 100%                     #117.3
                                # LOE rdi r10 eax ecx esi r8d r9d r12d xmm0
..B9.106:                       # Preds ..B9.64
                                # Execution count [3.96e-02]: Infreq
        cmpl      $1, %ebx                                      #102.5
        jae       ..B9.68       # Prob 50%                      #102.5
                                # LOE rcx rsi eax edx ebx r12d r13d
..B9.107:                       # Preds ..B9.108 ..B9.106
                                # Execution count [3.60e-02]: Infreq
        movl      $1, %edx                                      #102.5
        jmp       ..B9.86       # Prob 100%                     #102.5
                                # LOE rsi edx ebx r12d
..B9.108:                       # Preds ..B9.46
                                # Execution count [7.20e-02]: Infreq
        cmpl      $1, %ebx                                      #102.5
        jae       ..B9.50       # Prob 50%                      #102.5
        jmp       ..B9.107      # Prob 100%                     #102.5
        .align    16,0x90
                                # LOE rdx rcx rsi eax ebx r9d r12d r13d
	.cfi_endproc
# mark_end;
	.type	dynais_init.A,@function
	.size	dynais_init.A,.-dynais_init.A
	.section .rodata, "a"
	.align 8
.2.21_2.switchtab.2:
	.quad	..1.21_0.TAG.0
	.quad	..1.21_0.TAG.1
	.quad	..1.21_0.TAG.2
	.quad	..1.21_0.TAG.3
	.quad	..1.21_0.TAG.4
	.quad	..1.21_0.TAG.5
	.quad	..1.21_0.TAG.6
	.align 8
.2.21_2.switchtab.3:
	.quad	..1.21_1.TAG.0
	.quad	..1.21_1.TAG.1
	.quad	..1.21_1.TAG.2
	.quad	..1.21_1.TAG.3
	.quad	..1.21_1.TAG.4
	.quad	..1.21_1.TAG.5
	.quad	..1.21_1.TAG.6
	.align 8
.2.21_2.switchtab.4:
	.quad	..1.21_2.TAG.0
	.quad	..1.21_2.TAG.1
	.quad	..1.21_2.TAG.2
	.quad	..1.21_2.TAG.3
	.quad	..1.21_2.TAG.4
	.quad	..1.21_2.TAG.5
	.quad	..1.21_2.TAG.6
	.align 8
.2.21_2.switchtab.5:
	.quad	..1.21_3.TAG.0
	.quad	..1.21_3.TAG.1
	.quad	..1.21_3.TAG.2
	.quad	..1.21_3.TAG.3
	.quad	..1.21_3.TAG.4
	.quad	..1.21_3.TAG.5
	.quad	..1.21_3.TAG.6
	.data
# -- End  dynais_init.A
	.section .rodata, "a"
	.space 32, 0x00 	# pad
	.align 64
.L_2il0floatpacket.0:
	.long	0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010,0x00000010
	.type	.L_2il0floatpacket.0,@object
	.size	.L_2il0floatpacket.0,64
	.align 64
.L_2il0floatpacket.1:
	.long	0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001
	.type	.L_2il0floatpacket.1,@object
	.size	.L_2il0floatpacket.1,64
	.align 64
.L_2il0floatpacket.2:
	.long	0x00000000,0x00000001,0x00000002,0x00000003,0x00000004,0x00000005,0x00000006,0x00000007,0x00000008,0x00000009,0x0000000a,0x0000000b,0x0000000c,0x0000000d,0x0000000e,0x0000000f
	.type	.L_2il0floatpacket.2,@object
	.size	.L_2il0floatpacket.2,64
	.align 64
.L_2il0floatpacket.3:
	.long	0x00000000,0xffffffff,0xfffffffe,0xfffffffd,0xfffffffc,0xfffffffb,0xfffffffa,0xfffffff9,0xfffffff8,0xfffffff7,0xfffffff6,0xfffffff5,0xfffffff4,0xfffffff3,0xfffffff2,0xfffffff1
	.type	.L_2il0floatpacket.3,@object
	.size	.L_2il0floatpacket.3,64
	.align 64
.L_2il0floatpacket.6:
	.long	0xffffffff,0x00000000,0x00000001,0x00000002,0x00000003,0x00000004,0x00000005,0x00000006,0x00000007,0x00000008,0x00000009,0x0000000a,0x0000000b,0x0000000c,0x0000000d,0x0000000e
	.type	.L_2il0floatpacket.6,@object
	.size	.L_2il0floatpacket.6,64
	.align 32
.L_2il0floatpacket.5:
	.long	0x00000000,0x00000001,0x00000002,0x00000003,0x00000004,0x00000005,0x00000006,0x00000007
	.type	.L_2il0floatpacket.5,@object
	.size	.L_2il0floatpacket.5,32
	.align 4
.L_2il0floatpacket.4:
	.long	0xffffffff
	.type	.L_2il0floatpacket.4,@object
	.size	.L_2il0floatpacket.4,4
	.data
	.comm _levels,4,4
	.comm _window,4,4
	.comm samples_vec,80,32
	.comm zeros_vec,80,32
	.comm sizes_vec,80,32
	.comm indes_vec,80,32
	.comm level_result,40,4
	.comm level_previous_length,40,4
	.comm level_length,40,4
	.comm level_limit,40,4
	.comm level_index,40,4
	.section .note.GNU-stack, ""
// -- Begin DWARF2 SEGMENT .eh_frame
	.section .eh_frame,"a",@progbits
.eh_frame_seg:
	.align 8
# End
