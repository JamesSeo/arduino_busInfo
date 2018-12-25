#-*- coding: utf-8 -*-

import requests
import xml.etree.ElementTree as ET
import re


from flask import Flask
app = Flask(__name__)



server="http://ws.bus.go.kr/api/rest"
service_key="your_service_key" # 공공디비 사이트에서 등록후 받아야 함. 

api_station_bus = "stationinfo/getRouteByStation" # 버스정류장을 지나는 노선정보


busRouteId="123456789" # 마을버스 9 digit
  

api_arrive_info ="arrive/getArrInfoByRoute" # 도착 정보


def get_arrival(stationId, busRouteId, busOrdId, count=1):
        res = " "
        url_2  = "{}/{}?ServiceKey={}&stId={}&busRouteId={}&ord={}".format(server, api_arrive_info, service_key, stationId, busRouteId, busOrdId)
        r = requests.get(url_2)
        tree = ET.fromstring(r.text.encode('UTF-8'))

        for item in tree.find("msgBody").findall('itemList'):
                res = item.find('arrmsg1').text
                if (count>1):
                        res = "{},{}".format(res, item.find('arrmsg2').text)
        return res


def get_arrive_info(stId):
        stationId = stId
        dspOrd ={'your_stationsId1': 31, 'your_stationsId2':6} # dspOrd는 버스 정류장아이디에 매칭되므로, api를 불러서 확인해야 한다 

        res = get_arrival(stationId,busRouteId,dspOrd[stId])
        print(res)
        res1 = res.replace(u'초','s')
        res1 = res1.replace(u'분','m')
        res1 = res1.replace(u'후','a')
        res1 = res1.replace(u'곧','n') # now
        res1 = res1.replace(u'도','b') #  arr
        res1 = res1.replace(u'착','c') #  arr
        res1 = re.sub(r'\[.*\]', '', res1, flags=re.IGNORECASE)
        print(res1)
        return res1


# 정류장 한개에 대한 도착정보 
@app.route("/bus/v1/arrive/<stId>", methods=['GET'])
def api_get_arrival(stId):
        return get_arrive_info(stId)


# 동시에 두개를 조회 해서 보내주는 api 
@app.route("/bus/v1/arrive_all/", methods=['GET'])
def api_get_arrival_all():
        res1 = get_arrive_info('0000000001'). #정거장 아이디를 넣어야 함
        res2 = get_arrive_info('0000000002')
        return "{}:{}/{}:{}".format('xy',res1,'od',res2)


