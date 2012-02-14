/**
 * libtransport -- C++ library for easy XMPP Transports development
 *
 * Copyright (C) 2011, Jan Kaluza <hanzz.k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#ifdef WITH_MYSQL

#include "mysqlbackend.h"
#include "log.h"
#include "string.h"
#include "stdlib.h"

DEFINE_LOGGER(logger, "MySQLBackend");

using namespace log4cxx;

#define MYSQL_DB_VERSION 2
#define CHECK_DB_RESPONSE(stmt) \
	if(stmt) { \
		sqlite3_EXEC(m_db, "ROLLBACK;", NULL, NULL, NULL); \
		return 0; \
	}

// Prepare the SQL statement
#define PREP_STMT(sql, str) \
	sql = mysql_stmt_init(&m_conn);\
	if (mysql_stmt_prepare(sql, std::string(str).c_str(), std::string(str).size())) {\
		LOG4CXX_ERROR(logger, str << " " << mysql_error(&m_conn)); \
		return false; \
	}

// Finalize the prepared statement
#define FINALIZE_STMT(prep) \
	if(prep != NULL) { \
		mysql_stmt_close(prep); \
	}
	
#define BEGIN(STATEMENT, SIZE) 	MYSQL_BIND STATEMENT##_bind[SIZE]; \
							memset(STATEMENT##_bind, 0, sizeof(STATEMENT##_bind)); \
							int STATEMENT##_id = 1;\
							int STATEMENT##_id_get = 0;\
							(void)STATEMENT##_id_get;

#define BIND_INT(STATEMENT, VARIABLE) STATEMENT##_bind[STATEMENT##_id].buffer_type= MYSQL_TYPE_LONG;\
							STATEMENT##_bind[STATEMENT##_id].buffer= (char *)&VARIABLE;\
							STATEMENT##_bind[STATEMENT##_id].is_null= 0;\
							STATEMENT##_bind[STATEMENT##_id++].length= 0;
#define BIND_STR(STATEMENT, VARIABLE) STATEMENT##_bind[STATEMENT##_id].buffer_type= MYSQL_TYPE_STRING;\
							STATEMENT##_bind[STATEMENT##_id].buffer= VARIABLE.c_str();\
							STATEMENT##_bind[STATEMENT##_id].buffer_length= STRING_SIZE;\
							STATEMENT##_bind[STATEMENT##_id].is_null= 0;\
							STATEMENT##_bind[STATEMENT##_id++].length= VARIABLE.size();
#define RESET_GET_COUNTER(STATEMENT)	STATEMENT##_id_get = 0;
#define GET_INT(STATEMENT)	sqlite3_column_int(STATEMENT, STATEMENT##_id_get++)
#define GET_STR(STATEMENT)	(const char *) sqlite3_column_text(STATEMENT, STATEMENT##_id_get++)
#define EXECUTE_STATEMENT(STATEMENT, NAME) if (mysql_stmt_bind_param(STATEMENT, STATEMENT##_bind)) { \
		LOG4CXX_ERROR(logger, NAME << " " << mysql_error(&m_conn)); \
	} \
	if (mysql_stmt_execute(STATEMENT)) { \
		LOG4CXX_ERROR(logger, NAME << " " << mysql_error(&m_conn)); \
	}

#define EXEC(STMT, METHOD) \
	{\
	int ret = STMT->execute(); \
	if (ret == 0) \
		exec_ok = true; \
	else if (ret == 2013) { \
		LOG4CXX_INFO(logger, "MySQL connection lost. Reconnecting...");\
		disconnect(); \
		connect(); \
		return METHOD; \
	} \
	else \
		exec_ok = false; \
	}


static bool exec_ok;

MySQLBackend::Statement::Statement(MYSQL *conn, const std::string &format, const std::string &statement) {
	m_resultOffset = -1;
	m_conn = conn;
	m_offset = 0;
	m_string = statement;
	m_stmt = mysql_stmt_init(conn);
	if (mysql_stmt_prepare(m_stmt, statement.c_str(), statement.size())) {
		LOG4CXX_ERROR(logger, statement << " " << mysql_error(conn));
		return;
	}

	for (int i = 0; i < format.length() && m_resultOffset == -1; i++) {
		switch (format.at(i)) {
			case 's':
				m_params.resize(m_params.size() + 1);
				memset(&m_params.back(), 0, sizeof(MYSQL_BIND));

				m_params.back().buffer_type= MYSQL_TYPE_STRING;
				m_params.back().buffer= (char *) malloc(sizeof(char) * 4096);
				m_params.back().buffer_length= 4096;
				m_params.back().is_null= 0;
				m_params.back().length= (unsigned long *) malloc(sizeof(unsigned long));
				break;
			case 'i':
				m_params.resize(m_params.size() + 1);
				memset(&m_params.back(), 0, sizeof(MYSQL_BIND));

				m_params.back().buffer_type= MYSQL_TYPE_LONG;
				m_params.back().buffer= (int *) malloc(sizeof(int));
				m_params.back().is_null= 0;
				m_params.back().length= (unsigned long *) malloc(sizeof(unsigned long));
				break;
			case 'b':
				m_params.resize(m_params.size() + 1);
				memset(&m_params.back(), 0, sizeof(MYSQL_BIND));

				m_params.back().buffer_type= MYSQL_TYPE_TINY;
				m_params.back().buffer= (int *) malloc(sizeof(int));
				m_params.back().is_null= 0;
				m_params.back().length= (unsigned long *) malloc(sizeof(unsigned long));
				break;
			case '|':
				m_resultOffset = i;
				break;
		}
	}

	for (int i = m_resultOffset; i >= 0 && i < format.length(); i++) {
		switch (format.at(i)) {
			case 's':
				m_results.resize(m_results.size() + 1);
				memset(&m_results.back(), 0, sizeof(MYSQL_BIND));

				m_results.back().buffer_type= MYSQL_TYPE_STRING;
				m_results.back().buffer= (char *) malloc(sizeof(char) * 4096);
				m_results.back().buffer_length= 4096;
				m_results.back().is_null= 0;
				m_results.back().length= (unsigned long *) malloc(sizeof(unsigned long));
				break;
			case 'i':
				m_results.resize(m_results.size() + 1);
				memset(&m_results.back(), 0, sizeof(MYSQL_BIND));

				m_results.back().buffer_type= MYSQL_TYPE_LONG;
				m_results.back().buffer= (int *) malloc(sizeof(int));
				m_results.back().is_null= 0;
				m_results.back().length= (unsigned long *) malloc(sizeof(unsigned long));
				break;
			case 'b':
				m_results.resize(m_results.size() + 1);
				memset(&m_results.back(), 0, sizeof(MYSQL_BIND));

				m_results.back().buffer_type= MYSQL_TYPE_TINY;
				m_results.back().buffer= (int *) malloc(sizeof(int));
				m_results.back().is_null= 0;
				m_results.back().length= (unsigned long *) malloc(sizeof(unsigned long));
				break;
		}
	}

	if (mysql_stmt_bind_param(m_stmt, &m_params.front())) {
		LOG4CXX_ERROR(logger, statement << " " << mysql_error(conn));
	}

	if (m_resultOffset < 0)
		m_resultOffset = format.size();
	else {
		if (mysql_stmt_bind_result(m_stmt, &m_results.front())) {
			LOG4CXX_ERROR(logger, statement << " " << mysql_error(conn));
		}
	}
	m_resultOffset = 0;
}

MySQLBackend::Statement::~Statement() {
	for (int i = 0; i < m_params.size(); i++) {
		free(m_params[i].buffer);
		free(m_params[i].length);
	}
	for (int i = 0; i < m_results.size(); i++) {
		free(m_results[i].buffer);
		free(m_results[i].length);
	}
	FINALIZE_STMT(m_stmt);
}

int MySQLBackend::Statement::execute() {
	// If statement has some input and doesn't have any output, we have
	// to clear the offset now, because operator>> will not be called.
	m_offset = 0;
	m_resultOffset = 0;
	int ret;

	if ((ret = mysql_stmt_execute(m_stmt)) != 0) {
		LOG4CXX_ERROR(logger, m_string << " " << mysql_stmt_error(m_stmt) << "; " << mysql_error(m_conn));
		return mysql_stmt_errno(m_stmt);
	}
	return 0;
}

int MySQLBackend::Statement::fetch() {
	return mysql_stmt_fetch(m_stmt);
}

template <typename T>
MySQLBackend::Statement& MySQLBackend::Statement::operator << (const T& t) {
	if (m_offset >= m_params.size())
		return *this;
	int *data = (int *) m_params[m_offset].buffer;
	*data = (int) t;

// 	LOG4CXX_INFO(logger, "adding " << m_offset << ":" << (int) t);
	m_offset++;
	return *this;
}

MySQLBackend::Statement& MySQLBackend::Statement::operator << (const std::string& str) {
	if (m_offset >= m_params.size())
		return *this;
// 	LOG4CXX_INFO(logger, "adding " << m_offset << ":" << str << "(" << str.size() << ")");
	strncpy((char*) m_params[m_offset].buffer, str.c_str(), 4096);
	*m_params[m_offset].length = str.size();
	m_offset++;
	return *this;
}

template <typename T>
MySQLBackend::Statement& MySQLBackend::Statement::operator >> (T& t) {
	if (m_resultOffset > m_results.size())
		return *this;

	if (!m_results[m_resultOffset].is_null) {
		int *data = (int *) m_results[m_resultOffset].buffer;
		t = (int) *data;
// 		std::cout << "getting " << m_resultOffset << " " << (int) t << "\n";
	}

	if (++m_resultOffset == m_results.size())
		m_resultOffset = 0;
	return *this;
}

MySQLBackend::Statement& MySQLBackend::Statement::operator >> (std::string& t) {
// 	std::cout << "getting " << m_resultOffset << "\n";
	if (m_resultOffset > m_results.size())
		return *this;

	if (!m_results[m_resultOffset].is_null) {
		t = (char *) m_results[m_resultOffset].buffer;
	}

	if (++m_resultOffset == m_results.size())
		m_resultOffset = 0;
	return *this;
}

MySQLBackend::MySQLBackend() {
	m_prefix = "";
	mysql_init(&m_conn);
	my_bool my_true = 1;
	mysql_options(&m_conn, MYSQL_OPT_RECONNECT, &my_true);
}

MySQLBackend::~MySQLBackend(){
	disconnect();
}

void MySQLBackend::disconnect() {
	LOG4CXX_INFO(logger, "Disconnecting");
	delete m_setUser;
	delete m_getUser;
	mysql_close(&m_conn);
}

bool MySQLBackend::connect() {
// 	LOG4CXX_INFO(logger, "Connecting MySQL server " << CONFIG_STRING(m_config, "database.server") << ", user " <<
// 		CONFIG_STRING(m_config, "database.user") << ", database " << CONFIG_STRING(m_config, "database.database") <<
// 		", port " << CONFIG_INT(m_config, "database.port")
// 	);

	if (!mysql_real_connect(&m_conn, "localhost",
					   "root",
					   "kedsezis",
					   "stars",
					   0, NULL, 0)) {
		LOG4CXX_ERROR(logger, "Can't connect database: " << mysql_error(&m_conn));
		return false;
	}

	createDatabase();

	m_setUser = new Statement(&m_conn, "sss", "INSERT INTO " + m_prefix + "users (name, password, last_login) VALUES (?, ?, NOW()) ON DUPLICATE KEY UPDATE password=?");
	m_getUser = new Statement(&m_conn, "s|iss", "SELECT id, name, password FROM " + m_prefix + "users WHERE name=?");

	return true;
}

bool MySQLBackend::createDatabase() {
	int not_exist = exec("CREATE TABLE IF NOT EXISTS `" + m_prefix + "users` ("
				"`id` int(10) unsigned NOT NULL auto_increment,"
				"`name` varchar(255) collate utf8_bin NOT NULL,"
				"`password` varchar(255) collate utf8_bin NOT NULL,"
				"`last_login` datetime,"
				"`online` tinyint(1) NOT NULL  default '0',"
				"PRIMARY KEY (`id`),"
				"UNIQUE KEY `name` (`name`)"
			") ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;");

	if (not_exist) {
	}

	return true;
}

bool MySQLBackend::exec(const std::string &query) {
	if (mysql_query(&m_conn, query.c_str())) {
		LOG4CXX_ERROR(logger, query << " " << mysql_error(&m_conn));
		return false;
	}
	return true;
}

int MySQLBackend::setUser(const UserInfo &user) {
	*m_setUser << user.name << user.password << user.password;
	EXEC(m_setUser, setUser(user));

	return mysql_insert_id(&m_conn);
}

bool MySQLBackend::getUser(const std::string &name, UserInfo &user) {
// 		m_getUser = new Statement(&m_conn, "s|iss", "SELECT id, name, password FROM " + m_prefix + "users WHERE name=?");

	*m_getUser << name;
	EXEC(m_getUser, getUser(name, user));
	if (!exec_ok)
		return false;

	int ret = false;
	while (m_getUser->fetch() == 0) {
		ret = true;
		*m_getUser >> user.id >> user.name >> user.password;
	}

	return ret;
}


#endif
