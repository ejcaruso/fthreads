.global context_switch
.text

context_switch:
  pushl %ebp
  movl  %esp, %ebp
  subl  $0xc, %esp
  movl  %ebx, 0x8(%esp)
  movl  %esi, 0x4(%esp)
  movl  %edi, (%esp)
  movl  0x8(%ebp), %eax
  movl  0xc(%ebp), %ecx
  movl  %esp, 0x4(%eax)
  movl  %ebp, (%eax)
  movl  0x4(%ecx), %esp
  movl  (%ecx), %ebp
  movl  (%esp), %edi
  movl  0x4(%esp), %esi
  movl  0x8(%esp), %ebx
  movl  %ebp, %esp
  popl  %ebp
  ret
