; adds ntl::cxxrecord::catchguardhandler to the safeseh table
; compile with /safeseh

.386
.model flat

?catchguardhandler@cxxrecord@cxxruntime@ntl@@SA?AW4disposition@exception@nt@3@PAUrecord@563@PAUregistration@563@PAUcontext@63@PAUdispatcher_context@563@@Z PROTO SYSCALL
.safeseh ?catchguardhandler@cxxrecord@cxxruntime@ntl@@SA?AW4disposition@exception@nt@3@PAUrecord@563@PAUregistration@563@PAUcontext@63@PAUdispatcher_context@563@@Z

;_CxxCatchGuardHandler proto syscall
;.safeseh _CxxCatchGuardHandler

end
