# mark_description "Intel(R) C Intel(R) 64 Compiler for applications running on Intel(R) 64, Version 17.0.5.239 Build 20170817";
# mark_description "-I/home/xjcorbalan/git/EAR/tests/functionals -I/home/xjcorbalan/git/EAR/src/common -I/home/xjcorbalan/git/EA";
# mark_description "R/src/daemon -I/home/xjcorbalan/git/EAR/src/library -I/home/xjcorbalan/my_papi/include -O3 -malign-double -a";
# mark_description "xCORE-AVX512 -S";
	.file "simd_power_metrics_2.c"
	.text
..TXTST0:
# -- Begin  main
	.text
# mark_begin;
       .align    16,0x90
	.globl main
# --- main(int, char **)
main:
# parameter 1: %edi
# parameter 2: %rsi
..B1.1:                         # Preds ..B1.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_main.1:
..L2:
                                                          #386.1
        pushq     %rbp                                          #386.1
	.cfi_def_cfa_offset 16
        movq      %rsp, %rbp                                    #386.1
	.cfi_def_cfa 6, 16
	.cfi_offset 6, -16
        andq      $-128, %rsp                                   #386.1
        pushq     %r12                                          #386.1
        pushq     %r13                                          #386.1
        pushq     %r14                                          #386.1
        pushq     %r15                                          #386.1
        pushq     %rbx                                          #386.1
        subq      $472, %rsp                                    #386.1
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
        movq      %rsi, %r12                                    #386.1
        movl      %edi, %ebx                                    #386.1
        xorl      %esi, %esi                                    #386.1
        movl      $3, %edi                                      #386.1
        call      __intel_new_feature_proc_init                 #386.1
                                # LOE r12 ebx
..B1.178:                       # Preds ..B1.1
                                # Execution count [1.00e+00]
        stmxcsr   (%rsp)                                        #386.1
        movl      $.L_2__STRING.16, %edi                        #404.5
        movl      $4, %esi                                      #404.5
        orl       $32832, (%rsp)                                #386.1
        movl      $8, %edx                                      #404.5
        xorl      %eax, %eax                                    #404.5
        ldmxcsr   (%rsp)                                        #386.1
..___tag_value_main.11:
#       printf(const char *__restrict__, ...)
        call      printf                                        #404.5
..___tag_value_main.12:
                                # LOE r12 ebx
..B1.2:                         # Preds ..B1.178
                                # Execution count [1.00e+00]
        cmpl      $5, %ebx                                      #406.17
        je        ..B1.9        # Prob 40%                      #406.17
                                # LOE r12
..B1.3:                         # Preds ..B1.2
                                # Execution count [5.97e-01]
        movl      $il0_peep_printf_format_8, %edi               #407.9
        call      puts                                          #407.9
                                # LOE
..B1.4:                         # Preds ..B1.3
                                # Execution count [5.97e-01]
        movl      $il0_peep_printf_format_9, %edi               #407.9
        call      puts                                          #407.9
                                # LOE
..B1.5:                         # Preds ..B1.4
                                # Execution count [5.97e-01]
        movl      $il0_peep_printf_format_10, %edi              #407.9
        call      puts                                          #407.9
                                # LOE
..B1.6:                         # Preds ..B1.5
                                # Execution count [5.97e-01]
        movl      $il0_peep_printf_format_11, %edi              #407.9
        call      puts                                          #407.9
                                # LOE
..B1.7:                         # Preds ..B1.6
                                # Execution count [5.97e-01]
        movl      $il0_peep_printf_format_12, %edi              #407.9
        call      puts                                          #407.9
                                # LOE
..B1.8:                         # Preds ..B1.157 ..B1.153 ..B1.151 ..B1.149 ..B1.7
                                #      
                                # Execution count [5.97e-01]
        movl      $1, %edi                                      #407.9
#       exit(int)
        call      exit                                          #407.9
                                # LOE
..B1.9:                         # Preds ..B1.2
                                # Execution count [1.00e+00]
        movq      8(%r12), %rdi                                 #411.17
        call      atol                                          #411.17
                                # LOE r12
..B1.10:                        # Preds ..B1.9
                                # Execution count [1.00e+00]
        movq      16(%r12), %rdi                                #412.17
        call      atol                                          #412.17
                                # LOE rax r12
..B1.179:                       # Preds ..B1.10
                                # Execution count [1.00e+00]
        movq      %rax, %rbx                                    #412.17
                                # LOE rbx r12
..B1.11:                        # Preds ..B1.179
                                # Execution count [1.00e+00]
        xorl      %esi, %esi                                    #413.20
        movl      $10, %edx                                     #413.20
        movq      24(%r12), %rdi                                #413.20
        movl      %ebx, %r15d                                   #412.17
#       strtoul(const char *__restrict__, char **__restrict__, int)
        call      strtoul                                       #413.20
                                # LOE rax rbx r12 r15d
..B1.12:                        # Preds ..B1.11
                                # Execution count [1.00e+00]
        movq      32(%r12), %rdi                                #414.11
        movq      %rax, n_iterations(%rip)                      #413.5
        call      atol                                          #414.11
                                # LOE rax rbx r15d
..B1.181:                       # Preds ..B1.12
                                # Execution count [1.00e+00]
        movq      %rax, %r14                                    #414.11
                                # LOE rbx r14 r15d
..B1.13:                        # Preds ..B1.181
                                # Execution count [1.00e+00]
        movl      $128, %eax                                    #415.5
        pxor      %xmm0, %xmm0                                  #415.5
                                # LOE rax rbx r14 r15d xmm0
..B1.162:                       # Preds ..B1.13 ..B1.162
                                # Execution count [2.00e+00]
        movups    %xmm0, 48(%rsp,%rax)                          #415.5
        movups    %xmm0, 32(%rsp,%rax)                          #415.5
        movups    %xmm0, 16(%rsp,%rax)                          #415.5
        movups    %xmm0, (%rsp,%rax)                            #415.5
        subq      $64, %rax                                     #415.5
        jne       ..B1.162      # Prob 50%                      #415.5
                                # LOE rax rbx r14 r15d xmm0
..B1.14:                        # Preds ..B1.162
                                # Execution count [5.00e-01]
        xorl      %edi, %edi                                    #415.5
        movl      $128, %esi                                    #415.5
        orq       $1, 64(%rsp)                                  #415.5
        lea       64(%rsp), %rdx                                #415.5
..___tag_value_main.13:
#       sched_setaffinity(__pid_t, size_t, const cpu_set_t *)
        call      sched_setaffinity                             #415.5
..___tag_value_main.14:
                                # LOE rbx r14 r15d
..B1.15:                        # Preds ..B1.14
                                # Execution count [1.00e+00]
        movl      %ebx, %ebx                                    #423.37
        lea       (,%rbx,4), %rdi                               #423.14
#       malloc(size_t)
        call      malloc                                        #423.14
                                # LOE rax rbx r14 r15d
..B1.182:                       # Preds ..B1.15
                                # Execution count [1.00e+00]
        movq      %rax, %r12                                    #423.14
                                # LOE rbx r12 r14 r15d
..B1.16:                        # Preds ..B1.182
                                # Execution count [1.00e+00]
        lea       (,%rbx,8), %rdi                               #424.12
#       malloc(size_t)
        call      malloc                                        #424.12
                                # LOE rax rbx r12 r14 r15d
..B1.183:                       # Preds ..B1.16
                                # Execution count [1.00e+00]
        movq      %rax, %r13                                    #424.12
                                # LOE rbx r12 r13 r14 r15d
..B1.17:                        # Preds ..B1.183
                                # Execution count [1.00e+00]
        testl     %r14d, %r14d                                  #426.16
        jne       ..B1.23       # Prob 78%                      #426.16
                                # LOE rbx r12 r13 r15d
..B1.18:                        # Preds ..B1.17
                                # Execution count [2.20e-01]
        movl      $.L_2__STRING.17, %edi                        #427.9
        movl      $85, %esi                                     #427.9
        xorl      %eax, %eax                                    #427.9
..___tag_value_main.15:
#       printf(const char *__restrict__, ...)
        call      printf                                        #427.9
..___tag_value_main.16:
                                # LOE rbx r12 r13 r15d
..B1.19:                        # Preds ..B1.18
                                # Execution count [2.20e-01]
..___tag_value_main.17:
#       sched_getcpu(void)
        call      sched_getcpu                                  #428.46
..___tag_value_main.18:
                                # LOE rbx r12 r13 eax r15d
..B1.20:                        # Preds ..B1.19
                                # Execution count [2.20e-01]
        movl      $.L_2__STRING.18, %edi                        #428.9
        movl      %eax, %esi                                    #428.9
        movl      %r15d, %edx                                   #428.9
        xorl      %eax, %eax                                    #428.9
..___tag_value_main.19:
#       printf(const char *__restrict__, ...)
        call      printf                                        #428.9
..___tag_value_main.20:
                                # LOE rbx r12 r13 r15d
..B1.21:                        # Preds ..B1.20
                                # Execution count [2.20e-01]
        movl      $il0_peep_printf_format_0, %edi               #429.9
        call      puts                                          #429.9
                                # LOE rbx r12 r13 r15d
..B1.22:                        # Preds ..B1.21
                                # Execution count [2.20e-01]
        movl      $il0_peep_printf_format_1, %edi               #430.9
        call      puts                                          #430.9
                                # LOE rbx r12 r13 r15d
..B1.23:                        # Preds ..B1.22 ..B1.17
                                # Execution count [1.00e+00]
        xorl      %eax, %eax                                    #439.5
..___tag_value_main.21:
#       init_rapl_metrics()
        call      init_rapl_metrics                             #439.5
..___tag_value_main.22:
                                # LOE rbx r12 r13 eax r15d
..B1.24:                        # Preds ..B1.23
                                # Execution count [1.00e+00]
        cmpl      $-1, %eax                                     #439.5
        je        ..B1.157      # Prob 16%                      #439.5
                                # LOE rbx r12 r13 r15d
..B1.25:                        # Preds ..B1.24
                                # Execution count [8.40e-01]
        xorl      %eax, %eax                                    #440.5
..___tag_value_main.23:
#       init_basic_metrics()
        call      init_basic_metrics                            #440.5
..___tag_value_main.24:
                                # LOE rbx r12 r13 r15d
..B1.26:                        # Preds ..B1.25
                                # Execution count [8.40e-01]
        xorl      %eax, %eax                                    #441.5
..___tag_value_main.25:
#       init_flops_metrics()
        call      init_flops_metrics                            #441.5
..___tag_value_main.26:
                                # LOE rbx r12 r13 r15d
..B1.27:                        # Preds ..B1.26
                                # Execution count [8.40e-01]
        xorl      %eax, %eax                                    #442.5
..___tag_value_main.27:
#       ear_cpufreq_init()
        call      ear_cpufreq_init                              #442.5
..___tag_value_main.28:
                                # LOE rbx r12 r13 r15d
..B1.28:                        # Preds ..B1.27
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.22, %edi                        #444.5
        xorl      %eax, %eax                                    #444.5
..___tag_value_main.29:
#       printf(const char *__restrict__, ...)
        call      printf                                        #444.5
..___tag_value_main.30:
                                # LOE rbx r12 r13 r15d
..B1.29:                        # Preds ..B1.28
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.23, %edi                        #445.5
        xorl      %eax, %eax                                    #445.5
..___tag_value_main.31:
#       printf(const char *__restrict__, ...)
        call      printf                                        #445.5
..___tag_value_main.32:
                                # LOE rbx r12 r13 r15d
..B1.30:                        # Preds ..B1.29
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.24, %edi                        #446.5
        xorl      %eax, %eax                                    #446.5
..___tag_value_main.33:
#       printf(const char *__restrict__, ...)
        call      printf                                        #446.5
..___tag_value_main.34:
                                # LOE rbx r12 r13 r15d
..B1.31:                        # Preds ..B1.30
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.25, %edi                        #447.5
        xorl      %eax, %eax                                    #447.5
..___tag_value_main.35:
#       printf(const char *__restrict__, ...)
        call      printf                                        #447.5
..___tag_value_main.36:
                                # LOE rbx r12 r13 r15d
..B1.32:                        # Preds ..B1.31
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.26, %edi                        #448.5
        xorl      %eax, %eax                                    #448.5
..___tag_value_main.37:
#       printf(const char *__restrict__, ...)
        call      printf                                        #448.5
..___tag_value_main.38:
                                # LOE rbx r12 r13 r15d
..B1.33:                        # Preds ..B1.32
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.27, %edi                        #449.5
        xorl      %eax, %eax                                    #449.5
..___tag_value_main.39:
#       printf(const char *__restrict__, ...)
        call      printf                                        #449.5
..___tag_value_main.40:
                                # LOE rbx r12 r13 r15d
..B1.34:                        # Preds ..B1.33
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.28, %edi                        #450.5
        xorl      %eax, %eax                                    #450.5
..___tag_value_main.41:
#       printf(const char *__restrict__, ...)
        call      printf                                        #450.5
..___tag_value_main.42:
                                # LOE rbx r12 r13 r15d
..B1.35:                        # Preds ..B1.34
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.29, %edi                        #451.5
        xorl      %eax, %eax                                    #451.5
..___tag_value_main.43:
#       printf(const char *__restrict__, ...)
        call      printf                                        #451.5
..___tag_value_main.44:
                                # LOE rbx r12 r13 r15d
..B1.36:                        # Preds ..B1.35
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.30, %edi                        #452.5
        xorl      %eax, %eax                                    #452.5
..___tag_value_main.45:
#       printf(const char *__restrict__, ...)
        call      printf                                        #452.5
..___tag_value_main.46:
                                # LOE rbx r12 r13 r15d
..B1.37:                        # Preds ..B1.36
                                # Execution count [8.40e-01]
        movl      $il0_peep_printf_format_2, %edi               #453.5
        call      puts                                          #453.5
                                # LOE rbx r12 r13 r15d
..B1.38:                        # Preds ..B1.37
                                # Execution count [8.40e-01]
        movl      $2400000, %edi                                #455.5
..___tag_value_main.47:
#       ear_cpufreq_set_node(unsigned long)
        call      ear_cpufreq_set_node                          #455.5
..___tag_value_main.48:
                                # LOE rbx r12 r13 r15d
..B1.39:                        # Preds ..B1.38
                                # Execution count [8.40e-01]
        movl      $.L_2__STRING.32, %edi                        #456.5
        movl      $9, %esi                                      #456.5
        xorl      %eax, %eax                                    #456.5
..___tag_value_main.49:
#       printf(const char *__restrict__, ...)
        call      printf                                        #456.5
..___tag_value_main.50:
                                # LOE rbx r12 r13 r15d
..B1.40:                        # Preds ..B1.39
                                # Execution count [8.30e-01]
        pxor      %xmm0, %xmm0                                  #517.44
        movl      $6, %r14d                                     #458.10
        cvtsi2sdq %rbx, %xmm0                                   #517.44
        movsd     .L_2il0floatpacket.0(%rip), %xmm2             #510.41
        movsd     .L_2il0floatpacket.1(%rip), %xmm1             #513.37
        movsd     %xmm0, 312(%rsp)                              #517.44[spill]
        movq      %rax, 320(%rsp)                               #517.44[spill]
        movq      %rcx, 328(%rsp)                               #517.44[spill]
        movq      %r13, 344(%rsp)                               #517.44[spill]
        movq      %r12, 336(%rsp)                               #517.44[spill]
                                # LOE r14d r15d
..B1.41:                        # Preds ..B1.143 ..B1.40
                                # Execution count [1.26e+00]
        movl      %r14d, test(%rip)                             #461.9
        movl      $1, %eax                                      #465.14
        movl      $1, spinning(%rip)                            #462.9
        cmpl      $1, %r15d                                     #465.39
        jbe       ..B1.47       # Prob 3%                       #465.39
                                # LOE rax r14d r15d
..B1.42:                        # Preds ..B1.41
                                # Execution count [1.23e+00]
        movq      344(%rsp), %r12                               #467.13[spill]
        movl      $1, %r13d                                     #467.13
        movq      336(%rsp), %rbx                               #467.13[spill]
                                # LOE rax rbx r12 r13d r14d r15d
..B1.43:                        # Preds ..B1.45 ..B1.42
                                # Execution count [5.76e+00]
        xorl      %esi, %esi                                    #468.22
        lea       (%r12,%rax,8), %rdi                           #468.22
        movl      $threaded_run_test, %edx                      #468.22
        lea       (%rbx,%rax,4), %rcx                           #468.22
        movl      %r13d, (%rcx)                                 #467.13
#       pthread_create(pthread_t *__restrict__, const pthread_attr_t *__restrict__, void *(*)(void *), void *__restrict__)
        call      pthread_create                                #468.22
                                # LOE rbx r12 eax r13d r14d r15d
..B1.44:                        # Preds ..B1.43
                                # Execution count [5.76e+00]
        testl     %eax, %eax                                    #470.27
        jne       ..B1.155      # Prob 4%                       #470.27
                                # LOE rbx r12 eax r13d r14d r15d
..B1.45:                        # Preds ..B1.44
                                # Execution count [5.53e+00]
        incl      %r13d                                         #465.52
        movl      %r13d, %eax                                   #465.52
        cmpl      %r15d, %r13d                                  #465.39
        jb        ..B1.43       # Prob 82%                      #465.39
                                # LOE rax rbx r12 r13d r14d r15d
..B1.47:                        # Preds ..B1.45 ..B1.41
                                # Execution count [1.03e+00]
        xorl      %eax, %eax                                    #476.9
..___tag_value_main.51:
#       start_rapl_metrics()
        call      start_rapl_metrics                            #476.9
..___tag_value_main.52:
                                # LOE eax r14d r15d
..B1.48:                        # Preds ..B1.47
                                # Execution count [1.03e+00]
        cmpl      $-1, %eax                                     #476.9
        je        ..B1.153      # Prob 20%                      #476.9
                                # LOE r14d r15d
..B1.49:                        # Preds ..B1.48
                                # Execution count [8.27e-01]
        xorl      %eax, %eax                                    #477.9
..___tag_value_main.53:
#       start_flops_metrics()
        call      start_flops_metrics                           #477.9
..___tag_value_main.54:
                                # LOE r14d r15d
..B1.50:                        # Preds ..B1.49
                                # Execution count [8.27e-01]
        xorl      %eax, %eax                                    #478.2
..___tag_value_main.55:
#       start_basic_metrics()
        call      start_basic_metrics                           #478.2
..___tag_value_main.56:
                                # LOE r14d r15d
..B1.51:                        # Preds ..B1.50
                                # Execution count [8.27e-01]
..___tag_value_main.57:
#       PAPI_get_real_usec(void)
        call      PAPI_get_real_usec                            #479.22
..___tag_value_main.58:
                                # LOE rax r14d r15d
..B1.188:                       # Preds ..B1.51
                                # Execution count [8.27e-01]
        movq      %rax, %r13                                    #479.22
                                # LOE r13 r14d r15d
..B1.52:                        # Preds ..B1.188
                                # Execution count [8.27e-01]
        xorl      %eax, %eax                                    #480.9
..___tag_value_main.59:
#       ear_begin_compute_turbo_freq()
        call      ear_begin_compute_turbo_freq                  #480.9
..___tag_value_main.60:
                                # LOE r13 r14d r15d
..B1.53:                        # Preds ..B1.52
                                # Execution count [8.27e-01]
        movl      test(%rip), %edi                              #483.9
        movl      $0, spinning(%rip)                            #482.9
..___tag_value_main.61:
#       run_test(int)
        call      run_test                                      #483.9
..___tag_value_main.62:
                                # LOE r13 r14d r15d
..B1.54:                        # Preds ..B1.53
                                # Execution count [8.27e-01]
..___tag_value_main.63:
#       PAPI_get_real_usec(void)
        call      PAPI_get_real_usec                            #486.22
..___tag_value_main.64:
                                # LOE rax r13 r14d r15d
..B1.189:                       # Preds ..B1.54
                                # Execution count [8.27e-01]
        movq      %rax, %rbx                                    #486.22
                                # LOE rbx r13 r14d r15d
..B1.55:                        # Preds ..B1.189
                                # Execution count [8.27e-01]
        xorl      %eax, %eax                                    #489.21
..___tag_value_main.65:
#       ear_end_compute_turbo_freq()
        call      ear_end_compute_turbo_freq                    #489.21
..___tag_value_main.66:
                                # LOE rax rbx r13 r14d r15d
..B1.190:                       # Preds ..B1.55
                                # Execution count [8.27e-01]
        movq      %rax, %r12                                    #489.21
                                # LOE rbx r12 r13 r14d r15d
..B1.56:                        # Preds ..B1.190
                                # Execution count [8.27e-01]
        lea       192(%rsp), %rdi                               #490.9
..___tag_value_main.67:
#       stop_rapl_metrics(unsigned long long *)
        call      stop_rapl_metrics                             #490.9
..___tag_value_main.68:
                                # LOE rbx r12 r13 eax r14d r15d
..B1.57:                        # Preds ..B1.56
                                # Execution count [8.27e-01]
        cmpl      $-1, %eax                                     #490.9
        je        ..B1.151      # Prob 20%                      #490.9
                                # LOE rbx r12 r13 r14d r15d
..B1.58:                        # Preds ..B1.57
                                # Execution count [6.61e-01]
        xorl      %eax, %eax                                    #491.9
..___tag_value_main.69:
#       reset_rapl_metrics()
        call      reset_rapl_metrics                            #491.9
..___tag_value_main.70:
                                # LOE rbx r12 r13 eax r14d r15d
..B1.59:                        # Preds ..B1.58
                                # Execution count [6.61e-01]
        cmpl      $-1, %eax                                     #491.9
        je        ..B1.149      # Prob 20%                      #491.9
                                # LOE rbx r12 r13 r14d r15d
..B1.60:                        # Preds ..B1.59
                                # Execution count [5.29e-01]
        lea       240(%rsp), %rdi                               #492.9
        lea       (%rsp), %rsi                                  #492.9
..___tag_value_main.71:
#       stop_flops_metrics(long long *, long long *)
        call      stop_flops_metrics                            #492.9
..___tag_value_main.72:
                                # LOE rbx r12 r13 r14d r15d
..B1.61:                        # Preds ..B1.60
                                # Execution count [5.29e-01]
        lea       248(%rsp), %rdi                               #493.2
        lea       256(%rsp), %rsi                               #493.2
..___tag_value_main.73:
#       stop_basic_metrics(long long *, long long *)
        call      stop_basic_metrics                            #493.2
..___tag_value_main.74:
                                # LOE rbx r12 r13 r14d r15d
..B1.62:                        # Preds ..B1.61
                                # Execution count [5.29e-01]
        xorl      %eax, %eax                                    #494.9
..___tag_value_main.75:
#       reset_flops_metrics()
        call      reset_flops_metrics                           #494.9
..___tag_value_main.76:
                                # LOE rbx r12 r13 r14d r15d
..B1.63:                        # Preds ..B1.62
                                # Execution count [5.29e-01]
        xorl      %eax, %eax                                    #495.3
..___tag_value_main.77:
#       reset_basic_metrics()
        call      reset_basic_metrics                           #495.3
..___tag_value_main.78:
                                # LOE rbx r12 r13 r14d r15d
..B1.64:                        # Preds ..B1.63
                                # Execution count [5.29e-01]
        movl      $1, %eax                                      #497.14
        cmpl      $1, %r15d                                     #497.39
        jbe       ..B1.69       # Prob 10%                      #497.39
                                # LOE rbx r12 r13 eax r14d r15d
..B1.65:                        # Preds ..B1.64
                                # Execution count [4.76e-01]
        movq      %r13, 224(%rsp)                               #[spill]
        movl      %r14d, 232(%rsp)                              #[spill]
        movl      %eax, %r14d                                   #
        movq      344(%rsp), %r13                               #[spill]
                                # LOE rbx r12 r13 r14 r15d
..B1.66:                        # Preds ..B1.67 ..B1.65
                                # Execution count [2.65e+00]
        xorl      %esi, %esi                                    #498.13
        movq      (%r13,%r14,8), %rdi                           #498.13
..___tag_value_main.79:
#       pthread_join(pthread_t, void **)
        call      pthread_join                                  #498.13
..___tag_value_main.80:
                                # LOE rbx r12 r13 r14d r15d
..B1.67:                        # Preds ..B1.66
                                # Execution count [2.65e+00]
        incl      %r14d                                         #497.50
        cmpl      %r15d, %r14d                                  #497.39
        jb        ..B1.66       # Prob 82%                      #497.39
                                # LOE rbx r12 r13 r14 r15d
..B1.68:                        # Preds ..B1.67
                                # Execution count [4.76e-01]
        movq      %r13, 344(%rsp)                               #[spill]
        movq      224(%rsp), %r13                               #[spill]
        movl      232(%rsp), %r14d                              #[spill]
                                # LOE rbx r12 r13 r14d r15d
..B1.69:                        # Preds ..B1.68 ..B1.64
                                # Execution count [5.29e-01]
        pxor      %xmm0, %xmm0                                  #503.36
        movq      208(%rsp), %rax                               #503.36
        cvtsi2sdq %rax, %xmm0                                   #503.36
        movsd     %xmm0, 304(%rsp)                              #503.36[spill]
        testq     %rax, %rax                                    #503.36
        jge       ..B1.164      # Prob 70%                      #503.36
                                # LOE rax rbx r12 r13 r14d r15d xmm0
..B1.165:                       # Preds ..B1.69
                                # Execution count [1.59e-01]
        movq      %rax, %rcx                                    #503.36
        pxor      %xmm0, %xmm0                                  #503.36
        shrq      $1, %rax                                      #503.36
        andq      $1, %rcx                                      #503.36
        orq       %rax, %rcx                                    #503.36
        cvtsi2sdq %rcx, %xmm0                                   #503.36
        addsd     %xmm0, %xmm0                                  #503.36
        movsd     %xmm0, 304(%rsp)                              #503.36[spill]
                                # LOE rbx r12 r13 r14d r15d xmm0
..B1.164:                       # Preds ..B1.165 ..B1.69
                                # Execution count [5.29e-01]
        pxor      %xmm1, %xmm1                                  #503.36
        movq      216(%rsp), %rax                               #503.36
        cvtsi2sdq %rax, %xmm1                                   #503.36
        testq     %rax, %rax                                    #503.36
        jge       ..B1.166      # Prob 70%                      #503.36
                                # LOE rax rbx r12 r13 r14d r15d xmm0 xmm1
..B1.167:                       # Preds ..B1.164
                                # Execution count [1.59e-01]
        movq      %rax, %rcx                                    #503.36
        pxor      %xmm1, %xmm1                                  #503.36
        shrq      $1, %rax                                      #503.36
        andq      $1, %rcx                                      #503.36
        orq       %rax, %rcx                                    #503.36
        cvtsi2sdq %rcx, %xmm1                                   #503.36
        addsd     %xmm1, %xmm1                                  #503.36
                                # LOE rbx r12 r13 r14d r15d xmm0 xmm1
..B1.166:                       # Preds ..B1.167 ..B1.164
                                # Execution count [5.29e-01]
        subq      %r13, %rbx                                    #486.22
        addsd     %xmm1, %xmm0                                  #503.13
        pxor      %xmm1, %xmm1                                  #510.28
        cvtsi2sdq %rbx, %xmm1                                   #510.28
        movsd     %xmm0, 304(%rsp)                              #503.13[spill]
        movsd     %xmm1, 296(%rsp)                              #510.28[spill]
        jns       ..B1.168      # Prob 70%                      #510.28
                                # LOE rbx r12 r14d r15d
..B1.169:                       # Preds ..B1.166
                                # Execution count [1.59e-01]
        movq      %rbx, %rax                                    #510.28
        pxor      %xmm0, %xmm0                                  #510.28
        shrq      $1, %rbx                                      #510.28
        andq      $1, %rax                                      #510.28
        orq       %rbx, %rax                                    #510.28
        cvtsi2sdq %rax, %xmm0                                   #510.28
        addsd     %xmm0, %xmm0                                  #510.28
        movsd     %xmm0, 296(%rsp)                              #510.28[spill]
                                # LOE r12 r14d r15d
..B1.168:                       # Preds ..B1.169 ..B1.166
                                # Execution count [5.29e-01]
        movsd     304(%rsp), %xmm2                              #513.37[spill]
        movaps    %xmm2, %xmm0                                  #513.37
        divsd     312(%rsp), %xmm2                              #517.44[spill]
        movsd     296(%rsp), %xmm3                              #510.41[spill]
        movsd     .L_2il0floatpacket.1(%rip), %xmm1             #513.37
        divsd     .L_2il0floatpacket.0(%rip), %xmm3             #510.41
        divsd     %xmm1, %xmm0                                  #513.37
        divsd     %xmm1, %xmm2                                  #519.32
        movsd     %xmm0, 264(%rsp)                              #513.37[spill]
        divsd     %xmm3, %xmm0                                  #514.38
        movsd     %xmm2, 304(%rsp)                              #519.32[spill]
        divsd     %xmm3, %xmm2                                  #521.30
        movl      test(%rip), %ecx                              #528.38
        movsd     %xmm3, 296(%rsp)                              #510.41[spill]
        movsd     %xmm0, 272(%rsp)                              #514.38[spill]
        movsd     %xmm2, 280(%rsp)                              #521.30[spill]
        movq      n_iterations(%rip), %rbx                      #523.41
        cmpq      $8, %rcx                                      #528.25
        ja        ..B1.127      # Prob 28%                      #528.25
                                # LOE rcx rbx r12 r14d r15d
..B1.70:                        # Preds ..B1.168
                                # Execution count [3.81e-01]
        jmp       *.2.35_2.switchtab.16(,%rcx,8)                #528.25
                                # LOE rcx rbx r12 r14d r15d
..1.14_0.TAG.8.0.14.28:
..B1.72:                        # Preds ..B1.70
                                # Execution count [4.23e-02]
        movq      $16, 328(%rsp)                                #528.25[spill]
        jmp       ..B1.89       # Prob 100%                     #528.25
                                # LOE rcx rbx r12 r14d r15d
..1.14_0.TAG.7.0.14.28:
..1.14_0.TAG.6.0.14.28:
..1.14_0.TAG.5.0.14.28:
..B1.74:                        # Preds ..B1.70 ..B1.70 ..B1.70
                                # Execution count [4.23e-02]
        movq      $8, 328(%rsp)                                 #528.25[spill]
        jmp       ..B1.89       # Prob 100%                     #528.25
                                # LOE rcx rbx r12 r14d r15d
..1.14_0.TAG.4.0.14.28:
..1.14_0.TAG.3.0.14.28:
..1.14_0.TAG.2.0.14.28:
..B1.80:                        # Preds ..B1.70 ..B1.70 ..B1.70
                                # Execution count [4.23e-02]
        movq      $4, 328(%rsp)                                 #528.25[spill]
        jmp       ..B1.89       # Prob 100%                     #528.25
                                # LOE rcx rbx r12 r14d r15d
..1.14_0.TAG.0.0.14.28:
..1.14_0.TAG.1.0.14.28:
..B1.88:                        # Preds ..B1.70 ..B1.70
                                # Execution count [4.23e-02]
        movq      $2, 328(%rsp)                                 #528.25[spill]
                                # LOE rcx rbx r12 r14d r15d
..B1.89:                        # Preds ..B1.88 ..B1.80 ..B1.74 ..B1.72
                                # Execution count [3.81e-01]
        movq      %rbx, %r13                                    #528.46
        pxor      %xmm0, %xmm0                                  #530.29
        imulq     328(%rsp), %r13                               #528.46[spill]
        cvtsi2sdq %r13, %xmm0                                   #530.29
        movsd     %xmm0, 288(%rsp)                              #530.29[spill]
        testq     %r13, %r13                                    #530.29
        jge       ..B1.170      # Prob 70%                      #530.29
                                # LOE rcx rbx r12 r13 r14d r15d xmm0
..B1.171:                       # Preds ..B1.89
                                # Execution count [1.14e-01]
        movq      %r13, %rax                                    #530.29
        pxor      %xmm0, %xmm0                                  #530.29
        shrq      $1, %rax                                      #530.29
        cvtsi2sdq %rax, %xmm0                                   #530.29
        addsd     %xmm0, %xmm0                                  #530.29
        movsd     %xmm0, 288(%rsp)                              #530.29[spill]
                                # LOE rcx rbx r12 r13 r14d r15d xmm0
..B1.170:                       # Preds ..B1.171 ..B1.89
                                # Execution count [3.81e-01]
        divsd     296(%rsp), %xmm0                              #530.40[spill]
        divsd     .L_2il0floatpacket.0(%rip), %xmm0             #532.29
        movsd     %xmm0, 288(%rsp)                              #532.29[spill]
        jmp       *.2.35_2.switchtab.17(,%rcx,8)                #536.24
                                # LOE rcx rbx r12 r13 r14d r15d
..1.14_0.TAG.8.0.14.29:
..1.14_0.TAG.7.0.14.29:
..1.14_0.TAG.6.0.14.29:
..1.14_0.TAG.5.0.14.29:
..1.14_0.TAG.4.0.14.29:
..1.14_0.TAG.3.0.14.29:
..1.14_0.TAG.2.0.14.29:
..1.14_0.TAG.1.0.14.29:
..1.14_0.TAG.0.0.14.29:
..B1.108:                       # Preds ..B1.170 ..B1.170 ..B1.170 ..B1.170 ..B1.170
                                #       ..B1.170 ..B1.170 ..B1.170 ..B1.170
                                # Execution count [3.81e-01]
        jmp       *.2.35_2.switchtab.18(,%rcx,8)                #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.8.0.13.32:
..B1.110:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.10, 320(%rsp)                   #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.7.0.13.32:
..B1.112:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.9, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.6.0.13.32:
..B1.114:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.8, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.5.0.13.32:
..B1.116:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.7, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.4.0.13.32:
..B1.118:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.6, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.3.0.13.32:
..B1.120:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.5, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.2.0.13.32:
..B1.122:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.4, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.1.0.13.32:
..B1.124:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.3, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..1.13_0.TAG.0.0.13.32:
..B1.126:                       # Preds ..B1.108
                                # Execution count [4.23e-02]
        movq      $.L_2__STRING.2, 320(%rsp)                    #538.51[spill]
        jmp       ..B1.128      # Prob 100%                     #538.51
                                # LOE rbx r12 r13 r14d r15d
..B1.127:                       # Preds ..B1.168
                                # Execution count [1.48e-01]
        movq      %rbx, %r13                                    #528.46
        pxor      %xmm0, %xmm0                                  #530.29
        imulq     328(%rsp), %r13                               #528.46[spill]
        cvtsi2sdq %r13, %xmm0                                   #530.29
        movsd     %xmm0, 288(%rsp)                              #530.29[spill]
        testq     %r13, %r13                                    #530.29
        jge       ..B1.172      # Prob 70%                      #530.29
                                # LOE rbx r12 r13 r14d r15d xmm0
..B1.173:                       # Preds ..B1.127
                                # Execution count [4.44e-02]
        movq      %r13, %rcx                                    #530.29
        movq      %r13, %rax                                    #530.29
        shrq      $1, %rax                                      #530.29
        andq      $1, %rcx                                      #530.29
        orq       %rax, %rcx                                    #530.29
        pxor      %xmm0, %xmm0                                  #530.29
        cvtsi2sdq %rcx, %xmm0                                   #530.29
        addsd     %xmm0, %xmm0                                  #530.29
        movsd     %xmm0, 288(%rsp)                              #530.29[spill]
                                # LOE rbx r12 r13 r14d r15d xmm0
..B1.172:                       # Preds ..B1.173 ..B1.127
                                # Execution count [1.48e-01]
        divsd     296(%rsp), %xmm0                              #530.40[spill]
        divsd     .L_2il0floatpacket.0(%rip), %xmm0             #532.29
        movsd     %xmm0, 288(%rsp)                              #532.29[spill]
                                # LOE rbx r12 r13 r14d r15d
..B1.128:                       # Preds ..B1.172 ..B1.126 ..B1.124 ..B1.122 ..B1.120
                                #       ..B1.118 ..B1.116 ..B1.114 ..B1.112 ..B1.110
                                #      
                                # Execution count [5.29e-01]
        movl      $.L_2__STRING.37, %edi                        #538.9
        movl      %r14d, %esi                                   #538.9
        xorl      %eax, %eax                                    #538.9
        movq      320(%rsp), %rdx                               #538.9[spill]
..___tag_value_main.81:
#       printf(const char *__restrict__, ...)
        call      printf                                        #538.9
..___tag_value_main.82:
                                # LOE rbx r12 r13 r14d r15d
..B1.129:                       # Preds ..B1.128
                                # Execution count [5.29e-01]
        movsd     264(%rsp), %xmm0                              #539.9[spill]
        movl      $.L_2__STRING.38, %edi                        #539.9
        movl      $1, %eax                                      #539.9
..___tag_value_main.83:
#       printf(const char *__restrict__, ...)
        call      printf                                        #539.9
..___tag_value_main.84:
                                # LOE rbx r12 r13 r14d r15d
..B1.130:                       # Preds ..B1.129
                                # Execution count [5.29e-01]
        movsd     272(%rsp), %xmm0                              #540.9[spill]
        movl      $.L_2__STRING.39, %edi                        #540.9
        movl      $1, %eax                                      #540.9
..___tag_value_main.85:
#       printf(const char *__restrict__, ...)
        call      printf                                        #540.9
..___tag_value_main.86:
                                # LOE rbx r12 r13 r14d r15d
..B1.131:                       # Preds ..B1.130
                                # Execution count [5.29e-01]
        movsd     280(%rsp), %xmm0                              #541.9[spill]
        movl      $.L_2__STRING.40, %edi                        #541.9
        movl      $1, %eax                                      #541.9
..___tag_value_main.87:
#       printf(const char *__restrict__, ...)
        call      printf                                        #541.9
..___tag_value_main.88:
                                # LOE rbx r12 r13 r14d r15d
..B1.132:                       # Preds ..B1.131
                                # Execution count [5.29e-01]
        movsd     296(%rsp), %xmm0                              #542.9[spill]
        movl      $.L_2__STRING.41, %edi                        #542.9
        movl      $1, %eax                                      #542.9
..___tag_value_main.89:
#       printf(const char *__restrict__, ...)
        call      printf                                        #542.9
..___tag_value_main.90:
                                # LOE rbx r12 r13 r14d r15d
..B1.133:                       # Preds ..B1.132
                                # Execution count [5.29e-01]
        movsd     304(%rsp), %xmm0                              #543.9[spill]
        movl      $.L_2__STRING.42, %edi                        #543.9
        movl      $1, %eax                                      #543.9
..___tag_value_main.91:
#       printf(const char *__restrict__, ...)
        call      printf                                        #543.9
..___tag_value_main.92:
                                # LOE rbx r12 r13 r14d r15d
..B1.134:                       # Preds ..B1.133
                                # Execution count [5.29e-01]
        pxor      %xmm1, %xmm1                                  #544.9
        cvtsi2sdq %rbx, %xmm1                                   #544.9
        testq     %rbx, %rbx                                    #544.9
        jge       ..B1.174      # Prob 70%                      #544.9
                                # LOE rbx r12 r13 r14d r15d xmm1
..B1.175:                       # Preds ..B1.134
                                # Execution count [1.59e-01]
        movq      %rbx, %rax                                    #544.9
        pxor      %xmm1, %xmm1                                  #544.9
        shrq      $1, %rbx                                      #544.9
        andq      $1, %rax                                      #544.9
        orq       %rbx, %rax                                    #544.9
        cvtsi2sdq %rax, %xmm1                                   #544.9
        addsd     %xmm1, %xmm1                                  #544.9
                                # LOE r12 r13 r14d r15d xmm1
..B1.174:                       # Preds ..B1.175 ..B1.134
                                # Execution count [5.29e-01]
        divsd     296(%rsp), %xmm1                              #544.9[spill]
        movsd     280(%rsp), %xmm0                              #544.9[spill]
        movl      $.L_2__STRING.43, %edi                        #544.9
        divsd     %xmm1, %xmm0                                  #544.9
        mulsd     .L_2il0floatpacket.0(%rip), %xmm0             #544.9
        movl      $1, %eax                                      #544.9
..___tag_value_main.93:
#       printf(const char *__restrict__, ...)
        call      printf                                        #544.9
..___tag_value_main.94:
                                # LOE r12 r13 r14d r15d
..B1.135:                       # Preds ..B1.174
                                # Execution count [5.29e-01]
        movsd     288(%rsp), %xmm0                              #545.9[spill]
        movl      $.L_2__STRING.44, %edi                        #545.9
        movl      $1, %eax                                      #545.9
..___tag_value_main.95:
#       printf(const char *__restrict__, ...)
        call      printf                                        #545.9
..___tag_value_main.96:
                                # LOE r12 r13 r14d r15d
..B1.136:                       # Preds ..B1.135
                                # Execution count [5.29e-01]
        movsd     288(%rsp), %xmm0                              #546.9[spill]
        movl      $.L_2__STRING.45, %edi                        #546.9
        divsd     280(%rsp), %xmm0                              #546.9[spill]
        movl      $1, %eax                                      #546.9
..___tag_value_main.97:
#       printf(const char *__restrict__, ...)
        call      printf                                        #546.9
..___tag_value_main.98:
                                # LOE r12 r13 r14d r15d
..B1.137:                       # Preds ..B1.136
                                # Execution count [5.29e-01]
        movl      $.L_2__STRING.46, %edi                        #547.9
        movq      %r13, %rsi                                    #547.9
        xorl      %eax, %eax                                    #547.9
..___tag_value_main.99:
#       printf(const char *__restrict__, ...)
        call      printf                                        #547.9
..___tag_value_main.100:
                                # LOE r12 r14d r15d
..B1.138:                       # Preds ..B1.137
                                # Execution count [5.29e-01]
        movl      $.L_2__STRING.47, %edi                        #548.9
        movq      %r12, %rsi                                    #548.9
        xorl      %eax, %eax                                    #548.9
..___tag_value_main.101:
#       printf(const char *__restrict__, ...)
        call      printf                                        #548.9
..___tag_value_main.102:
                                # LOE r14d r15d
..B1.139:                       # Preds ..B1.138
                                # Execution count [5.29e-01]
        movq      240(%rsp), %rsi                               #551.29
        pxor      %xmm0, %xmm0                                  #553.9
        cvtsi2sdq %rsi, %xmm0                                   #553.9
        movsd     296(%rsp), %xmm1                              #553.9[spill]
        lea       (%rsi,%rsi,4), %rax                           #553.9
        divsd     %xmm1, %xmm0                                  #553.9
        movsd     .L_2il0floatpacket.1(%rip), %xmm2             #553.9
        mulsd     %xmm1, %xmm2                                  #553.9
        pxor      %xmm1, %xmm1                                  #553.9
        divsd     .L_2il0floatpacket.0(%rip), %xmm0             #553.9
        shlq      $3, %rax                                      #553.9
        cvtsi2sdq %rax, %xmm1                                   #553.9
        mulsd     272(%rsp), %xmm2                              #553.9[spill]
        divsd     %xmm2, %xmm1                                  #553.9
        pxor      %xmm2, %xmm2                                  #553.9
        movq      n_iterations(%rip), %rax                      #553.9
        cvtsi2sdq %rax, %xmm2                                   #553.9
        testq     %rax, %rax                                    #553.9
        jge       ..B1.176      # Prob 70%                      #553.9
                                # LOE rax rsi r14d r15d xmm0 xmm1 xmm2
..B1.177:                       # Preds ..B1.139
                                # Execution count [1.59e-01]
        movq      %rax, %rcx                                    #553.9
        pxor      %xmm2, %xmm2                                  #553.9
        shrq      $1, %rax                                      #553.9
        andq      $1, %rcx                                      #553.9
        orq       %rax, %rcx                                    #553.9
        cvtsi2sdq %rcx, %xmm2                                   #553.9
        addsd     %xmm2, %xmm2                                  #553.9
                                # LOE rsi r14d r15d xmm0 xmm1 xmm2
..B1.176:                       # Preds ..B1.177 ..B1.139
                                # Execution count [5.29e-01]
        divsd     296(%rsp), %xmm2                              #553.9[spill]
        movl      $.L_2__STRING.48, %edi                        #553.9
        movl      $3, %eax                                      #553.9
..___tag_value_main.103:
#       printf(const char *__restrict__, ...)
        call      printf                                        #553.9
..___tag_value_main.104:
                                # LOE r14d r15d
..B1.140:                       # Preds ..B1.176
                                # Execution count [5.29e-01]
        pxor      %xmm0, %xmm0                                  #554.2
        pxor      %xmm1, %xmm1                                  #554.2
        movq      256(%rsp), %rsi                               #554.60
        movl      $.L_2__STRING.49, %edi                        #554.2
        cvtsi2sdq 56(%rsp), %xmm0                               #554.2
        cvtsi2sdq %rsi, %xmm1                                   #554.2
        divsd     %xmm1, %xmm0                                  #554.2
        movl      $1, %eax                                      #554.2
..___tag_value_main.105:
#       printf(const char *__restrict__, ...)
        call      printf                                        #554.2
..___tag_value_main.106:
                                # LOE r14d r15d
..B1.141:                       # Preds ..B1.140
                                # Execution count [5.29e-01]
        movl      $il0_peep_printf_format_3, %edi               #555.2
        call      puts                                          #555.2
                                # LOE r14d r15d
..B1.142:                       # Preds ..B1.141
                                # Execution count [5.29e-01]
        movl      $1, %edi                                      #556.9
#       sleep(unsigned int)
        call      sleep                                         #556.9
                                # LOE r14d r15d
..B1.143:                       # Preds ..B1.142
                                # Execution count [5.29e-01]
        incl      %r14d                                         #458.42
        cmpl      $9, %r14d                                     #458.31
        jb        ..B1.41       # Prob 82%                      #458.31
                                # LOE r14d r15d
..B1.144:                       # Preds ..B1.143
                                # Execution count [1.05e-01]
        movl      $2400000, %edi                                #558.5
        movq      344(%rsp), %r13                               #[spill]
        movq      336(%rsp), %r12                               #[spill]
..___tag_value_main.107:
#       ear_cpufreq_set_node(unsigned long)
        call      ear_cpufreq_set_node                          #558.5
..___tag_value_main.108:
                                # LOE r12 r13
..B1.145:                       # Preds ..B1.144
                                # Execution count [1.05e-01]
        xorl      %eax, %eax                                    #560.5
..___tag_value_main.109:
#       ear_cpufreq_end()
        call      ear_cpufreq_end                               #560.5
..___tag_value_main.110:
                                # LOE r12 r13
..B1.146:                       # Preds ..B1.145
                                # Execution count [1.05e-01]
        movq      %r12, %rdi                                    #562.5
#       free(void *)
        call      free                                          #562.5
                                # LOE r13
..B1.147:                       # Preds ..B1.146
                                # Execution count [1.05e-01]
        movq      %r13, %rdi                                    #563.5
#       free(void *)
        call      free                                          #563.5
                                # LOE
..B1.148:                       # Preds ..B1.147
                                # Execution count [1.05e-01]
        xorl      %eax, %eax                                    #564.12
        addq      $472, %rsp                                    #564.12
	.cfi_restore 3
        popq      %rbx                                          #564.12
	.cfi_restore 15
        popq      %r15                                          #564.12
	.cfi_restore 14
        popq      %r14                                          #564.12
	.cfi_restore 13
        popq      %r13                                          #564.12
	.cfi_restore 12
        popq      %r12                                          #564.12
        movq      %rbp, %rsp                                    #564.12
        popq      %rbp                                          #564.12
	.cfi_def_cfa 7, 8
	.cfi_restore 6
        ret                                                     #564.12
	.cfi_def_cfa 6, 16
	.cfi_escape 0x10, 0x03, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xd8, 0xff, 0xff, 0xff, 0x22
	.cfi_offset 6, -16
	.cfi_escape 0x10, 0x0c, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0d, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xf0, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0e, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe8, 0xff, 0xff, 0xff, 0x22
	.cfi_escape 0x10, 0x0f, 0x0e, 0x38, 0x1c, 0x0d, 0x80, 0xff, 0xff, 0xff, 0x1a, 0x0d, 0xe0, 0xff, 0xff, 0xff, 0x22
                                # LOE
..B1.149:                       # Preds ..B1.59
                                # Execution count [1.32e-01]: Infreq
        movl      $il0_peep_printf_format_4, %edi               #491.9
        call      puts                                          #491.9
        jmp       ..B1.8        # Prob 100%                     #491.9
                                # LOE
..B1.151:                       # Preds ..B1.57
                                # Execution count [1.65e-01]: Infreq
        movl      $il0_peep_printf_format_5, %edi               #490.9
        call      puts                                          #490.9
        jmp       ..B1.8        # Prob 100%                     #490.9
                                # LOE
..B1.153:                       # Preds ..B1.48
                                # Execution count [2.07e-01]: Infreq
        movl      $il0_peep_printf_format_6, %edi               #476.9
        call      puts                                          #476.9
        jmp       ..B1.8        # Prob 100%                     #476.9
                                # LOE
..B1.155:                       # Preds ..B1.44
                                # Execution count [2.30e-01]: Infreq
        movl      %eax, %ebx                                    #
#       __errno_location(void)
        call      __errno_location                              #471.73
                                # LOE rax ebx
..B1.194:                       # Preds ..B1.155
                                # Execution count [2.30e-01]: Infreq
        movl      (%rax), %edi                                  #471.64
#       strerror(int)
        call      strerror                                      #471.64
                                # LOE rax ebx
..B1.193:                       # Preds ..B1.194
                                # Execution count [2.30e-01]: Infreq
        movl      $.L_2__STRING.33, %edi                        #471.17
        movq      %rax, %rsi                                    #471.17
        xorl      %eax, %eax                                    #471.17
..___tag_value_main.125:
#       printf(const char *__restrict__, ...)
        call      printf                                        #471.17
..___tag_value_main.126:
                                # LOE ebx
..B1.156:                       # Preds ..B1.193
                                # Execution count [2.30e-01]: Infreq
        movl      %ebx, %edi                                    #472.17
#       exit(int)
        call      exit                                          #472.17
                                # LOE
..B1.157:                       # Preds ..B1.24
                                # Execution count [1.60e-01]: Infreq
        movl      $il0_peep_printf_format_7, %edi               #439.5
        call      puts                                          #439.5
        jmp       ..B1.8        # Prob 100%                     #439.5
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	main,@function
	.size	main,.-main
	.section .rodata, "a"
	.align 8
	.align 8
.2.35_2.switchtab.16:
	.quad	..1.14_0.TAG.0.0.14.28
	.quad	..1.14_0.TAG.0.0.14.28
	.quad	..1.14_0.TAG.4.0.14.28
	.quad	..1.14_0.TAG.4.0.14.28
	.quad	..1.14_0.TAG.4.0.14.28
	.quad	..1.14_0.TAG.7.0.14.28
	.quad	..1.14_0.TAG.7.0.14.28
	.quad	..1.14_0.TAG.7.0.14.28
	.quad	..1.14_0.TAG.8.0.14.28
	.align 8
.2.35_2.switchtab.17:
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.quad	..1.14_0.TAG.8.0.14.29
	.align 8
.2.35_2.switchtab.18:
	.quad	..1.13_0.TAG.0.0.13.32
	.quad	..1.13_0.TAG.1.0.13.32
	.quad	..1.13_0.TAG.2.0.13.32
	.quad	..1.13_0.TAG.3.0.13.32
	.quad	..1.13_0.TAG.4.0.13.32
	.quad	..1.13_0.TAG.5.0.13.32
	.quad	..1.13_0.TAG.6.0.13.32
	.quad	..1.13_0.TAG.7.0.13.32
	.quad	..1.13_0.TAG.8.0.13.32
	.section .rodata.str1.4, "aMS",@progbits,1
	.align 4
	.align 4
il0_peep_printf_format_9:
	.long	1601052717
	.long	1801678707
	.long	980644965
	.long	1836412448
	.long	544367970
	.long	1931503215
	.long	1701536623
	.long	1763734388
	.long	1752440942
	.long	1869488229
	.word	25956
	.byte	0
	.space 1, 0x00 	# pad
	.align 4
il0_peep_printf_format_10:
	.long	1601052717
	.long	1701996660
	.long	980640865
	.long	1919448096
	.long	1935958373
	.long	544175136
	.long	1634038371
	.long	1629513076
	.long	1646290030
	.long	6581865
	.align 4
il0_peep_printf_format_12:
	.long	1935876141
	.long	1881160310
	.long	1953393010
	.long	1953853216
	.long	544503152
	.long	1663069801
	.long	1713403507
	.long	1634562671
	.long	807936116
	.long	2699564
	.align 4
il0_peep_printf_format_0:
	.long	757935405
	.long	757935405
	.long	757935405
	.long	757935405
	.long	757935405
	.long	757935405
	.long	1461726509
	.long	1768649327
	.word	26478
	.byte	0
	.space 1, 0x00 	# pad
	.align 4
il0_peep_printf_format_1:
	.long	757935405
	.long	757935405
	.long	757935405
	.long	757935405
	.long	757935405
	.long	757935405
	.long	1394617645
	.long	1634561397
	.word	31090
	.byte	0
	.space 1, 0x00 	# pad
	.align 4
il0_peep_printf_format_2:
	.long	1635020628
	.long	1970151532
	.long	1886330989
	.word	115
	.space 2, 0x00 	# pad
	.align 4
il0_peep_printf_format_3:
	.long	1600085855
	.long	6250335
	.align 4
il0_peep_printf_format_4:
	.long	1280328018
	.long	1936028192
	.long	1696625765
	.long	1953391990
	.long	1634082931
	.long	1684368489
	.byte	0
	.space 3, 0x00 	# pad
	.align 4
il0_peep_printf_format_5:
	.long	1280328018
	.long	1869902624
	.long	1986338928
	.long	1937010277
	.long	1767990816
	.long	6579564
	.align 4
il0_peep_printf_format_6:
	.long	1280328018
	.long	1635021600
	.long	1696625778
	.long	1953391990
	.long	1634082931
	.long	1684368489
	.byte	0
	.space 3, 0x00 	# pad
	.align 4
il0_peep_printf_format_7:
	.long	1280328018
	.long	1768843552
	.long	1818323316
	.long	1952545385
	.long	544108393
	.long	1818845542
	.word	25701
	.byte	0
	.section .rodata.str1.32, "aMS",@progbits,1
	.align 32
	.align 32
il0_peep_printf_format_8:
	.long	1734439765
	.long	1931491941
	.long	1600417129
	.long	1702326128
	.long	1701666674
	.long	1667854964
	.long	1601052787
	.long	1801678707
	.long	544437349
	.long	1752457070
	.long	1684104562
	.long	1601052787
	.long	1919251561
	.long	1869182049
	.long	1663071086
	.word	30323
	.byte	0
	.space 1, 0x00 	# pad
	.align 32
il0_peep_printf_format_11:
	.long	1601052717
	.long	1919251561
	.long	1869182049
	.long	540701550
	.long	1651340654
	.long	1864397413
	.long	1601052774
	.long	1919251561
	.long	1869182049
	.long	1948283758
	.long	1634148463
	.long	1919248500
	.long	1701733664
	.long	544827250
	.long	1920230765
	.long	7562089
	.data
# -- End  main
	.text
# -- Begin  run_test
	.text
# mark_begin;
       .align    16,0x90
# --- run_test(int)
run_test:
# parameter 1: %edi
..B2.1:                         # Preds ..B2.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_run_test.128:
..L129:
                                                        #314.1
        movl      test(%rip), %eax                              #315.12
        cmpq      $8, %rax                                      #315.5
        ja        ..B2.13       # Prob 50%                      #315.5
                                # LOE rax rbx rbp r12 r13 r14 r15
..B2.2:                         # Preds ..B2.1
                                # Execution count [5.00e-01]
        jmp       *.2.36_2.switchtab.1(,%rax,8)                 #315.5
                                # LOE rbx rbp r12 r13 r14 r15
..1.12_0.TAG.8.0.12:
..1.12_0.TAG.7.0.12:
..1.12_0.TAG.6.0.12:
..1.12_0.TAG.5.0.12:
..1.12_0.TAG.4.0.12:
..1.12_0.TAG.3.0.12:
..1.12_0.TAG.2.0.12:
..1.12_0.TAG.1.0.12:
..1.12_0.TAG.0.0.12:
..B2.13:                        # Preds ..B2.2 ..B2.2 ..B2.2 ..B2.2 ..B2.2
                                #       ..B2.2 ..B2.2 ..B2.2 ..B2.2 ..B2.1
                                #      
                                # Execution count [1.00e+00]
        ret                                                     #328.1
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	run_test,@function
	.size	run_test,.-run_test
	.section .rodata, "a"
	.align 8
.2.36_2.switchtab.1:
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.quad	..1.12_0.TAG.8.0.12
	.data
# -- End  run_test
	.text
# -- Begin  get_test_name
	.text
# mark_begin;
       .align    16,0x90
	.globl get_test_name
# --- get_test_name(int)
get_test_name:
# parameter 1: %edi
..B3.1:                         # Preds ..B3.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_get_test_name.131:
..L132:
                                                        #331.1
        cmpl      $8, %edi                                      #332.5
        ja        ..B3.21       # Prob 28%                      #332.5
                                # LOE rbx rbp r12 r13 r14 r15 edi
..B3.2:                         # Preds ..B3.1
                                # Execution count [7.20e-01]
        movl      %edi, %edi                                    #331.1
        jmp       *.2.37_2.switchtab.1(,%rdi,8)                 #332.5
                                # LOE rbx rbp r12 r13 r14 r15
..1.13_0.TAG.8.0.13:
..B3.4:                         # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.10, %eax                        #342.24
        ret                                                     #342.24
                                # LOE
..1.13_0.TAG.7.0.13:
..B3.6:                         # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.9, %eax                         #341.24
        ret                                                     #341.24
                                # LOE
..1.13_0.TAG.6.0.13:
..B3.8:                         # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.8, %eax                         #340.24
        ret                                                     #340.24
                                # LOE
..1.13_0.TAG.5.0.13:
..B3.10:                        # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.7, %eax                         #338.24
        ret                                                     #338.24
                                # LOE
..1.13_0.TAG.4.0.13:
..B3.12:                        # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.6, %eax                         #337.24
        ret                                                     #337.24
                                # LOE
..1.13_0.TAG.3.0.13:
..B3.14:                        # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.5, %eax                         #336.24
        ret                                                     #336.24
                                # LOE
..1.13_0.TAG.2.0.13:
..B3.16:                        # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.4, %eax                         #335.24
        ret                                                     #335.24
                                # LOE
..1.13_0.TAG.1.0.13:
..B3.18:                        # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.3, %eax                         #334.24
        ret                                                     #334.24
                                # LOE
..1.13_0.TAG.0.0.13:
..B3.20:                        # Preds ..B3.2
                                # Execution count [8.00e-02]
        movl      $.L_2__STRING.2, %eax                         #333.24
        ret                                                     #333.24
                                # LOE
..B3.21:                        # Preds ..B3.1
                                # Execution count [2.80e-01]
        ret                                                     #345.1
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	get_test_name,@function
	.size	get_test_name,.-get_test_name
	.section .rodata, "a"
	.align 8
.2.37_2.switchtab.1:
	.quad	..1.13_0.TAG.0.0.13
	.quad	..1.13_0.TAG.1.0.13
	.quad	..1.13_0.TAG.2.0.13
	.quad	..1.13_0.TAG.3.0.13
	.quad	..1.13_0.TAG.4.0.13
	.quad	..1.13_0.TAG.5.0.13
	.quad	..1.13_0.TAG.6.0.13
	.quad	..1.13_0.TAG.7.0.13
	.quad	..1.13_0.TAG.8.0.13
	.data
# -- End  get_test_name
	.text
# -- Begin  get_test_ops
	.text
# mark_begin;
       .align    16,0x90
	.globl get_test_ops
# --- get_test_ops(int)
get_test_ops:
# parameter 1: %edi
..B4.1:                         # Preds ..B4.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_get_test_ops.134:
..L135:
                                                        #348.1
        cmpl      $8, %edi                                      #349.5
        ja        ..B4.21       # Prob 28%                      #349.5
                                # LOE rbx rbp r12 r13 r14 r15 edi
..B4.2:                         # Preds ..B4.1
                                # Execution count [7.20e-01]
        movl      %edi, %edi                                    #348.1
        jmp       *.2.38_2.switchtab.1(,%rdi,8)                 #349.5
                                # LOE rbx rbp r12 r13 r14 r15
..1.14_0.TAG.8.0.14:
..B4.4:                         # Preds ..B4.2
                                # Execution count [8.00e-02]
        movl      $16, %eax                                     #359.24
        ret                                                     #359.24
                                # LOE
..1.14_0.TAG.7.0.14:
..1.14_0.TAG.6.0.14:
..1.14_0.TAG.5.0.14:
..B4.6:                         # Preds ..B4.2 ..B4.2 ..B4.2
                                # Execution count [8.00e-02]
        movl      $8, %eax                                      #358.24
        ret                                                     #358.24
                                # LOE
..1.14_0.TAG.4.0.14:
..1.14_0.TAG.3.0.14:
..1.14_0.TAG.2.0.14:
..B4.12:                        # Preds ..B4.2 ..B4.2 ..B4.2
                                # Execution count [8.00e-02]
        movl      $4, %eax                                      #354.24
        ret                                                     #354.24
                                # LOE
..1.14_0.TAG.1.0.14:
..1.14_0.TAG.0.0.14:
..B4.18:                        # Preds ..B4.2 ..B4.2
                                # Execution count [8.00e-02]
        movl      $2, %eax                                      #351.24
        ret                                                     #351.24
                                # LOE
..B4.21:                        # Preds ..B4.1
                                # Execution count [2.80e-01]
        ret                                                     #362.1
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	get_test_ops,@function
	.size	get_test_ops,.-get_test_ops
	.section .rodata, "a"
	.align 8
.2.38_2.switchtab.1:
	.quad	..1.14_0.TAG.1.0.14
	.quad	..1.14_0.TAG.1.0.14
	.quad	..1.14_0.TAG.4.0.14
	.quad	..1.14_0.TAG.4.0.14
	.quad	..1.14_0.TAG.4.0.14
	.quad	..1.14_0.TAG.7.0.14
	.quad	..1.14_0.TAG.7.0.14
	.quad	..1.14_0.TAG.7.0.14
	.quad	..1.14_0.TAG.8.0.14
	.data
# -- End  get_test_ops
	.text
# -- Begin  threaded_run_test
	.text
# mark_begin;
       .align    16,0x90
# --- threaded_run_test(void *)
threaded_run_test:
# parameter 1: %rdi
..B5.1:                         # Preds ..B5.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_threaded_run_test.137:
..L138:
                                                        #365.1
        subq      $136, %rsp                                    #365.1
	.cfi_def_cfa_offset 144
        movl      $128, %eax                                    #367.5
        movslq    (%rdi), %rcx                                  #366.26
        pxor      %xmm0, %xmm0                                  #367.5
                                # LOE rax rcx rbx rbp r12 r13 r14 r15 xmm0
..B5.13:                        # Preds ..B5.1 ..B5.13
                                # Execution count [2.00e+00]
        movups    %xmm0, -16(%rsp,%rax)                         #367.5
        movups    %xmm0, -32(%rsp,%rax)                         #367.5
        movups    %xmm0, -48(%rsp,%rax)                         #367.5
        movups    %xmm0, -64(%rsp,%rax)                         #367.5
        subq      $64, %rax                                     #367.5
        jne       ..B5.13       # Prob 50%                      #367.5
                                # LOE rax rcx rbx rbp r12 r13 r14 r15 xmm0
..B5.2:                         # Preds ..B5.13
                                # Execution count [1.00e+00]
        cmpq      $1024, %rcx                                   #367.5
        jae       ..B5.4        # Prob 50%                      #367.5
                                # LOE rcx rbx rbp r12 r13 r14 r15
..B5.3:                         # Preds ..B5.2
                                # Execution count [5.00e-01]
        movq      %rcx, %rdx                                    #367.5
        movl      $1, %eax                                      #367.5
        shrq      $6, %rdx                                      #367.5
        shlq      %cl, %rax                                     #367.5
        orq       %rax, (%rsp,%rdx,8)                           #367.5
                                # LOE rbx rbp r12 r13 r14 r15
..B5.4:                         # Preds ..B5.2 ..B5.3
                                # Execution count [1.00e+00]
        xorl      %edi, %edi                                    #367.5
        movl      $128, %esi                                    #367.5
        lea       (%rsp), %rdx                                  #367.5
..___tag_value_threaded_run_test.140:
#       sched_setaffinity(__pid_t, size_t, const cpu_set_t *)
        call      sched_setaffinity                             #367.5
..___tag_value_threaded_run_test.141:
                                # LOE rbx rbp r12 r13 r14 r15
..B5.5:                         # Preds ..B5.4
                                # Execution count [1.00e+00]
        cmpl      $1, spinning(%rip)                            #369.24
        jne       ..B5.8        # Prob 10%                      #369.24
                                # LOE rbx rbp r12 r13 r14 r15
..B5.7:                         # Preds ..B5.5 ..B5.7
                                # Execution count [0.00e+00]
        jmp       ..B5.7        # Prob 100%                     #
                                # LOE
..B5.8:                         # Preds ..B5.5
                                # Execution count [1.00e+00]: Infreq
        movl      test(%rip), %edi                              #370.5
..___tag_value_threaded_run_test.142:
#       run_test(int)
        call      run_test                                      #370.5
..___tag_value_threaded_run_test.143:
                                # LOE rbx rbp r12 r13 r14 r15
..B5.9:                         # Preds ..B5.8
                                # Execution count [1.00e+00]: Infreq
        xorl      %edi, %edi                                    #372.5
..___tag_value_threaded_run_test.144:
#       pthread_exit(void *)
        call      pthread_exit                                  #372.5
..___tag_value_threaded_run_test.145:
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	threaded_run_test,@function
	.size	threaded_run_test,.-threaded_run_test
	.data
# -- End  threaded_run_test
	.text
# -- Begin  usage
	.text
# mark_begin;
       .align    16,0x90
	.globl usage
# --- usage()
usage:
..B6.1:                         # Preds ..B6.0
                                # Execution count [1.00e+00]
	.cfi_startproc
..___tag_value_usage.147:
..L148:
                                                        #376.1
        pushq     %rsi                                          #376.1
	.cfi_def_cfa_offset 16
        movl      $il0_peep_printf_format_13, %edi              #377.5
        call      puts                                          #377.5
                                # LOE
..B6.2:                         # Preds ..B6.1
                                # Execution count [1.00e+00]
        movl      $il0_peep_printf_format_14, %edi              #378.5
        call      puts                                          #378.5
                                # LOE
..B6.3:                         # Preds ..B6.2
                                # Execution count [1.00e+00]
        movl      $il0_peep_printf_format_15, %edi              #379.5
        call      puts                                          #379.5
                                # LOE
..B6.4:                         # Preds ..B6.3
                                # Execution count [1.00e+00]
        movl      $il0_peep_printf_format_16, %edi              #380.5
        call      puts                                          #380.5
                                # LOE
..B6.5:                         # Preds ..B6.4
                                # Execution count [1.00e+00]
        movl      $il0_peep_printf_format_17, %edi              #381.5
        call      puts                                          #381.5
                                # LOE
..B6.6:                         # Preds ..B6.5
                                # Execution count [1.00e+00]
        movl      $1, %edi                                      #382.5
#       exit(int)
        call      exit                                          #382.5
        .align    16,0x90
                                # LOE
	.cfi_endproc
# mark_end;
	.type	usage,@function
	.size	usage,.-usage
	.section .rodata.str1.4, "aMS",@progbits,1
	.space 1, 0x00 	# pad
	.align 4
il0_peep_printf_format_14:
	.long	1601052717
	.long	1801678707
	.long	980644965
	.long	1836412448
	.long	544367970
	.long	1931503215
	.long	1701536623
	.long	1763734388
	.long	1752440942
	.long	1869488229
	.word	25956
	.byte	0
	.space 1, 0x00 	# pad
	.align 4
il0_peep_printf_format_15:
	.long	1601052717
	.long	1701996660
	.long	980640865
	.long	1919448096
	.long	1935958373
	.long	544175136
	.long	1634038371
	.long	1629513076
	.long	1646290030
	.long	6581865
	.align 4
il0_peep_printf_format_17:
	.long	1935876141
	.long	1881160310
	.long	1953393010
	.long	1953853216
	.long	544503152
	.long	1663069801
	.long	1713403507
	.long	1634562671
	.long	807936116
	.long	2699564
	.section .rodata.str1.32, "aMS",@progbits,1
	.align 32
il0_peep_printf_format_13:
	.long	1734439765
	.long	1931491941
	.long	1600417129
	.long	1702326128
	.long	1701666674
	.long	1667854964
	.long	1601052787
	.long	1801678707
	.long	544437349
	.long	1752457070
	.long	1684104562
	.long	1601052787
	.long	1919251561
	.long	1869182049
	.long	1663071086
	.word	30323
	.byte	0
	.space 1, 0x00 	# pad
	.align 32
il0_peep_printf_format_16:
	.long	1601052717
	.long	1919251561
	.long	1869182049
	.long	540701550
	.long	1651340654
	.long	1864397413
	.long	1601052774
	.long	1919251561
	.long	1869182049
	.long	1948283758
	.long	1634148463
	.long	1919248500
	.long	1701733664
	.long	544827250
	.long	1920230765
	.long	7562089
	.data
# -- End  usage
	.bss
	.align 8
	.align 8
n_iterations:
	.type	n_iterations,@object
	.size	n_iterations,8
	.space 8	# pad
	.align 8
	.globl my_omp_get_max_threads
my_omp_get_max_threads:
	.type	my_omp_get_max_threads,@object
	.size	my_omp_get_max_threads,8
	.space 8	# pad
	.align 4
	.globl ear_papi_init
ear_papi_init:
	.type	ear_papi_init,@object
	.size	ear_papi_init,4
	.space 4	# pad
	.align 4
	.globl test
test:
	.type	test,@object
	.size	test,4
	.space 4	# pad
	.data
	.align 4
	.align 4
spinning:
	.long	1
	.type	spinning,@object
	.size	spinning,4
	.align 4
	.globl EAR_VERBOSE_LEVEL
EAR_VERBOSE_LEVEL:
	.long	4
	.type	EAR_VERBOSE_LEVEL,@object
	.size	EAR_VERBOSE_LEVEL,4
	.section .rodata, "a"
	.align 8
.L_2il0floatpacket.0:
	.long	0x00000000,0x412e8480
	.type	.L_2il0floatpacket.0,@object
	.size	.L_2il0floatpacket.0,8
	.align 8
.L_2il0floatpacket.1:
	.long	0x00000000,0x41cdcd65
	.type	.L_2il0floatpacket.1,@object
	.size	.L_2il0floatpacket.1,8
	.section .rodata.str1.4, "aMS",@progbits,1
	.align 4
.L_2__STRING.16:
	.long	544566309
	.long	175467557
	.byte	0
	.type	.L_2__STRING.16,@object
	.size	.L_2__STRING.16,9
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.17:
	.long	542462019
	.long	1701080909
	.long	153107052
	.long	174662921
	.byte	0
	.type	.L_2__STRING.17,@object
	.size	.L_2__STRING.17,17
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.18:
	.long	1852732754
	.long	543649385
	.long	1663069801
	.long	979726959
	.long	1680148768
	.long	174335279
	.byte	0
	.type	.L_2__STRING.18,@object
	.size	.L_2__STRING.18,25
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.22:
	.long	1953719668
	.long	1835101728
	.word	15205
	.byte	0
	.type	.L_2__STRING.22,@object
	.size	.L_2__STRING.22,11
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.23:
	.long	1635020660
	.long	1634738284
	.long	1734437731
	.long	1852121189
	.long	2036822629
	.long	692725792
	.word	59
	.type	.L_2__STRING.23,@object
	.size	.L_2__STRING.23,26
	.space 2, 0x00 	# pad
	.align 4
.L_2__STRING.24:
	.long	778532417
	.long	2003791904
	.long	1881174629
	.long	1881174629
	.long	1936157539
	.long	1634740256
	.long	1734437731
	.long	1881878629
	.long	1634427745
	.long	691103079
	.long	992565032
	.byte	0
	.type	.L_2__STRING.24,@object
	.size	.L_2__STRING.24,45
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.25:
	.long	778532417
	.long	2003791904
	.long	1881174629
	.long	1663070821
	.long	543519343
	.long	992565032
	.byte	0
	.type	.L_2__STRING.25,@object
	.size	.L_2__STRING.25,25
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.26:
	.long	1667594341
	.long	1769218094
	.long	673211757
	.long	3877235
	.type	.L_2__STRING.26,@object
	.size	.L_2__STRING.26,16
	.align 4
.L_2__STRING.27:
	.long	1701998435
	.long	1701733664
	.long	544827250
	.long	992561704
	.byte	0
	.type	.L_2__STRING.27,@object
	.size	.L_2__STRING.27,17
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.28:
	.long	1953721961
	.long	1869619246
	.long	544367991
	.long	693597480
	.word	59
	.type	.L_2__STRING.28,@object
	.size	.L_2__STRING.28,18
	.space 2, 0x00 	# pad
	.align 4
.L_2__STRING.29:
	.long	1869375053
	.long	3896176
	.type	.L_2__STRING.29,@object
	.size	.L_2__STRING.29,8
	.align 4
.L_2__STRING.30:
	.long	1869375053
	.long	1462727536
	.long	997487713
	.byte	0
	.type	.L_2__STRING.30,@object
	.size	.L_2__STRING.30,13
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.32:
	.long	1667594309
	.long	1852404853
	.long	1680154727
	.long	1936028704
	.long	684916
	.type	.L_2__STRING.32,@object
	.size	.L_2__STRING.32,20
	.align 4
.L_2__STRING.10:
	.long	760763969
	.long	1177694517
	.long	542131232
	.long	1145130310
	.long	842085700
	.byte	0
	.type	.L_2__STRING.10,@object
	.size	.L_2__STRING.10,21
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.9:
	.long	760763969
	.long	1177694517
	.long	542131232
	.long	894195021
	.word	12849
	.byte	0
	.type	.L_2__STRING.9,@object
	.size	.L_2__STRING.9,19
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.8:
	.long	760763969
	.long	1177694517
	.long	542131232
	.long	893666369
	.word	12849
	.byte	0
	.type	.L_2__STRING.8,@object
	.size	.L_2__STRING.8,19
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.7:
	.long	541150534
	.long	1176522820
	.long	1145323853
	.long	3552562
	.type	.L_2__STRING.7,@object
	.size	.L_2__STRING.7,16
	.align 4
.L_2__STRING.6:
	.long	542660161
	.long	1293963332
	.long	892488789
	.word	54
	.type	.L_2__STRING.6,@object
	.size	.L_2__STRING.6,14
	.space 2, 0x00 	# pad
	.align 4
.L_2__STRING.5:
	.long	542660161
	.long	1092636740
	.long	892486724
	.word	54
	.type	.L_2__STRING.5,@object
	.size	.L_2__STRING.5,14
	.space 2, 0x00 	# pad
	.align 4
.L_2__STRING.4:
	.long	541150534
	.long	1176522820
	.long	1145323853
	.long	3682865
	.type	.L_2__STRING.4,@object
	.size	.L_2__STRING.4,16
	.align 4
.L_2__STRING.3:
	.long	843404115
	.long	542131232
	.long	827086157
	.word	14386
	.byte	0
	.type	.L_2__STRING.3,@object
	.size	.L_2__STRING.3,15
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.2:
	.long	843404115
	.long	542131232
	.long	826557505
	.word	14386
	.byte	0
	.type	.L_2__STRING.2,@object
	.size	.L_2__STRING.2,15
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.37:
	.long	1600064010
	.long	1415536479
	.long	542397253
	.long	1600398624
	.long	1931829087
	.word	10
	.type	.L_2__STRING.37,@object
	.size	.L_2__STRING.37,22
	.space 2, 0x00 	# pad
	.align 4
.L_2__STRING.38:
	.long	1701725449
	.long	544827250
	.long	539585064
	.long	1815227941
	.word	2662
	.byte	0
	.type	.L_2__STRING.38,@object
	.size	.L_2__STRING.38,19
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.39:
	.long	1735803145
	.long	1867522094
	.long	544367991
	.long	1735091024
	.long	539580200
	.long	1815227941
	.word	2662
	.byte	0
	.type	.L_2__STRING.39,@object
	.size	.L_2__STRING.39,27
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.40:
	.long	1735803145
	.long	1867522094
	.long	544367991
	.long	1701998435
	.long	693577760
	.long	841884960
	.long	681580
	.type	.L_2__STRING.40,@object
	.size	.L_2__STRING.40,28
	.align 4
.L_2__STRING.41:
	.long	1835619337
	.long	695412837
	.long	841884960
	.long	681580
	.type	.L_2__STRING.41,@object
	.size	.L_2__STRING.41,16
	.align 4
.L_2__STRING.42:
	.long	1701725449
	.long	679044978
	.long	1701998435
	.long	1244143657
	.long	774185001
	.long	174484530
	.byte	0
	.type	.L_2__STRING.42,@object
	.size	.L_2__STRING.42,25
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.43:
	.long	2003783689
	.long	1227387493
	.long	695497582
	.long	693597480
	.long	858662176
	.long	681580
	.type	.L_2__STRING.43,@object
	.size	.L_2__STRING.43,24
	.align 4
.L_2__STRING.44:
	.long	1816546569
	.long	678654063
	.long	1701998435
	.long	774185001
	.long	174484531
	.byte	0
	.type	.L_2__STRING.44,@object
	.size	.L_2__STRING.44,21
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.45:
	.long	1816546569
	.long	796094575
	.long	1953784151
	.long	858662176
	.long	681580
	.type	.L_2__STRING.45,@object
	.size	.L_2__STRING.45,20
	.align 4
.L_2__STRING.46:
	.long	1953453065
	.long	1176530017
	.long	1347379024
	.long	1814372435
	.word	2677
	.byte	0
	.type	.L_2__STRING.46,@object
	.size	.L_2__STRING.46,19
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.47:
	.long	1735803145
	.long	1701987886
	.long	1852142961
	.long	622885219
	.long	685420
	.type	.L_2__STRING.47,@object
	.size	.L_2__STRING.47,20
	.align 4
.L_2__STRING.48:
	.long	1346457609
	.long	1279664201
	.long	978538575
	.long	1819026720
	.long	807739509
	.long	1718367022
	.long	1279674144
	.long	1462718543
	.long	980710497
	.long	1718363424
	.long	1701867296
	.long	1769234802
	.long	796094063
	.long	543384947
	.long	174484517
	.byte	0
	.type	.L_2__STRING.48,@object
	.size	.L_2__STRING.48,61
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.49:
	.long	1346457609
	.long	1852383305
	.long	1970435187
	.long	1869182051
	.long	622883694
	.long	544566380
	.long	1668441424
	.long	1635020389
	.long	1629513063
	.long	825587830
	.long	1814372402
	.word	2662
	.byte	0
	.type	.L_2__STRING.49,@object
	.size	.L_2__STRING.49,47
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.33:
	.long	1869771333
	.long	1752637554
	.long	543517801
	.long	1634038371
	.long	1735289204
	.long	1948279072
	.long	1634038376
	.long	623386724
	.long	665971
	.type	.L_2__STRING.33,@object
	.size	.L_2__STRING.33,36
	.data
	.section .note.GNU-stack, ""
// -- Begin DWARF2 SEGMENT .eh_frame
	.section .eh_frame,"a",@progbits
.eh_frame_seg:
	.align 8
# End
