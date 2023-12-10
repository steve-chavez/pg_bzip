create extension if not exists bzip;

select pg_read_file('./all_movies.csv') = convert_from(bzcat(pg_read_binary_file('./all_movies.csv.bz2')), 'utf8') as successful_decompression;

select pg_read_binary_file('./all_movies.csv.bz2') = bzip2(pg_read_binary_file('./all_movies.csv')) as successful_compression;
