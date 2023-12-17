create extension if not exists bzip;
\echo

select bzcat(pg_read_binary_file('./damaged_all_movies.csv.bz2'));
\echo

select bzcat(pg_read_binary_file('./empty_file'));
\echo

select bzcat(pg_read_binary_file('./wrong_header_all_movies.csv.bz2'));
