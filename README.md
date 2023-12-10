# pg_bzip

## Motivation

If you obtain data compressed as bzip2, whether through HTTP (with [pgsql-http](https://github.com/pramsey/pgsql-http)) or from a file
(with [pgsql-fio](https://github.com/csimsek/pgsql-fio) or the native [pg_read_binary_file](https://pgpedia.info/p/pg_read_binary_file.html)), it's convenient to
decompress it in SQL directly. This extension is just for that, it provides functions to decompress and compress data using bzip2.

## Functions

- `bzcat(data bytea) returns bytea`

This function mimics the [bzcat](https://linux.die.net/man/1/bzcat) command, which decompresses data using bzip2.

```sql
select convert_from(bzcat(pg_read_binary_file('/path/to/all_movies.csv.bz2')), 'utf8') as contents;

                                                                  contents
--------------------------------------------------------------------------------------------------------------------------------------------
 "id","name","parent_id","date"                                                                                                            +
 "2","Ariel","8384","1988-10-21"                                                                                                           +
 "3","Varjoja paratiisissa","8384","1986-10-17"                                                                                            +
 "4","État de siège",\N,"1972-12-30"                                                                                                       +
 "5","Four Rooms",\N,"1995-12-22"                                                                                                          +
 "6","Judgment Night",\N,"1993-10-15"                                                                                                      +
 "8","Megacities - Life in Loops",\N,"2006-01-01"                                                                                          +
 "9","Sonntag, im August",\N,"2004-09-22"                                                                                                  +
 "11","Star Wars: Episode IV – A New Hope","10","1977-05-25"                                                                               +
 "12","Finding Nemo","112246","2003-05-30"                                                                                                 +
 ...
 ....
 .....
```

- `bzip2(data bytea, compression_level int default 9) returns bytea`

This function is a simplified version of the [bzip2](https://linux.die.net/man/1/bzip2) command. It compresses data using bzip2.

For this example we'll use `fio_writefile` from [pgsql-fio](https://github.com/csimsek/pgsql-fio), which offers a convenient way to write a file from SQL.

```sql
select fio_writefile('/home/stevechavez/Projects/pg_bzip/my_text.bz2', bzip2(repeat('my secret text to be compressed', 1000)::bytea)) as writesize;

 writesize
-----------
       109
```

## Installation

bzip2 is required. Under Debian/Ubuntu you can get it with

```
sudo apt install libbz2-dev
```

Then on this repo

```
make && make install
```

Now on SQL you can do:

```
CREATE EXTENSION bzip;
```

`pg_bzip` is tested to work on PostgreSQL >= 12.

## Development

[Nix](https://nixos.org/download.html) is used to get an isolated and reproducible enviroment with multiple postgres versions.

```
# enter the Nix environment
$ nix-shell

# to run the tests
$ with-pg-16 make installcheck

# to interact with the isolated pg
$ with-pg-16 psql

# you can choose the pg version
$ with-pg-15 psql
```
