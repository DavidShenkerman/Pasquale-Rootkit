# rootkit
Steps for finishing this project:

#1. Fix netstat. If i can make this part work, this alone is enough to differenatiate my rootkit from h0mbres.
#2. Hijack an FTP server so I can copy files from the infected computer to mine (My personal touch) https://stackoverflow.com/questions/1658476/c-fopen-vs-open add to the fopen syscall hook. make usre it doesn't open that log file basically. connect to ftp via the root user. this gives us a root backdoor into the entire filesystem through ftp. make sure to add to the netstat stuff as well to make sure our connection is invisible. 
#3. Hijack LSOF (There was something on the blog, in resources about this)
#4 Openssl back connect
#5 Extra Bonus on the Blog
#6 Implement reverse shells
#7 (Maybe add ipv6 but why

