-- an implementation of printf

function printf(...)
 trace(string.format(...))
end

printf("Hello %s from %s on %s\n",os.getenv"USER" or "there",_VERSION,os.date())
