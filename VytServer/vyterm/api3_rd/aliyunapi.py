from aliyunsdkcore.client import AcsClient
from aliyunsdkcore.request import CommonRequest
from vyterm.utils import *
import random
import csv


@get_instance
@singleton
class AliyunAdapter:
    def __init__(self):
        with open(r'C:\Common Projects\Werewolf\VytServer\vyterm\api3_rd\accessKey.csv') as csvfile:
            reader = csv.reader(csvfile)
            rows = [row for row in reader]
            self.client = AcsClient(ak=rows[1][1], secret=rows[1][2], region_id='cn-hangzhou')
            del rows

    @staticmethod
    def RandomCode():
        return random.randint(100000, 999999)

    def SendSms(self, number: str, sign: str, template: str, code: str):
        request = CommonRequest()
        request.set_accept_format('json')
        request.set_domain('dysmsapi.aliyuncs.com')
        request.set_method('POST')
        request.set_protocol_type('https')  # https | http
        request.set_version('2017-05-25')
        request.set_action_name('SendSms')

        request.add_query_param('RegionId', 'cn-hangzhou')
        request.add_query_param('PhoneNumbers', number)
        request.add_query_param('SignName', sign)
        request.add_query_param('TemplateCode', template)
        request.add_query_param('TemplateParam', '{"code":"%s"}' % (code,))

        return self.client.do_action_with_exception(request)


if __name__ == '__main__':
    adapter = AliyunAdapter()
    response = adapter.SendSms('18986251734', 'Vyterm', 'SMS_150577820', '554687')
    print(str(response, encoding='utf-8'))
    # import time
    # time.sleep(60)
    # response = adapter.SendSms('18986251734', 'Vyterm', 'SMS_150577822', '554687')
    # print(str(response, encoding='utf-8'))
    # time.sleep(60)
    # response = adapter.SendSms('18986251734', 'Vyterm', 'SMS_150577823', '554687')
    # print(str(response, encoding='utf-8'))
    for _ in range(20):
        print(AliyunAdapter.RandomCode(), end=' ')
