IFNDEF _M_X64
; x86 specific code

; adds ntl::cxxrecord::catchguardhandler to the safeseh table
; compile with /safeseh

.386
.model flat

?catchguardhandler@cxxrecord@cxxruntime@ntl@@SA?AW4disposition@exception@nt@3@PAUrecord@563@PAUregistration@563@PAUcontext@63@PAUdispatcher_context@563@@Z PROTO SYSCALL
.safeseh ?catchguardhandler@cxxrecord@cxxruntime@ntl@@SA?AW4disposition@exception@nt@3@PAUrecord@563@PAUregistration@563@PAUcontext@63@PAUdispatcher_context@563@@Z

;_CxxCatchGuardHandler proto syscall
;.safeseh _CxxCatchGuardHandler

ELSE
; x64 specific code

include ksamd64.inc

cxxmagic = 19930520h



LEAF_ENTRY _NLG_Notify, _TEXT
	mov		[rsp+8*1],rcx
	mov		[rsp+8*2],r8d
	mov		[rsp+8*3],rdx
	mov		r9,cxxmagic
	jmp		__NLG_Dispatch2
LEAF_END _NLG_Notify, _TEXT


LEAF_ENTRY __NLG_Dispatch2, _TEXT
	ret
LEAF_END __NLG_Dispatch2, _TEXT

LEAF_ENTRY __NLG_Return2, _TEXT
	ret
LEAF_END __NLG_Return2, _TEXT




LEAF_ENTRY2 macro Name, Section

 Section segment para 'CODE'
        db      6 dup (0cch)
        align   16

        public  Name
 Name proc frame
endm

save_reg macro Reg, Offset

        mov     [csp+Offset], Reg
        .savereg Reg, cso+Offset
endm
        
LEAF_ENTRY2 _CallSettingFrame, _TEXT

	; save locals
	cso equ 8*3
	csp equ rsp+cso
	alloc_stack cso
		save_reg rcx,8*1 ; action
		save_reg rdx,8*2 ; frame
		save_reg r8 ,8*3 ; param
	END_PROLOGUE

	mov		rdx,[rdx]		; states
	mov		rax,rcx			; action
	call	_NLG_Notify
	call	rax
	call	__NLG_Return2
	mov		rcx,rax
	mov		rdx,[csp+8*2]
	mov		rdx,[rdx]		; states
	mov		r8d,2
	call	_NLG_Notify
	mov		[csp-8],rax

	add		rsp,8*3
	ret

LEAF_END _CallSettingFrame, _TEXT

ENDIF

end