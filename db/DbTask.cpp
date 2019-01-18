#include "DbTask.h"
#include "DbApp.h"

DbQueryTask::DbQueryTask(const char* sql, size_t size) {
	sql_ = strdup(sql);
	size_ = size;
}

DbQueryTask::~DbQueryTask() {
	if (sql_) {
		free(sql_);
	}
}

void DbQueryTask::ThreadDo(DbMysql* db) {
	db->Execute(sql_, size_, result_);
}

void DbQueryTask::MainDo() {

}

DbRawSqlTask::DbRawSqlTask(const char* sql, size_t size) {
	sql_ = strdup(sql);
	size_ = size;
}

DbRawSqlTask::~DbRawSqlTask() {
	if (sql_) {
		free(sql_);
	}
}

void DbRawSqlTask::ThreadDo(DbMysql* db) {
	db->Execute(sql_, size_, result_);
}

void DbRawSqlTask::MainDo() {

}
