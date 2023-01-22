from flask import Flask
import time
import paho.mqtt.client as paho
from paho import mqtt
import json
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy import Column, Integer, String , Sequence
from datetime import datetime
import ast

app = Flask(__name__)

app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///cbes.db'
app.config['SQLALCHMY_TRACK_MODIFICATION'] = False
db = SQLAlchemy(app)

class Ubibot_data(db.Model):
    id = Column(String,primary_key=True)
    temp = Column(Integer,primary_key = True)
    ubibot_hum = Column(Integer)
    ubibot_lux = Column(Integer)

    def __repr__(self):
        env_data ={}
        env_vals={}
        env_vals["Temperature"] = self.temp
        env_vals["Humidity"] = self.ubibot_hum
        env_vals["lux"] = self.ubibot_lux
        env_data[self.id] = env_vals

        return f' {env_data}'


class Temp_tanks(db.Model):
    id = db.Column(db.Integer,primary_key=True)
    temp_tak1 = db.Column(db.Integer,primary_key=True)
    temp_tak2 = db.Column(db.Integer,primary_key=True)
    temp_tak3 = db.Column(db.Integer,primary_key=True)
    def __repr__(self): # to return a dictionery that gives two dicts
        temp = {}
        value = {}
        value['Tank1'] = self.temp_tak1
        value['Tank2'] = self.temp_tak1
        value['Tank3'] = self.temp_tak1
        temp[self.id] = value
        return f'{temp}'

class Flow1(db.Model):
    id = db.Column(db.Integer,primary_key=True)
    f1 = db.Column(db.Integer,primary_key=True)
    f2 = db.Column(db.Integer,primary_key=True)
    
    def __repr__(self) :
        flow = {}
        f_val = {}
        f_val['Flow_rate1'] = self.f1
        f_val['Flow_rate2'] = self.f2
        flow[self.id] = f_val
        return f'{flow}'

class power_unit(db.Model):

    name = Column(String,primary_key=True)
    OUTPUT_POWER = Column(Integer,primary_key=True)
    PV_POWER = Column(Integer)
    def __repr__(self) :
        power = {}
        p_values = {}
        p_values['Output_power'] = self.OUTPUT_POWER
        p_values['Solar_power'] = self.PV_POWER

        power[self.name] = p_values

        return f'{power}'


port = 5000

def on_connect(client, userdata, flags, rc, properties=None):
    
    print("CONNACK received with code %s." % rc)
    client.subscribe("cbes/dekut/data/heating_unit", qos=1)
    client.subscribe("cbes/dekut/data/environment_meter", qos=1)
    client.subscribe("cbes/dekut/data/electric_power", qos=1)
def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))
def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("happy_nnew year")
    #print("Subscribed: " + str(mid) + " " + str(granted_qos))
def on_message(client, userdata, msg):
    #print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    print(msg.topic + " " + str(msg.qos) + " " )
    if msg.topic == "cbes/dekut/data/heating_unit":
        data2 = (msg.payload).decode()
        data = ast.literal_eval(data2)
    
        #Adding data to the data_base
        tempt1 = Temp_tanks(
            id = (datetime.today()).strftime("%d-%b-%Y %H:%M:%S:%f"),
            temp_tak1 = int(data['TankT1']),
            temp_tak2 = int(data['TankT2']),
            temp_tak3 = int(data['TankT3']))

        db.session.add(tempt1)
        # Adding data for flow
        flow = Flow1(
            id = (datetime.today()).strftime("%d-%b-%Y %H:%M:%S:%f"),
            f1 = int(data['Flow1']),
            f2 = int(data['Flow2']))
        db.session.add(flow)
        db.session.commit()
    if msg.topic == "cbes/dekut/data/electric_power":
        power_data = (msg.payload).decode()
        #pwr_data = ast.literal_eval(power_data)
        pwr_data = eval(power_data)
        print(type(pwr_data))
        for elements in pwr_data:
            print(elements)
            print(pwr_data[elements]["Output_power"])
            print(pwr_data[elements]["pv_power"])
            date = str(elements)
            PV_Pr = int(pwr_data[elements]["pv_power"])
            Otput = int(pwr_data[elements]["Output_power"])
            # print(type(PV_Pr))
            # print(type(Otput))
            # print(type(date))
            E_power_data =power_unit(
                name = elements,
                OUTPUT_POWER = Otput,
                PV_POWER = PV_Pr
                )
            db.session.add(E_power_data)
            db.session.commit()
    if msg.topic == 'cbes/dekut/data/environment_meter':
        ubi_bot = (msg.payload).decode()
        d_ubibot_data = eval(ubi_bot)
        x = []
        for i in d_ubibot_data["feeds"]:
            x.append(i)
        ubi_sql = Ubibot_data(
            id =  (datetime.today()).strftime("%d-%b-%Y %H:%M:%S:%f"),
            temp = int(x[2]['field1']),
            ubibot_hum = int(x[3]['field2']),
            ubibot_lux = int(x[0]['field3'])
            )
        db.session.add(ubi_sql)
        db.session.commit()
        

        # print (f'temp {tem} hum {hum} lux{lux}')







@app.route('/')
def hello_world():
    return 'Hello World! I am running on port_This is CBES '

@app.route('/cbes/tank_temp',methods=["GET","POST"])
def get_tempdata():
   # return " Hello_World"
    data = Temp_tanks.query.all()
    #print(data)
    return f'{data}'

@app.route('/cbes/flow_rates',methods=["Get","POST"])
def get_flowdata():
    data2 = Flow1.query.all()

    return f'{data2}' 


@app.route('/cbes/ubibot')
def get_ubibot():
    environment = Ubibot_data.query.all()
    return f'{environment}'

@app.route('/cbes/powerunit')
def get_solar():
    mpp_solar = power_unit.query.all()
    return f'{mpp_solar}'
    

if __name__ == '__main__':
    client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
    #client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.on_message = on_message
#     client.connect('localhost')
    client.on_connect = on_connect
    client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
    client.loop_start()

    app.run(host='0.0.0.0', port=port)
