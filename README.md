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

API
---

- /last?count=*last_records_num*
- /between?from=*from_date*&to=*to_date*

- - -

Happy birthday! ;)

*-- Vadim*
