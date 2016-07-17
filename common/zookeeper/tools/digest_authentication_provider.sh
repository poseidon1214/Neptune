#!/bin/sh

if [ $# -ne 1 ]; then
    echo "Usage:"
    echo "    $0 username:password"
    exit 1
fi

id=$1

pos=`expr index "$id" :`
if [ $pos -eq 0 -o $pos -eq 1 -o $pos -eq "${#id}" ]; then
    echo "Usage:"
    echo "    $0 username:password"
    exit 1
fi

username=${id:0:pos-1}
passwd=${id:pos}
digest=`echo -n "$1" | openssl dgst -binary -sha1 | openssl base64`

echo "$1->$username:$digest"
