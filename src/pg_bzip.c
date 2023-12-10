/*system*/
#include <stdio.h>
#include <stdlib.h>

/*postgres*/
#include <postgres.h>
#include <fmgr.h>
#include <funcapi.h>
#include <utils/builtins.h>

/* bzip2 https://sourceware.org/bzip2/manual/manual.html */
#include <bzlib.h>

#define BZ2_VERBOSITY 0

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(bzcat);
PG_FUNCTION_INFO_V1(bzip2);

/*
 *custom memory allocators for bzip2
 */
static void *
pg_bz2alloc(void *opaque, int n, int m) {
  return palloc(n * m);
}

static void
pg_bz2free(void *opaque, void *p) {
  pfree(p);
}

/* compress using bzip2 */
Datum bzip2(PG_FUNCTION_ARGS){
  bytea *arg_data = PG_GETARG_BYTEA_P(0);
  int32 compression_level = PG_GETARG_INT32(1);

  if (compression_level < 1 || compression_level > 9)
    ereport(ERROR,
        errmsg("compression level out of range: %d", compression_level),
        errdetail("the compression level should be an int between 1 and 9 inclusive"));

  char buffer[BZ_MAX_UNUSED];
  int status;

  bz_stream stream =
    { .next_in   = VARDATA(arg_data)
    , .avail_in  = VARSIZE_ANY_EXHDR(arg_data)
    , .next_out  = buffer
    , .avail_out = BZ_MAX_UNUSED
    , .bzalloc   = pg_bz2alloc
    , .bzfree    = pg_bz2free
    , .opaque    = NULL
    };

  status = BZ2_bzCompressInit(&stream,
      compression_level,
      BZ2_VERBOSITY,
      0); // according to the man pages (on --repetitive-fast --repetitive-best), the workFactor is unused, so we just leave it at 0 which will take the default.

  if ( status != BZ_OK ) {
    ereport(ERROR, errmsg("bzip2 compression initialization failed"));
  }

  StringInfoData si;
  initStringInfo(&si);

  do {
    // when avail_in is 0, the compression will be finished but the whole data won't necessarily be transferred because of the buffer size.
    // changing the mode to BZ_FINISH transfers the remaining data to the buffer
    status = BZ2_bzCompress(&stream, (stream.avail_in) ? BZ_RUN : BZ_FINISH);

    // avail_out indicates how much of the buffer is empty, so we do a substraction to append the buffer part that is filled
    // only during the last iteration avail_out is actually non-zero
    appendBinaryStringInfo(&si, (char*)buffer, BZ_MAX_UNUSED - stream.avail_out);

    // avail_out and next_out will be reset (0) after one iteration so we set them again
    stream.avail_out = BZ_MAX_UNUSED;
    stream.next_out = buffer;
  } while (status == BZ_RUN_OK || status == BZ_FINISH_OK);

  if ( status != BZ_STREAM_END) {
    BZ2_bzCompressEnd(&stream);
    ereport(ERROR, errmsg("bzip2 compression failed"));
  }

  BZ2_bzCompressEnd(&stream);

  bytea *result = palloc(si.len + VARHDRSZ);
  memcpy(VARDATA(result), si.data, si.len);
  SET_VARSIZE(result, si.len + VARHDRSZ);
  PG_FREE_IF_COPY(arg_data, 0);
  PG_RETURN_POINTER(result);
}

/* decompress bzip2 */
Datum bzcat(PG_FUNCTION_ARGS){
  bytea *arg_data = PG_GETARG_BYTEA_P(0);

  char buffer[BZ_MAX_UNUSED];
  int status;

  bz_stream stream =
    { .next_in   = VARDATA(arg_data)
    , .avail_in  = VARSIZE_ANY_EXHDR(arg_data)
    , .next_out  = buffer
    , .avail_out = BZ_MAX_UNUSED
    , .bzalloc   = pg_bz2alloc
    , .bzfree    = pg_bz2free
    , .opaque    = NULL
    };

  status = BZ2_bzDecompressInit(&stream, BZ2_VERBOSITY, 0);

  if ( status != BZ_OK ) {
    ereport(ERROR, errmsg("bzip2 decompression initialization failed"));
  }

  StringInfoData si;
  initStringInfo(&si);

  do {
    status = BZ2_bzDecompress(&stream);

    appendBinaryStringInfo(&si, (char*)buffer, BZ_MAX_UNUSED - stream.avail_out);

    stream.avail_out = BZ_MAX_UNUSED;
    stream.next_out = buffer;
  } while (status == BZ_OK);

  if ( status != BZ_STREAM_END ){
    BZ2_bzDecompressEnd(&stream);
    ereport(ERROR, errmsg("bzip2 decompression failed"));
  }

  BZ2_bzDecompressEnd(&stream);

  bytea *result = palloc(si.len + VARHDRSZ);
  memcpy(VARDATA(result), si.data, si.len);
  SET_VARSIZE(result, si.len + VARHDRSZ);
  PG_FREE_IF_COPY(arg_data, 0);
  PG_RETURN_POINTER(result);
}
