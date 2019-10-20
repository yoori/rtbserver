/* 
 * This file is part of the RTBServer distribution (https://github.com/yoori/rtbserver).
 * RTBServer is DSP server that allow to bid (see RTB auction) targeted ad
 * via RTB protocols (OpenRTB, Google AdExchange, Yandex RTB)
 *
 * Copyright (c) 2014 Yuri Kuznecov <yuri.kuznecov@gmail.com>.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

template<typename T, size_t Count>
bool AutoTest::ORM::clear_stats(StatsDB::IConn& connection,
  const char* column_name,
  const T (&column_values)[Count])
{
  StatsDB::Query query(connection.get_query(
     "SELECT nspname, relname FROM pg_attribute "
     "JOIN pg_class ON pg_class.oid = attrelid "
     "JOIN pg_namespace ON pg_namespace.oid = relnamespace "
     "WHERE attname = :1 AND relkind = 'r' "
     "AND (nspname in ('stat', 'ctr') OR "
     "(nspname = 'adserver' AND relname like 'snapshot_%'))"));
  query.set(tolower(column_name));

  StatsDB::Result result(query.ask());
  bool ret = true;
  while (result.next())
  {
    std::string schema_name, table_name;
    result.get(schema_name);
    result.get(table_name);
    std::ostringstream query_str;
    query_str << "DELETE FROM ONLY " << schema_name << "." <<
      table_name << " WHERE " << column_name << " in (";
    for (size_t i = 0; i < Count; ++i)
    {
      query_str << ":i" << i <<
        (i == Count - 1? ")": ", ");
    }
    StatsDB::Query delete_query(connection.get_query(query_str.str()));
    for (size_t i = 0; i < Count; ++i)
    {
      delete_query.set(column_values[i]);
    }
    ret &= delete_query.update() > 0;
    connection.commit();
  }
  return ret;
}

template<typename T>
bool AutoTest::ORM::clear_stats(StatsDB::IConn& connection,
  const char* column_name,
  const T& column_value)
{
  const T array[] = { column_value };
  return clear_stats(connection, column_name, array);
}
