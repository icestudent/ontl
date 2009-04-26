; adds ntl::cxxrecord::catchguardhandler to the safeseh table
; compile with /safeseh

.386
.model flat

?catchguardhandler@cxxrecord@cxxruntime@ntl@@SA?AW4disposition@exception@nt@3@PAUrecord@563@PAUregistration@563@PAUcontext@63@PAUdispatcher_context@563@@Z PROTO SYSCALL
.safeseh ?catchguardhandler@cxxrecord@cxxruntime@ntl@@SA?AW4disposition@exception@nt@3@PAUrecord@563@PAUregistration@563@PAUcontext@63@PAUdispatcher_context@563@@Z

EXTERNDEF ___security_cookie:DWORD
.data?
___security_cookie dd ?

end
