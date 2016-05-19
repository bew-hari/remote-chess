from flask import Flask
from flask_pymongo import PyMongo

app = Flask(__name__)
PARTICLE_URI = 'https://api.particle.io/v1/devices/'