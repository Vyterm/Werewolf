# coding: utf-8

import pymysql


class SqlAdapter:
    def __init__(self):
        try:
            self.sql = pymysql.connect(host='127.0.0.1', db='vytwerewolf', user='vyterm', password='123456', port=3306)
            self.cursor = self.sql.cursor()
        except Exception as exception:
            print(exception)
