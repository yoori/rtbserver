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


template <typename Entity>
AutoTest::ORM::ORMRestorer<Entity>*
BaseDBUnit::create(unsigned long id)
  throw(eh::Exception)
{
  typedef AutoTest::ORM::ORMRestorer<Entity> ORMEntity;
  typedef typename Entity::Connection ConnectionType;

  ORMEntity* entity =
    new ORMEntity(
      get_conn(ConnectionType()), id);
  
  restorers_.push_front(entity);

  return entity;
}

template <typename Entity>
AutoTest::ORM::ORMRestorer<Entity>*
BaseDBUnit::create(const Entity& e)
  throw(eh::Exception)
{
  typedef AutoTest::ORM::ORMRestorer<Entity> ORMEntity;

  ORMEntity* entity =
    new ORMEntity(e);

  restorers_.push_front(entity);

  return entity;
}

template <typename Entity>
AutoTest::ORM::ORMRestorer<Entity>*
BaseDBUnit::create()
  throw(eh::Exception)
{
  typedef AutoTest::ORM::ORMRestorer<Entity> ORMEntity;
  typedef typename Entity::Connection ConnectionType;

  ORMEntity* entity =
    new ORMEntity(
      get_conn(ConnectionType()));
  
  restorers_.push_front(entity);

  return entity;
}

inline
AutoTest::DBC::Conn&
BaseDBUnit::get_conn(
  AutoTest::ORM::postgres_connection)
{
  return pq_conn_;
}

