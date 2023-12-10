create or replace function bzcat(data bytea)
returns bytea
language 'c'
immutable
strict
as 'bzip';

create or replace function bzip2(data bytea, compression_level int default 9)
returns bytea
language 'c'
immutable
strict
as 'bzip';
