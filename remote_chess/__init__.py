from flask import Flask
from flask_pymongo import PyMongo

app = Flask(__name__)

app.config['MONGO_DBNAME'] = 'remote_chess'
mongo = PyMongo(app, config_prefix='MONGO')