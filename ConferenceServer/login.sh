#!/usr/bin/expect -f
set timeout 30
spawn ssh -Y spff@140.117.183.100
expect "password:"
send "+-*/Aa26515909\r"
expect "~$*"
interact
