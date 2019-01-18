#include "DbTask.h"
#include "DbApp.h"

DbQueryTask::DbQueryTask(int reference, const char* sql, size_t size) {
	sql_ = strdup(sql);
	size_ = size;
	reference_ = reference;
}

DbQueryTask::~DbQueryTask() {
	if (sql_) {
		free(sql_);
	}
}

void DbQueryTask::ThreadDo(DbMysql* db) {
	db->Execute(sql_, size_, result_);
	APP->GetQueue()->Push(this);
}

void DbQueryTask::MainDo() {

}

DbRawSqlTask::DbRawSqlTask(int reference, const char* sql, size_t size) {
	sql_ = strdup(sql);
	size_ = size;
	reference_ = reference;
}

DbRawSqlTask::~DbRawSqlTask() {
	if (sql_) {
		free(sql_);
	}
}

void DbRawSqlTask::ThreadDo(DbMysql* db) {
	db->Execute(sql_, size_, result_);
	APP->GetQueue()->Push(this);
}

void DbRawSqlTask::MainDo() {

}
