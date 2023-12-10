create extension if not exists bzip;

SELECT encode(bzcat(bzip2('this will be compresssed and then uncompressed')), 'escape') as result;

with repeated_str as (
  select repeat('some text', 100000) as val
)
select convert_from(bzcat(bzip2(val::bytea)), 'utf8') = val as succesful_compression_decompression
from repeated_str;
