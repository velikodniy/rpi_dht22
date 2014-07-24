RPi-DHT22
=========

Dependencies
------------

- libbcm2835 (you can get it by running `make getlibs`)
- libsqlite3-dev
- libmicrohttpd-dev

Compile
-------

    make getlibs
    make
 
Connecting
----------

| Wire   | Pin        |
|:-------|:-----------|
| orange | P4 (+5V)   |
| black  | P6 (GND)   |
| white  | P7 (GPIO4) |

Usage
-----

    ./dht_daemon [-p port] [-d dbpath] [-t interval] [-s]

Options:

-s	Run only HTTP server

-p  Port

-d  Path to database file

-t  Time interval to update info (seconds)

Defaults:

| Option    | Value      |
|:----------|:-----------|
| port      | 8080       |
| dbpath    | 'data.db'  |
| interval  | 300        |

HTTP API
--------

- /last?count=*last_records_num*
- /between?from=*from_date*&to=*to_date*

- - -

Happy birthday! ;)

*-- Vadim*
