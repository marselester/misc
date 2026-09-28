#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t db_status_t;
#define DB_OK                    0
#define DB_ERR_IO               -1
#define DB_ERR_INVALID_DB       -2
#define DB_ERR_OUT_OF_MEMORY    -3
#define DB_ERR_INVALID_ARGUMENT -4

typedef struct database database_t;

db_status_t db_open(
    const char* path,
    size_t path_len,
    database_t** out_db
);

void db_close(database_t* db);

db_status_t db_lookup_string(
    const database_t *db,
    const char **out,
    size_t *out_len
);

db_status_t db_lookup_u8(const database_t *db, uint8_t *out);
db_status_t db_lookup_u16(const database_t *db, uint16_t *out);
db_status_t db_lookup_u32(const database_t *db, uint32_t *out);
db_status_t db_lookup_u64(const database_t *db, uint64_t *out);
db_status_t db_lookup_i8(const database_t *db, int8_t *out);
db_status_t db_lookup_i16(const database_t *db, int16_t *out);
db_status_t db_lookup_i32(const database_t *db, int32_t *out);
db_status_t db_lookup_i64(const database_t *db, int64_t *out);

#ifdef __cplusplus
}
#endif
