## Howto

### Server
```bash
./server listen_port
```
例如 `./server 8080`

### Client
```bash
./client [-n number] [-t timeout] host_1:port_1 host_2:port_2 ...
```
例如 `./client -n 5 -t 3 localhost:8080 127.0.0.1:6666`


連結順利的話，顯示訊息則會顯示如下
### Server
```bash
recv from [client_ip:client_port]
如 `recv from 0.0.0.0:48592
```

### Client
```bash
recv from [server_ip], RTT = [delay] msec
```
如 `recv from 127.0.0.1, RTT=0.183msec`

## BuiltUp & Other

### Server
* 建立好連結後，我讓`addrinfo *c`跑完`clinfo`的linked list，以達成作業描述中的 **receive/send message from multiple client** ，但其實我有點不太確定題意QQ
* server端顯示的訊息數，在我自己的電腦上跑都會遠大於client送出的數量，不知道是什麼原因

### Client
* 沒看到封包大小的規定，因此我以char[32]將封包大小設為32bytes

## Reference 
* [Beej's Guide to Network Programming](http://beej-zhtw.netdpi.net)
* https://www.bogotobogo.com/cplusplus/sockets_server_client.php
* https://opensource.apple.com/source/network_cmds/network_cmds-77/ping.tproj/ping.c