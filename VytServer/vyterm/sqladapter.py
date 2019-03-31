# coding: utf-8

import pymysql
import hashlib


def sqlprotect(func):
    def protect(self, query: str):
        try:
            r = func(self, query)
            return r if r else True
        except Exception as e:
            print(e)
            self.sql.rollback()
            return False

    return protect


class SqlAdapter:
    def __init__(self, host='127.0.0.1', port=3306, user='root', db='', password=''):
        try:
            self.sql = pymysql.connect(host=host, db=db, user=user, password=password, port=port)
            self.cursor = self.sql.cursor()
        except Exception as exception:
            print(exception)
            raise exception

    @sqlprotect
    def execute(self, query: str):
        self.cursor.execute(query)
        self.sql.commit()

    @sqlprotect
    def select(self, query: str):
        self.cursor.execute(query)
        return self.cursor.fetchall()


if __name__ == '__main__':
    adapter = SqlAdapter(db='test', user='root', password='1JXahbu230x1Zehim88t')
    [print(row) for row in adapter.select("Select * from user_table")]
    m = hashlib.md5()
    m.update('885546845'.encode())
    print(m.hexdigest())
