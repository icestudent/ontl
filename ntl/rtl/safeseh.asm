; adds ntl::cxxrecord::catchguardhandler to the safeseh table
; compile with /safeseh

.386
.model flat, stdcall

?catchguardhandler@cxxrecord@ntl@@SA?AW4disposition@exception@nt@2@PAUrecord@452@PAUregistration@452@PAUcontext@52@PAUdispatcher_context@452@@Z PROTO SYSCALL
.safeseh ?catchguardhandler@cxxrecord@ntl@@SA?AW4disposition@exception@nt@2@PAUrecord@452@PAUregistration@452@PAUcontext@52@PAUdispatcher_context@452@@Z

EXTERNDEF ___security_cookie:DWORD, __security_cookie:DWORD, _security_cookie:DWORD
.data?
___security_cookie dd ?
__security_cookie dd ?
_security_cookie dd ?

end
