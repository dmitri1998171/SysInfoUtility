#xterm -geometry 60x25 -e ./server 1 1122 tcp & 
xterm -geometry 60x25 -e ./client 127.0.0.1 1122 tcp &
xterm -geometry 60x25+1000+1000 -e ./client 127.0.0.1 1122 tcp &
xterm -geometry 60x25+500+10 -e ./client 127.0.0.1 1122 tcp &
xterm -geometry 60x25 -e ./client 127.0.0.1 1122 tcp &
xterm -geometry 60x25+1000+1000 -e ./client 127.0.0.1 1122 tcp &
xterm -geometry 60x25+500+10 -e ./client 127.0.0.1 1122 tcp &