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

#pragma once

#ifdef WITH_MYSQL

#include <string>
#include <map>
#include <vector>
#include "mysql.h"

struct UserInfo {
	int id;				///< id of user used as primary key in database
	std::string jid;		///< barejid of XMPP user
	std::string uin;		///< legacy network username
	std::string password;	///< password for legacy network
	std::string language;	///< user's preferred language
	std::string encoding;	///< user's preferred encoding
	bool vip;				///< true if user is VIP
};

class MySQLBackend {
	public:
		MySQLBackend();

		/// Destructor.
		~MySQLBackend();

		/// Connects to the database and creates it if it's needed. This method call createDatabase() function
		/// automatically.
		/// \return true if database is opened successfully.
		bool connect();
		void disconnect();

		/// Creates database structure.
		/// \see connect()
		/// \return true if database structure has been created successfully. Note that it returns True also if database structure
		/// already exists.
		bool createDatabase();

		void setUser(const UserInfo &user);

		bool getUser(const std::string &barejid, UserInfo &user);

	private:
		bool exec(const std::string &query);

		class Statement {
			public:
				Statement(MYSQL *conn, const std::string &format, const std::string &statement);
				~Statement();

				int execute();

				int fetch();

				// Pushes new data used as input for the statement.
				template <typename T>
				Statement& operator << (const T& t);

				Statement& operator << (const std::string& str);

				// Pulls fetched data by previous execute(); call.
				template <typename T>
				Statement& operator >> (T& t);
				
				Statement& operator >> (std::string& t);
			private:
				MYSQL_STMT *m_stmt;
				MYSQL *m_conn;
				std::vector<MYSQL_BIND> m_params;
				std::vector<MYSQL_BIND> m_results;
				int m_resultOffset;
				int m_offset;
				int m_error;
				std::string m_string;
		};

		MYSQL m_conn;
		std::string m_prefix;

		// statements
		Statement *m_setUser;
		Statement *m_getUser;
};


#endif
